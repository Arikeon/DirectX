#include "Input.h"

TInput::TInput()
{
	const int NumKeys = 256;
	keyStrokes.resize(NumKeys);
	isExiting = false;
}

TInput::~TInput()
{
	keyStrokes.clear();
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
	keyStrokes[wParam] = 0;
}

void TInput::KeyDown(WPARAM wParam)
{
	keyStrokes[wParam] = 1;
}

bool TInput::IsKeyUp(uint32_t id)
{
	return keyStrokes[id] == 0;
}

bool TInput::IsKeyDown(uint32_t id)
{
	return keyStrokes[id] == 1;
}
