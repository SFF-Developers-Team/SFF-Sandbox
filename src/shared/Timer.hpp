#pragma once
#include <chrono>

class Timer {
private:
    std::chrono::high_resolution_clock m_clock;
    uint32_t m_ticks;
    int64_t m_lastTime;
    float m_ticksPerSecond;
    float m_passedTime;
    float m_timeScale;

    int64_t getNanos();
    
public:
    Timer(float ticksPerSecond);
    void advanceTime();
    uint32_t getTicks();
};