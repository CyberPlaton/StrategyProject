#include "DBMS.h"

namespace dbms
{
	using bsoncxx::builder::stream::close_document;
	using bsoncxx::builder::stream::document;
	using bsoncxx::builder::stream::finalize;
	using bsoncxx::builder::stream::open_array;
	using bsoncxx::builder::stream::close_array;
	using bsoncxx::builder::stream::open_document;
	using bsoncxx::builder::basic::kvp;
	using bsoncxx::builder::basic::make_array;
	using bsoncxx::builder::basic::make_document;
	using bsoncxx::builder::basic::concatenate;



	DBMS* DBMS::g_DBMS = nullptr;
	const std::string DBMS::m_database = "test_database";
	const std::string DBMS::m_userDataCollection = "test_collection_user_data";
	const std::string DBMS::m_leaderboardCollection = "test_collection_leaderboard";
	size_t DBMS::m_nextUUID = 0;
	bool DBMS::m_initialized = false;


	bool DBMS::DeleteUser(size_t id)
	{
		if (!m_initialized) return false;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];

		try
		{
			auto result = user_data.delete_one(
				make_document(kvp("_id", (int64_t)id))
			);

			return (result->deleted_count() > 0) ? true : false;
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::DeleteUser - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}

		return false;
	}

	bool DBMS::UpdateUser(net::UserDesc& desc)
	{
		using namespace std;

		if (!m_initialized) return false;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];


		// Create Achievements Array.
		auto achiev_array = bsoncxx::builder::basic::array{};
		for (auto e : desc.m_achievementsVec)
		{
			achiev_array.append(e);
		}

		// Create Service Items Array.
		auto item_array = bsoncxx::builder::basic::array{};
		for (auto e : desc.m_serviceItemsVec)
		{
			item_array.append(e);
		}

		auto update_doc = make_document( kvp("$set", make_document(kvp("SteamId", (int64_t)desc.m_steamId),
																   kvp("SteamName", desc.m_steamName),
																   kvp("NintendoId", (int64_t)desc.m_nintendoId),
																   kvp("NintendoName", desc.m_nintendoName),
																   kvp("Currency", (int64_t)desc.m_currency),
																   kvp("Achievements", achiev_array),
																   kvp("ServiceItems", item_array)))
		);

		try
		{
			auto result = user_data.update_one(
				make_document(kvp("_id", (int64_t)desc.m_uuid)), // Find this Doc.
				std::move(update_doc.view())					 // Update with this Doc.
			);

			return (result->modified_count() > 0) ? true : false;
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::UpdateUser - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}

		return false;
	}

	bool DBMS::GetUserDesc(net::UserDesc& desc)
	{
		if (!m_initialized) return false;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];

		boost::optional<bsoncxx::v_noabi::document::value> found_doc;
		if (!_findOneByKeyValuePair(user_data, found_doc, "_id", (int64_t)desc.m_uuid))
		{
			switch (desc.m_userPlatform)
			{
			case net::EUserPlatform::UP_STEAM:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "SteamId", (int64_t)desc.m_steamId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "SteamName", desc.m_steamName.c_str());
				}
			}
			break;
			case net::EUserPlatform::UP_SWITCH:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "NintendoId", (int64_t)desc.m_nintendoId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "NintendoName", desc.m_nintendoName.c_str());
				}
			}
			break;
			case net::EUserPlatform::UP_XBOX:
			{
				//if (!_findOneByKeyValuePair(user_data, found_doc, "XboxId", (int64_t)desc.m_xboxId))
				//{
				//	_findOneByKeyValuePair(user_data, found_doc, "XboxName", desc.m_xboxName.c_str());
				//}
			}
			break;
			case net::EUserPlatform::UP_PS:
			{
				//if (!_findOneByKeyValuePair(user_data, found_doc, "PSId", (int64_t)desc.m_psId))
				//{
				//	_findOneByKeyValuePair(user_data, found_doc, "PSName", desc.m_psName.c_str());
				//}
			}
			break;

			default:
				printf("DBMS::GetUserDesc - Undefined User Platform!\n");
				return false;
			}
		}


		// If we couldnt find User by any ids, he does not exist!		
		if (!found_doc.has_value()) return false;
		
		auto doc = found_doc->view();
		

		// Insert Data into UserDesc.
		desc.m_steamId = doc["SteamId"].get_int64();
		desc.m_nintendoId = doc["NintendoId"].get_int64();
		desc.m_steamName = doc["SteamName"].get_utf8().value.to_string();
		desc.m_nintendoName = doc["NintendoName"].get_utf8().value.to_string();

		desc.m_currency = doc["Currency"].get_int64();


		// Get the achievements and service items.
		if (!doc["Achievements"].get_array().value.empty())
		{
			
			for (int i = 0; i < net::EAchievement::ACHIEV_ACHIEVEMENTS_COUNT; i++)
			{
				// Access to an invalid index yields an invalid element, not an expection.
				auto elem = doc["Achievements"][i];

				if (!elem) break;

				if (elem)
				{
					try
					{
						// Store valid Achievement Elements.
						auto e = (int64_t)elem.get_int32();
						desc.m_achievementsVec.push_back((net::EAchievement)e);
					}
					catch (const bsoncxx::v_noabi::exception& e)
					{
						printf("DBMS::GetUserDesc - bsoncxx::v_noabi::exception - Message: \n\t\t \"%s\"\n", e.what());
					}
				}
			}
			
		}
		if (!doc["ServiceItems"].get_array().value.empty())
		{
			for (int i = 0; i < net::EServiceItem::ITEM_ITEMS_COUNT; i++)
			{
				// Access to an invalid index yields an invalid element, not an expection.
				auto elem = doc["ServiceItems"][i];
				
				if (!elem) break;

				if (elem)
				{
					try
					{
						// Store valid Achievement Elements.
						auto e = (int64_t)elem.get_int32();
						desc.m_serviceItemsVec.push_back((net::EServiceItem)e);
					}
					catch (const bsoncxx::v_noabi::exception& e)
					{
						printf("DBMS::GetUserDesc - bsoncxx::v_noabi::exception - Message: \n\t\t \"%s\"\n", e.what());
					}
				}
			}
		}

		return true;
	}


	size_t DBMS::CreateUser()
	{
		if (!m_initialized) return -1;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];

		// Get the next uuid for a user.
		int64_t uuid = m_nextUUID;

		// Create a Document/UserEntry with default values.
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc = builder
			// Custom id
			<< "_id" << uuid

			// Platform Information
			<< "ClientPlatform" << 0
			<< "SteamId" << 0
			<< "SteamName" << ""
			<< "NintendoId" << 0
			<< "NintendoName" << ""

			// Gamerelated Information
			<< "Currency" << 0
			<< "Achievements" << open_array << close_array
			<< "ServiceItems" << open_array << close_array


			<< bsoncxx::builder::stream::finalize;

		try
		{
			bsoncxx::stdx::optional<mongocxx::result::insert_one> result = user_data.insert_one(std::move(doc));

			if (result->result().inserted_count() > 0)
			{
				printf("DBMS::CreateUser - New UserId=%d\n", uuid);
				m_nextUUID += 1;
				return uuid;
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::CreateUser - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}

	
		return 0;
	}


	bool DBMS::_init()
	{
		// Try connecting to Database.
		// Exception is catched in case it is not running.
		try
		{
			// Check connection.
			auto admin = m_mongoClient["admin"];
			auto result = admin.run_command(make_document(kvp("ping", 1)));
			

			// Get or Create our Database.
			mongocxx::database db = m_mongoClient[m_database];
			mongocxx::collection user_data = db[m_userDataCollection]; 
			mongocxx::collection leaderboard = db[m_leaderboardCollection];


			std::ifstream infile("UserUUID.ini");

			std::string line;
			std::getline(infile, line);

			std::istringstream iss(line);

			iss >> m_nextUUID;


			printf("[DBMS] Initialized DBMS!\n");
			m_initialized = true;
			return true;
		}
		catch (const std::exception& e)
		{
			printf("[DBMS] Could not connect to Database:\n\t \"%s\"\n", e.what());
			m_initialized = false;
			return false;
		}
	}

	void DBMS::_shutdown() noexcept
	{
		// Write current UUID in the File.
		std::ofstream outfile("UserUUID.ini", std::ofstream::out | std::ofstream::trunc);
		outfile << m_nextUUID;
		outfile.close();
	}

	void DBMS::BackupUserNumber()
	{
		// Write current UUID in the File.
		std::ofstream outfile("UserUUID.ini", std::ofstream::out | std::ofstream::trunc);
		outfile << m_nextUUID;
		outfile.close();
	}
}