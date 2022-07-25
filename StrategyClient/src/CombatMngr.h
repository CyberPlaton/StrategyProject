#ifndef _COMBAT_MNGR_H_
#define _COMBAT_MNGR_H_
#pragma once

#include "CommonDefines.h"
#include "Logging.h"

#include "Entity.h"
#include "Unit.h"


namespace cherrysoda
{
	class CCombatMngr
	{
	public:
		STATIC_GET_DEL(CCombatMngr, g_pCCombatMngr);


		static void DoMeleeAttack();
		
		static void DoRangedAttack();
		
		static void DoMagicalAttack();


	private:
		static CCombatMngr* g_pCCombatMngr;
	};

}

#endif
