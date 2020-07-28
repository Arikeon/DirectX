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
	m_deltaX = m_deltaY = m_currX = m_currY = 0.f;
}

void TInput::ReadMouseDelta(LPARAM lparam)
{
	POINT P;
	GetCursorPos(&P);

	m_deltaX = m_currX - (float)P.x;
	m_deltaY = m_currY - (float)P.y;

	m_currX = (float)P.x;
	m_currY = (float)P.y;
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