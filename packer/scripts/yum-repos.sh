#!/bin/sh -eux

sudo mkdir -p /var/cache/yum/base /var/cache/yum/extras /var/cache/yum/updates

sudo mv /tmp/base-mirrorlist.txt    /var/cache/yum/base/mirrorlist.txt
sudo mv /tmp/updates-mirrorlist.txt /var/cache/yum/updates/mirrorlist.txt
sudo mv /tmp/extras-mirrorlist.txt  /var/cache/yum/extras/mirrorlist.txt

rm -f /etc/yum.repos.d/libselinux.repo

yum -y --enablerepo=extras install epel-release
