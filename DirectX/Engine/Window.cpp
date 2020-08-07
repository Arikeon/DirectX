#include "Window.h"
#include "Stream.h"

//Engine.cpp
extern LRESULT CALLBACK WndProc(HWND _hWnd, uint32 _msg, WPARAM _wParam, LPARAM _lParam);

bool TWindow::ConstructWindow(int inposX, int inposY, int inwidth, int inheight)
{
	LPCWSTR dir = Stream::GetDefaultDir();
	LPCWSTR sectionname = L"Startup";
	std::wstring AppName = L"NULL";
	Stream::ReadIniString(dir, sectionname, L"WindowName", AppName);

	FOV = (float)Stream::ReadIniInt(dir, L"Startup", L"FOV");

	m_hInstance = GetModuleHandle(NULL);
	m_wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	m_wc.lpfnWndProc = WndProc;
	m_wc.cbClsExtra = 0;
	m_wc.cbWndExtra = 0;
	m_wc.hInstance = m_hInstance;
	m_wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	m_wc.hIconSm = m_wc.hIcon;
	m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//m_wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	m_wc.lpszMenuName = NULL;
	m_wc.lpszClassName = AppName.c_str();
	m_wc.cbSize = sizeof(WNDCLASSEX);

	if (inposX == -1 || inposY == -1 || inwidth == -1 || inheight == -1)
	{
		//Read from Default.ini
		Stream::ReadIniInt(dir, sectionname, L"WindowPosX", m_posX);
		Stream::ReadIniInt(dir, sectionname, L"WindowPosY", m_posY);
		Stream::ReadIniInt(dir, sectionname, L"WindowWidth", m_width);
		Stream::ReadIniInt(dir, sectionname, L"WindowHeight", m_height);
	}
	else
	{
		m_posX = inposX;
		m_posY = inposY;
		m_width = inwidth;
		m_height = inheight;
	}

	if (!RegisterClassEx(&m_wc))
	{
		MessageBox(NULL, L"RegisterClassEx() failed", L"Error", MB_OK);
		return false;
	}

	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;

	m_hWND = CreateWindowEx(
		WS_EX_APPWINDOW,
		AppName.c_str(),
		AppName.c_str(),
		nStyle,
		m_posX,
		m_posY,
		m_width,
		m_height,
		NULL,
		NULL,
		m_hInstance,
		NULL);

	//Success
	if (m_hWND == NULL)
	{
		return false;
	}

	ShowWindow(m_hWND, SW_SHOW);
	SetForegroundWindow(m_hWND);
	SetFocus(m_hWND);

	if (!m_isInitialized)
		Stream::ReadIniBool(dir, sectionname, L"FullScreen", m_isFullScreen);
	SetFullScreen(m_isFullScreen);

	HideCursor();

	m_isInitialized = true;
	return true;
}

void TWindow::SetFullScreen(bool isFullScreen)
{
	//TODO Incomplete - returning to windowed loses window borders
	WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
	DWORD dwStyle = GetWindowLong(m_hWND, GWL_STYLE);
	if (isFullScreen) {
		SetWindowLong(m_hWND, GWL_STYLE, GetWindowLong(m_hWND, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
		SetWindowLong(m_hWND, GWL_EXSTYLE, GetWindowLong(m_hWND, GWL_EXSTYLE) & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
		MONITORINFO monitor_info;
		monitor_info.cbSize = sizeof(monitor_info);
		GetMonitorInfo(MonitorFromWindow(m_hWND, MONITOR_DEFAULTTONEAREST), &monitor_info);
		RECT window_rect(monitor_info.rcMonitor);
		SetWindowPos(m_hWND, NULL, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(m_hWND, GWL_STYLE, GetWindowLong(m_hWND, GWL_STYLE));
		SetWindowLong(m_hWND, GWL_EXSTYLE, GetWindowLong(m_hWND, GWL_EXSTYLE));
		SetWindowPos(m_hWND, NULL, m_posX, m_posY, m_width, m_height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	}
	HideCursor();
}

void TWindow::ResizeWindow(int inposX, int inposY, int inwidth, int inheight)
{
	//TODO finish
	const LPCWSTR dir = Stream::GetDefaultDir();
	const LPCWSTR sectionname = L"Startup";

	if (inposX == -1 || inposY == -1 || inwidth == -1 || inheight == -1)
	{
		//Read from Default.ini
		Stream::ReadIniInt(dir, sectionname, L"WindowPosX", m_posX);
		Stream::ReadIniInt(dir, sectionname, L"WindowPosY", m_posY);
		Stream::ReadIniInt(dir, sectionname, L"WindowWidth", m_width);
		Stream::ReadIniInt(dir, sectionname, L"WindowHeight", m_height);
	}
	else
	{
		m_posX = inposX;
		m_posY = inposY;
		m_width = inwidth;
		m_height = inheight;
	}
	HideCursor();
}

RECT TWindow::GetRECT()
{
	RECT size;

	size.left = m_posX;
	size.top = m_posY;
	size.right = m_width;
	size.bottom = m_height;

	return size;
}

void TWindow::HideCursor()
{
	ShowCursor(false);
	RECT r;
	GetWindowRect(m_hWND, &r);
	ClipCursor(&r);
}