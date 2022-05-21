#pragma once

#include "MasterServer.h"
#include "DBMS.h"

class CommandDebug : public ICommand
{
public:
	CommandDebug(uint64_t param1, std::string param2) : m_param1(param1), m_param2(param2)
	{
	}

	void Execute() override final;
	bool Persistent() override final;
	std::string RetrieveCommandOutput() override final;
	bool ExecuteFinished() override final;

private:
	bool m_finished;
	std::string m_output;

	uint64_t m_param1;
	std::string m_param2;
};