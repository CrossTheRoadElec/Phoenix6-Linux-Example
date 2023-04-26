#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <chrono>
#include <mutex>

/**
 * Manages a game controller using the SDL 2 library.
 */
class GameController {
private:
    SDL_GameController *_joy = nullptr;
    int _port;

    static std::mutex s_joyCntLck;
    static uint64_t s_joyCnt;

    /** Initialize this game controller. */
    void Init()
    {
        if (_joy) {
            /* close any existing game controller first */
            Close();
        }

        _joy = CreateGameController();
        if (_joy) {
            /* Print information about the game controller */
            PrintGameControllerInfo();
        }
    }

    /** Closes this game controller. */
    void Close()
    {
        /* do nothing if no game controller */
        if (!_joy) return;

        SDL_GameControllerClose(_joy);
        _joy = nullptr;
    }

public:
    /**
     * Creates a game controller on the given port.
     */
    GameController(int port) : _port{port}
    {
        /* first increment game controller count */
        {
            std::lock_guard lck{s_joyCntLck};
            ++s_joyCnt;
        }

        /* then initialize this game controller */
        Init();
    }

    ~GameController()
    {
        /* first close this game controller */
        Close();

        /* then decrement game controller count */
        {
            std::lock_guard lck{s_joyCntLck};
            if (--s_joyCnt == 0) {
                /* this was the last game controller, quit SDL */
                SDL_Quit();
            }
        }
    }

    /**
     * Returns the port of this game controller.
     */
    int GetPort() const { return _port; }

    /**
     * Returns the name of this game controller.
     */
    std::string GetName() const
    {
        if (_joy) {
            return SDL_GameControllerName(_joy);
        } else {
            return "";
        }
    }

    /**
     * Returns the SDL type of this game controller.
     */
    SDL_GameControllerType GetType() const
    {
        if (_joy) {
            return SDL_GameControllerGetType(_joy);
        } else {
            return SDL_CONTROLLER_TYPE_UNKNOWN;
        }
    }

    /**
     * Returns true if the given SDL button is pressed,
     * false otherwise or in the case of an error.
     */
    bool GetButton(SDL_GameControllerButton button) const
    {
        if (_joy) {
            return SDL_GameControllerGetButton(_joy, button);
        } else {
            return false;
        }
    }

    /**
     * Returns the value of the given SDL axis from
     * -1.0 to 1.0, or 0 in the case of an error.
     */
    double GetAxis(SDL_GameControllerAxis axis) const
    {
        if (_joy) {
            return SDL_GameControllerGetAxis(_joy, axis) / 32767.0;
        } else {
            return 0.0;
        }
    }

    /**
     * Returns whether this game controller is currently connected.
     */
    bool IsConnected() const;

    /**
     * Periodically manages this game controller, handling disconnect
     * events in the process.
     */
    void Periodic()
    {
        /* poll for game controller disconnects */
        if (!IsConnected()) {
            /* one of the game controllers disconnected, assume it was ours */
            Close();
        }

        /* game controller may have disconnected, make sure it's still valid */
        if (!_joy) {
            /* no game controller, initialize a new one */
            Init();
        }
    }

private:
    static constexpr auto kErrorTimeMs = 2000;
    std::chrono::time_point<std::chrono::steady_clock> _lastErrorTime = std::chrono::steady_clock::now();

    /** Reports a missing game controller with debouncing. */
    void ReportMissingGameController();
    /** Creates and returns a game controller. */
    SDL_GameController *CreateGameController();
    /** Prints out information about this game controller. */
    void PrintGameControllerInfo() const;
};
