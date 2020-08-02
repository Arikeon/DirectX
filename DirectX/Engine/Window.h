#pragma once
#include "Build.h"
#include <Window.h>

struct TWindow
{
	HWND m_hWND;
	HMODULE m_hInstance;
	WNDCLASSEX m_wc;
	MSG m_message;

	float FOV;
	int m_posX, m_posY, m_width, m_height;
	bool m_isFullScreen, m_isInitialized = false;

	//Grabs defaults from DirectX/Defaults.ini
	bool ConstructWindow(int inposX = -1, int inposY = -1, int inwidth = -1, int inheight = -1);
	void SetFullScreen(bool isFullScreen);
	void ResizeWindow(int inposX = -1, int inposY = -1, int inwidth = -1, int inheight = -1);
	RECT GetRECT();
	void HideCursor();
};