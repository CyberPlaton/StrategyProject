#pragma once

#include <NetLib/NetLib.h>
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

#include <memory>
#include <sstream>

namespace dbms
{
	class DBMS
	{
	public:
		STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(DBMS, g_DBMS);

		
		/*
		* GAME/GAMEOBJECT MANAGEMENT
		*/
		// Create a new Game entry.
		// Always succeeds.
		static void CreateGame(const std::string& gamename);
		
		// Delete a Game from entry.
		// Always succeeds.
		static void DeleteGame(std::string gamename);

		// Retrieve all Gameobjects in a Game from Database.
		// Objects are pushed into given vector.
		// Returns true on success.
		static bool GetNetGameobjects(const std::string& gamename, std::vector< net::SGameobject* >& backv);

		// Add a Gameobject to Game storage in Database
		// If it already exists, override it.
		// Returns true on success.
		static bool TryEmplaceNetGameobject(net::SGameobject* object, const std::string& gamename);


		
		/*
		* USER MANAGEMENT
		*/
		// Create an empty entry for a new user in the DB.
		// Returns the uuid of the new User.
		static size_t CreateUser();

		// Update Data of a user in the DB from given description.
		// Returns true if everything went OK.
		static bool UpdateUser(net::SClientDescription& desc);
		
		// Fill description object with information from the DB.
		// Returns true if everything went OK.
		static bool GetUserDesc(net::SClientDescription& desc);
		
		// Delete a user entry from the Database.
		// Returns true on success.
		static bool DeleteUser(size_t id);

		static void BackupUserNumber();
		static const bool Initialized() { return m_initialized; }

	private:

		static DBMS* g_DBMS;
		static bool m_initialized;
		static const std::string m_database;
		static const std::string m_userDataCollection;
		static const std::string m_leaderboardCollection;
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
				printf("DBMS::_findOneByKeyValuePair - mongocxx::query_exception - Message: \n\t\t \"%s\"\n", e.what());
			}

			return doc.has_value();
		}




		static bool TryEmplaceNetGameobjectUnit(net::SUnitGameobject* object, mongocxx::collection& game, bool update);
		static bool TryEmplaceNetGameobjectBuilding(net::SBuildingGameobject* object, mongocxx::collection& game, bool update);
		static bool TryEmplaceNetGameobjectMapobject(net::SMapobjectGameobject* object, mongocxx::collection& game, bool update);
		static bool TryEmplaceNetGameobjectMaptile(net::SMaptileGameobject* object, mongocxx::collection& game, bool update);
	};
}