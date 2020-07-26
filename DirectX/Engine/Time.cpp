#include "Time.h"
#include "Stream.h"

TTime::TTime() :
	timeFreq({}),
	timeStart({}),
	timePrev({}),
	timeCurr({}),
	deltaTime(0.f),
	totalTime(0.f),
	maxFrameTime(0)
{
	Restart();
}

void TTime::Restart()
{
	std::wstring MaximumFrameTime;
	Stream::ReadIniString(Stream::GetDefaultDir(), L"Time", L"MaximumFrameTime", MaximumFrameTime);
	maxFrameTime = std::wcstof(MaximumFrameTime.c_str(), NULL);

	QueryPerformanceFrequency(&timeFreq);
	QueryPerformanceCounter(&timeStart);
	QueryPerformanceCounter(&timePrev);
}

void TTime::Tick()
{
	QueryPerformanceFrequency(&timeFreq);
	QueryPerformanceCounter(&timeCurr);

	totalTime = float(timeCurr.QuadPart - timeStart.QuadPart) / float(timeFreq.QuadPart);
	deltaTime = float(timeCurr.QuadPart - timePrev.QuadPart) / float(timeFreq.QuadPart);

	if (deltaTime > maxFrameTime)
	{
		deltaTime = maxFrameTime;
	}

	QueryPerformanceCounter(&timePrev);
}