#!/bin/bash
# Usage: ./run.sh [demo_name]
# Demo names: 01_flames .. 16_nnt (default: 16_nnt)
DEMO=${1:-16_nnt}

PULSE_SOCKET=/run/user/$(id -u)/pulse/native

docker run --rm \
    --user "$(id -u):$(id -g)" \
    -e DISPLAY="$DISPLAY" \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -e SDL_AUDIODRIVER=pulseaudio \
    -e PULSE_SERVER="unix:$PULSE_SOCKET" \
    -v "$PULSE_SOCKET:$PULSE_SOCKET" \
    neru-no-toki ./build/"$DEMO"
