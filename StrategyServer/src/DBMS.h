#pragma once

#include "NetLib.h"
#include "Logging.h"

#include <memory>
#include <sstream>

#include <tinyxml2.h>
#include <steam_api.h>


#define STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS(); if(!STATIC_MEMBER->Initialize()){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){STATIC_MEMBER->Terminate(); delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}


// MONGODB INCLUDES
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/config/prelude.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/exception/error_code.hpp>


namespace dbms
{
	class DBMS
	{
	public:
		STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(DBMS, g_DBMS);

		
		/*
		* GAME MANAGEMENT
		*/
		// Create a new Game entry.
		// Always succeeds.
		static void CreateGame(const std::string& gamename);
		
		// Delete a Game from entry.
		// Always succeeds.
		static void DeleteGame(std::string gamename);



		/*
		* GAME RELEVANT FUNCTIONALITY
		*/
		/// @brief Retrieve all networked Gameobjects in a game currently stored in database.
		/// @param gamename Name of the game.
		/// @param backv Vector in which to put the Gameobjects.
		/// @return True on success.
		static bool GetNetGameobjects(const std::string& gamename, std::vector< net::SGameobject* >& backv);

		/// @brief Try adding a networked Gameobject to Database.
		/// @param object Object to be added.
		/// @param gamename Name of the game.
		/// @return True on success.
		static bool TryEmplaceNetGameobject(net::SGameobject* object, const std::string& gamename);

		/// @brief Retrieve the requested effect and fill given effect object.
		/// @param effect_name Name of the effect to be retrieved.
		/// @param effect Object which to fill with data.
		/// @return True on success. False if net::SStatusEffectData* is nullptr or we couldnt find the Status Effect by name.
		static bool GetStatusEffect(const std::string& effect_name, net::SStatusEffectData* effect);

		/// @brief Retrieve the requested Entity Ability and fill given object with data.
		/// @param ability_name Name of the Ability to be retrieved.
		/// @param ability Object to be filled with data.
		/// @return True on success. False if net::SAbilityData* is nullptr or we couldnt find the Ability by name.
		static bool GetAbility(const std::string& ability_name, net::SAbilityData* ability);

		/// @brief Retrieve all Status Effects.
		/// @param storage Storage object where to store the Status Effects.
		/// @return True on success. False if could not retrieve any.
		static bool GetAllStatusEffectData(std::vector< net::SStatusEffectData >& storage);

		/// @brief Retrieve all Abilities.
		/// @param storage Storage object where to store the Abilities.
		/// @return True on success. False if could not retrieve any.
		static bool GetAllAbilityData(std::vector< net::SAbilityData >& storage);


		/*
		* DBMS DATA MANAGEMENT
		*/
		
		/// @brief Load status effect definitions from files into the database.
		/// @param filepath Filepath of the Status Effect Cache.
		/// @return True, if all Status Effects were loaded into database.
		static bool LoadStatusEffectDefinitions(const std::string& status_effects_filepath, const std::string& filepath, const std::string& collection);

		/// @brief Upload the Status Effect data into the database.
		/// @param data Data to be uploaded.
		/// @return True if uploading was successful.
		static bool StoreStatusEffect(net::SStatusEffectData& data, const std::string& collection);

		/// @brief Load ability definitions from files into the database.
		/// @param filepath Filepath of the Ability Cache.
		/// @return True, if all Abilities were loaded into database.
		static bool LoadAbilityDefinitions(const std::string& abilities_path, const std::string& filepath, const std::string& collection);

		/// @brief Upload the Ability data into the database.
		/// @param data Data to be uploaded.
		/// @return True if uploading was successful.
		static bool StoreAbility(net::SAbilityData& data, const std::string& collection);

		/// @brief Load unit and building prefab definitions from files into the database. We determine dynamic whether its a unit or building.
		/// @param filepath Filepath of the Unit Prefab Cache.
		/// @return True if uploading was successful.
		static bool LoadUnitAndBuildingPrefabDefinitions(const std::string& prefab_file_path, const std::string& filepath, const std::string& collection);

		/// @brief Upload the Unit Prefab data into the database.
		/// @param prefab Unit Prefab to be uploaded.
		/// @return True if uploading was successful.
		static bool StoreUnitPrefab(net::SUnitGameobject& prefab, const std::string& collection);


		/// @brief Upload the Building Prefab data into the database.
		/// @param prefab Building Prefab to be uploaded.
		/// @return True if uploading was successful.
		static bool StoreBuidlingPrefab(net::SBuildingGameobject& prefab, const std::string& collection);


		
		/*
		* USER MANAGEMENT
		*/
		// Create an empty entry for a new user in the DB.
		// Returns the uuid of the new User.
		static uint32_t CreateUser();

		// Update Data of a user in the DB from given description.
		// Returns true if everything went OK.
		static bool UpdateUser(net::SClientDescription& desc);
		
		// Fill description object with information from the DB.
		// Returns true if everything went OK.
		static bool GetUserDesc(net::SClientDescription& desc);
		
		// Delete a user entry from the Database.
		// Returns true on success.
		static bool DeleteUser(size_t id);


		/*
		* UTIL
		*/
		static void BackupUserNumber();
		static const bool Initialized() { return m_initialized; }

	private:

		static DBMS* g_DBMS;
		static bool m_initialized;
		static const std::string m_database;
		static const std::string m_userDataCollection;
		static const std::string m_leaderboardCollection;
		static const std::string m_abilitiesCollection;
		static const std::string m_statusEffectsCollection;
		static size_t m_nextUUID;

		mongocxx::instance m_mongoInstance{};
		mongocxx::client m_mongoClient{ mongocxx::uri("mongodb://localhost:27017") };


	private:
		DBMS() = default;
		~DBMS() = default;

		bool Initialize();
		void Terminate() noexcept;

		template < typename Q , typename P>
		static bool _findOneByKeyValuePair(mongocxx::collection& c, boost::optional<bsoncxx::v_noabi::document::value>& doc, Q&& key, P&& value)
		{
			try
			{
				doc = c.find_one(make_document(kvp(key, value)));
			}
			catch (const mongocxx::query_exception& e)
			{
				LOG_DBG_INFO("[{:.4f}][DBMS::_findOneByKeyValuePair] mongocxx::query_exception: \"{}\"! Searched for kvp(\"{}\", \"{}\").", Logger::AppRunningTime(), e.what(), key, value);
				LOG_FILE_INFO("[{:.4f}][DBMS::_findOneByKeyValuePair] mongocxx::query_exception: \"{}\"! Searched for kvp(\"{}\", \"{}\").", Logger::AppRunningTime(), e.what(), key, value);
			}

			return doc.has_value();
		}




		static bool TryEmplaceNetGameobjectUnit(net::SUnitGameobject* object, mongocxx::collection& game, bool update);
		static bool TryEmplaceNetGameobjectBuilding(net::SBuildingGameobject* object, mongocxx::collection& game, bool update);
		static bool TryEmplaceNetGameobjectMapobject(net::SMapobjectGameobject* object, mongocxx::collection& game, bool update);
		static bool TryEmplaceNetGameobjectMaptile(net::SMaptileGameobject* object, mongocxx::collection& game, bool update);
	};
}