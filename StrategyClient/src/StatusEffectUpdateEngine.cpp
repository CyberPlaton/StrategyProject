#include "StatusEffectUpdateEngine.h"


namespace cherrysoda
{
	void StatusEffectScriptingEngine::DoUpdate(EUpdateOperation operation, EUpdateField field, cherrysoda::Entity* entity, size_t update_value)
	{
		auto engine = StatusEffectScriptingEngine::get();
		Unit* unit = nullptr;
		if(unit = entity->Get< Unit >(); unit != nullptr) // Check that Entity has unit component.
		{
			switch(field)
			{
			case EUpdateField::UF_DEFENSE:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_defense, update_value);
				break;
			}


			case EUpdateField::UF_HEALTH:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_health, update_value);
				break;
			}


			case EUpdateField::UF_MIN_ATTACK:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_minAttack, update_value);
				break;
			}


			case EUpdateField::UF_MAX_ATTACK:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_maxAttack, update_value);
				break;
			}


			case EUpdateField::UF_ARMOR:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_armor, update_value);
				break;
			}


			case EUpdateField::UF_MOVEMENT_TYPE:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_movementType, update_value);
				break;
			}


			case EUpdateField::UF_ACTION_POINTS:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_actionPoints, update_value);
				break;
			}


			case EUpdateField::UF_RANGED_MIN_ATTACK:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_rangedMinAttack, update_value);
				break;
			}


			case EUpdateField::UF_RANGED_MAX_ATTACK:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_rangedMaxAttack, update_value);
				break;
			}


			case EUpdateField::UF_RANGED_MIN_RANGE:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_rangedMinRange, update_value);
				break;
			}


			case EUpdateField::UF_RANGED_MAX_RANGE:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_rangedMaxRange, update_value);
				break;
			}



			case EUpdateField::UF_SIGHT_RANGE:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_sightRadius, update_value);
				break;
			}


			case EUpdateField::UF_EXPIRIENCE:
			{
				engine->_doOperationOnField<size_t>(operation, unit->m_experience, update_value);
				break;
			}


			default:
			{
				LOG_GAME_ERROR("[%.4f][StatusEffectScriptingEngine::DoUpdate] Unable to Perform Operation: Field \"%d\" does not exist!", APP_RUN_TIME, field);
				LOG_DBG_ERROR("[{:.4f}][StatusEffectScriptingEngine::DoUpdate] Unable to Perform Operation: Field \"{}\" does not exist!", APP_RUN_TIME, field);
				LOG_FILE_ERROR("[{:.4f}][StatusEffectScriptingEngine::DoUpdate] Unable to Perform Operation: Field \"{}\" does not exist!", APP_RUN_TIME, field);

				break;
			}
			}
		}
	}
}
