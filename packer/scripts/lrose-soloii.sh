#!/bin/sh -eux

export PACKAGES_KEEP="gtk+ libX11.i386 netcdf.i386 libgcc.i386 fonts-ISO8859-2 fonts-ISO8859-2-100dpi fonts-ISO8859-2-75dpi"
export PACKAGES="gcc gcc-gfortran pkgconfig make glibc-utils glibc-devel ncurses-devel"
export PACKAGES_i386="gtk+-devel.i386 glibc-devel.i386 glib-devel.i386 libXi-devel.i386 libXext-devel.i386 libX11-devel.i386 netcdf-devel.i386"

#
# configure: error: Failed to run gtk-config. Is gtk+ (gtk+-devel) installed on the system?
#
export PKG_CONFIG_PATH=/usr/lib/pkgconfig

#
# build & install soloii
#

export SOLO_DIR=/tmp/lrose-soloii

yum -y install $PACKAGES_KEEP $PACKAGES $PACKAGES_i386 \
    && cd $SOLO_DIR \
    && ./configure --prefix=/usr \
    && make \
    && cd - && rm -rf $SOLO_DIR \
    && yum -y remove $PACKAGES $PACKAGES_i386 \
    && yum -y clean all
