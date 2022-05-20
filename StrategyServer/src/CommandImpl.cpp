#include "CommandImpl.h"

void CommandDebug::Execute()
{
	m_output.append("CommandDebug!Executing Debug Command\n");
	m_finished = true;
}

bool CommandDebug::Persistent()
{
	return false;
}

std::string CommandDebug::RetrieveCommandOutput()
{
	return m_output;
}

bool CommandDebug::ExecuteFinished()
{
	return m_finished;
}
