#pragma once


class ICommand
{
public:
	virtual void Execute() = 0;
	virtual bool ExecuteFinished() = 0;
	virtual bool Persistent() = 0;
	virtual std::string RetrieveCommandOutput() = 0;
};