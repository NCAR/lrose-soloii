#!/bin/sh -eux

#
# install, enable docker
#

yum install -y docker
systemctl enable docker

#
# create docker group, add LROSE_USERNAME user to docker group
#
groupadd docker
usermod -aG docker $LROSE_USERNAME
