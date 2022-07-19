#ifndef _STATUS_EFFECT_UPDATE_ENGINE_H_
#define _STATUS_EFFECT_UPDATE_ENGINE_H_
#pragma once

#include "CommonDefines.h"
#include "Entity.h"
#include "Unit.h"
#include "Logging.h"


namespace cherrysoda
{
	/// @brief 
	class StatusEffectScriptingEngine
	{
	public:
		enum EUpdateOperation
		{
			UO_ADD,
			UO_SUBTRACT,
			UO_MULTIPLY,
			UO_DIVIDE,
			UO_SET,
		};
		enum EUpdateField
		{
			UF_DEFENSE,
			UF_HEALTH,
			UF_MIN_ATTACK,
			UF_MAX_ATTACK,
			UF_ARMOR,
			UF_MOVEMENT_TYPE,
			UF_ACTION_POINTS,
			UF_RANGED_MIN_ATTACK,
			UF_RANGED_MAX_ATTACK,
			UF_RANGED_MIN_RANGE,
			UF_RANGED_MAX_RANGE,
			UF_SIGHT_RANGE,
			UF_EXPIRIENCE,
		};

	public:
		STATIC_GET_DEL(StatusEffectScriptingEngine, g_StatusEffectScriptingEngine);


		/// @brief Performs an operation on an Entities Unit component altering its state.
		/// @param operation The kind of operation to perform.
		/// @param field On which field to perform the operation.
		/// @param unit The Entity containing Unit Component on which to perform the update operation.
		/// @param update_value The value with which to perform the operation.
		static void DoUpdate(EUpdateOperation operation, EUpdateField field, cherrysoda::Entity* entity, size_t update_value);


	private:
		static StatusEffectScriptingEngine* g_StatusEffectScriptingEngine;


	private:

		template< class T >
		void _doOperationOnField(EUpdateOperation operation, T& field, T& value);
	};






	template< class T >
	void cherrysoda::StatusEffectScriptingEngine::_doOperationOnField(EUpdateOperation operation, T& field, T& value)
	{
		switch (operation)
		{
		case cherrysoda::StatusEffectScriptingEngine::UO_ADD:
		{
			field += value;
			break;
		}
		case cherrysoda::StatusEffectScriptingEngine::UO_SUBTRACT:
		{
			field -= value;
			break;
		}
		case cherrysoda::StatusEffectScriptingEngine::UO_MULTIPLY:
		{
			field = field * value;
			break;
		}
		case cherrysoda::StatusEffectScriptingEngine::UO_DIVIDE:
		{
			field = field / value;
			break;
		}
		case cherrysoda::StatusEffectScriptingEngine::UO_SET:
		{
			field = value;
			break;
		}
		default:
		{
			LOG_GAME_ERROR("[%.4f][StatusEffectScriptingEngine::_doOperationOnField] Unable to Perform Operation: Operation \"%d\" does not exist!", APP_RUN_TIME, operation);
			LOG_DBG_ERROR("[{:.4f}][StatusEffectScriptingEngine::_doOperationOnField] Unable to Perform Operation: Operation \"{}\" does not exist!", APP_RUN_TIME, operation);
			LOG_FILE_ERROR("[{:.4f}][StatusEffectScriptingEngine::_doOperationOnField] Unable to Perform Operation: Operation \"{}\" does not exist!", APP_RUN_TIME, operation);

			break;
		}
		}
	}

}

#endif