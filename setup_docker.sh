#!/bin/sh

set -e

echo USERNAME=plt > ./.env
echo USER_UID=$(id -u) >> ./.env
echo USER_GID=$(id -g) >> ./.env

echo "Checking for display server..."
if [ "$XDG_SESSION_TYPE" != "wayland" ]; then
    xhost +SI:localuser:$(id -un)
    echo DISPLAY=$DISPLAY >> ./.env
fi
