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
# disable lockscreen
#
gconftool-2 --set /schemas/desktop/gnome/lockdown/disable_lock_screen --type boolean true
