#include "Engine.h"
#include "Renderer.h"
#include "Stream.h"
#include "Time.h"
#include "Input.h"
#include "Algorithm.h"
#include <wchar.h>
#include <string>

CEngine::~CEngine()
{
	delete m_renderer;

	TInput::DeleteInstance();
}

void CEngine::Initialize()
{
	m_window.ConstructWindow();
	m_isRunning = true;

	m_renderer = new CRenderer;
	m_renderer->Initialize(m_window);
}

void CEngine::Update()
{
	TTime Time;
	MSG WndEvent = {};

	while (m_isRunning)
	{
		Time.Tick();
		if (isRestarting)
		{
			Time.Restart();
			Restart();
		}

		//Recieve window event
		while (PeekMessage(&WndEvent, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&WndEvent);
			DispatchMessage(&WndEvent);
		}

		m_renderer->Update(Time.m_deltaTime);

		//Check if shutdown bit
		{
			if (InputInstance()->m_isExiting ||
				InputIsKeyDown(VK_ESCAPE))
			{
				m_isRunning = false;
			}
		}
	}
}

void CEngine::Restart()
{
	isRestarting = false;
}

LRESULT CALLBACK WndProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		InputInstance()->KeyDown(wParam);
	}
	break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		InputIsKeyUp((uint32)wParam);
	}
	break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_CLOSE:
	{
		InputInstance()->m_isExiting = true;
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
