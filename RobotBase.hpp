#pragma once

#include "units/time.h"
#include <chrono>
#include <thread>
#include <stdint.h>

/**
 * Manages a robot program.
 */
class RobotBase {
public:
    virtual void RobotInit() = 0;
    virtual void RobotPeriodic() = 0;

    virtual bool IsEnabled() = 0;
    virtual void EnabledInit() = 0;
    virtual void EnabledPeriodic() = 0;

    virtual void DisabledInit() = 0;
    virtual void DisabledPeriodic() = 0;

    virtual bool IsRunning() { return true; }

private:
    units::millisecond_t _loopTime = 20_ms;
    int _lastEnabled = -1;

public:
    /**
     * Sleeps for the specified amount of time.
     */
    static inline void SleepFor(units::microsecond_t us)
    {
        std::this_thread::sleep_for(std::chrono::microseconds{(uint64_t)us.value()});
    }

    /**
     * Sets the loop time for the robot program periodic calls.
     */
    void SetLoopTime(units::millisecond_t loopTime = 20_ms)
    {
        _loopTime = loopTime;
    }

    /**
     * Runs the robot program.
     */
    int Run();

private:
    static constexpr auto kErrorTimeMs = 500;
    std::chrono::time_point<std::chrono::steady_clock> _lastErrorTime = std::chrono::steady_clock::now();

    /** Reports a loop overrun with debouncing. */
    void ReportLoopOverrun(units::millisecond_t measured);
};
