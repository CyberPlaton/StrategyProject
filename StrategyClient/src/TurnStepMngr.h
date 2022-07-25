#ifndef _TURN_STEP_MNGR_H_
#define _TURN_STEP_MNGR_H_
#pragma once

// Calling CEntityStatusEffectMngr OnTurnEnd function.
#include "Status.h"
#include "CommonDefines.h"
#include "Logging.h"
#include "NetCommMngr.h"


/// @brief CTurnStepMngr is the central Object to query whether this client has turn or
/// indicating other players and the servers that this client has ended his turn.
class CTurnStepMngr
{
public:
	STATIC_GET_DEL(CTurnStepMngr, g_TurnStepMngr);

	static bool Initialize();
	static void Terminate();

	/// @brief End the turn for the client and inform the master server, and call an update on all status effects for the units/buildings etc.
	static void EndTurn();

	static void StartTurn();

	static bool ClientHasTurn();

private:
	static CTurnStepMngr* g_TurnStepMngr;
	static bool g_clientHasTurn;

private:
	CTurnStepMngr() = default;
};

#endif