#include "Input.h"
#include <Window.h>
#include <string>

TInput* TInput::Instance = nullptr;

TInput::TInput()
{
	const int NumKeys = 256;
	m_keyStrokes.resize(NumKeys);
	m_isExiting = false;
	m_deltaX = m_deltaY = m_currX = m_currY = 0.f;
}

TInput::~TInput()
{
}

TInput*& TInput::GetInstance()
{
	static bool bInitialized = false;

	if (!bInitialized)
	{
		Instance = new TInput;
		bInitialized = true;
	}

	return Instance;
}

void TInput::DeleteInstance()
{
	delete Instance;
	Instance = nullptr;
}

void TInput::Reset()
{
	//Delta
#if 0
	CONSOLE_LOG(std::to_wstring(m_deltaX) + L"  " + std::to_wstring(m_deltaY));
#endif

	//Position
#if 0
	CONSOLE_LOG(std::to_wstring(m_currX) + L"  " + std::to_wstring(m_currY));
#endif
	m_deltaX = m_deltaY = 0.f;
}

void TInput::ReadMouseDelta(POINT P)
{
	m_deltaX = m_currX - (float)P.x;
	m_deltaY = m_currY - (float)P.y;

	m_currX += m_deltaX;
	m_currY += m_deltaY;
}

void TInput::SetMouseRawInput(HWND& window)
{
	m_rawinput.usUsagePage = HID_USAGE_PAGE_GENERIC;
	m_rawinput.usUsage = HID_USAGE_GENERIC_MOUSE;
	m_rawinput.dwFlags = 0;
	m_rawinput.hwndTarget = window;
	RegisterRawInputDevices(&m_rawinput, 1, sizeof(m_rawinput));
}

void TInput::SetMouse(LPARAM lParam)
{
	UINT dwSize = 48;
	static BYTE lpb[48];

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
		lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		POINT P;
		m_deltaX = raw->data.mouse.lLastX;
		m_deltaY = raw->data.mouse.lLastY;
	}
}

float2 TInput::GetMouseDelta()
{
	return float2{ m_deltaX, m_deltaY };
}

float2 TInput::GetMousePoint()
{
	return float2{ m_currX, m_currY };
}

void TInput::KeyUp(WPARAM wParam)
{
	m_keyStrokes[wParam] = 0;
}

void TInput::KeyDown(WPARAM wParam)
{
	m_keyStrokes[wParam] = 1;
}

bool TInput::IsKeyUp(uint32 id)
{
	return m_keyStrokes[id] == 0;
}

bool TInput::IsKeyDown(uint32 id)
{
	return m_keyStrokes[id] == 1;
}