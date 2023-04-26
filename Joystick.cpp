#include "Joystick.hpp"

/*static*/ std::mutex Joystick::s_joyCntLck;
/*static*/ uint64_t Joystick::s_joyCnt{0};

bool Joystick::IsConnected() const
{
    if (!_joy) {
        /* no joystick */
        return false;
    }

    /* poll for joystick disconnects */
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            /* SDL shut down */
            return false;
        }
        else if (event.cdevice.type == SDL_JOYDEVICEREMOVED) {
            /* SDL joystick removed */
            return false;
        }
    }

    return true;
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
    /* SDL seems somewhat fragile, shut it down and bring it up */
    SDL_Quit();
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); // so Ctrl-C still works
    SDL_Init(SDL_INIT_GAMECONTROLLER);

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
