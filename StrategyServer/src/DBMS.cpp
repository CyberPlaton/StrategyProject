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
	const std::string DBMS::m_abilitiesCollection = "test_collection_abilities";
	const std::string DBMS::m_statusEffectsCollection = "test_collection_status_effects";

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
			LOG_DBG_INFO("[{:.4f}][DBMS::DeleteUser] mongocxx::v_noabi::logic_error: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), id);
			LOG_FILE_INFO("[{:.4f}][DBMS::DeleteUser] mongocxx::v_noabi::logic_error: \"{}\"! Searched for kvp(\"_id\", \"{}\").", APP_RUN_TIME, e.what(), id);
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
																   kvp("SteamName", desc.m_steamName.C_String()),
																   kvp("NintendoId", (int64_t)desc.m_nintendoId),
																   kvp("NintendoName", desc.m_nintendoName.C_String()),
																   kvp("XboxLiveId", (int64_t)desc.m_xboxLiveId),
																   kvp("XboxLiveName", desc.m_xboxLiveName.C_String()),
																   kvp("PSNId", (int64_t)desc.m_psnId),
																   kvp("PSNName", desc.m_psnName.C_String()),
																   kvp("Currency", (int64_t)desc.m_currencyAmount),
																   kvp("Version", (int64_t)desc.m_version),
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
			LOG_DBG_ERROR("[{:.4f}][DBMS::UpdateUser] mongocxx::v_noabi::logic_error: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), desc.m_uuid);
			LOG_FILE_ERROR("[{:.4f}][DBMS::UpdateUser] mongocxx::v_noabi::logic_error: \"{}\"! Searched for kvp(\"_id\", \"{}\").", APP_RUN_TIME, e.what(), desc.m_uuid);
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
				break;
			}
			case net::EClientPlatform::UP_SWITCH:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "NintendoId", (int64_t)desc.m_nintendoId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "NintendoName", desc.m_nintendoName.C_String());
				}
				break;
			}
			case net::EClientPlatform::UP_XBOX:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "XboxLiveId", (int64_t)desc.m_xboxLiveId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "XboxLiveName", desc.m_xboxLiveName.C_String());
				}
				break;
			}
			case net::EClientPlatform::UP_PS:
			{
				if (!_findOneByKeyValuePair(user_data, found_doc, "PSNId", (int64_t)desc.m_psnId))
				{
					_findOneByKeyValuePair(user_data, found_doc, "PSNName", desc.m_psnName.C_String());
				}
				break;
			}


			default:
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::GetUserDesc] Undefined User Platform! Searched for kvp(\"_id\", \"{}\"), Platform \"{}\".", Logger::AppRunningTime(), desc.m_uuid, desc.m_platform);
				LOG_FILE_ERROR("[{:.4f}][DBMS::GetUserDesc] Undefined User Platform! Searched for kvp(\"_id\", \"{}\"), Platform \"{}\".", APP_RUN_TIME, desc.m_uuid, desc.m_platform);
				
				return false;
			}
			}
		}


		// If we could not find User by any ids, he does not exist!		
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
						LOG_DBG_ERROR("[{:.4f}][DBMS::GetUserDesc - Achievements] bsoncxx::v_noabi::exception: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), desc.m_uuid);
						LOG_FILE_ERROR("[{:.4f}][DBMS::GetUserDesc - Achievements] bsoncxx::v_noabi::exception: \"{}\"! Searched for kvp(\"_id\", \"{}\").", APP_RUN_TIME, e.what(), desc.m_uuid);
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
						LOG_DBG_ERROR("[{:.4f}][DBMS::GetUserDesc - ServiceItems] bsoncxx::v_noabi::exception: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), desc.m_uuid);
						LOG_FILE_ERROR("[{:.4f}][DBMS::GetUserDesc - ServiceItems] bsoncxx::v_noabi::exception: \"{}\"! Searched for kvp(\"_id\", \"{}\").", APP_RUN_TIME, e.what(), desc.m_uuid);
					}
				}
			}
		}

		return true;
	}


	bool DBMS::GetAbility(const std::string& ability_name, net::SAbilityData* ability)
	{
		if (!m_initialized || !ability) return false;


		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection abilityCollection = db[m_abilitiesCollection];

		boost::optional<bsoncxx::v_noabi::document::value> doc;

		// Find Ability.
		if (_findOneByKeyValuePair(abilityCollection, doc, "Name", ability_name.c_str()))
		{
			auto view = doc->view();

			// Retrieve data.
			std::string name = view["Name"].get_utf8().value.to_string().c_str();
			std::string display_name = view["DisplayName"].get_utf8().value.to_string().c_str();
			std::string desc = view["Desc"].get_utf8().value.to_string().c_str();
			bool useable_on_enemy = view["UsableOnEnemy"].get_bool();
			bool useable_on_self = view["UsableOnSelf"].get_bool();
			bool useable_on_friendly = view["UsableOnFriendly"].get_bool();
			size_t applicable_to = view["ApplicableTo"].get_int64();
			std::vector< std::string > applied_status_effects;

			auto array_of_status_effects = view["AppliedStatusEffectsOnUse"].get_array().value;

			if (!array_of_status_effects.empty())
			{
				
				for (const bsoncxx::array::element& subdocument : array_of_status_effects) 
				{
					std::string elem = subdocument.get_utf8().value.to_string().c_str();
					applied_status_effects.push_back(elem);
				}
			}

			// Fill SAbilityData.
			ability->m_abilityName = name.c_str();
			ability->m_abilityDisplayName = display_name.c_str();
			ability->m_abilityDesc = desc.c_str();
			ability->m_abilityUsableOnEnemies = useable_on_enemy;
			ability->m_abilityUsableOnSelf = useable_on_self;
			ability->m_abilityUsableOnFriendlies = useable_on_friendly;
			ability->m_abilityApplicableTo = net::EGameobjectType(applicable_to);
			for(auto& e: applied_status_effects)
			{
				ability->m_appliedStatusEffectsOnUse.push_back(e.c_str());
			}


			return true;
		}


		LOG_DBG_ERROR("[{:.4f}][DBMS::GetAbility] Could not retrieve net::SAbilityData by name \"{}\"!", Logger::AppRunningTime(), ability_name);
		LOG_FILE_ERROR("[{:.4f}][DBMS::GetAbility] Could not retrieve net::SAbilityData by name \"{}\"!", APP_RUN_TIME, ability_name);
		return false;
	}

	bool DBMS::GetAllStatusEffectData(net::SStatusEffectDataStorageObject& storage)
	{
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];

		try
		{
			if (db.has_collection(m_statusEffectsCollection))
			{
				mongocxx::collection effects_collection = db[m_statusEffectsCollection];

				// Get cursor to all Documents in Collection.
				auto cursor = effects_collection.find({});

				for (auto&& view : cursor)
				{
					net::SStatusEffectData effect;

					// Retrieve data.
					std::string name = view["Name"].get_utf8().value.to_string().c_str();
					std::string display_name = view["DisplayName"].get_utf8().value.to_string().c_str();
					std::string desc = view["Desc"].get_utf8().value.to_string().c_str();
					std::string effect_type = view["Type"].get_utf8().value.to_string().c_str();
					size_t appl_to = view["ApplicableTo"].get_int64();
					std::string timer_type = view["TimerType"].get_utf8().value.to_string().c_str();
					size_t apl_prob = view["ApplicationProbability"].get_int64();
					size_t val_max = view["ValueMax"].get_int64();
					size_t val_min = view["ValueMin"].get_int64();
					size_t timer_val = view["TimerValue"].get_int64();


					// Fill SStatusEffectData.
					effect.m_effectName = name.c_str();
					effect.m_effectDisplayName = display_name.c_str();
					effect.m_effectDesc = desc.c_str();
					effect.m_effectType = effect_type.c_str();
					effect.m_effectTimerType = effect_type.c_str();
					effect.m_effectTimerValue = timer_val;
					effect.m_effectApplicableTo = net::EGameobjectType(appl_to);
					effect.m_effectApplicationProbability = apl_prob;
					effect.m_effectValueMax = val_max;
					effect.m_effectValueMax = val_min;

					storage.m_data.push_back(std::move(effect));
				}

				return true;
			}
		}
		catch(const mongocxx::v_noabi::logic_error& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::logic_error: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::logic_error: \"{}\"!", APP_RUN_TIME, e.what());
		}
		catch(const bsoncxx::v_noabi::exception& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::exception: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::exception: \"{}\"!", APP_RUN_TIME, e.what());
		}

		return false;
	}

	bool DBMS::GetAllAbilityData(net::SAbilityDataStorageObject& storage)
	{
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];

		try
		{
			if (db.has_collection(m_statusEffectsCollection))
			{
				mongocxx::collection effects_collection = db[m_statusEffectsCollection];

				// Get cursor to all Documents in Collection.
				auto cursor = effects_collection.find({});

				for (auto&& view : cursor)
				{
					net::SAbilityData ability;

					// Retrieve data.
					std::string name = view["Name"].get_utf8().value.to_string().c_str();
					std::string display_name = view["DisplayName"].get_utf8().value.to_string().c_str();
					std::string desc = view["Desc"].get_utf8().value.to_string().c_str();
					bool useable_on_enemy = view["UsableOnEnemy"].get_bool();
					bool useable_on_self = view["UsableOnSelf"].get_bool();
					bool useable_on_friendly = view["UsableOnFriendly"].get_bool();
					size_t applicable_to = view["ApplicableTo"].get_int64();
					std::vector< std::string > applied_status_effects;

					auto array_of_status_effects = view["AppliedStatusEffectsOnUse"].get_array().value;

					if (!array_of_status_effects.empty())
					{

						for (const bsoncxx::array::element& subdocument : array_of_status_effects)
						{
							std::string elem = subdocument.get_utf8().value.to_string().c_str();
							applied_status_effects.push_back(elem);
						}
					}

					// Fill SAbilityData.
					ability.m_abilityName = name.c_str();
					ability.m_abilityDisplayName = display_name.c_str();
					ability.m_abilityDesc = desc.c_str();
					ability.m_abilityUsableOnEnemies = useable_on_enemy;
					ability.m_abilityUsableOnSelf = useable_on_self;
					ability.m_abilityUsableOnFriendlies = useable_on_friendly;
					ability.m_abilityApplicableTo = net::EGameobjectType(applicable_to);
					for (auto& e : applied_status_effects)
					{
						ability.m_appliedStatusEffectsOnUse.push_back(e.c_str());
					}

					storage.m_data.push_back(std::move(ability));
				}
				return true;
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::logic_error: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::logic_error: \"{}\"!", APP_RUN_TIME, e.what());
		}
		catch (const bsoncxx::v_noabi::exception& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::exception: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::exception: \"{}\"!", APP_RUN_TIME, e.what());
		}

		return false;
	}

	bool DBMS::GetStatusEffect(const std::string& effect_name, net::SStatusEffectData* effect)
	{
		if (!m_initialized || !effect) return false;


		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection effectCollection = db[m_statusEffectsCollection];

		boost::optional<bsoncxx::v_noabi::document::value> doc;

		// Find Ability.
		if (_findOneByKeyValuePair(effectCollection, doc, "Name", effect_name.c_str()))
		{
			auto view = doc->view();

			// Retrieve data.
			std::string name = view["Name"].get_utf8().value.to_string().c_str();
			std::string display_name = view["DisplayName"].get_utf8().value.to_string().c_str();
			std::string desc = view["Desc"].get_utf8().value.to_string().c_str();
			std::string effect_type = view["Type"].get_utf8().value.to_string().c_str();
			size_t appl_to = view["ApplicableTo"].get_int64();
			std::string timer_type = view["TimerType"].get_utf8().value.to_string().c_str();
			size_t apl_prob = view["ApplicationProbability"].get_int64();
			size_t val_max = view["ValueMax"].get_int64();
			size_t val_min = view["ValueMin"].get_int64();
			size_t timer_val = view["TimerValue"].get_int64();


			// Fill SStatusEffectData.
			effect->m_effectName = name.c_str();
			effect->m_effectDisplayName = display_name.c_str();
			effect->m_effectDesc = desc.c_str();
			effect->m_effectType = effect_type.c_str();
			effect->m_effectTimerType = effect_type.c_str();
			effect->m_effectTimerValue = timer_val;
			effect->m_effectApplicableTo = net::EGameobjectType(appl_to);
			effect->m_effectApplicationProbability = apl_prob;
			effect->m_effectValueMax = val_max;
			effect->m_effectValueMax = val_min;
			
			return true;
		}

		LOG_DBG_ERROR("[{:.4f}][DBMS::GetStatusEffect] Could not retrieve net::SStatusEffectData by name \"{}\"!", Logger::AppRunningTime(), effect_name);
		LOG_FILE_ERROR("[{:.4f}][DBMS::GetStatusEffect] Could not retrieve net::SStatusEffectData by name \"{}\"!", APP_RUN_TIME, effect_name);
		return false;
	}


	uint32_t DBMS::CreateUser()
	{
		if (!m_initialized) return -1;

		// Get Database and relevant collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection user_data = db[m_userDataCollection];

		// Get the next uuid for a user.
		int64_t uuid = m_nextUUID++;

		// Create a Document/UserEntry with default values.
		auto doc = make_document(
			kvp("_id",					uuid),
			kvp("ClientPlatform",		0),
			kvp("SteamId",				0),
			kvp("SteamName",			""),
			kvp("NintendoId",			0),
			kvp("NintendoName",			""),
			kvp("XboxLiveId",			0),
			kvp("XboxLiveName",			""),
			kvp("PSNId",				0),
			kvp("PSNName",				""),
			kvp("Currency",				0),
			kvp("Version",				0),
			kvp("Achievements",			make_array()),
			kvp("ServiceItems",			make_array())
		);

		try
		{
			bsoncxx::stdx::optional<mongocxx::result::insert_one> result = user_data.insert_one(std::move(doc));

			if (result->result().inserted_count() > 0)
			{

				LOG_DBG_INFO("[{:.4f}][DBMS::CreateUser] Created new user with id \"{}\"!", Logger::AppRunningTime(), uuid);
				LOG_FILE_INFO("[{:.4f}][DBMS::CreateUser] Created new user with id \"{}\"!", APP_RUN_TIME, uuid);
				
				m_nextUUID += 1;
				return uuid;
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::CreateUser] mongocxx::v_noabi::logic_error: \"{}\"! Failed to create user.", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::CreateUser] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to create user.", APP_RUN_TIME, e.what());
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

			m_initialized = true;
			return true;
		}
		catch (const std::exception& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::Initialize] std::exception: \"{}\"! Failed to connect to Database \"{}\".", Logger::AppRunningTime(), e.what(), m_database);
			LOG_FILE_ERROR("[{:.4f}][DBMS::Initialize] std::exception: \"{}\"!  Failed to connect to Database \"{}\".", APP_RUN_TIME, e.what(), m_database);

			m_initialized = false;
		}


		return false;
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
		
			LOG_DBG_INFO("[{:.4f}][DBMS::CreateGame] Create game \"{}\"!", Logger::AppRunningTime(), name.c_str());
			LOG_FILE_INFO("[{:.4f}][DBMS::CreateGame] Create game \"{}\"!", APP_RUN_TIME, name.c_str());
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

			LOG_DBG_INFO("[{:.4f}][DBMS::DeleteGame] Delete game \"{}\"!", Logger::AppRunningTime(), name.c_str());
			LOG_FILE_INFO("[{:.4f}][DBMS::DeleteGame] Delete game \"{}\"!", APP_RUN_TIME, name.c_str());
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
							AS_UNIT(obj)->m_unitSightRadius = doc["unitSightRadius"].get_int64();
							AS_UNIT(obj)->m_unitMovementType = doc["unitMovementType"].get_int64();
							AS_UNIT(obj)->m_unitMovementPoints = doc["unitMovementPoints"].get_int64();
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
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetNetGameobjects] mongocxx::v_noabi::logic_error: \"{}\"! Failed to retrieve Net Gameobjects for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), gamename);
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetNetGameobjects] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to retrieve Net Gameobjects for Game \"game_{}\".", APP_RUN_TIME, e.what(), gamename);
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
				kvp("unitExperience", (int64_t)object->m_unitExperience),
				kvp("unitSightRadius", (int64_t)object->m_unitSightRadius),
				kvp("unitMovementType", (int64_t)object->m_unitMovementType),
				kvp("unitMovementPoints", (int64_t)object->m_unitMovementPoints)
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
			LOG_DBG_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectUnit] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectUnit] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", APP_RUN_TIME, e.what(), object->m_name, game.name().to_string());
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
			LOG_DBG_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectBuilding] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectBuilding] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", APP_RUN_TIME, e.what(), object->m_name, game.name().to_string());
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
			LOG_DBG_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectMapobject] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectMapobject] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", APP_RUN_TIME, e.what(), object->m_name, game.name().to_string());
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
			LOG_DBG_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectMaptile] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectMaptile] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", APP_RUN_TIME, e.what(), object->m_name, game.name().to_string());
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