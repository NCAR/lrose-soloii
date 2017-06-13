#!/bin/sh -eux

#
# desktop
#
yum -y install xorg-x11-server-Xorg
yum -y groupinstall 'GNOME Desktop Environment'
yum -y install gdm

sed --in-place -e 's/id:3:initdefault/id:5:initdefault/' /etc/inittab

mv /tmp/custom.conf /etc/gdm/

# sed --in-place -e 's/[daemon]/[daemon]\
# AutomaticLoginEnable=true\
# AutomaticLogin=vagrant\
# /' /etc/gdm/custom.conf
