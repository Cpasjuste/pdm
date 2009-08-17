#!/bin/bash
export DISPLAY=:0
Xorg &
/etc/X11/Xinit &
/etc/X11/gpe-login.pre-session root
/etc/X11/Xsession
killall Xorg

