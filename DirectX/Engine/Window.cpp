#include "Window.h"
#include "Stream.h"

//Engine.cpp
extern LRESULT CALLBACK WndProc(HWND _hWnd, uint32_t _msg, WPARAM _wParam, LPARAM _lParam);

bool TWindow::ConstructWindow()
{
	LPCWSTR dir = L"DirectX/Defaults.ini";
	LPCWSTR sectionname = L"Startup";
	std::wstring AppName = L"NULL";
	Stream::ReadIniString(dir, sectionname, L"WindowName", AppName);

	hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = AppName.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	//Read from Default.ini
	Stream::ReadIniInt(dir, sectionname, L"WindowPosX", posX);
	Stream::ReadIniInt(dir, sectionname, L"WindowPosY", posY);
	Stream::ReadIniInt(dir, sectionname, L"WindowWidth", width);
	Stream::ReadIniInt(dir, sectionname, L"WindowHeight", height);
	Stream::ReadIniBool(dir, sectionname, L"FullScreen", isFullScreen);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"RegisterClassEx() failed", L"Error", MB_OK);
		return false;
	}

	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;

	hWND = CreateWindowEx(
		WS_EX_APPWINDOW,
		AppName.c_str(),
		AppName.c_str(),
		nStyle,
		posX,
		posY,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);

	//Success
	if (hWND == NULL)
	{
		return false;
	}

	ShowWindow(hWND, SW_SHOW);
	SetForegroundWindow(hWND);
	SetFocus(hWND);
	return true;
}

void SetFullScreen(bool isFullScreen)
{

}