#pragma once
#include <SDL_hints.h>

class Timer
{
public:
	Timer();
	~Timer();

	static void Init();

	static void EndFrame();

	static float GetDeltaTime() { return m_DeltaTime; };
	static float GetTotalTime() { return m_TotalTime; };

private:
	static float m_DeltaTime;
	static float m_TotalTime;

	static Uint64 m_LastPerformanceCount;
	static Uint64 m_PerformanceFrequency;
};

