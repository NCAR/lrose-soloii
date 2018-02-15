#!/bin/sh -eux

#
# start docker, for subsequent `docker pull`
#
systemctl start docker

ASUSER="sudo -u $LROSE_USERNAME"

#
# - docker pull
# - install ISO8859 scripts
# - add helper script
# - add desktop icon
#

yum  -y install xorg-x11-fonts-ISO8859-1-75dpi xorg-x11-fonts-ISO8859-1-100dpi gedit

sudo -u $LROSE_USERNAME mkdir /home/$LROSE_USERNAME/Desktop
sudo -u $LROSE_USERNAME mv /tmp/lrose-soloii.desktop /home/$LROSE_USERNAME/Desktop/lrose-soloii.desktop

$ASUSER mkdir /home/$LROSE_USERNAME/bin
$ASUSER cp /tmp/soloii-docker /home/$LROSE_USERNAME/bin

$ASUSER chmod +x /home/$LROSE_USERNAME/bin/soloii-docker

$ASUSER ln -s /home/$LROSE_USERNAME/bin/soloii-docker /home/$LROSE_USERNAME/bin/soloii

docker pull ncareol/soloii

# yum install -y gedit

$ASUSER cp /tmp/README.txt /home/$LROSE_USERNAME/Desktop
$ASUSER chmod -w /home/$LROSE_USERNAME/Desktop/README.txt
