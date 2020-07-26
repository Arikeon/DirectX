#include "Build.h"
#include "Engine.h"
#include <Windows.h>

/*
*	WinMain
*/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//Add line# where the memory leak originated
	MEMORY_LEAK_START(0);
	CEngine Engine;
	Engine.Initialize();

	if (Engine.isRunning)
	{
		Engine.Start();
		Engine.Update();
	}
	else
	{
		CONSOLE_LOG("Failed to start...")
		return 0;
	}

	Engine.End();
	MEMORY_LEAK_END;
	return 1;
}

