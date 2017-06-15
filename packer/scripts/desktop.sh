#!/bin/sh -eux

#
# desktop
#
yum -y install xorg-x11-server-Xorg
yum -y groupinstall 'GNOME Desktop Environment'
yum -y install gdm

#
# set runlevel to 5, to boot to GDM
#
sed --in-place -e 's/id:3:initdefault/id:5:initdefault/' /etc/inittab

#
# configure GDM to automatically log in user vagrant
#
mv /tmp/custom.conf /etc/gdm/

#
# disable lockscreen and screensaver
#
sudo -u lrose gconftool-2 --set /schemas/desktop/gnome/lockdown/disable_lock_screen --type boolean true
sudo -u lrose gconftool-2 --set /schemas/apps/gnome-screensaver/lock_enabled --type boolean false
sudo -u lrose gconftool-2 --set /schemas/apps/gnome-screensaver/idle_activation_enabled --type boolean false

#
# disable cursor blinking
#
sudo -u lrose gconftool-2 --set /apps/gnome-terminal/profiles/Default/cursor_blink --type boolean false

#
# stage soloii desktop icon
#
sudo -u lrose mkdir /home/lrose/Desktop
sudo -u lrose mv /tmp/lrose-soloii.desktop /home/lrose/Desktop/lrose-soloii.desktop

#
# add lrose user to vboxsf group, for accessing shared folders
#
/usr/sbin/usermod -aG vboxsf lrose
