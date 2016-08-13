#pragma once

#include "stdafx.h"

namespace DX11Engine
{
	class FPSTimer
	{
	public:
		FPSTimer();
		~FPSTimer();

		void StartTimer();
		void Update();

		double GetTime();
		double GetFrameTime();

		int FPS = 0;
		double FrameTime;

	private:
		double m_countsPerSecond = 0.0;
		__int64 m_counterStart = 0;

		int m_frameCount = 0;

		__int64 m_frameTimeOld = 0;
	};

}
