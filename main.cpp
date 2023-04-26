#include "ctre/phoenixpro/TalonFX.hpp"
#include "RobotBase.hpp"
#include "Joystick.hpp"

using namespace ctre::phoenixpro;

/**
 * This is the main robot. Put all actuators, sensors,
 * game controllers, etc. in this class.
 */
class Robot : public RobotBase {
private:
    /* This can be a CANivore name, CANivore serial number,
     * SocketCAN interface, or "*" to select any CANivore. */
    static constexpr char const *CANBUS_NAME = "*";

    /* devices */
    hardware::TalonFX leftLeader{0, CANBUS_NAME};
    hardware::TalonFX leftFollower{1, CANBUS_NAME};
    hardware::TalonFX rightLeader{2, CANBUS_NAME};
    hardware::TalonFX rightFollower{3, CANBUS_NAME};

    /* control requests */
    controls::DutyCycleOut leftOut{0};
    controls::DutyCycleOut rightOut{0};

    /* joystick */
    Joystick joy{0};

public:
    /* main robot interface */
    void RobotInit() override;
    void RobotPeriodic() override;

    bool IsEnabled() override;
    void EnabledInit() override;
    void EnabledPeriodic() override;

    void DisabledInit() override;
    void DisabledPeriodic() override;
};

/**
 * Runs once at code initialization.
 */
void Robot::RobotInit()
{
    configs::TalonFXConfiguration fx_cfg{};

    /* the left motor is CCW+ */
    fx_cfg.MotorOutput.Inverted = signals::InvertedValue::CounterClockwise_Positive;
    leftLeader.GetConfigurator().Apply(fx_cfg);

    /* the right motor is CW+ */
    fx_cfg.MotorOutput.Inverted = signals::InvertedValue::Clockwise_Positive;
    rightLeader.GetConfigurator().Apply(fx_cfg);

    /* set follower motors to follow leaders; do NOT oppose the leaders' inverts */
    leftFollower.SetControl(controls::Follower{leftLeader.GetDeviceID(), false});
    rightFollower.SetControl(controls::Follower{rightLeader.GetDeviceID(), false});
}

/**
 * Runs periodically during program execution.
 */
void Robot::RobotPeriodic()
{
    /* periodically check that the joystick is still good */
    joy.Periodic();
}

/**
 * Returns whether the robot should be enabled.
 */
bool Robot::IsEnabled()
{
    /* enable while joystick is an Xbox controller (6 axes),
     * and we are holding the right bumper */
    if (joy.GetNumAxes() < 6) return false;
    return joy.GetButton(5); // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
}

/**
 * Runs when transitioning from disabled to enabled.
 */
void Robot::EnabledInit() {}

/**
 * Runs periodically while enabled.
 */
void Robot::EnabledPeriodic()
{
    /* arcade drive */
    double speed = -joy.GetAxis(1); // SDL_CONTROLLER_AXIS_LEFTY
    double turn = joy.GetAxis(4); // SDL_CONTROLLER_AXIS_RIGHTX

    leftOut.Output = speed + turn;
    rightOut.Output = speed - turn;

    leftLeader.SetControl(leftOut);
    rightLeader.SetControl(rightOut);
}

/**
 * Runs when transitioning from enabled to disabled,
 * including after robot startup.
 */
void Robot::DisabledInit() {}

/**
 * Runs periodically while disabled.
 */
void Robot::DisabledPeriodic()
{
    leftLeader.SetControl(controls::NeutralOut{});
    rightLeader.SetControl(controls::NeutralOut{});
}

/* ------ main function ------ */
int main()
{
    /* create and run robot */
    Robot robot{};
    // robot.SetLoopTime(20_ms); // optionally change loop time for periodic calls
    return robot.Run();
}
