[![Nightly Build](https://github.com/CrossTheRoadElec/Phoenix6-Linux-Example/actions/workflows/nightly-build.yml/badge.svg)](https://github.com/CrossTheRoadElec/Phoenix6-Linux-Example/actions/workflows/nightly-build.yml)

# Requirements

1. Make sure you meet the [Phoenix 6 System Requirements](https://v6.docs.ctr-electronics.com/en/stable/docs/installation/requirements.html)
1. [Install Phoenix 6](https://v6.docs.ctr-electronics.com/en/stable/docs/installation/installation-nonfrc.html) using the C++ (Linux) instructions
2. If using a CANivore, [install canivore-usb](https://v6.docs.ctr-electronics.com/en/stable/docs/canivore/canivore-setup.html#linux-non-frc)
3. Install libsdl2: `sudo apt install libsdl2-dev`

# Modifying the Robot Program

The main robot program is located inside main.cpp.

By default, the Joystick class is used for controller input. Users on Ubuntu 22.04+ or Debian Bullseye may choose to use the GameController class instead.

# Build Process

 1. Make a build directory: `mkdir build`
 2. CD into it: `cd build`
 3. Generate cmake: `cmake ..`
 4. Make the code: `make`
 5. Execute the code: `./Phoenix6-Example`

## Setting up Generic SocketCAN Adapters

When the canivore-usb kernel module is installed (required when using CANivore), **all** SocketCAN adapters will be automatically started by the robot program.

However, if it is not installed, then SocketCAN adapters must be manually brought up before running the robot program using `./generic_socketcan_start.sh [CAN interface (default: can0)]`.
