#include "Main.h"

void MainRender(GameEditor* editor)
{
	editor->OnUserUpdate((float)1/ 60);
}

int main()
{
	GameEditor demo;
	if (demo.Construct(1360, 768, 1, 1))
		demo.Start();

	return 0;
}