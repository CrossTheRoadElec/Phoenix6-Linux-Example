#include "Joystick.hpp"

/*static*/ std::mutex Joystick::s_joyCntLck;
/*static*/ uint64_t Joystick::s_joyCnt{0};

void Joystick::Periodic()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {}

    /* poll for joystick disconnects */
    if (!IsConnected()) {
        /* one of the joysticks disconnected, assume it was ours */
        Close();
    }

    /* joystick may have disconnected, make sure it's still valid */
    if (!_joy) {
        /* no joystick, initialize a new one */
        Init();
    }
}

void Joystick::ReportMissingJoystick()
{
    auto const now = std::chrono::steady_clock::now();
    auto const dtMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastErrorTime).count();

    if (dtMs > kErrorTimeMs) {
        fprintf(stderr, "Warning: Could not find joystick on port %d\n", _port);
        _lastErrorTime = now;
    }
}

SDL_Joystick *Joystick::CreateJoystick()
{
    /* poll for joysticks */
    int res = SDL_NumJoysticks();
    if (res < 0) {
        /* error trying to get joysticks */
        fprintf(stderr, "Error getting joysticks: %d\n", res);
        ReportMissingJoystick();
        return nullptr;
    } else if (res <= _port) {
        /* not enough joysticks for given port */
        ReportMissingJoystick();
        return nullptr;
    }

    /* joystick found */
    return SDL_JoystickOpen(_port);
}

void Joystick::PrintJoystickInfo() const
{
    /* Get information about the joystick */
    auto const name = GetName();
    auto const num_axes = GetNumAxes();
    auto const num_buttons = GetNumButtons();
    auto const num_hats = GetNumHats();

    /* print information */
    printf("Connected to joystick '%s'\n"
            "    Port: %d\n"
            "    Num Axes: %d\n"
            "    Num Buttons: %d\n"
            "    Num Hats: %d\n",
            name.c_str(), _port,
            num_axes, num_buttons, num_hats);
}
