#include "Timer.h"
#include <SDL.h>
#include <iostream>

float Timer::m_DeltaTime = 0.f;
float Timer::m_TotalTime = 0.f;
Uint64 Timer::m_LastPerformanceCount = 0;
Uint64 Timer::m_PerformanceFrequency = 0;

Timer::Timer(){}
Timer::~Timer(){}

void Timer::Init()
{
	m_PerformanceFrequency = SDL_GetPerformanceFrequency();
	m_LastPerformanceCount = SDL_GetPerformanceCounter();
}

void Timer::EndFrame()
{
	auto newTime = SDL_GetPerformanceCounter();
	m_DeltaTime = (float)(newTime - m_LastPerformanceCount) / (float)SDL_GetPerformanceFrequency();
	m_TotalTime += m_DeltaTime;
	m_LastPerformanceCount = newTime;
}
