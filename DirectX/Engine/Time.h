#pragma once
#include <Window.h>

typedef LARGE_INTEGER Lint;
struct TTime
{
	TTime();

	void Restart();
	void Tick();
	float deltaTime;
	float totalTime;
	float maxFrameTime;
	Lint timeFreq;
	Lint timeStart;
	Lint timePrev;
	Lint timeCurr;
};
