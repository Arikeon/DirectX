#pragma once
#include "Build.h"
#include "Window.h"
#include <memory>
#include <thread>

class CRenderer;
class CScene;

class CEngine
{
	friend int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow);

	~CEngine();

	void Initialize();
	void Update();
	void Restart();

	TWindow m_window;
	CRenderer* m_renderer;
	CScene* m_scene;
	bool m_isRunning, isRestarting;
};