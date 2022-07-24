#include "UnitLevelProgressionMngr.h"

namespace cherrysoda
{
	void CUnitLevelProgressionMngr::LevelUp(Unit* unit)
	{
		auto type = unit->GetLevelProgressionType();

		switch(type)
		{
		case 0: // Melee
		{
			_levelUpMeele(unit);
			break;
		}

		case 1: // Ranged
		{
			_levelUpRanged(unit);
			break;
		}

		case 2: // Mage
		{
			_levelUpMage(unit);
			break;
		}
		
		default:
		{

			LOG_DBG_CRITICAL("[{:.4f}][CUnitLevelProgressionMngr::LevelUp] Invalid Level Progression Type: \"{}\" in \"{}\"!", APP_RUN_TIME, type, unit->GetUnitName().c_str());
			LOG_FILE_CRITICAL("[{:.4f}][CUnitLevelProgressionMngr::LevelUp] Invalid Level Progression Type: \"{}\" in \"{}\"!", APP_RUN_TIME, type, unit->GetUnitName().c_str());
			LOG_GAME_CRITICAL("[%.4f][CUnitLevelProgressionMngr::LevelUp] Invalid Level Progression Type: \"%zu\" in \"%s\"!", APP_RUN_TIME, type, unit->GetUnitName().c_str());
			break;
		}
		}
	}

	void CUnitLevelProgressionMngr::_levelUpMeele(Unit* unit)
	{
		auto minA = unit->GetMinAttack();
		auto maxA = unit->GetMaxAttack();
		auto def = unit->GetDefense();
		auto ap = unit->GetActionPoints();
		auto lvl = unit->GetLevel();

		unit->SetMinAttack(minA		+ 3);
		unit->SetMaxAttack(maxA		+ 5);
		unit->SetDefense(def		+ 5);
		unit->SetActionPoints(ap	+ 3);
		unit->SetLevel(lvl			+ 1);
	}

	void CUnitLevelProgressionMngr::_levelUpRanged(Unit* unit)
	{
		auto minA = unit->GetRangedMinAttack();
		auto maxA = unit->GetRangedMaxAttack();
		auto def = unit->GetDefense();
		auto ap = unit->GetActionPoints();
		auto lvl = unit->GetLevel();

		unit->SetRangedMinAttack(minA + 3);
		unit->SetRangedMaxAttack(maxA + 5);
		unit->SetDefense(def + 5);
		unit->SetActionPoints(ap + 3);
		unit->SetLevel(lvl + 1);
	}

	void CUnitLevelProgressionMngr::_levelUpMage(Unit* unit)
	{
		auto def = unit->GetDefense();
		auto ap = unit->GetActionPoints();
		auto lvl = unit->GetLevel();

		unit->SetDefense(def + 5);
		unit->SetActionPoints(ap + 6);
		unit->SetLevel(lvl + 1);
	}

}
