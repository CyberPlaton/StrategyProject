#ifndef _UNIT_LEVEL_PROGRESSION_MNGR_H_
#define _UNIT_LEVEL_PROGRESSION_MNGR_H_
#pragma once

#include "Unit.h"
#include "Logging.h"

namespace cherrysoda
{
	class CUnitLevelProgressionMngr
	{
	public:
		STATIC_GET_DEL(CUnitLevelProgressionMngr, g_pCUnitLevelProgressionMngr);

		/// @brief Perform a Level up on the given unit.
		/// @param unit Pointer to Unit Component.
		void LevelUp(Unit* unit);


	private:
		static CUnitLevelProgressionMngr* g_pCUnitLevelProgressionMngr;


	private:
		void _levelUpMeele(Unit* unit);
		void _levelUpRanged(Unit* unit);
		void _levelUpMage(Unit* unit);
	};
}


#endif