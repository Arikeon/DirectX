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
	bool isFullScreen;

	//Grabs defaults from DirectX/Defaults.ini
	bool ConstructWindow();
	void SetFullScreen(bool isFullScreen);
};