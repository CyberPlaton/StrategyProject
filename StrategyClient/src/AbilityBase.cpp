#include "AbilityBase.h"


namespace cherrysoda
{

	CEntityAbilityBase::CEntityAbilityBase(Entity* user_entity_of_the_ability) : m_self(user_entity_of_the_ability)
	{
	}

	CEntityAbilityBase::~CEntityAbilityBase()
	{
		m_self = nullptr;
	}

	void CEntityAbilityBase::Use(Entity* target)
	{
		m_last_turn_ability_was_used = CTurnStepMngr::CurrentTurnNumber();
	}

	Entity* CEntityAbilityBase::Self()
	{
		return m_self;
	}

	void CEntityAbilityBase::Data(net::SAbilityData* data)
	{
		m_data.m_abilityName = data->m_abilityName;
		m_data.m_abilityDisplayName = data->m_abilityDisplayName;
		m_data.m_abilityApplicableTo = data->m_abilityApplicableTo;
		m_data.m_abilityUsableOnSelf = data->m_abilityUsableOnSelf;
		m_data.m_abilityUsableOnFriendlies = data->m_abilityUsableOnFriendlies;
		m_data.m_abilityUsableOnEnemies = data->m_abilityUsableOnEnemies;
		m_data.m_abilityDesc = data->m_abilityDesc;

		for(auto& se: data->m_appliedStatusEffectsOnUse)
		{
			m_data.m_appliedStatusEffectsOnUse.push_back(se);
		}
	}

	String CEntityAbilityBase::Name()
	{
		return "ABL_Ability_Base";
	}

	bool CEntityAbilityBase::CheckConditions(Entity* target)
	{
		// Assure that the Ability was not already used this turn.
		if (WasUsedThisTurn()) return false;

		// Assure caster has enough AP for the ability.
		if (!HasEnoughActionPoints()) return false;

		// Assure the ability is applicable to Target entity.
		if (!IsApplicableTo(target)) return false;

		// Assure caster fulfills the range requirements set by designer.
		if (!IsTargetInRange(target)) return false;

		// Assure caster is allowed to use ability on the Target.
		if (!IsTargetValid(target)) return false;

		return true;
	}

	bool CEntityAbilityBase::CheckConditionsAllowMultipleUse(Entity* target)
	{
		// Assure caster has enough AP for the ability.
		if (!HasEnoughActionPoints()) return false;

		// Assure the ability is applicable to Target entity.
		if (!IsApplicableTo(target)) return false;

		// Assure caster fulfills the range requirements set by designer.
		if (!IsTargetInRange(target)) return false;

		// Assure caster is allowed to use ability on the Target.
		if (!IsTargetValid(target)) return false;

		return true;
	}

	bool CEntityAbilityBase::WasUsedThisTurn() const
	{
		return m_last_turn_ability_was_used == CTurnStepMngr::CurrentTurnNumber();
	}

	bool CEntityAbilityBase::HasEnoughActionPoints() const
	{
		auto self_unit = m_self->Get < Unit >();
		return self_unit->GetActionPoints() >= m_data.m_abilityActionPointCost;
	}

	bool CEntityAbilityBase::IsTargetValid(Entity* target)
	{
		if (m_data.m_abilityUsableOnSelf)
		{
			if (IsTargetSelf(target)) return true;
		}
		else if (m_data.m_abilityUsableOnFriendlies)
		{
			if (!IsTargetFriendly(target)) return true;
		}
		else if (m_data.m_abilityUsableOnEnemies)
		{
			if (!IsTargetEnemy(target)) return true;
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][CEntityAbilityBase::IsTargetValid] Ability \"{}\": No usability specification!", APP_RUN_TIME, Name().c_str());
			LOG_GAME_ERROR("[%.4f][CEntityAbilityBase::IsTargetValid] Ability \"%s\": No usability specification!", APP_RUN_TIME, Name().c_str());
			LOG_FILE_ERROR("[{:.4f}][CEntityAbilityBase::IsTargetValid] Ability \"{}\": No usability specification!", APP_RUN_TIME, Name().c_str());
		}


		return false;
	}

	bool CEntityAbilityBase::IsTargetSelf(Entity* target)
	{
		return m_self->TagFullCheck(target->Tag());
	}

	bool CEntityAbilityBase::IsTargetFriendly(Entity* target)
	{
		auto self_unit = m_self->Get< Unit >();
		auto target_unit = target->Get< Unit >();

		return self_unit->GetPlayerId() == target_unit->GetPlayerId();
	}

	bool CEntityAbilityBase::IsTargetEnemy(Entity* target)
	{
		auto self_unit = m_self->Get< Unit >();
		auto target_unit = target->Get< Unit >();

		return self_unit->GetPlayerId() != target_unit->GetPlayerId();
	}

	bool CEntityAbilityBase::IsApplicableTo(Entity* target)
	{
		// Same as net::EGameobjectType: 0=Maptile, 1=Mapobject, 2=Unit, 3=Building. 
		auto entity_type = m_data.m_abilityApplicableTo;
		bool applicable = false;

		switch(entity_type)
		{
		case 0: // Check whether Target is a Maptile.
		{
			// TODO
			LOG_DBG_WARN("[{:.4f}][CEntityAbilityBase::IsApplicableTo] Ability \"{}\": Cannot use Ability on Maptile, not implemented!", APP_RUN_TIME, Name().c_str());
			LOG_GAME_WARN("[%.4f][CEntityAbilityBase::IsApplicableTo] Ability \"%s\": Cannot use Ability on Maptile, not implemented!", APP_RUN_TIME, Name().c_str());
			LOG_FILE_WARN("[{:.4f}][CEntityAbilityBase::IsApplicableTo] Ability \"{}\": Cannot use Ability on Maptile, not implemented!", APP_RUN_TIME, Name().c_str());
			return false;
		}
		case 1: // Check whether Target is a Mapobject, like Forest or Mountain etc.
		{
			// TODO
			LOG_DBG_WARN("[{:.4f}][CEntityAbilityBase::IsApplicableTo] Ability \"{}\": Cannot use Ability on Mapobject, not implemented!", APP_RUN_TIME, Name().c_str());
			LOG_GAME_WARN("[%.4f][CEntityAbilityBase::IsApplicableTo] Ability \"%s\": Cannot use Ability on Mapobject, not implemented!", APP_RUN_TIME, Name().c_str());
			LOG_FILE_WARN("[{:.4f}][CEntityAbilityBase::IsApplicableTo] Ability \"{}\": Cannot use Ability on Mapobject, not implemented!", APP_RUN_TIME, Name().c_str());
			return false;
		}
		case 2: // Check whether Target is a Unit.
		{
			applicable = target->Get< Unit >() != nullptr;
			break;
		}
		case 3: // Check whether Target is a Building.
		{
			applicable = target->Get< Building >() != nullptr;
			break;
		}
		default:
		{
			LOG_DBG_ERROR("[{:.4f}][CEntityAbilityBase::IsApplicableTo] Ability \"{}\": Unknown Entity Type specified!", APP_RUN_TIME, Name().c_str());
			LOG_GAME_ERROR("[%.4f][CEntityAbilityBase::IsApplicableTo] Ability \"%s\": Unknown Entity Type specified!", APP_RUN_TIME, Name().c_str());
			LOG_FILE_ERROR("[{:.4f}][CEntityAbilityBase::IsApplicableTo] Ability \"{}\": Unknown Entity Type specified!", APP_RUN_TIME, Name().c_str());
			return false;
		}
		}


		return applicable;
	}

	bool CEntityAbilityBase::IsTargetInRange(Entity* target)
	{
		// Quick Hack.
		// We need a structure from which Unit and Building inherit...
		long long self_tile_x =		-INT_MAX;
		long long self_tile_y =		-INT_MAX;
		long long target_tile_x =	-INT_MAX;
		long long target_tile_y =	-INT_MAX;

		// Get our Tile position.
		if(auto self_unit = m_self->Get< Unit >(); self_unit)
		{
			self_tile_x = self_unit->GetTilePositionX();
			self_tile_y = self_unit->GetTilePositionY();
		}
		else if(auto self_build = m_self->Get< Building >(); self_build)
		{
			self_tile_x = self_build->GetTilePositionX();
			self_tile_y = self_build->GetTilePositionY();
		}

		// Get other Tile position.
		if (auto target_unit = target->Get< Unit >(); target_unit)
		{
			target_tile_x = target_unit->GetTilePositionX();
			target_tile_y = target_unit->GetTilePositionY();
		}
		else if (auto target_build = target->Get< Building >(); target_build)
		{
			target_tile_x = target_build->GetTilePositionX();
			target_tile_y = target_build->GetTilePositionY();
		}



		// Check for Ranged conditions, Compute Euclidian distance.
		size_t dist = std::sqrt((target_tile_x - self_tile_x) * (target_tile_x - self_tile_x) + (target_tile_y - self_tile_y) * (target_tile_y - self_tile_y));
		
		if (dist >= m_data.m_abilityMinRange && dist <= m_data.m_abilityMaxRange) return true;


		return false;
	}

}