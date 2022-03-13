#pragma once

// USER DESC
#include "NetCommon.h"
// NETWORKING
#include <olcPGEX_Network.h>
// XML
#include <tinyxml2.h>
// STEAM
#include <steam_api.h>

#define STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS(); if(!STATIC_MEMBER->_init()){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){STATIC_MEMBER->_shutdown(); delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}



// MONGODBO INCLUDES
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

		// Get the Data for a gamemap from DB as XML.
		static tinyxml2::XMLDocument GetMapdata(size_t gameid);

		// Create an empty entry for a new user in the DB.
		// Returns the uuid of the new User.
		static size_t CreateUser();

		// Update Data of a user in the DB from given description.
		// Returns true if everything went OK.
		static bool UpdateUser(net::UserDesc& desc);
		
		// Fill description object with information from the DB.
		// Returns true if everything went OK.
		static bool GetUserDesc(net::UserDesc& desc);
		
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

		bool _init();
		void _shutdown() noexcept;

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
	};
}