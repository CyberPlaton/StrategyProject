#include "TurnStepMngr.h"

CTurnStepMngr* CTurnStepMngr::g_TurnStepMngr = nullptr;
bool CTurnStepMngr::g_clientHasTurn = false;

bool CTurnStepMngr::Initialize()
{
	return true;
}

void CTurnStepMngr::Terminate()
{
	CTurnStepMngr::del();
}

void CTurnStepMngr::EndTurn()
{
	cherrysoda::CEntityStatusEffectMngr::OnTurnEnd();
	CNetCommMngr::CreateAndDispatchMasterServerMessage(net::EMessageId::NET_MSG_TURN_END);
}

void CTurnStepMngr::StartTurn()
{
	g_clientHasTurn = true;
}

bool CTurnStepMngr::ClientHasTurn()
{
	return g_clientHasTurn;
}
