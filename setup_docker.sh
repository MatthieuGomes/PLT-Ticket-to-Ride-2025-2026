#!/bin/sh

set -e

echo XDG_SESSION_TYPE=$XDG_SESSION_TYPE > ./.env
echo USERNAME=plt >> ./.env
echo USER_UID=$(id -u) >> ./.env
echo USER_GID=$(id -g) >> ./.env

echo "Setup host for docker compose with graphical support..."

if [ "$XDG_SESSION_TYPE" != "wayland" ]; then
    echo -e "\tX11 server detected."
    echo -e "\tSetting environment for X11..."
    echo DISPLAY=$DISPLAY >> ./.env
    echo -e "\tSetting environment for X11... done."
    echo -e "\tChecking if xhost needs to be configured..."
    if ! xhost | grep -q "localuser:$(id -un)"; then
        echo -e "\t\t Xhost is not configured for local user $(id -un)."
        xhost +SI:localuser:$(id -un) | sed 's/^/\t\t/'
        echo -e "\t\tChecking if xsession is set..."
        if [ -z "$(cat ~/.xsession | grep "xhost")" ]; then
            echo -e "\t\t\tSetting xsession to configure xhost on login..."
            echo "xhost +SI:localuser:$(id -un)" >> ~/.xsession
        else
            echo -e "\t\t\tXsession already set to $(cat ~/.xsession | grep "xhost")."
        fi
        echo -e "\t\tChecking if xsession is set... done."
    else
        echo -e "\t\tXhost already configured for local user $(id -un)."
    fi
    echo -e "\tChecking if xhost needs to be configured... done."
else
    echo -e "\tWayland server detected."
    echo -e "\tSetting environment for wayland..."
    echo WAYLAND_DISPLAY=$WAYLAND_DISPLAY >> ./.env
    echo -e "\tSetting environment for wayland... done."
fi
echo "Setup host for docker compose with graphical support... done."