#include "DeltaTime.h"
#include "Stream.h"

TTime::TTime() :
	m_timeFreq({}),
	m_timeStart({}),
	m_timePrev({}),
	m_timeCurr({}),
	m_deltaTime(0.f),
	m_totalTime(0.f),
	m_maxFrameTime(0)
{
	Restart();
}

void TTime::Restart()
{
	std::wstring MaximumFrameTime;
	Stream::ReadIniString(Stream::GetDefaultDir(), L"Time", L"MaximumFrameTime", MaximumFrameTime);
	m_maxFrameTime = std::wcstof(MaximumFrameTime.c_str(), NULL);

	QueryPerformanceFrequency(&m_timeFreq);
	QueryPerformanceCounter(&m_timeStart);
	QueryPerformanceCounter(&m_timePrev);
}

void TTime::Tick()
{
	QueryPerformanceFrequency(&m_timeFreq);
	QueryPerformanceCounter(&m_timeCurr);

	m_totalTime = float(m_timeCurr.QuadPart - m_timeStart.QuadPart) / float(m_timeFreq.QuadPart);
	m_deltaTime = float(m_timeCurr.QuadPart - m_timePrev.QuadPart) / float(m_timeFreq.QuadPart);

	if (m_deltaTime > m_maxFrameTime)
	{
		m_deltaTime = m_maxFrameTime;
	}

	QueryPerformanceCounter(&m_timePrev);
}