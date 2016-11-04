#include "timing.h"

Timer::Timer() {
	QueryPerformanceFrequency(&freq);
	PauseTime.QuadPart = 0;
	paused = false;
}

void Timer::Start() {
	PauseTime.QuadPart = 0;
	QueryPerformanceCounter(&start);
}

void Timer::Stop() {
	QueryPerformanceCounter(&LastStop);
}

void Timer::Resume() {
	LARGE_INTEGER resume;

	QueryPerformanceCounter(&resume);

	PauseTime.QuadPart += resume.QuadPart - LastStop.QuadPart;
}


unsigned long Timer::GetTicks() const {
	LARGE_INTEGER ticks;

	QueryPerformanceCounter(&ticks);
	return (unsigned long)(ticks.QuadPart - start.QuadPart - PauseTime.QuadPart);
}

unsigned long Timer::GetSec() const {
	LARGE_INTEGER ticks;

	QueryPerformanceCounter(&ticks);
	return (unsigned long)((ticks.QuadPart - start.QuadPart - PauseTime.QuadPart)/freq.QuadPart);
}

unsigned long Timer::GetMilliSec() const {
	LARGE_INTEGER ticks;

	QueryPerformanceCounter(&ticks);
	return (unsigned long)((ticks.QuadPart - start.QuadPart - PauseTime.QuadPart)/(freq.QuadPart/1000));
}
