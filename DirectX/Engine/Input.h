#pragma once
#include <Window.h>
#include <vector>
#include "D3DDefines.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif


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

	void Reset();
	void ReadMouseDelta(POINT P);
	void SetMouseRawInput(HWND& window);
	void SetMouse(LPARAM lParam);
	float2 GetMouseDelta();
	float2 GetMousePoint();
	bool IsKeyUp(uint32 key);
	bool IsKeyDown(uint32 key);
	float m_deltaX, m_deltaY, m_currX, m_currY;

	RAWINPUTDEVICE m_rawinput;

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