#include "Input.h"

TInput::TInput()
{
	const int NumKeys = 256;
	m_keyStrokes.resize(NumKeys);
	m_isExiting = false;
}

TInput::~TInput()
{
	m_keyStrokes.clear();
}

TInput*& TInput::GetInstance()
{
	static TInput* Instance = nullptr;;
	static bool bInitialized = false;

	if (!bInitialized)
	{
		Instance = new TInput;
		bInitialized = true;
	}

	return Instance;
}

void TInput::KeyUp(WPARAM wParam)
{
	m_keyStrokes[wParam] = 0;
}

void TInput::KeyDown(WPARAM wParam)
{
	m_keyStrokes[wParam] = 1;
}

bool TInput::IsKeyUp(uint32_t id)
{
	return m_keyStrokes[id] == 0;
}

bool TInput::IsKeyDown(uint32_t id)
{
	return m_keyStrokes[id] == 1;
}
