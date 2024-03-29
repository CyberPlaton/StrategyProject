#pragma once

#include "Random.h"

// CONSIDER DEFINING _BITSTREAM_NATIVE_END
// if we need Byte Endian swapping.

#include <RakString.h>
#include <BitStream.h>
#include <MessageIdentifiers.h>

#include <vector>
#include <string>

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
	struct SGameServerDescription;
	struct SSerializable;
	struct SGameobject;
	struct SBuildingGameobject;
	struct SUnitGameobject;
	struct SMaptileGameobject;
	struct SMapobjectGameobject;
	struct SStatusEffectData;
	struct SAbilityData;
	

	FORCE_INLINE static uint64_t CreateGameobjectNetworkUUID()
	{
		Random rand;
		return rand.Int();
	}

	FORCE_INLINE static RakNet::RakString MessageIDTypeToString(RakNet::MessageID id);

#define CREATE_MESSAGE(var, id) \
RakNet::BitStream var; \
var.Write((RakNet::MessageID)id) \

#define READ_MESSAGE(var, packet) \
RakNet::BitStream var(packet->data, packet->length, false) \

	enum EMessageId
	{
		NET_MSG_REQUEST_USER_VALIDATION_DATA = DefaultMessageIDTypes::ID_USER_PACKET_ENUM + 1,  // MasterServer requests data for validation.
		NET_MSG_USER_VALIDATION_DATA,															// Message containing validation data.

		NET_MSG_USER_DATA,																		// Message containing full client and user data.
		NET_MSG_REQUEST_USER_DATA_UPDATE,														// Client requests an update of his data in the DB. This message contains full client data.

		NET_MSG_CLIENT_ACCEPT,																	// Client was accepted.
		NET_MSG_CLIENT_REJECT,																	// Client was rejected.

		NET_MSG_REQUEST_ABILITY_AND_STATUS_EFFECTS_DATA,										// Client requesting Data of all Abilities and Status Effects.
		NET_MSG_ABILITY_DATA,																	// Message contains Ability Data.
		NET_MSG_STATUS_EFFECTS_DATA,															// Message contains Status Effect Data.
		NET_MSG_ABILITY_AND_STATUS_EFFECT_DATA_COMPLETE,										// Indicate that there is no more Data to be send out.

		NET_MSG_DELETE_GAME,
		NET_MSG_CREATE_GAME,
		NET_MSG_GAMEOBJECT_UPDATE,
		NET_MSG_MAPDATA,

		NET_MSG_TURN_END,																		// Client indicating that he ended his turn.
		NET_MSG_TURN_START,																		// MasterServer/GameServer indicating a client that his turn started.
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
		virtual void Deserialize(RakNet::BitStream&, bool) = 0;
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
			stream.Write((EClientPlatform)m_platform);
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
			stream.Write(m_version);
			stream.Write(m_achievements.size());
			for (auto e : m_achievements)
			{
				stream.Write((uint64_t)e);
			}
			stream.Write(m_serviceItems.size());
			for (auto e : m_serviceItems)
			{
				stream.Write((uint64_t)e);
			}
		}
		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if(ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

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
			stream.Read(m_version);

			int vector_size = 0;
			stream.Read(vector_size);
			for (int i = 0; i < vector_size; i++)
			{
				uint64_t achivement;
				stream.Read(achivement);
				m_achievements.push_back((EAchievement)achivement);
			}
			stream.Read(vector_size);
			for (int i = 0; i < vector_size; i++)
			{
				uint64_t item;
				stream.Read(item);
				m_serviceItems.push_back((EServiceItem)item);
			}
		}


		// GENERAL INFORMATION
		int64_t m_networkId = 0;
		EClientPlatform m_platform = EClientPlatform::UP_INVALID;

		// DATABASE INFORMATION
		int64_t m_uuid = 0;
		RakNet::RakString m_steamName = "";
		int64_t m_steamId = 0;
		RakNet::RakString m_nintendoName = "";
		int64_t m_nintendoId = 0;
		RakNet::RakString m_xboxLiveName = "";
		int64_t m_xboxLiveId = 0;
		RakNet::RakString m_psnName = "";
		int64_t m_psnId = 0;

		int64_t m_currencyAmount = 0;
		std::vector< EAchievement > m_achievements;
		std::vector< EServiceItem > m_serviceItems;

		// APP INFORMATION
		int64_t m_version = 0;
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
		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_gameId);
			stream.Read(m_gameData);

			int vector_size = 0;
			stream.Read(vector_size);
			for (int i = 0; i < vector_size; i++)
			{
				uint64_t player;
				stream.Read(player);
				m_players.push_back(player);
			}
		}

		RakNet::RakString m_gameId;
		RakNet::RakString m_gameData;
		std::vector< int64_t > m_players;
	};


	struct SGameobject
	{
		SGameobject() = default;
		SGameobject(const SGameobject& rh) = default;
		SGameobject& operator=(const SGameobject& rh) = default;
		~SGameobject() = default;


		RakNet::RakString m_name;
		int64_t m_networkId;
		int64_t m_playerId;
		int64_t m_gameobjectType;

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
			stream.Write(m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);

			stream.Write(m_buildingName);
			stream.Write(m_buildingHealth);
			stream.Write(m_buildingLevel);
		}
		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read(m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			stream.Read(m_buildingName);
			stream.Read(m_buildingHealth);
			stream.Read(m_buildingLevel);
		}

		RakNet::RakString m_buildingName;
		RakNet::RakString m_buildingSprite;
		RakNet::RakString m_buildingPredecessorBuilding;

		bool m_buildingCanDetectHidden;

		int64_t m_buildingHealth;
		int64_t m_buildingLevel;
		int64_t m_buildingDefense;
		int64_t m_buildingGoldProduction;
		int64_t m_buildingResearchPointsProduction;
		int64_t m_buildingSightRange;
		int64_t m_buildingGoldCost;
	};
	struct SUnitGameobject : public SGameobject, public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{
			// Write General Information
			stream.Write(m_name);
			stream.Write(m_networkId);
			stream.Write(m_playerId);
			stream.Write(m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);
			stream.Write(m_playerId);

			// Write Unit General Information.
			stream.Write(m_unitName);
			stream.Write(m_unitHealth);
			stream.Write(m_unitArmor);
			stream.Write(m_unitDefense);
			stream.Write(m_unitLevel);
			stream.Write(m_unitExperience);
			stream.Write(m_unitSightRadius);
			stream.Write(m_unitMovementType);
			stream.Write(m_unitActionPoints);

			// Write Unit Attack information based on whether its a melee or ranged unit.
			stream.Write(m_unitMinAttack);
			stream.Write(m_unitMaxAttack);

			stream.Write(m_unitRanged);
			if(m_unitRanged)
			{
				stream.Write(m_unitRangedMinAttack);
				stream.Write(m_unitRangedMaxAttack);
				stream.Write(m_unitRangedMinRange);
				stream.Write(m_unitRangedMaxRange);
			}

			stream.Write(m_unitArmorPiercingWeapon);
			stream.Write(m_unitAntiCavalry);
			if(m_unitAntiCavalry)
			{
				stream.Write(m_unitAntiCavalryBonus);
			}
			stream.Write(m_unitFlankingBonus);
			stream.Write(m_unitBackstabBonus);

			// Serialize abilities and status effects.
			stream.Write(m_unitAbilities.size());
			for (auto abl : m_unitAbilities)
			{
				stream.Write(abl);
			}
			stream.Write(m_unitStartingStatusEffects.size());
			for (auto se : m_unitStartingStatusEffects)
			{
				stream.Write(se);
			}

		}
		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			// Read General Information
			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read(m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			// Read General Unit Information
			stream.Read(m_unitName);
			stream.Read(m_unitHealth);
			stream.Read(m_unitArmor);
			stream.Read(m_unitDefense);
			stream.Read(m_unitLevel);
			stream.Read(m_unitExperience);
			stream.Read(m_unitSightRadius);
			stream.Read(m_unitMovementType);
			stream.Read(m_unitActionPoints);

			// Read Unit Attack definition, based on whether it is a melee or ranged unit
			stream.Read(m_unitMinAttack);
			stream.Read(m_unitMaxAttack);

			stream.Read(m_unitRanged);
			if(m_unitRanged)
			{
				stream.Read(m_unitRangedMinAttack);
				stream.Read(m_unitRangedMaxAttack);
				stream.Read(m_unitRangedMinRange);
				stream.Read(m_unitRangedMaxRange);
			}

			stream.Read(m_unitArmorPiercingWeapon);
			stream.Read(m_unitAntiCavalry);
			if(m_unitAntiCavalry)
			{
				stream.Read(m_unitAntiCavalryBonus);
			}

			stream.Read(m_unitFlankingBonus);
			stream.Read(m_unitBackstabBonus);

			// Deserialize Abilities and Status Effects.
			int abilities_count = 0;
			stream.Read(abilities_count);
			for (int i = 0; i < abilities_count; i++)
			{
				RakNet::RakString ability;
				stream.Read(ability);
				m_unitAbilities.push_back(ability);
			}
			int status_effect_count = 0;
			stream.Read(status_effect_count);
			for (int i = 0; i < status_effect_count; i++)
			{
				RakNet::RakString se;
				stream.Read(se);
				m_unitStartingStatusEffects.push_back(se);
			}
		}

		RakNet::RakString m_unitSprite;
		RakNet::RakString m_unitLayoutTemplate;

		RakNet::RakString m_unitName;
		int64_t m_unitHealth;
		int64_t m_unitArmor;
		int64_t m_unitDefense;
		int64_t m_unitLevel;
		int64_t m_unitExperience;
		int64_t m_unitSightRadius;
		int64_t m_unitMovementType;
		int64_t m_unitActionPoints;
		int64_t m_unitGoldCost;

		int64_t m_unitMinAttack;
		int64_t m_unitMaxAttack;

		bool m_unitRanged;
		int64_t m_unitRangedMinAttack;
		int64_t m_unitRangedMaxAttack;
		int64_t m_unitRangedMinRange;
		int64_t m_unitRangedMaxRange;

		bool m_unitArmorPiercingWeapon = false;
		bool m_unitAntiCavalry = false;
		int64_t m_unitAntiCavalryBonus = 0;
		int64_t m_unitFlankingBonus = 0;
		int64_t m_unitBackstabBonus = 0;

		std::vector< RakNet::RakString > m_unitAbilities;
		std::vector< RakNet::RakString > m_unitStartingStatusEffects;
	};
	struct SMaptileGameobject : public SGameobject, public SSerializable
	{
		void Serialize(RakNet::BitStream& stream) override final
		{

			stream.Write(m_name);
			stream.Write(m_networkId);
			stream.Write(m_playerId);
			stream.Write(m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);
			stream.Write(m_playerId);

			stream.Write(m_maptileBiome);
			stream.Write(m_maptileType);
		}
		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read(m_gameobjectType);
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
			stream.Write(m_gameobjectType);
			stream.Write(m_positionX);
			stream.Write(m_positionY);
			stream.Write(m_playerId);

			stream.Write(m_maptileObjectType);
			stream.Write(m_maptileObjectBiome);
		}
		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_name);
			stream.Read(m_networkId);
			stream.Read(m_playerId);
			stream.Read(m_gameobjectType);
			stream.Read(m_positionX);
			stream.Read(m_positionY);
			stream.Read(m_playerId);

			stream.Read(m_maptileObjectType);
			stream.Read(m_maptileObjectBiome);
		}

		RakNet::RakString m_maptileObjectType;
		RakNet::RakString m_maptileObjectBiome;
	};


	/// @brief Status effects define changes in properties for an Entity: Unit, Building etc.
	// E.g. a poison effect damages a unit each turn.
	struct SStatusEffectData : public SSerializable
	{
		SStatusEffectData() = default;


		void Serialize(RakNet::BitStream& stream) override final
		{
			stream.Write(m_effectName);
			stream.Write(m_effectDisplayName);
			stream.Write(m_effectType);

			stream.Write(m_effectRemovableByResting);

			stream.Write(m_effectValueMin);
			stream.Write(m_effectValueMax);
			stream.Write(m_effectApplicationProbability);

			stream.Write(m_effectApplicableTo);

			stream.Write(m_effectTimerType);

			stream.Write(m_effectTimerValue);

			stream.Write(m_effectDesc);
		}


		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false)  override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_effectName);
			stream.Read(m_effectDisplayName);
			stream.Read(m_effectType);

			stream.Read(m_effectRemovableByResting);

			stream.Read(m_effectValueMin);
			stream.Read(m_effectValueMax);
			stream.Read(m_effectApplicationProbability);

			stream.Read(m_effectApplicableTo);

			stream.Read(m_effectTimerType);

			stream.Read(m_effectTimerValue);

			stream.Read(m_effectDesc);
		}


		/// @brief The name of the Effect. NOT for the user.
		RakNet::RakString m_effectName;
		
		/// @brief Name of the Effect. Displayed to the user.
		RakNet::RakString m_effectDisplayName;

		/// @brief Whether this Effect does 'Damage' or 'Heal' etc.
		RakNet::RakString m_effectType;
		
		/// @brief Whether this Effect is removed by merely resting (units RestAbility) or
		/// if he requires a special Ability to remove it or unremovable at all.
		bool m_effectRemovableByResting;

		/// @brief How much minimally to apply. Whether healing or damaging or other effect type.
		// The Applied value lies between m_effectValueMin and m_effectValueMax.
		int64_t m_effectValueMin;

		/// @brief How much maximally to apply. Whether healing or damaging or other effect type.
		// The Applied value lies between m_effectValueMin and m_effectValueMax.
		int64_t m_effectValueMax;

		/// @brief How probable it is, that the effect will be applied to the target.
		// Number between 1 and 100, where 100 means that the effect is applied in all cases, i.e. 100% probability.
		int64_t m_effectApplicationProbability;

		/// @brief To what kind of entity the effect is applicable. E.g. 'Unit' or 'Townhall' (Building).
		int64_t m_effectApplicableTo;

		/// @brief Either 'Turn' (meaning is applied for a number of turns) or ... .
		RakNet::RakString m_effectTimerType;

		/// @brief For how many turns the effect persists.
		int64_t m_effectTimerValue;

		/// @brief Description of the Effect. Intended for user.
		RakNet::RakString m_effectDesc;

		/// @brief The in-game sprite icon of the Status Effect.
		RakNet::RakString m_effectSprite;

		/// @brief The in-game Behavior Tree implementation name; used to create the appropriate BT for the Status Effect.
		RakNet::RakString m_effectBehaviorTreeImpl;
	};



	/// @brief An Ability defines what a unit or a building can do. Those options can be selected in-game
	// by the player and applied to self or other units/buildings.
	// The SAbilityData structure specifies needed data for an Ability. Operating with that data and relevant
	// data in-game (unit race or Map-Tile type etc.) the Ability executes its function.
	struct SAbilityData : public SSerializable
	{
		SAbilityData() = default;
		

		void Serialize(RakNet::BitStream& stream) override final
		{
			// Serialize Default data.
			stream.Write(m_abilityName);
			stream.Write(m_abilityDisplayName);

			stream.Write(m_abilityApplicableTo);

			stream.Write(m_abilityUsableOnSelf);
			stream.Write(m_abilityUsableOnFriendlies);
			stream.Write(m_abilityUsableOnEnemies);

			stream.Write(m_abilityDesc);

			// Serialize the applied effect vector.
			stream.Write((int64_t)m_appliedStatusEffectsOnUse.size());
			for (auto& status :m_appliedStatusEffectsOnUse)
			{
				stream.Write(status);
			}
		}

		void Deserialize(RakNet::BitStream& stream, bool ignore_id = false) override final
		{
			if (ignore_id) stream.IgnoreBytes(sizeof(RakNet::MessageID));

			stream.Read(m_abilityName);
			stream.Read(m_abilityDisplayName);

			stream.Read(m_abilityApplicableTo);

			stream.Read(m_abilityUsableOnSelf);
			stream.Read(m_abilityUsableOnFriendlies);
			stream.Read(m_abilityUsableOnEnemies);

			stream.Read(m_abilityDesc);

			int64_t vector_size = 0;
			stream.Read(vector_size);
			for (int64_t i = 0; i < vector_size; i++)
			{
				RakNet::RakString status;
				stream.Read(status);

				m_appliedStatusEffectsOnUse.push_back(status);
			}
		}

		/// @brief Name of the Ability. NOT for the user.
		RakNet::RakString m_abilityName;

		/// @brief Name of the Ability. Displayed to the user.
		RakNet::RakString m_abilityDisplayName;

		/// @brief On what kind of Game Objects the ability can be used. E.g. "Attack" can be used on NET_GO_UNIT and
		// "Move" can be used on NET_GO_MAPTILE.
		int64_t m_abilityApplicableTo;

		/// @brief How far someone must be from the user in order to allow us to use on him.
		/// E.g. 0 means on the map tile the unit is standing on.
		int64_t m_abilityMinRange;

		/// @brief How far someone must be from the user in order to allow us to use on him.
		/// E.g. 1 means all the map tiles directly around the unit.
		int64_t m_abilityMaxRange;

		/// @brief How much does it cost for the unit to use this ability? Each ability should have a cost associated with it.
		int64_t m_abilityActionPointCost;

		/// @brief Whether the Ability can be used on the one unit or building using it = self.
		bool m_abilityUsableOnSelf;

		/// @brief Whether the Ability can be used on players friendly units or buildings.
		bool m_abilityUsableOnFriendlies;

		/// @brief Whether the Ability can be used on enemy players units or buildings.
		bool m_abilityUsableOnEnemies;

		/// @brief Holds all the Status Effects that will be applied to the target on use of this Ability.
		// E.g. "AbilitySlash" used on an enemy unit not only takes away his health, but also applies "StatusEffectBleeding" to him.
		std::vector< RakNet::RakString > m_appliedStatusEffectsOnUse;

		/// @brief Description of the Ability. Intended for user.
		RakNet::RakString m_abilityDesc;
	};


	FORCE_INLINE static RakNet::RakString MessageIDTypeToString(RakNet::MessageID id)
	{
		switch (id)
		{
		case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:
			return "id_connection_request_accepted";
		case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
			return "id_disconnection_notification";
		case DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED:
			return "id_connection_attempt_failed";
		case DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS:
			return "id_no_free_incoming_connections";
		case DefaultMessageIDTypes::ID_CONNECTION_BANNED:
			return "id_connection_banned";
		case DefaultMessageIDTypes::ID_ALREADY_CONNECTED:
			return "id_already_connected";
		case EMessageId::NET_MSG_REQUEST_USER_VALIDATION_DATA:
			return "id_request_user_validation_data";
		case EMessageId::NET_MSG_USER_VALIDATION_DATA:
			return "id_user_validation_data";
		case EMessageId::NET_MSG_USER_DATA:
			return "id_user_data";
		case EMessageId::NET_MSG_REQUEST_USER_DATA_UPDATE:
			return "id_request_user_data_update";
		case EMessageId::NET_MSG_CLIENT_ACCEPT:
			return "id_client_accept";
		case EMessageId::NET_MSG_CLIENT_REJECT:
			return "id_client_reject";
		case EMessageId::NET_MSG_REQUEST_ABILITY_AND_STATUS_EFFECTS_DATA:
			return "id_request_ability_and_status_effects_data";
		case EMessageId::NET_MSG_ABILITY_DATA:
			return "id_ability_data";
		case EMessageId::NET_MSG_STATUS_EFFECTS_DATA:
			return "id_status_effects_data";
		case EMessageId::NET_MSG_ABILITY_AND_STATUS_EFFECT_DATA_COMPLETE:
			return "id_ability_and_status_effects_data_complete";
		default:
			return std::to_string((int)id).c_str();
		}
	}
}