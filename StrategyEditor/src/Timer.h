#pragma once

#include <chrono>


class Timer {
public:
    Timer() = default;
    ~Timer() = default;

    bool Started()
    {
        return m_startTimePoint.time_since_epoch().count() > 0;
    }
    void StartTimer()
    {
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }
    double SecondsElapsed()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

        auto duration = end - start;
        return duration / (1000 * 1000);
    }
    double MillisecondsElapsed()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

        auto duration = end - start;
        return duration / 1000;
    }
    double MicrosecondsElapsed()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

        auto duration = end - start;
        return duration;
    }


private:

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;

};