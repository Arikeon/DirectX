#include "Input.h"

TInput* TInput::Instance = nullptr;

TInput::TInput()
{
	const int NumKeys = 256;
	m_keyStrokes.resize(NumKeys);
	m_isExiting = false;
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

void TInput::DeleteInstance()
{
	delete Instance;
	Instance = nullptr;
}