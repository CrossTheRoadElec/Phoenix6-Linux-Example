#pragma once

#include <units/time.h>
#include <chrono>
#include <optional>
#include <thread>
#include <stdint.h>

/**
 * Manages a robot program.
 */
class RobotBase {
    units::millisecond_t _loopTime;
    std::optional<bool> _lastEnabled;

public:
    RobotBase(units::millisecond_t loopTime = 20_ms) :
        _loopTime{loopTime}
    {}

    template <std::derived_from<RobotBase> Robot, typename... Args>
    static int StartRobot(Args&&... args)
    {
        printf("Starting robot program...\n");

        Robot robot(std::forward<Args>(args)...);
        int const retval = robot.Run();

        printf("Stopping robot program...\n");
        return retval;
    }

    virtual void RobotPeriodic() = 0;

    virtual bool IsEnabled() = 0;
    virtual void EnabledInit() = 0;
    virtual void EnabledPeriodic() = 0;

    virtual void DisabledInit() = 0;
    virtual void DisabledPeriodic() = 0;

    virtual bool IsRunning() { return true; }

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

private:
    static constexpr auto kErrorTimeMs = 3000;
    std::chrono::time_point<std::chrono::steady_clock> _lastErrorTime = std::chrono::steady_clock::now();

    /**
     * Runs the robot program.
     */
    int Run();

    /** Reports a loop overrun with debouncing. */
    void ReportLoopOverrun(units::millisecond_t measured);
};
