#pragma once
#include "Build.h"
#include "Window.h"

class CEngine
{
	friend int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow);

	void Initialize();
	void Start();
	void Update();
	void Restart();
	void End();

	TWindow window;
	bool isRunning, isRestarting;
};