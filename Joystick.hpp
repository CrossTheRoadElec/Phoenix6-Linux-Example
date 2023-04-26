#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <chrono>
#include <mutex>

/**
 * Manages a joystick using the SDL 2 library.
 *
 * Note: This is a legacy class for users on Ubuntu
 *       20.04 and older. Users on newer systems
 *       can use the GameController class.
 */
class Joystick {
private:
    SDL_Joystick *_joy = nullptr;
    int _port;

    static std::mutex s_joyCntLck;
    static uint64_t s_joyCnt;

    /** Initialize this joystick. */
    void Init()
    {
        if (_joy) {
            /* close any existing joystick first */
            Close();
        }

        _joy = CreateJoystick();
        if (_joy) {
            /* Print information about the joystick */
            PrintJoystickInfo();
        }
    }

    /** Closes this joystick. */
    void Close()
    {
        /* do nothing if no joystick */
        if (!_joy) return;

        SDL_JoystickClose(_joy);
        _joy = nullptr;
    }

public:
    /**
     * Creates a joystick on the given port.
     */
    Joystick(int port) : _port{port}
    {
        /* first increment joystick count */
        {
            std::lock_guard lck{s_joyCntLck};
            ++s_joyCnt;
        }

        /* then initialize this joystick */
        Init();
    }

    ~Joystick()
    {
        /* first close this joystick */
        Close();

        /* then decrement joystick count */
        {
            std::lock_guard lck{s_joyCntLck};
            if (--s_joyCnt == 0) {
                /* this was the last joystick, quit SDL */
                SDL_Quit();
            }
        }
    }

    /**
     * Returns the port of this joystick.
     */
    int GetPort() const { return _port; }

    /**
     * Returns the name of this joystick.
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
     * Returns the SDL type of this joystick.
     */
    SDL_JoystickType GetType() const
    {
        if (_joy) {
            return SDL_JoystickGetType(_joy);
        } else {
            return SDL_JOYSTICK_TYPE_UNKNOWN;
        }
    }

    /**
     * Returns the number of buttons on this joystick.
     */
    int GetNumButtons() const
    {
        if (_joy) {
            return SDL_JoystickNumButtons(_joy);
        } else {
            return -1;
        }
    }

    /**
     * Returns the number of axes on this joystick.
     */
    int GetNumAxes() const
    {
        if (_joy) {
            return SDL_JoystickNumAxes(_joy);
        } else {
            return -1;
        }
    }

    /**
     * Returns the number of hats on this joystick.
     */
    int GetNumHats() const
    {
        if (_joy) {
            return SDL_JoystickNumHats(_joy);
        } else {
            return -1;
        }
    }

    /**
     * Returns true if the given button is pressed,
     * false otherwise or in the case of an error.
     */
    bool GetButton(int button) const
    {
        if (_joy) {
            return SDL_JoystickGetButton(_joy, button);
        } else {
            return false;
        }
    }

    /**
     * Returns the value of the given axis from
     * -1.0 to 1.0, or 0 in the case of an error.
     */
    double GetAxis(int axis) const
    {
        if (_joy) {
            return SDL_JoystickGetAxis(_joy, axis) / 32768.0;
        } else {
            return 0.0;
        }
    }

    /**
     * Returns whether this joystick is currently connected.
     */
    bool IsConnected() const;

    /**
     * Periodically manages this joystick, handling disconnect
     * events in the process.
     */
    void Periodic()
    {
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

private:
    static constexpr auto kErrorTimeMs = 2000;
    std::chrono::time_point<std::chrono::steady_clock> _lastErrorTime = std::chrono::steady_clock::now();

    /** Reports a missing joystick with debouncing. */
    void ReportMissingJoystick();
    /** Creates and returns a joystick. */
    SDL_Joystick *CreateJoystick();
    /** Prints out information about this joystick. */
    void PrintJoystickInfo() const;
};
