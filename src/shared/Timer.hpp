#pragma once
#include <chrono>

class Timer {
public:
    Timer(float ticksPerSecond);
    void AdvanceTime();
    inline uint32_t GetTicks() const { return m_ticks; }
    inline float GetPartialTicks() const { return m_partialTicks; }

private:
    int64_t GetNanos();

    std::chrono::high_resolution_clock m_clock;
    float m_ticksPerSecond;
    int64_t m_lastTime;
    float m_passedTime;
    float m_timeScale;
    uint32_t m_ticks;
    float m_partialTicks;
};