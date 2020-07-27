#pragma once
#include <Window.h>
#include <vector>


class TInput
{
	friend LRESULT CALLBACK WndProc(HWND _hWnd, uint32 _msg, WPARAM _wParam, LPARAM _lParam);
private:
	void KeyUp(WPARAM wParam);
	void KeyDown(WPARAM wParam);
	TInput();
	static TInput* Instance;

public:
	~TInput();
	static TInput*& GetInstance();
	static void DeleteInstance();

	bool IsKeyUp(uint32 key);
	bool IsKeyDown(uint32 key);

	std::vector<bool> m_keyStrokes;
	bool m_isExiting;
};

static INLINE auto InputInstance()
{
	return TInput::GetInstance();
}

static INLINE auto InputIsKeyDown(uint32 key)
{
	return InputInstance()->IsKeyDown(key);
}

static INLINE auto InputIsKeyUp(uint32 key)
{
	return InputInstance()->IsKeyUp(key);
}