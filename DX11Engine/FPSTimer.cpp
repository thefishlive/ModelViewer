#include "FPSTimer.h"



DX11Engine::FPSTimer::FPSTimer()
{
}


DX11Engine::FPSTimer::~FPSTimer()
{
}

void DX11Engine::FPSTimer::StartTimer()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	m_countsPerSecond = double(frequency.QuadPart);

	QueryPerformanceCounter(&frequency);
	m_counterStart = frequency.QuadPart;
}

void DX11Engine::FPSTimer::Update()
{
	m_frameCount++;
	if (GetTime() > 1.0f)
	{
		FPS = m_frameCount;
		m_frameCount = 0;
		StartTimer();
	}

	FrameTime = GetFrameTime();
}

double DX11Engine::FPSTimer::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - m_counterStart) / m_countsPerSecond;
}

double DX11Engine::FPSTimer::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - m_frameTimeOld;
	m_frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / m_countsPerSecond;
}
