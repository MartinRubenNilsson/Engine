#pragma once

class DeltaTimer
{
public:
    float Query();

private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    TimePoint myLastQueryTime = Clock::now();
};

