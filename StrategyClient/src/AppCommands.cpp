#include "AppCommands.h"

void TestCommandBatchFunction(const std::vector< cherrysoda::String >& args)
{
	printf("TestCommandBatchFunction\n");
	for (auto& t : args)
	{
		printf("\t\t%s\n", t);
	}
}