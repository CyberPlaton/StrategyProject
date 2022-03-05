#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <malloc.h>


#include "Random.h"
#include "olcPGEX_Network.h"

#define FORCE_INLINE __forceinline 


#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}

namespace net
{
	// Forward Decl.
	// Description of the clients user specific data.
	struct UserDesc;
	// Description of the clients application.
	struct ClientAppDesc;
	// Description of Game Server the client is playing on.
	struct GameServerDesc;

	enum Message
	{

		NET_MSG_REQUEST_USER_VALIDATION_DATA, // MSs validation data request for client.
		NET_MSG_USER_VALIDATION_DATA,		  // Clients validation data.


		// Management related messages
		NET_MSG_USER_DATA,					// Data of a User in it
		NET_MSG_REQUEST_USER_DATA_UPDATE,	// Client requests to update data for a given user

		NET_MSG_CLIENT_ACCEPT,				// A Clients connection attempt was accepted
		NET_MSG_CLIENT_REJECT,				// A Clients connection attempt was rejected




		// Game related messages
		NET_MSG_GAMEOBJECT_UPDATE,
		NET_MSG_MAPDATA,
	};


	enum EAchievement
	{
		ACHIEV_TEST_ACHIEVEMENT_1,
		ACHIEV_TEST_ACHIEVEMENT_2,
		ACHIEV_TEST_ACHIEVEMENT_3,
		ACHIEV_TEST_ACHIEVEMENT_4,
		ACHIEV_TEST_ACHIEVEMENT_5,
		ACHIEV_TEST_ACHIEVEMENT_6,
		ACHIEV_TEST_ACHIEVEMENT_7,
		ACHIEV_TEST_ACHIEVEMENT_8,
		ACHIEV_TEST_ACHIEVEMENT_9,
		ACHIEV_TEST_ACHIEVEMENT_10,
		ACHIEV_TEST_ACHIEVEMENT_11,
		ACHIEV_TEST_ACHIEVEMENT_12,
		ACHIEV_TEST_ACHIEVEMENT_13,
		ACHIEV_TEST_ACHIEVEMENT_14,
		ACHIEV_TEST_ACHIEVEMENT_15,
		ACHIEV_TEST_ACHIEVEMENT_16,
		ACHIEV_TEST_ACHIEVEMENT_17,
		ACHIEV_TEST_ACHIEVEMENT_18,
		ACHIEV_TEST_ACHIEVEMENT_19,
		ACHIEV_ACHIEVEMENTS_COUNT = ACHIEV_TEST_ACHIEVEMENT_19
	};

	enum EServiceItem
	{
		ITEM_TEST_ITEM_1,
		ITEM_TEST_ITEM_2,
		ITEM_TEST_ITEM_3,
		ITEM_TEST_ITEM_4,
		ITEM_TEST_ITEM_5,
		ITEM_TEST_ITEM_6,
		ITEM_TEST_ITEM_7,
		ITEM_TEST_ITEM_8,
		ITEM_TEST_ITEM_9,
		ITEM_TEST_ITEM_10,
		ITEM_TEST_ITEM_11,
		ITEM_TEST_ITEM_12,
		ITEM_TEST_ITEM_13,
		ITEM_TEST_ITEM_14,
		ITEM_TEST_ITEM_15,
		ITEM_TEST_ITEM_16,
		ITEM_TEST_ITEM_17,
		ITEM_TEST_ITEM_18,
		ITEM_TEST_ITEM_19,
		ITEM_ITEMS_COUNT = ITEM_TEST_ITEM_19
	};
	

	enum EUserPlatform
	{
		UP_INVALID = 0,
		UP_STEAM,
		UP_SWITCH,
		UP_XBOX,
		UP_PS
	};



	// Description of the Client.
	// Holding information related to client, database entries and such.
	struct UserDesc
	{
		UserDesc() = default;
		UserDesc(const UserDesc& rh) = default;
		UserDesc& operator=(const UserDesc& rh) = default;
		~UserDesc() = default;

		bool operator==(const UserDesc& other);
		
		// Network Info.
		size_t m_netId = 0;

		// Platform Info.
		EUserPlatform m_userPlatform = EUserPlatform::UP_INVALID;

		// User Info.
		size_t m_uuid = 0;
		std::string m_steamName;
		size_t m_steamId = 0;
		std::string m_nintendoName;
		size_t m_nintendoId = 0;

		size_t m_currency = 0;

		std::vector< EAchievement > m_achievementsVec;
		std::vector< EServiceItem > m_serviceItemsVec;
	};
	struct ClientAppDesc
	{
		size_t m_majorVersion = 0;
		size_t m_minorVersion = 0;
		size_t m_reviewVersion = 0;
	};
	struct GameServerDesc
	{
		size_t id;
	};


	enum ENetGameobject
	{
		NET_GO_MAPTILE,		// General Maptiles.
		NET_GO_MAPOBJECT,	// Forests, Mountains ...
		NET_GO_BUILDING,	// Townhalls, Castles, Buildings ...
		NET_GO_UNIT			// All units ...
	};
	struct NetGameobject
	{
		std::string m_name;
		size_t m_netId = 0;
		size_t m_playerId = 0;
		ENetGameobject m_objectType;

		// General stats.
		float m_positionX = 0;
		float m_positionY = 0;
		size_t m_tilePositionX = 0;
		size_t m_tilePositionY = 0;

		// Unit stats.
		std::string m_unitName;
		size_t m_unitHealth = 0;
		size_t m_unitArmor = 0;
		size_t m_unitAttack = 0;
		size_t m_unitDefense = 0;

		// Building stats.
		std::string m_buildingName;
		size_t m_buildingHealth = 0;

		// Maptile stats.
		std::string m_MaptileType;
		std::string m_MaptileBiome;

		// Mapobject stats.
		std::string m_mapObjectType;
		std::string m_mapObjectBiome;
	};





	FORCE_INLINE static uint32_t CreateNetworkUUID()
	{
		Random rand;
		return rand.Int();
	}


	FORCE_INLINE static const char* GetAchievementText(EAchievement achiev)
	{
		switch (achiev)
		{
		case net::ACHIEV_TEST_ACHIEVEMENT_1:
			return "Test_Achievement_1";
		case net::ACHIEV_TEST_ACHIEVEMENT_2:
			return "Test_Achievement_2";
		case net::ACHIEV_TEST_ACHIEVEMENT_3:
			return "Test_Achievement_3";
		case net::ACHIEV_TEST_ACHIEVEMENT_4:
			return "Test_Achievement_4";
		case net::ACHIEV_TEST_ACHIEVEMENT_5:
			return "Test_Achievement_5";
		case net::ACHIEV_TEST_ACHIEVEMENT_6:
			return "Test_Achievement_6";
		case net::ACHIEV_TEST_ACHIEVEMENT_7:
			return "Test_Achievement_7";
		case net::ACHIEV_TEST_ACHIEVEMENT_8:
			return "Test_Achievement_8";
		case net::ACHIEV_TEST_ACHIEVEMENT_9:
			return "Test_Achievement_9";
		case net::ACHIEV_TEST_ACHIEVEMENT_10:
			return "Test_Achievement_10";
		case net::ACHIEV_TEST_ACHIEVEMENT_11:
			return "Test_Achievement_11";
		case net::ACHIEV_TEST_ACHIEVEMENT_12:
			return "Test_Achievement_12";
		case net::ACHIEV_TEST_ACHIEVEMENT_13:
			return "Test_Achievement_13";
		case net::ACHIEV_TEST_ACHIEVEMENT_14:
			return "Test_Achievement_14";
		case net::ACHIEV_TEST_ACHIEVEMENT_15:
			return "Test_Achievement_15";
		case net::ACHIEV_TEST_ACHIEVEMENT_16:
			return "Test_Achievement_16";
		case net::ACHIEV_TEST_ACHIEVEMENT_17:
			return "Test_Achievement_17";
		case net::ACHIEV_TEST_ACHIEVEMENT_18:
			return "Test_Achievement_18";
		case net::ACHIEV_TEST_ACHIEVEMENT_19:
			return "Test_Achievement_19";
		default:
			return nullptr;
		}
	}
	FORCE_INLINE static const char* GetServiceItemText(EServiceItem item)
	{
		switch (item)
		{
		case net::ITEM_TEST_ITEM_1:
			return "Test_ServiceItem_1";
		case net::ITEM_TEST_ITEM_2:
			return "Test_ServiceItem_2";
		case net::ITEM_TEST_ITEM_3:
			return "Test_ServiceItem_3";
		case net::ITEM_TEST_ITEM_4:
			return "Test_ServiceItem_4";
		case net::ITEM_TEST_ITEM_5:
			return "Test_ServiceItem_5";
		case net::ITEM_TEST_ITEM_6:
			return "Test_ServiceItem_6";
		case net::ITEM_TEST_ITEM_7:
			return "Test_ServiceItem_7";
		case net::ITEM_TEST_ITEM_8:
			return "Test_ServiceItem_8";
		case net::ITEM_TEST_ITEM_9:
			return "Test_ServiceItem_9";
		case net::ITEM_TEST_ITEM_10:
			return "Test_ServiceItem_10";
		case net::ITEM_TEST_ITEM_11:
			return "Test_ServiceItem_11";
		case net::ITEM_TEST_ITEM_12:
			return "Test_ServiceItem_12";
		case net::ITEM_TEST_ITEM_13:
			return "Test_ServiceItem_13";
		case net::ITEM_TEST_ITEM_14:
			return "Test_ServiceItem_14";
		case net::ITEM_TEST_ITEM_15:
			return "Test_ServiceItem_15";
		case net::ITEM_TEST_ITEM_16:
			return "Test_ServiceItem_16";
		case net::ITEM_TEST_ITEM_17:
			return "Test_ServiceItem_17";
		case net::ITEM_TEST_ITEM_18:
			return "Test_ServiceItem_18";
		case net::ITEM_TEST_ITEM_19:
			return "Test_ServiceItem_19";
		default:
			return nullptr;
		}
	}


}



// Security sensititive functions,
// Thus enforce inlining to protect.
#define FORCE_INLINE __forceinline 
#define COMMON_RSA_PRIME_NUMBER 7
#define COMMON_NET_KEY 134
FORCE_INLINE void EncryptText(std::string& text, size_t key)
{
	std::string encr;
	for (int i = 0; i < text.size(); i++)
	{
		char c = text[i] ^ (char)key;
		encr.push_back(c);
	}

	text.replace(0, text.size(), encr);
}
FORCE_INLINE void DecryptText(std::string& text, size_t key)
{
	std::string encr;
	for (int i = 0; i < text.size(); i++)
	{
		char c = text[i] ^ (char)key;
		encr.push_back(c);
	}

	text.replace(0, text.size(), encr);
}

// Should not be used for Text.
// More for standard values which can be expressed as numbers.
template < typename T >
FORCE_INLINE void EncryptData(T& data, size_t key)
{
	data = data ^ (T)key;
}
template < typename T >
FORCE_INLINE void DecryptData(T& data, size_t key)
{
	data = data ^ (T)key;
}
FORCE_INLINE size_t GCD(size_t a, size_t b)
{
	if (a == b) return b;
	if (b == 0) return a;
	if (a == 0) return b;
	if (a == b) return a;
	if (a > b) return GCD(a - b, b);
	return GCD(a, b - a);
}
FORCE_INLINE void EncryptMessage(net::UserDesc& desc)
{
	using namespace net;

	// Should find a better key gen.
	size_t key = GCD(COMMON_NET_KEY, COMMON_RSA_PRIME_NUMBER);

	EncryptData<size_t>(desc.m_currency, key);
	EncryptData<size_t>(desc.m_uuid, key);
	EncryptData<size_t>(desc.m_netId, key);

	EncryptData<size_t>(desc.m_steamId, key);
	EncryptData<size_t>(desc.m_nintendoId, key);

	EncryptText(desc.m_steamName, key);
	EncryptText(desc.m_nintendoName, key);
}



FORCE_INLINE void DecryptMessage(net::UserDesc& desc)
{
	using namespace net;

	// Should find a better key gen.
	size_t key = GCD(COMMON_NET_KEY, COMMON_RSA_PRIME_NUMBER);

	DecryptData<size_t>(desc.m_currency, key);
	DecryptData<size_t>(desc.m_uuid, key);
	DecryptData<size_t>(desc.m_netId, key);

	DecryptData<size_t>(desc.m_steamId, key);
	DecryptData<size_t>(desc.m_nintendoId, key);

	DecryptText(desc.m_steamName, key);
	DecryptText(desc.m_nintendoName, key);
}
FORCE_INLINE void EncryptMessage(net::ClientAppDesc& desc)
{
	// TODO
}
FORCE_INLINE void DecryptMessage(net::ClientAppDesc& desc)
{
	// TODO
}
FORCE_INLINE void EncryptMessage(net::GameServerDesc& desc)
{
	// TODO
}
FORCE_INLINE void DecryptMessage(net::GameServerDesc& desc)
{
	// TODO
}

