#!/bin/sh -eux

#
# desktop
#
# yum groupinstall -y "GNOME Desktop" "Graphical Administration Tools"
# yum groupinstall -y "GNOME Desktop"

#
# minimal?
#
yum -y groupinstall "X Window System"
# yum -y install gnome-shell gnome-terminal
yum -y install gnome-classic-session gnome-terminal nautilus-open-terminal control-center liberation-mono-fonts

#
# set runlevel to 5, to boot to GDM
#
systemctl set-default graphical.target
#
# configure GDM to automatically log in user USERNAME
#
mv /tmp/custom.conf /etc/gdm/

#
# add LROSE_USERNAME user to vboxsf group, for accessing shared folders
#
/usr/sbin/usermod -aG vboxsf $LROSE_USERNAME

#
# start gnome-terminal on desktop login
#
sudo -u $LROSE_USERNAME mkdir -p /home/$LROSE_USERNAME/.config/autostart
# sudo -u $USERNAME ln -s /usr/share/applications/gnome-terminal.desktop /home/vagrant/.config/autostart

#
# stage script to initialize gsettings
#
sudo -u $LROSE_USERNAME cp /tmp/gsettings.sh /home/$LROSE_USERNAME/
sudo -u $LROSE_USERNAME cp /tmp/gsettings.desktop /home/$LROSE_USERNAME/.config/autostart
