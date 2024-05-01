#!/bin/bash

# Sets up a generic (non-CANivore) SocketCAN adapter for CAN 2.0.
#
# When the canivore-usb kernel module is installed, all SocketCAN
# adapters are set up automatically by the robot program, and this
# script should not be used.

interface=can0
if [ $# -gt 0 ]; then
    interface=$1
fi

sudo ip link set $interface type can bitrate 1000000
sudo ip link set $interface up
sudo ip link set $interface txqueuelen 1000
