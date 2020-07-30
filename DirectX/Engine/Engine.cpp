#include "Engine.h"
#include "Renderer.h"
#include "Stream.h"
#include "Time.h"
#include "Input.h"
#include "Algorithm.h"
#include <wchar.h>
#include <string>

//Scenes
#include "Scenes/Scene.h"
#include "Scenes/BasicShapesScene.h"

CEngine::~CEngine()
{
	delete m_renderer;
	delete m_scene;

	TInput::DeleteInstance();
}

void CEngine::Initialize()
{
	m_window.ConstructWindow();
	m_isRunning = true;

	m_renderer = new CRenderer;
	m_renderer->Initialize(m_window);

	EScene::Type selectedscene = (EScene::Type)Stream::ReadIniInt(Stream::GetDefaultDir(), L"Scene", L"SelectedScene");

	switch (selectedscene)
	{
	case EScene::eBasicShapesScene:
		m_scene = new CBasicShapesScene;
		break;
	default:
		check(0);
		break;
	}

	m_scene->InitializePasses(m_renderer);
	m_scene->LoadScene(m_renderer);
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

		m_scene->RenderScene(m_renderer, m_window, Time.m_deltaTime);

		//Check if shutdown bit
		{
			if (InputInstance()->m_isExiting ||
				InputIsKeyDown(VK_ESCAPE))
			{
				m_isRunning = false;
			}
		}

		//Has to be last
		InputInstance()->Reset();
	}
}

void CEngine::Restart()
{
	isRestarting = false;
	m_scene->UnloadScene();
	m_renderer->Restart();
}

LRESULT CALLBACK WndProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		InputInstance()->ReadMouseDelta(lParam);
	}
	break;
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
