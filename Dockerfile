FROM ncareol/centos:5.11-0
MAINTAINER Erik Johnson <ej@ucar.edu>

COPY docker/iptables /etc/sysconfig/iptables

#
# https://github.com/NCAR/lrose-soloii
#
# Required packages are gcc42 (or later), gtk+, and glib.
#
# The minimum set of *specifically i386* packages required for build on Fedora or Red Hat systems includes:
#
# gtk+-devel, glibc-devel, glib-devel, libXi-devel, libXext-devel, libX11-devel, netcdf-devel.
#

ENV PACKAGES_KEEP gtk+ libX11.i386 netcdf.i386 libgcc.i386 fonts-ISO8859-2
ENV PACKAGES gcc gcc-gfortran pkgconfig make glibc-utils glibc-devel ncurses-devel
ENV PACKAGES_i386 gtk+-devel.i386 glibc-devel.i386 glib-devel.i386 libXi-devel.i386 libXext-devel.i386 libX11-devel.i386  netcdf-devel.i386

#
# configure: error: Failed to run gtk-config. Is gtk+ (gtk+-devel) installed on the system?
#
ENV PKG_CONFIG_PATH /usr/lib/pkgconfig

#
# build & install soloii
#

ENV SOLO_DIR /tmp/lrose-soloii
RUN mkdir $SOLO_DIR
COPY . $SOLO_DIR

#
# downgrade selinux to avoid error when installing fonts-ISO8859-2
#     package libselinux-1.33.4-5.7.el5.centos.x86_64 (which is newer than libselinux-1.33.4-5.7.el5.i386) is already installed
#

RUN yum -y downgrade libselinux \
    && yum -y install $PACKAGES_KEEP $PACKAGES $PACKAGES_i386 \
    && cd $SOLO_DIR \
    && ./configure --prefix=/usr \
    && make \
    && cd - && rm -rf $SOLO_DIR \
    && yum -y remove $PACKAGES $PACKAGES_i386 \
    && yum -y clean all

# RUN mkdir /app
# WORKDIR /app

COPY docker/entrypoint.sh /usr/bin/soloii-entrypoint

ENTRYPOINT ["/usr/bin/soloii-entrypoint"]
