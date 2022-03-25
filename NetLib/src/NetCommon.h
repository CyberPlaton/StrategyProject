#pragma once

#include "Random.h"

// CONSIDER DEFINING _BITSTREAM_NATIVE_END
// if we need Byteendian swapping.

#include <RakString.h>
#include <BitStream.h>
#include <MessageIdentifiers.h>

#include <vector>

namespace net
{
#define FORCE_INLINE __forceinline 

	// Forward decl.
	enum EAchievement;
	enum EServiceItem;
	enum EClientPlatform;
	enum EGameobjectType;
	enum EMessageId;
	struct SClientDescription;
	struct SClientAppDescription;
	struct SGameServerDescription;
	struct SSerializable;
	struct SGameobject;
	struct SBuildingGameobject;
	struct SUnitGameobject;
	struct SMaptileGameobject;
	struct SMapobjectGameobject;

	FORCE_INLINE static uint32_t CreateGameobjectNetworkUUID()
	{
		Random rand;
		return rand.Int();
	}

	FORCE_INLINE static RakNet::BitStream& CreateMessage(EMessageId id)
	{
		RakNet::BitStream stream;
		stream.Write((RakNet::MessageID)id);
		return stream;
	}

	enum EMessageId
	{
		NET_MSG_REQUEST_USER_VALIDATION_DATA = DefaultMessageIDTypes::ID_USER_PACKET_ENUM + 1,
		NET_MSG_USER_VALIDATION_DATA,

		NET_MSG_USER_DATA,
		NET_MSG_REQUEST_USER_DATA_UPDATE,

		NET_MSG_CLIENT_ACCEPT,
		NET_MSG_CLIENT_REJECT,

		NET_MSG_DELETE_GAME,
		NET_MSG_CREATE_GAME,
		NET_MSG_GAMEOBJECT_UPDATE,
		NET_MSG_MAPDATA,
	};
	enum EClientPlatform
	{
		UP_INVALID = -1,
		UP_STEAM,
		UP_SWITCH,
		UP_XBOX,
		UP_PS,
	};
	enum EGameobjectType
	{
		NET_GO_MAPTILE,
		NET_GO_MAPOBJECT,
		NET_GO_UNIT,
		NET_GO_BUILDING,
	};
	enum EAchievement
	{
		ACHIEV_TEST_1,
		ACHIEV_TEST_2,
		ACHIEV_TEST_3,
		ACHIEV_TEST_4,
		ACHIEV_TEST_5,

		// Enum size indicator. Internal.
		ACHIEV_COUNT
	};
	enum EServiceItem
	{
		ITEM_TEST_1,
		ITEM_TEST_2,
		ITEM_TEST_3,
		ITEM_TEST_4,
		ITEM_TEST_5,

		// Enum size indicator. Internal.
		ITEM_COUNT
	};

	struct SSerializable
	{
		virtual void Serialize(RakNet::BitStream&) = 0;
		virtual void Deserialize(RakNet::BitStream&) = 0;
	};


	struct SClientDescription : public SSerializable
	{
		SClientDescription() = default;
		SClientDescription(const SClientDescription& rh) = default;
		SClientDescription& operator=(const SClientDescription& rh) = default;
		~SClientDescription() = default;

		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_networkId);
			stream.Write((uint32_t)m_platform);
			stream.Write(m_uuid);
			stream.Write(m_steamName);
			stream.Write(m_steamId);
			stream.Write(m_nintendoName);
			stream.Write(m_nintendoId);
			stream.Write(m_xboxLiveName);
			stream.Write(m_xboxLiveId);
			stream.Write(m_psnName);
			stream.Write(m_psnId);
			stream.Write(m_currencyAmount);
			stream.Write(m_achievements.size());
			for (auto e : m_achievements)
			{
				stream.Write((uint32_t)e);
			}
			stream.Write(m_serviceItems.size());
			for (auto e : m_serviceItems)
			{
				stream.Write((uint32_t)e);
			}
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));
			stream.Read(m_networkId);
			stream.Read((EClientPlatform)m_platform);
			stream.Read(m_uuid);
			stream.Read(m_steamName);
			stream.Read(m_steamId);
			stream.Read(m_nintendoName);
			stream.Read(m_nintendoId);
			stream.Read(m_xboxLiveName);
			stream.Read(m_xboxLiveId);
			stream.Read(m_psnName);
			stream.Read(m_psnId);
			stream.Read(m_currencyAmount);

			int vector_size = 0;
			stream.Read(vector_size);
			for (int i = 0; i < vector_size; i++)
			{
				uint32_t achivement;
				stream.Read(achivement);
				m_achievements.push_back((EAchievement)achivement);
			}
			stream.Read(vector_size);
			for (int i = 0; i < vector_size; i++)
			{
				uint32_t item;
				stream.Read(item);
				m_serviceItems.push_back((EServiceItem)item);
			}
		}


		// GENERAL INFORMATION
		uint32_t m_networkId;
		EClientPlatform m_platform;

		// DATABASE INFORMATION
		uint32_t m_uuid;
		RakNet::RakString m_steamName;
		uint32_t m_steamId;
		RakNet::RakString m_nintendoName;
		uint32_t m_nintendoId;
		RakNet::RakString m_xboxLiveName;
		uint32_t m_xboxLiveId;
		RakNet::RakString m_psnName;
		uint32_t m_psnId;

		uint32_t m_currencyAmount;
		std::vector< EAchievement > m_achievements;
		std::vector< EServiceItem > m_serviceItems;
	};


	struct SClientAppDescription : public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_majorVersion);
			stream.Write(m_minorVersion);
			stream.Write(m_patchVersion);
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_majorVersion);
			stream.Read(m_minorVersion);
			stream.Read(m_patchVersion);
		}

		uint32_t m_majorVersion;
		uint32_t m_minorVersion;
		uint32_t m_patchVersion;
	};


	struct SGameServerDescription : public SSerializable
	{
		SGameServerDescription() = default;
		SGameServerDescription(const SGameServerDescription& rh) = default;
		SGameServerDescription& operator=(const SGameServerDescription& rh) = default;
		~SGameServerDescription() = default;

		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_gameId);
			stream.Write(m_gameData);
			stream.Write(m_players.size());
			for (auto e : m_players)
			{
				stream.Write((uint32_t)e);
			}
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));
			stream.Read(m_gameId);
			stream.Read(m_gameData);

			int vector_size = 0;
			stream.Read(vector_size);
			for (int i = 0; i < vector_size; i++)
			{
				uint32_t player;
				stream.Read(player);
				m_players.push_back(player);
			}
		}

		RakNet::RakString m_gameId;
		RakNet::RakString m_gameData;
		std::vector< uint32_t > m_players;
	};


	struct SGameobject
	{
		SGameobject() = default;
		SGameobject(const SGameobject& rh) = default;
		SGameobject& operator=(const SGameobject& rh) = default;
		~SGameobject() = default;


		RakNet::RakString m_name;
		uint32_t m_networkId;
		uint32_t m_playerId;
		EGameobjectType m_gameobjectType;

		// GENERAL INFORMATION
		float m_positionX;
		float m_positionY;
	};


	struct SBuildingGameobject : public SGameobject, public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_name);
			stream.Write(m_networkId);
			stream.Write(m_playerId);
			stream.Write((uint32_t)m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);

			stream.Write(m_buildingName);
			stream.Write(m_buildingHealth);
			stream.Write(m_buildingLevel);
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));
			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read((EGameobjectType)m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			stream.Read(m_buildingName);
			stream.Read(m_buildingHealth);
			stream.Read(m_buildingLevel);
		}

		RakNet::RakString m_buildingName;
		uint32_t m_buildingHealth;
		uint32_t m_buildingLevel;
	};
	struct SUnitGameobject : public SGameobject, public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_name);
			stream.Write(m_networkId);
			stream.Write(m_playerId);
			stream.Write((uint32_t)m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);
			stream.Write(m_playerId);

			stream.Write(m_unitName);
			stream.Write(m_unitHealth);
			stream.Write(m_unitArmor);
			stream.Write(m_unitAttack);
			stream.Write(m_unitDefense);
			stream.Write(m_unitLevel);
			stream.Write(m_unitExperience);
			stream.Write(m_unitSightRadius);
			stream.Write(m_unitMovementType);
			stream.Write(m_unitMovementPoints);
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));
			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read((EGameobjectType)m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			stream.Read(m_unitName);
			stream.Read(m_unitHealth);
			stream.Read(m_unitArmor);
			stream.Read(m_unitAttack);
			stream.Read(m_unitDefense);
			stream.Read(m_unitLevel);
			stream.Read(m_unitExperience);
			stream.Read(m_unitSightRadius);
			stream.Read(m_unitMovementType);
			stream.Read(m_unitMovementPoints);
		}

		RakNet::RakString m_unitName;
		uint32_t m_unitHealth;
		uint32_t m_unitArmor;
		uint32_t m_unitAttack;
		uint32_t m_unitDefense;
		uint32_t m_unitLevel;
		uint32_t m_unitExperience;
		uint32_t m_unitSightRadius;
		uint32_t m_unitMovementType;
		uint32_t m_unitMovementPoints;
	};
	struct SMaptileGameobject : public SGameobject, public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{

			stream.Write(m_name);
			stream.Write(m_networkId);
			stream.Write(m_playerId);
			stream.Write((uint32_t)m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);
			stream.Write(m_playerId);

			stream.Write(m_maptileBiome);
			stream.Write(m_maptileType);
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));
			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read((EGameobjectType)m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			stream.Read(m_maptileBiome);
			stream.Read(m_maptileType);
		}

		RakNet::RakString m_maptileType;
		RakNet::RakString m_maptileBiome;
	};
	struct SMapobjectGameobject : public SGameobject, public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_name);
			stream.Write(m_networkId);
			stream.Write(m_playerId);
			stream.Write((uint32_t)m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);
			stream.Write(m_playerId);

			stream.Write(m_maptileObjectType);
			stream.Write(m_maptileObjectBiome);
		}
		void Deserialize(RakNet::BitStream& stream)  override final
		{
			stream.IgnoreBytes(sizeof(RakNet::MessageID));
			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read((EGameobjectType)m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			stream.Read(m_maptileObjectType);
			stream.Read(m_maptileObjectBiome);
		}

		RakNet::RakString m_maptileObjectType;
		RakNet::RakString m_maptileObjectBiome;
	};
}