#pragma once
#include <Window.h>
#include <vector>

class TInput
{
	friend LRESULT CALLBACK WndProc(HWND _hWnd, uint32_t _msg, WPARAM _wParam, LPARAM _lParam);
private:
	void KeyUp(WPARAM wParam);
	void KeyDown(WPARAM wParam);
	TInput();

public:
	~TInput();
	static TInput*& GetInstance();

	bool IsKeyUp(uint32_t key);
	bool IsKeyDown(uint32_t key);

	std::vector<bool> keyStrokes;
	bool isExiting;
};
