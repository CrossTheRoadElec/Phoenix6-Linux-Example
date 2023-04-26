#include "GameController.hpp"

/*static*/ std::mutex GameController::s_joyCntLck;
/*static*/ uint64_t GameController::s_joyCnt{0};

bool GameController::IsConnected() const
{
    if (!_joy) {
        /* no game controller */
        return false;
    }

    /* poll for game controller disconnects */
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            /* SDL shut down */
            return false;
        }
        else if (event.cdevice.type == SDL_CONTROLLERDEVICEREMOVED) {
            /* SDL game controller removed */
            return false;
        }
    }

    return true;
}

void GameController::ReportMissingGameController()
{
    auto const now = std::chrono::steady_clock::now();
    auto const dtMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastErrorTime).count();

    if (dtMs > kErrorTimeMs) {
        fprintf(stderr, "Warning: Could not find game controller on port %d\n", _port);
        _lastErrorTime = now;
    }
}

SDL_GameController *GameController::CreateGameController()
{
    /* SDL seems somewhat fragile, shut it down and bring it up */
    SDL_Quit();
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); // so Ctrl-C still works
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    /* poll for game controller */
    int res = SDL_NumJoysticks();
    if (res < 0) {
        /* error trying to get joysticks */
        fprintf(stderr, "Error getting game controllers: %d\n", res);
        ReportMissingGameController();
        return nullptr;
    } else if (res <= _port) {
        /* not enough joysticks for given port */
        ReportMissingGameController();
        return nullptr;
    }

    /* joystick found */
    if (SDL_IsGameController(_port)) {
        /* valid game controller */
        return SDL_GameControllerOpen(_port);
    } else {
        /* not a valid game controller */
        ReportMissingGameController();
        return nullptr;
    }
}

void GameController::PrintGameControllerInfo() const
{
    /* Get information about the game controller */
    auto const name = GetName();
    auto const type = GetType();

    char const *typeStr;
    switch (type) {
        case SDL_CONTROLLER_TYPE_XBOX360: typeStr = "Xbox 360"; break;
        case SDL_CONTROLLER_TYPE_XBOXONE: typeStr = "Xbox One"; break;
        case SDL_CONTROLLER_TYPE_PS3: typeStr = "PS3"; break;
        case SDL_CONTROLLER_TYPE_PS4: typeStr = "PS4"; break;
        case SDL_CONTROLLER_TYPE_PS5: typeStr = "PS5"; break;

        default:
        case SDL_CONTROLLER_TYPE_UNKNOWN: typeStr = "Unknown"; break;
    }

    /* print information */
    printf("Connected to game controller '%s'\n"
            "    Type: %s\n"
            "    Port: %d\n",
            name.c_str(), typeStr, _port);
}
