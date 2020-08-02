#include "Build.h"
#include "Engine.h"
#include <Windows.h>
#include "Colors.h"

const float3 TColors::Black		= float3(0, 0, 0);
const float3 TColors::Red		= float3(1, 0, 0);
const float3 TColors::Green		= float3(0, 1, 0);
const float3 TColors::Blue		= float3(0, 0, 1);
const float3 TColors::Yellow	= float3(1, 1, 0);
const float3 TColors::Purple	= float3(1, 0, 1);
const float3 TColors::Cyan		= float3(0, 1, 1);
const float3 TColors::White		= float3(1, 1, 1);

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
	{
		CEngine Engine;
		Engine.Initialize();

		if (Engine.m_isRunning)
		{
			Engine.Update();
		}
		else
		{
			CONSOLE_LOG(L"Failed to start...")
				return 0;
		}
	}
	MEMORY_LEAK_END;
	return 1;
}

