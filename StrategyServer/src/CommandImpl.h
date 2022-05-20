#pragma once

#include "MasterServer.h"
#include "DBMS.h"

class CommandDebug : public ICommand
{
public:
	void Execute() override final;
	bool Persistent() override final;
	std::string RetrieveCommandOutput() override final;
	bool ExecuteFinished() override final;

private:
	bool m_finished;
	std::string m_output;
};