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

	bool DBMS::UpdateUser(net::SClientDescription& desc)
	{
		using namespace std;

		if (!m_initialized) return false;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];


		// Create Achievements Array.
		auto achiev_array = bsoncxx::builder::basic::array{};
		for (auto e : desc.m_achievements)
		{
			achiev_array.append(e);
		}

		// Create Service Items Array.
		auto item_array = bsoncxx::builder::basic::array{};
		for (auto e : desc.m_serviceItems)
		{
			item_array.append(e);
		}

		auto update_doc = make_document( kvp("$set", make_document(kvp("SteamId", (int64_t)desc.m_steamId),
																   kvp("SteamName", desc.m_steamName),
																   kvp("NintendoId", (int64_t)desc.m_nintendoId),
																   kvp("NintendoName", desc.m_nintendoName),
																   kvp("Currency", (int64_t)desc.m_currencyAmount),
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

	bool DBMS::GetUserDesc(net::SClientDescription& desc)
	{
		if (!m_initialized) return false;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];

		boost::optional<bsoncxx::v_noabi::document::value> found_doc;
		if (!_findOneByKeyValuePair(user_data, found_doc, "_id", (int64_t)desc.m_uuid))
		{
			switch (desc.m_platform)
			{
			case net::EClientPlatform::UP_STEAM:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "SteamId", (int64_t)desc.m_steamId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "SteamName", desc.m_steamName.C_String());
				}
			}
			break;
			case net::EClientPlatform::UP_SWITCH:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "NintendoId", (int64_t)desc.m_nintendoId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "NintendoName", desc.m_nintendoName.C_String());
				}
			}
			break;
			case net::EClientPlatform::UP_XBOX:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "XboxLiveId", (int64_t)desc.m_xboxLiveId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "XboxLiveName", desc.m_xboxLiveName.C_String());
				}
			}
			break;
			case net::EClientPlatform::UP_PS:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "PSNId", (int64_t)desc.m_psnId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "PSNName", desc.m_psnName.C_String());
				}
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
		desc.m_xboxLiveId = doc["XboxLiveId"].get_int64();
		desc.m_psnId = doc["PSNId"].get_int64();

		desc.m_steamName = doc["SteamName"].get_utf8().value.to_string().c_str();
		desc.m_nintendoName = doc["NintendoName"].get_utf8().value.to_string().c_str();
		desc.m_xboxLiveName = doc["XboxLiveName"].get_utf8().value.to_string().c_str();
		desc.m_psnName = doc["PSNName"].get_utf8().value.to_string().c_str();

		desc.m_currencyAmount = doc["Currency"].get_int64();


		// Get the achievements and service items.
		if (!doc["Achievements"].get_array().value.empty())
		{
			
			for (int i = 0; i < net::EAchievement::ACHIEV_COUNT; i++)
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
						desc.m_achievements.push_back((net::EAchievement)e);
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
			for (int i = 0; i < net::EServiceItem::ITEM_COUNT; i++)
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
						desc.m_serviceItems.push_back((net::EServiceItem)e);
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


	bool DBMS::Initialize()
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


	void DBMS::CreateGame(const std::string& gamename)
	{
		if (!m_initialized) return;

		std::string name = "game_" + gamename;

		// Get Database.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];

		if (!db.has_collection(name))
		{
			db.create_collection(name);
		}
	}

	void DBMS::DeleteGame(std::string gamename)
	{
		if (!m_initialized) return;

		std::string name = "game_" + gamename;

		// Get Database.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];

		if (db.has_collection(name))
		{
			mongocxx::collection game = db[name];
			game.drop();
		}
	}

#define AS_BUILDING(OBJ) reinterpret_cast<net::SBuildingGameobject*>(OBJ)
#define AS_UNIT(OBJ) reinterpret_cast<net::SUnitGameobject*>(OBJ)
#define AS_MAPOBJECT(OBJ) reinterpret_cast<net::SMapobjectGameobject*>(OBJ)
#define AS_MAPTILE(OBJ) reinterpret_cast<net::SMaptileGameobject*>(OBJ)
	bool DBMS::GetNetGameobjects(const std::string& gamename, std::vector< net::SGameobject*  >& backv)
	{
		std::string name = "game_" + gamename;
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
			
		try
		{
			if (db.has_collection(name))
			{
				mongocxx::collection game = db[name];

				// Get cursor to all Documents in Collection.
				auto cursor = game.find({});

				for (auto&& doc : cursor)
				{
					net::SGameobject* obj = nullptr;
					auto type = (net::EGameobjectType)doc["gameobjectType"].get_int64().value;
					switch (type)
					{
						case net::EGameobjectType::NET_GO_MAPTILE:
						{
							obj = new net::SMaptileGameobject();
							AS_MAPTILE(obj)->m_maptileType = doc["maptileType"].get_utf8().value.to_string().c_str();
							AS_MAPTILE(obj)->m_maptileBiome = doc["maptileBiome"].get_utf8().value.to_string().c_str();
							break;
						}
						case net::EGameobjectType::NET_GO_MAPOBJECT:
						{
							obj = new net::SMapobjectGameobject();
							AS_MAPOBJECT(obj)->m_maptileObjectType = doc["maptileObjectType"].get_utf8().value.to_string().c_str();
							AS_MAPOBJECT(obj)->m_maptileObjectBiome = doc["maptileObjectBiome"].get_utf8().value.to_string().c_str();
							break;
						}
						case net::EGameobjectType::NET_GO_UNIT:
						{
							obj = new net::SUnitGameobject();
							AS_UNIT(obj)->m_unitHealth = doc["unitHealth"].get_int64();
							AS_UNIT(obj)->m_unitArmor = doc["unitArmor"].get_int64();
							AS_UNIT(obj)->m_unitAttack = doc["unitAttack"].get_int64();
							AS_UNIT(obj)->m_unitDefense = doc["unitDefense"].get_int64();
							AS_UNIT(obj)->m_unitLevel = doc["unitLevel"].get_int64();
							AS_UNIT(obj)->m_unitExperience = doc["unitExperience"].get_int64();
							AS_UNIT(obj)->m_unitName = doc["unitName"].get_utf8().value.to_string().c_str();
							break;
						}
						case net::EGameobjectType::NET_GO_BUILDING:
						{
							obj = new net::SBuildingGameobject();
							AS_BUILDING(obj)->m_buildingHealth = doc["buildingHealth"].get_int64();
							AS_BUILDING(obj)->m_buildingLevel = doc["playerId"].get_int64();
							AS_BUILDING(obj)->m_buildingName = doc["unitName"].get_utf8().value.to_string().c_str();
							break;
						}
					}
					// Get basic data common to all.
					obj->m_name = "NetGameobject";
					obj->m_networkId = doc["networkId"].get_int64();
					obj->m_playerId = doc["playerId"].get_int64();
					obj->m_gameobjectType = (net::EGameobjectType)doc["gameobjectType"].get_int64().value;
					obj->m_positionX = doc["positionX"].get_double();
					obj->m_positionY = doc["positionY"].get_double();

					backv.push_back(obj);
				}

				return true;
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::GetNetGameobjects - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}

		return false;
	}



	bool DBMS::TryEmplaceNetGameobject(net::SGameobject* object, const std::string& gamename)
	{
		std::string name = "game_" + gamename;

		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection game = db[name];

		// Try finding the Gameobject.
		// If it exists, update it.
		bool found = false;
		auto cursor = game.find(
			make_document(kvp("networkId", (int64_t)object->m_networkId))
		);

		for (auto& it : cursor)
		{
			// Found the Gameobject.
			found = true;
			break;
		}

		switch (object->m_gameobjectType)
		{
			case net::EGameobjectType::NET_GO_MAPOBJECT:
			{
				if (found)
				{
					return TryEmplaceNetGameobjectMapobject(AS_MAPOBJECT(object), game, true);
				}
				else
				{
					return TryEmplaceNetGameobjectMapobject(AS_MAPOBJECT(object), game, false);
				}
			}
			case net::EGameobjectType::NET_GO_MAPTILE:
			{
				if (found)
				{
					return TryEmplaceNetGameobjectMaptile(AS_MAPTILE(object), game, true);
				}
				else
				{
					return TryEmplaceNetGameobjectMaptile(AS_MAPTILE(object), game, false);
				}
			}
			case net::EGameobjectType::NET_GO_BUILDING:
			{
				if (found)
				{
					return TryEmplaceNetGameobjectBuilding(AS_BUILDING(object), game, true);
				}
				else
				{
					return TryEmplaceNetGameobjectBuilding(AS_BUILDING(object), game, false);
				}
			}
			case net::EGameobjectType::NET_GO_UNIT:
			{
				if (found)
				{
					return TryEmplaceNetGameobjectUnit(AS_UNIT(object), game, true);
				}
				else
				{
					return TryEmplaceNetGameobjectUnit(AS_UNIT(object), game, false);
				}
			}
		}
	}

	bool DBMS::TryEmplaceNetGameobjectUnit(net::SUnitGameobject* object, mongocxx::collection& game, bool update)
	{
		try
		{
			// Create the Document.
			auto update_or_create_doc = make_document(
				kvp("networkId", (int64_t)object->m_networkId),
				kvp("name", object->m_name.C_String()),
				kvp("playerId", (int64_t)object->m_playerId),
				kvp("gameobjectType", (int64_t)object->m_gameobjectType),
				kvp("positionX", (double)object->m_positionX),
				kvp("positionY", (double)object->m_positionY),

				kvp("unitName", object->m_unitName.C_String()),
				kvp("unitHealth", (int64_t)object->m_unitHealth),
				kvp("unitArmor", (int64_t)object->m_unitArmor),
				kvp("unitAttack", (int64_t)object->m_unitAttack),
				kvp("unitDefense", (int64_t)object->m_unitDefense),
				kvp("unitLevel", (int64_t)object->m_unitLevel),
				kvp("unitExperience", (int64_t)object->m_unitExperience)
			);


			if (update)
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = game.update_one(
					make_document(kvp("networkId", (int64_t)object->m_networkId)),
					make_document(kvp("$set", update_or_create_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
			}
			else
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = game.insert_one(
					std::move(update_or_create_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::TryEmplaceNetGameobjectUnit - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}
	}
	bool DBMS::TryEmplaceNetGameobjectBuilding(net::SBuildingGameobject* object, mongocxx::collection& game, bool update)
	{
		try
		{
			// Create the Document.
			auto update_or_create_doc = make_document(
				kvp("networkId", (int64_t)object->m_networkId),
				kvp("name", object->m_name.C_String()),
				kvp("playerId", (int64_t)object->m_playerId),
				kvp("gameobjectType", (int64_t)object->m_gameobjectType),
				kvp("positionX", (double)object->m_positionX),
				kvp("positionY", (double)object->m_positionY),

				kvp("buildingName", object->m_buildingName.C_String()),
				kvp("buildingHealth", (int64_t)object->m_buildingHealth),
				kvp("buildingLevel", (int64_t)object->m_buildingLevel)
			);

			if (update)
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = game.update_one(
					make_document(kvp("networkId", (int64_t)object->m_networkId)),
					make_document(kvp("$set", update_or_create_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
			}
			else
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = game.insert_one(
					std::move(update_or_create_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::TryEmplaceNetGameobjectBuilding - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}
	}
	bool DBMS::TryEmplaceNetGameobjectMapobject(net::SMapobjectGameobject* object, mongocxx::collection& game, bool update)
	{
		try
		{
			// Create the Document.
			auto update_or_create_doc = make_document(
				kvp("networkId", (int64_t)object->m_networkId),
				kvp("name", object->m_name.C_String()),
				kvp("playerId", (int64_t)object->m_playerId),
				kvp("gameobjectType", (int64_t)object->m_gameobjectType),
				kvp("positionX", (double)object->m_positionX),
				kvp("positionY", (double)object->m_positionY),

				kvp("maptileObjectType", object->m_maptileObjectType.C_String()),
				kvp("maptileObjectBiome", object->m_maptileObjectBiome.C_String())
			);

			if (update)
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = game.update_one(
					make_document(kvp("networkId", (int64_t)object->m_networkId)),
					make_document(kvp("$set", update_or_create_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
			}
			else
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = game.insert_one(
					std::move(update_or_create_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::TryEmplaceNetGameobjectMapobject - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}
	}
	bool DBMS::TryEmplaceNetGameobjectMaptile(net::SMaptileGameobject* object, mongocxx::collection& game, bool update)
	{
		try
		{
			// Create the Document.
			auto update_or_create_doc = make_document(
				kvp("networkId", (int64_t)object->m_networkId),
				kvp("name", object->m_name.C_String()),
				kvp("playerId", (int64_t)object->m_playerId),
				kvp("gameobjectType", (int64_t)object->m_gameobjectType),
				kvp("positionX", (double)object->m_positionX),
				kvp("positionY", (double)object->m_positionY),

				kvp("maptileType", object->m_maptileType.C_String()),
				kvp("maptileBiome", object->m_maptileBiome.C_String())
			);

			if (update)
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = game.update_one(
					make_document(kvp("networkId", (int64_t)object->m_networkId)),
					make_document(kvp("$set", update_or_create_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
			}
			else
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = game.insert_one(
					std::move(update_or_create_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			printf("DBMS::TryEmplaceNetGameobjectMaptile - mongocxx::v_noabi::logic_error - Message: \n\t\t \"%s\"\n", e.what());
		}
	}

#undef AS_BUILDING
#undef AS_UNIT
#undef AS_MAPOBJECT
#undef AS_MAPTILE

	void DBMS::Terminate() noexcept
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