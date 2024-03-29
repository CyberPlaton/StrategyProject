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
			LOG_FILE_INFO("[{:.4f}][DBMS::DeleteUser] mongocxx::v_noabi::logic_error: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), id);
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::UpdateUser] mongocxx::v_noabi::logic_error: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), desc.m_uuid);
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
				LOG_FILE_ERROR("[{:.4f}][DBMS::GetUserDesc] Undefined User Platform! Searched for kvp(\"_id\", \"{}\"), Platform \"{}\".", Logger::AppRunningTime(), desc.m_uuid, desc.m_platform);
				
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
						LOG_FILE_ERROR("[{:.4f}][DBMS::GetUserDesc - Achievements] bsoncxx::v_noabi::exception: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), desc.m_uuid);
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
						LOG_FILE_ERROR("[{:.4f}][DBMS::GetUserDesc - ServiceItems] bsoncxx::v_noabi::exception: \"{}\"! Searched for kvp(\"_id\", \"{}\").", Logger::AppRunningTime(), e.what(), desc.m_uuid);
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
		LOG_FILE_ERROR("[{:.4f}][DBMS::GetAbility] Could not retrieve net::SAbilityData by name \"{}\"!", Logger::AppRunningTime(), ability_name);
		return false;
	}

	bool DBMS::GetAllStatusEffectData(std::vector< net::SStatusEffectData >& storage)
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
					effect.m_effectTimerType = timer_type.c_str();
					effect.m_effectTimerValue = timer_val;
					effect.m_effectApplicableTo = net::EGameobjectType(appl_to);
					effect.m_effectApplicationProbability = apl_prob;
					effect.m_effectValueMax = val_max;
					effect.m_effectValueMin = val_min;

					storage.push_back(std::move(effect));
				}

				return true;
			}
		}
		catch(const mongocxx::v_noabi::logic_error& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::logic_error: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::logic_error: \"{}\"!", Logger::AppRunningTime(), e.what());
		}
		catch(const bsoncxx::v_noabi::exception& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::exception: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllStatusEffectData] bsoncxx::v_noabi::exception: \"{}\"!", Logger::AppRunningTime(), e.what());
		}

		return false;
	}

	bool DBMS::GetAllAbilityData(std::vector< net::SAbilityData >& storage)
	{
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];

		try
		{
			if (db.has_collection(m_statusEffectsCollection))
			{
				mongocxx::collection effects_collection = db[m_abilitiesCollection];

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

					storage.push_back(std::move(ability));
				}
				return true;
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::logic_error: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::logic_error: \"{}\"!", Logger::AppRunningTime(), e.what());
		}
		catch (const bsoncxx::v_noabi::exception& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::exception: \"{}\"!", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetAllAbilityData] bsoncxx::v_noabi::exception: \"{}\"!", Logger::AppRunningTime(), e.what());
		}

		return false;
	}

	bool DBMS::LoadStatusEffectDefinitions(const std::string& status_effects_filepath, const std::string& filepath, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Load cache XML.
		tinyxml2::XMLDocument cache;
		if (cache.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::LoadStatusEffectDefinitions] Could not load Status Effect Cache: \"{}\"!", Logger::AppRunningTime(), filepath);
			LOG_FILE_ERROR("[{:.4f}][DBMS::LoadStatusEffectDefinitions] Could not load Status Effect Cache: \"{}\"!", Logger::AppRunningTime(), filepath);
			cache.Clear();
			return false;
		}


		auto cache_root = cache.RootElement();
		auto se = cache_root->FirstChildElement("StatusEffect");
		while(se)
		{
			std::string path = se->Attribute("path");
			std::string final_path = status_effects_filepath + path + ".xml";

			// Load Status Effect from XML.
			tinyxml2::XMLDocument seXML;
			if (seXML.LoadFile(final_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::LoadStatusEffectDefinitions] Could not load Status Effect: \"{}\"!", Logger::AppRunningTime(), final_path);
				LOG_FILE_ERROR("[{:.4f}][DBMS::LoadStatusEffectDefinitions] Could not load Status Effect: \"{}\"!", Logger::AppRunningTime(), final_path);
				seXML.Clear();
				return false;
			}

			auto se_root = seXML.RootElement();
			auto seData = se_root->FirstChildElement("Data");

			net::SStatusEffectData data;
			data.m_effectName = seData->Attribute("name");
			data.m_effectDisplayName = seData->Attribute("displayName");
			data.m_effectDesc = seData->Attribute("description");
			data.m_effectBehaviorTreeImpl = seData->Attribute("behaviorTreeImplName");
			data.m_effectSprite = seData->Attribute("sprite");
			data.m_effectTimerType = seData->Attribute("timerType");

			data.m_effectTimerValue = seData->Int64Attribute("timerValue");
			data.m_effectApplicableTo = (net::EGameobjectType)seData->Int64Attribute("applicableTo");
			data.m_effectApplicationProbability = seData->Int64Attribute("applicationProbability");
			data.m_effectValueMin = seData->Int64Attribute("minValue");
			data.m_effectValueMax = seData->Int64Attribute("maxValue");



			// Store Status Effect in DB.
			if(!StoreStatusEffect(data, collection))
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::LoadStatusEffectDefinitions] Could not upload Status Effect: \"{}\"!", Logger::AppRunningTime(), data.m_effectName);
				LOG_FILE_ERROR("[{:.4f}][DBMS::LoadStatusEffectDefinitions] Could not upload Status Effect: \"{}\"!", Logger::AppRunningTime(), data.m_effectName);

				seXML.Clear();
				cache.Clear();
				return false;
			}

			se = se->NextSiblingElement("StatusEffect");
		}

		return true;
	}

	bool DBMS::StoreStatusEffect(net::SStatusEffectData& data, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Get database and collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection seCollection = db[collection];

		boost::optional<bsoncxx::v_noabi::document::value> doc;

		// Check whether the Prefab Already exists.
		const bool prefab_exists = _findOneByKeyValuePair(seCollection, doc, "name", data.m_effectName.C_String());

		// Create the MongoDB Document,
		auto update_or_insert_doc = make_document(
			kvp("effectName",					data.m_effectName.C_String()),
			kvp("effectDisplayName",			data.m_effectDisplayName.C_String()),

			kvp("effectDesc",					data.m_effectDesc.C_String()),
			kvp("effectSprite",					data.m_effectSprite.C_String()),
			kvp("effectBehaviorTreeImplName",	data.m_effectBehaviorTreeImpl.C_String()),

			kvp("effectType",					data.m_effectType.C_String()),
			kvp("effectValueMin",				(int64_t)data.m_effectValueMin),
			kvp("effectValueMax",				(int64_t)data.m_effectValueMax),
			kvp("effectApplicationProbability", (int64_t)data.m_effectApplicationProbability),
			kvp("effectApplicableTo",			(int64_t)data.m_effectApplicableTo),
			kvp("effectTimerType",				data.m_effectTimerType.C_String()),
			kvp("effectTimerValue",				(int64_t)data.m_effectTimerValue)
		);


		// If it exists, update it.
		if (prefab_exists)
		{
			// Doc safe to view as he exists.
			auto view = doc->view();

			try
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = seCollection.update_one(
					make_document(kvp("name", view["name"].get_utf8().value.to_string().c_str())),
					make_document(kvp("$set", update_or_insert_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreStatusEffect] Failed to update Status Effect \"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_effectName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreStatusEffect] Failed to update Status Effect\"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_effectName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreStatusEffect] mongocxx::v_noabi::logic_error: \"{}\"! Failed to update Status Effect\"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreStatusEffect] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to update Status Effect \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreStatusEffect] bsoncxx::v_noabi::exception: \"{}\"! Failed to update Status Effect \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreStatusEffect] bsoncxx::v_noabi::exception: \"{}\"!  Failed to update Status Effect \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				return false;
			}

		}
		// If it does not exist, create new entry.
		else
		{
			try
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = seCollection.insert_one(
					std::move(update_or_insert_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to upsert Status Effect \"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_effectName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to upsert Status Effect\"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_effectName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"! Failed to upsert Status Effect\"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"! Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"!  Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_effectName, collection.c_str());
				return false;
			}
		}


		return false;
	}

	bool DBMS::LoadAbilityDefinitions(const std::string& abilities_path, const std::string& filepath, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Load cache XML.
		tinyxml2::XMLDocument cache;
		if (cache.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::LoadAbilityDefinitions] Could not load Ability Cache: \"{}\"!", Logger::AppRunningTime(), filepath);
			LOG_FILE_ERROR("[{:.4f}][DBMS::LoadAbilityDefinitions] Could not load Ability Cache: \"{}\"!", Logger::AppRunningTime(), filepath);
			cache.Clear();
			return false;
		}


		auto cache_root = cache.RootElement();
		auto abl = cache_root->FirstChildElement("Ability");
		while (abl)
		{
			std::string path = abl->Attribute("path");
			std::string final_path = abilities_path + path + ".xml";

			// Load Status Effect from XML.
			tinyxml2::XMLDocument ablXml;
			if (ablXml.LoadFile(final_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::LoadAbilityDefinitions] Could not load Ability: \"{}\"!", Logger::AppRunningTime(), final_path);
				LOG_FILE_ERROR("[{:.4f}][DBMS::LoadAbilityDefinitions] Could not load Ability: \"{}\"!", Logger::AppRunningTime(), final_path);
				ablXml.Clear();
				return false;
			}

			auto abl_root = ablXml.RootElement();
			auto ablData = abl_root->FirstChildElement("Data");

			net::SAbilityData data;

			// Load ablility data.
			data.m_abilityName = ablData->Attribute("name");
			data.m_abilityDisplayName = ablData->Attribute("displayName");
			data.m_abilityDesc = ablData->Attribute("description");
			data.m_abilityApplicableTo = ablData->Int64Attribute("applicableTo");
			data.m_abilityUsableOnSelf = ablData->BoolAttribute("usableOnSelf");
			data.m_abilityUsableOnFriendlies = ablData->BoolAttribute("usableOnFriendlies");
			data.m_abilityUsableOnEnemies = ablData->BoolAttribute("usableOnEnemies");

			// TODO in ability editor.
			data.m_abilityActionPointCost = 0;
			data.m_abilityMaxRange = 0;
			data.m_abilityMinRange = 0;

			auto applied_status_effects = ablData->FirstChildElement("AppliedStatusEffectsOnUse");
			auto effect = applied_status_effects->FirstChildElement("StatusEffect");
			while(effect)
			{
				data.m_appliedStatusEffectsOnUse.push_back(effect->Attribute("name"));
				effect = effect->NextSiblingElement("StatusEffect");
			}


			// Store Abiliy in DB.
			if (!StoreAbility(data, collection))
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::LoadAbilityDefinitions] Could not upload Ability: \"{}\"!", Logger::AppRunningTime(), data.m_abilityName);
				LOG_FILE_ERROR("[{:.4f}][DBMS::LoadAbilityDefinitions] Could not upload Ability: \"{}\"!", Logger::AppRunningTime(), data.m_abilityName);

				ablXml.Clear();
				cache.Clear();
				return false;
			}

			abl = abl->NextSiblingElement("Ability");
		}

		return true;
	}

	bool DBMS::StoreAbility(net::SAbilityData& data, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Get database and collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection ablCollection = db[collection];

		boost::optional<bsoncxx::v_noabi::document::value> doc;

		// Check whether the Prefab Already exists.
		const bool prefab_exists = _findOneByKeyValuePair(ablCollection, doc, "name", data.m_abilityName.C_String());

		// Create the MongoDB Document.
		auto applied_status_effects = bsoncxx::builder::basic::array{};
		for (auto e : data.m_appliedStatusEffectsOnUse)
		{
			applied_status_effects.append(e);
		}
		auto update_or_insert_doc = make_document(
			kvp("abilityName",						data.m_abilityName.C_String()),
			kvp("abilityDisplayName",				data.m_abilityDisplayName.C_String()),
			kvp("abilityDesc",						data.m_abilityDesc.C_String()),

			kvp("abilityActionPointCost",			data.m_abilityActionPointCost),
			kvp("abilityMaxRange",					data.m_abilityMaxRange),
			kvp("abilityMinRange",					data.m_abilityMinRange),

			kvp("abilityApplicableTo",				data.m_abilityApplicableTo),
			kvp("abilityUsableOnSelf",				data.m_abilityUsableOnSelf),
			kvp("abilityUsableOnFriendlies",		data.m_abilityUsableOnFriendlies),
			kvp("abilityUsableOnEnemies",			data.m_abilityUsableOnEnemies),

			kvp("appliedStatusEffectsOnUse",		applied_status_effects)
		);


		// If it exists, update it.
		if (prefab_exists)
		{
			// Doc safe to view as he exists.
			auto view = doc->view();

			try
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = ablCollection.update_one(
					make_document(kvp("name", view["name"].get_utf8().value.to_string().c_str())),
					make_document(kvp("$set", update_or_insert_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreStatusEffect] Failed to update Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_abilityName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreStatusEffect] Failed to update Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_abilityName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreStatusEffect] mongocxx::v_noabi::logic_error: \"{}\"! Failed to update Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreStatusEffect] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to update Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreStatusEffect] bsoncxx::v_noabi::exception: \"{}\"! Failed to update Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreStatusEffect] bsoncxx::v_noabi::exception: \"{}\"!  Failed to update Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				return false;
			}

		}
		// If it does not exist, create new entry.
		else
		{
			try
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = ablCollection.insert_one(
					std::move(update_or_insert_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to upsert Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_abilityName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to upsert Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), data.m_abilityName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"! Failed to upsert Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to upsert Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"! Failed to upsert Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"!  Failed to upsert Ability \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), data.m_abilityName, collection.c_str());
				return false;
			}
		}


		return false;
	}

	bool DBMS::LoadUnitAndBuildingPrefabDefinitions(const std::string& prefab_file_path, const std::string& filepath, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Load cache XML.
		tinyxml2::XMLDocument cache;
		if (cache.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not load Unit Prefab Cache: \"{}\"!", Logger::AppRunningTime(), filepath);
			LOG_FILE_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not load Unit Prefab Cache: \"{}\"!", Logger::AppRunningTime(), filepath);
			cache.Clear();
			return false;
		}


		auto cache_root = cache.RootElement();
		auto prefab = cache_root->FirstChildElement("Prefab");
		while (prefab)
		{
			std::string path = prefab->Attribute("path");
			std::string final_path = prefab_file_path + path + ".xml";

			// Load Status Effect from XML.
			tinyxml2::XMLDocument prefabXml;
			if (prefabXml.LoadFile(final_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not load Unit Prefab: \"{}\"!", Logger::AppRunningTime(), final_path);
				LOG_FILE_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not load Unit Prefab: \"{}\"!", Logger::AppRunningTime(), final_path);
				prefabXml.Clear();
				return false;
			}

			auto prefab_root = prefabXml.RootElement();
			auto prefabData = prefab_root->FirstChildElement("Data");

			const bool unit_prefab = prefabData->BoolAttribute("unit_prefab");
			if(unit_prefab)
			{
				// Load unit prefab data.
				net::SUnitGameobject data;

				data.m_unitName = prefabData->Attribute("name");
				data.m_unitSprite = prefabData->Attribute("sprite");
				data.m_unitLayoutTemplate = prefabData->Attribute("layout_template");
				data.m_unitGoldCost = prefab->Int64Attribute("gold_cost");

				data.m_unitHealth = prefabData->Int64Attribute("health");
				data.m_unitArmor = prefabData->Int64Attribute("armor");
				data.m_unitDefense = prefabData->Int64Attribute("defense");
				data.m_unitLevel = prefabData->Int64Attribute("level");
				data.m_unitExperience = prefabData->Int64Attribute("experience");
				data.m_unitSightRadius = prefabData->Int64Attribute("sightRadius");
				data.m_unitMovementType = prefabData->Int64Attribute("movement_type");
				data.m_unitActionPoints = prefabData->Int64Attribute("action_points");
				data.m_unitMinAttack = prefabData->Int64Attribute("attack_min");
				data.m_unitMaxAttack = prefabData->Int64Attribute("attack_max");
				
				const bool unit_ranged = prefabData->BoolAttribute("ranged_unit");
				data.m_unitRanged = unit_ranged;
				if(unit_ranged)
				{
					data.m_unitRangedMinAttack = prefabData->Int64Attribute("ranged_attack_min");
					data.m_unitRangedMaxAttack = prefabData->Int64Attribute("ranged_attack_max");
					data.m_unitRangedMinRange = prefabData->Int64Attribute("ranged_range_min");
					data.m_unitRangedMaxRange = prefabData->Int64Attribute("ranged_range_max");
				}


				// Load available abilities and starting status effects.
				auto starting_statusXml = prefabData->FirstChildElement("StartingStatus");
				auto abilitiesXml = prefabData->FirstChildElement("Abilities");

				// SE
				auto status = starting_statusXml->FirstChildElement("Status");
				while(status)
				{
					data.m_unitStartingStatusEffects.push_back(status->Attribute("name"));
					status = status->NextSiblingElement("Status");
				}

				// ABL
				auto abl = abilitiesXml->FirstChildElement("Ability");
				while(abl)
				{
					data.m_unitAbilities.push_back(abl->Attribute("name"));
					abl = abl->NextSiblingElement("Ability");
				}

				// Store Status Effect in DB.
				if (!StoreUnitPrefab(data, collection))
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not upload Unit Prefab: \"{}\"!", Logger::AppRunningTime(), data.m_name);
					LOG_FILE_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not upload Unit Prefab: \"{}\"!", Logger::AppRunningTime(), data.m_name);

					prefabXml.Clear();
					cache.Clear();
					return false;
				}
			}
			else
			{
				// Load building prefab data.
				net::SBuildingGameobject data;

				data.m_buildingName = prefabData->Attribute("name");
				data.m_buildingSprite = prefabData->Attribute("sprite");
				data.m_buildingPredecessorBuilding = prefabData->Attribute("predecessor_building");
				data.m_buildingCanDetectHidden = prefabData->BoolAttribute("detect_hidden");
				data.m_buildingHealth = prefabData->Int64Attribute("health");
				data.m_buildingLevel = prefabData->Int64Attribute("level");
				data.m_buildingDefense = prefabData->Int64Attribute("defense");
				data.m_buildingGoldProduction = prefabData->Int64Attribute("gold_production");
				data.m_buildingResearchPointsProduction = prefabData->Int64Attribute("research_points_production");
				data.m_buildingSightRange = prefabData->Int64Attribute("visibility_distance");
				data.m_buildingGoldCost = prefabData->Int64Attribute("gold_cost");

				// Store Building Prefab in DB.
				if (!StoreBuidlingPrefab(data, collection))
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not upload Building Prefab: \"{}\"!", Logger::AppRunningTime(), data.m_name);
					LOG_FILE_ERROR("[{:.4f}][DBMS::LoadUnitAndBuildingPrefabDefinitions] Could not upload Building Prefab: \"{}\"!", Logger::AppRunningTime(), data.m_name);

					prefabXml.Clear();
					cache.Clear();
					return false;
				}
			}

			prefab = prefab->NextSiblingElement("Prefab");
		}

		return true;
	}

	bool DBMS::StoreUnitPrefab(net::SUnitGameobject& prefab, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Get database and collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection prefabCollection = db[collection];

		boost::optional<bsoncxx::v_noabi::document::value> doc;

		// Check whether the Prefab Already exists.
		const bool prefab_exists = _findOneByKeyValuePair(prefabCollection, doc, "name", prefab.m_unitName.C_String());

		// Create the MongoDB Document,
		auto abilities = bsoncxx::builder::basic::array{};
		for (auto e : prefab.m_unitAbilities)
		{
			abilities.append(e);
		}

		auto status_effects = bsoncxx::builder::basic::array{};
		for (auto e : prefab.m_unitStartingStatusEffects)
		{
			status_effects.append(e);
		}



		auto update_or_insert_doc = make_document(
			kvp("unitSprite", prefab.m_unitSprite.C_String()),
			kvp("unitLayoutTemplate", prefab.m_unitLayoutTemplate.C_String()),

			kvp("unitName", prefab.m_unitName.C_String()),
			kvp("unitHealth", (int64_t)prefab.m_unitHealth),
			kvp("unitArmor", (int64_t)prefab.m_unitArmor),
			kvp("unitDefense", (int64_t)prefab.m_unitDefense),
			kvp("unitLevel", (int64_t)prefab.m_unitLevel),
			kvp("unitExperience", (int64_t)prefab.m_unitExperience),
			kvp("unitSightRadius", (int64_t)prefab.m_unitSightRadius),
			kvp("unitMovementType", (int64_t)prefab.m_unitMovementType),
			kvp("unitMovementPoints", (int64_t)prefab.m_unitActionPoints),

			kvp("unitRanged", prefab.m_unitRanged),
			kvp("unitRangedMinAttack", (int64_t)prefab.m_unitRangedMinAttack),
			kvp("unitRangedMaxAttack", (int64_t)prefab.m_unitRangedMaxAttack),
			kvp("unitRangedMinRange", (int64_t)prefab.m_unitRangedMinRange),
			kvp("unitRangedMaxRange", (int64_t)prefab.m_unitRangedMaxRange),

			kvp("unitAbilities", abilities),
			kvp("unitStartingStatusEffects", status_effects)
		);


		// If it exists, update it.
		if(prefab_exists)
		{
			// Doc safe to view as he exists.
			auto view = doc->view();

			try
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = prefabCollection.update_one(
					make_document(kvp("name", view["name"].get_utf8().value.to_string().c_str())),
					make_document(kvp("$set", update_or_insert_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_unitName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_unitName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"! Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"! Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"!  Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				return false;
			}

		}
		// If it does not exist, create new entry.
		else
		{
			try
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = prefabCollection.insert_one(
					std::move(update_or_insert_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_unitName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_unitName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"! Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"! Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreUnitPrefab] bsoncxx::v_noabi::exception: \"{}\"!  Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_unitName, collection.c_str());
				return false;
			}
		}


		return false;
	}


	bool DBMS::StoreBuidlingPrefab(net::SBuildingGameobject& prefab, const std::string& collection)
	{
		if (!m_initialized) return false;

		// Get database and collection.
		mongocxx::database db = DBMS::get()->m_mongoClient[m_database];
		mongocxx::collection prefabCollection = db[collection];

		boost::optional<bsoncxx::v_noabi::document::value> doc;

		// Check whether the Prefab Already exists.
		const bool prefab_exists = _findOneByKeyValuePair(prefabCollection, doc, "name", prefab.m_buildingName.C_String());

		// Create the MongoDB Document,
		auto update_or_insert_doc = make_document(
			kvp("buildingSprite",					prefab.m_buildingSprite.C_String()),
			kvp("buildingPredecessorBuilding",		prefab.m_buildingPredecessorBuilding.C_String()),

			kvp("buildingName",						prefab.m_buildingName.C_String()),
			kvp("buildingHealth",					(int64_t)prefab.m_buildingHealth),
			kvp("buildingDefense",					(int64_t)prefab.m_buildingDefense),
			kvp("buildingLevel",					(int64_t)prefab.m_buildingLevel),
			kvp("buildingCanDetectHidden",			prefab.m_buildingCanDetectHidden),
			kvp("buildingGoldProduction",			(int64_t)prefab.m_buildingGoldProduction),
			kvp("buildingResearchPointsProduction", (int64_t)prefab.m_buildingResearchPointsProduction),
			kvp("buildingSightRange",				(int64_t)prefab.m_buildingSightRange),
			kvp("buildingGoldCost",					(int64_t)prefab.m_buildingGoldCost)
		);



		// If it exists, update it.
		if (prefab_exists)
		{
			// Doc safe to view as he exists.
			auto view = doc->view();

			try
			{
				bsoncxx::stdx::optional<mongocxx::result::update> result = prefabCollection.update_one(
					make_document(kvp("name", view["name"].get_utf8().value.to_string().c_str())),
					make_document(kvp("$set", update_or_insert_doc.view()))
				);

				if (result.value().result().modified_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_buildingName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_buildingName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] mongocxx::v_noabi::logic_error: \"{}\"! Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] bsoncxx::v_noabi::exception: \"{}\"! Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] bsoncxx::v_noabi::exception: \"{}\"!  Failed to update Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				return false;
			}
		}
		// If it does not exist, create new entry.
		else
		{
			try
			{
				bsoncxx::stdx::optional<mongocxx::result::insert_one> result = prefabCollection.insert_one(
					std::move(update_or_insert_doc)
				);

				if (result.value().result().inserted_count() > 0)
				{
					return true;
				}
				else
				{
					LOG_DBG_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_buildingName, collection.c_str());
					LOG_FILE_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), prefab.m_buildingName, collection.c_str());
					return false;
				}
			}
			catch (const mongocxx::v_noabi::logic_error& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] mongocxx::v_noabi::logic_error: \"{}\"! Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				return false;
			}
			catch (const bsoncxx::v_noabi::exception& e)
			{
				LOG_DBG_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] bsoncxx::v_noabi::exception: \"{}\"! Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				LOG_FILE_ERROR("[{:.4f}][DBMS::StoreBuidlingPrefab] bsoncxx::v_noabi::exception: \"{}\"!  Failed to upsert Unit Prefab \"{}\" to collection \"{}\".", Logger::AppRunningTime(), e.what(), prefab.m_buildingName, collection.c_str());
				return false;
			}
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
		LOG_FILE_ERROR("[{:.4f}][DBMS::GetStatusEffect] Could not retrieve net::SStatusEffectData by name \"{}\"!", Logger::AppRunningTime(), effect_name);
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
				LOG_FILE_INFO("[{:.4f}][DBMS::CreateUser] Created new user with id \"{}\"!", Logger::AppRunningTime(), uuid);
				
				m_nextUUID += 1;
				return uuid;
			}
		}
		catch (const mongocxx::v_noabi::logic_error& e)
		{
			LOG_DBG_ERROR("[{:.4f}][DBMS::CreateUser] mongocxx::v_noabi::logic_error: \"{}\"! Failed to create user.", Logger::AppRunningTime(), e.what());
			LOG_FILE_ERROR("[{:.4f}][DBMS::CreateUser] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to create user.", Logger::AppRunningTime(), e.what());
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::Initialize] std::exception: \"{}\"!  Failed to connect to Database \"{}\".", Logger::AppRunningTime(), e.what(), m_database);

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
			LOG_FILE_INFO("[{:.4f}][DBMS::CreateGame] Create game \"{}\"!", Logger::AppRunningTime(), name.c_str());
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
			LOG_FILE_INFO("[{:.4f}][DBMS::DeleteGame] Delete game \"{}\"!", Logger::AppRunningTime(), name.c_str());
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
							AS_UNIT(obj)->m_unitDefense = doc["unitDefense"].get_int64();
							AS_UNIT(obj)->m_unitLevel = doc["unitLevel"].get_int64();
							AS_UNIT(obj)->m_unitSightRadius = doc["unitSightRadius"].get_int64();
							AS_UNIT(obj)->m_unitMovementType = doc["unitMovementType"].get_int64();
							AS_UNIT(obj)->m_unitActionPoints = doc["unitMovementPoints"].get_int64();
							AS_UNIT(obj)->m_unitExperience = doc["unitExperience"].get_int64();
							AS_UNIT(obj)->m_unitName = doc["unitName"].get_utf8().value.to_string().c_str();

							AS_UNIT(obj)->m_unitMinAttack = doc["unitMinAttack"].get_int64();
							AS_UNIT(obj)->m_unitMaxAttack = doc["unitMaxAttack"].get_int64();

							AS_UNIT(obj)->m_unitRanged = doc["unitRanged"].get_bool();
							AS_UNIT(obj)->m_unitRangedMinAttack = doc["unitRangedMinAttack"].get_int64();
							AS_UNIT(obj)->m_unitRangedMaxAttack = doc["unitRangedMaxAttack"].get_int64();
							AS_UNIT(obj)->m_unitRangedMinRange = doc["unitRangedMinRange"].get_int64();
							AS_UNIT(obj)->m_unitRangedMaxRange = doc["unitRangedMaxRange"].get_int64();
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::GetNetGameobjects] mongocxx::v_noabi::logic_error: \"{}\"!  Failed to retrieve Net Gameobjects for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), gamename);
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
				kvp("unitDefense", (int64_t)object->m_unitDefense),
				kvp("unitLevel", (int64_t)object->m_unitLevel),
				kvp("unitExperience", (int64_t)object->m_unitExperience),
				kvp("unitSightRadius", (int64_t)object->m_unitSightRadius),
				kvp("unitMovementType", (int64_t)object->m_unitMovementType),
				kvp("unitMovementPoints", (int64_t)object->m_unitActionPoints),

				kvp("unitRanged", object->m_unitRanged),
				kvp("unitRangedMinAttack", (int64_t)object->m_unitRangedMinAttack),
				kvp("unitRangedMaxAttack", (int64_t)object->m_unitRangedMaxAttack),
				kvp("unitRangedMinRange", (int64_t)object->m_unitRangedMinRange),
				kvp("unitRangedMaxRange", (int64_t)object->m_unitRangedMaxRange)
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectUnit] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectBuilding] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectMapobject] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
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
			LOG_FILE_ERROR("[{:.4f}][DBMS::TryEmplaceNetGameobjectMaptile] mongocxx::v_noabi::logic_error: \"{}\"! Failed to emplace Net Gameobject \"{}\" for Game \"game_{}\".", Logger::AppRunningTime(), e.what(), object->m_name, game.name().to_string());
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