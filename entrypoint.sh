#!/bin/bash

export GDK_BACKEND=$(if [ ! $WAYLAND_DISPLAY ]; then echo x11; else echo wayland; fi)
export QT_QPA_PLATFORM=$(if [ ! $WAYLAND_DISPLAY ]; then echo xcb; else echo wayland; fi)

exec "$@"