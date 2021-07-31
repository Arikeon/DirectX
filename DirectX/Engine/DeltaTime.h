#pragma once
#include <Windows.h>

typedef LARGE_INTEGER Lint;
struct TTime
{
	TTime();

	void Restart();
	void Tick();
	float m_deltaTime;
	float m_totalTime;
	float m_maxFrameTime;
	Lint m_timeFreq;
	Lint m_timeStart;
	Lint m_timePrev;
	Lint m_timeCurr;
};
