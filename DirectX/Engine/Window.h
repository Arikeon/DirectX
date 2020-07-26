#pragma once
#include "Build.h"
#include <Window.h>

struct TWindow
{
	HWND hWND;
	HMODULE hInstance;
	WNDCLASSEX wc;
	MSG message;

	int posX, posY, width, height;
	bool isFullScreen, isInitialized = false;

	//Grabs defaults from DirectX/Defaults.ini
	bool ConstructWindow(int inposX = -1, int inposY = -1, int inwidth = -1, int inheight = -1);
	void SetFullScreen(bool isFullScreen);
};