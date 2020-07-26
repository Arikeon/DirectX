#include "Window.h"
#include "Stream.h"

//Engine.cpp
extern LRESULT CALLBACK WndProc(HWND _hWnd, uint32_t _msg, WPARAM _wParam, LPARAM _lParam);

bool TWindow::ConstructWindow(int inposX, int inposY, int inwidth, int inheight)
{
	LPCWSTR dir = Stream::GetDefaultDir();
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

	if (inposX == -1 || inposY == -1 || inwidth == -1 || inheight == -1)
	{
		//Read from Default.ini
		Stream::ReadIniInt(dir, sectionname, L"WindowPosX", posX);
		Stream::ReadIniInt(dir, sectionname, L"WindowPosY", posY);
		Stream::ReadIniInt(dir, sectionname, L"WindowWidth", width);
		Stream::ReadIniInt(dir, sectionname, L"WindowHeight", height);
	}
	else
	{
		posX = inposX;
		posY = inposY;
		width = inwidth;
		height = inheight;
	}

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

	if (!isInitialized)
		Stream::ReadIniBool(dir, sectionname, L"FullScreen", isFullScreen);
	SetFullScreen(isFullScreen);

	isInitialized = true;
	return true;
}

void TWindow::SetFullScreen(bool isFullScreen)
{
	//TODO Incomplete - returning to windowed loses window borders
	WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
	DWORD dwStyle = GetWindowLong(hWND, GWL_STYLE);
	if (isFullScreen) {
		SetWindowLong(hWND, GWL_STYLE, GetWindowLong(hWND, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
		SetWindowLong(hWND, GWL_EXSTYLE, GetWindowLong(hWND, GWL_EXSTYLE) & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
		MONITORINFO monitor_info;
		monitor_info.cbSize = sizeof(monitor_info);
		GetMonitorInfo(MonitorFromWindow(hWND, MONITOR_DEFAULTTONEAREST), &monitor_info);
		RECT window_rect(monitor_info.rcMonitor);
		SetWindowPos(hWND, NULL, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(hWND, GWL_STYLE, GetWindowLong(hWND, GWL_STYLE));
		SetWindowLong(hWND, GWL_EXSTYLE, GetWindowLong(hWND, GWL_EXSTYLE));
		SetWindowPos(hWND, NULL, posX, posY, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	}
}