#include "CommandImpl.h"

void CommandDebug::Execute()
{
	m_output.append("CommandDebug!Executing Debug Command: param1=");
	m_output.append(std::to_string(m_param1));
	m_output.append(", param2=");
	m_output.append(m_param2);
	m_output.append("\n");
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
