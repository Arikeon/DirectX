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
	}
	else
	{
		CONSOLE_LOG("Invalid Params");
		return 0;
	}

	Engine.End();
	MEMORY_LEAK_END;
	return 1;
}

LRESULT CALLBACK WndProc(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
		///////////////
		//Keyboard Inputs
		///////////////
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
	}
	break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
	}
	break;

	///////////////
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_CLOSE:
	{
	}
	break;

	default:
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	break;
	};
	return 0;
}
