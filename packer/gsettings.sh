#!/usr/bin/env bash

#
# disable screensaver and lock
#
gsettings set org.gnome.desktop.session idle-delay 0
gsettings set org.gnome.desktop.screensaver lock-enabled false
gsettings set org.gnome.desktop.lockdown disable-lock-screen true

#
# disable cursor blinking
#
gsettings set org.gnome.Terminal.Legacy.Profile:/org/gnome/terminal/legacy/profiles:/:$(gsettings get org.gnome.Terminal.ProfilesList default|tr -d \')/ cursor-blink-mode off

#
# clean up desktop icons
#
gsettings set org.gnome.nautilus.desktop home-icon-visible false
gsettings set org.gnome.nautilus.desktop trash-icon-visible false

rm -f ~/.config/autostart/gsettings.desktop

echo `date`: gsettings set, autostart file removed  >> ~/.gsettings.out
