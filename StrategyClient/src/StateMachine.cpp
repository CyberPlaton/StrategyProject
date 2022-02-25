#include "StateMachine.h"

void StateMachine::Update()
{
	if (m_currentState)
	{
		m_currentState->OnUpdate();
	}
}

