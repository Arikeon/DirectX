#include "Engine.h"
#include "Stream.h"
#include <wchar.h>
#include <string>

LRESULT CALLBACK WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
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
		//TInput::GetInstance()->KeyDown(wParam);
	}
	break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		//TInput::GetInstance()->KeyUp(wParam);
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
		//CApplication::ExitProgram();
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

void CEngine::Initialize()
{
	window.ConstructWindow();
	isRunning = true;
}

void CEngine::Start()
{

}

void CEngine::End()
{

}