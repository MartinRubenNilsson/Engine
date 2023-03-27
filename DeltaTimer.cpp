#include "pch.h"
#include "DeltaTimer.h"

float DeltaTimer::Query()
{
	using Seconds = std::chrono::duration<float>;

	TimePoint now = Clock::now();
	float deltaTime = Seconds{ now - myLastQueryTime }.count();
	myLastQueryTime = now;

	return deltaTime;
}
