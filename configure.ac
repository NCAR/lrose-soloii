# $Id$
#

dnl Process this file with autoconf to produce a configure script.

AC_INIT(perusal/soloii.c, 1.16)

#
# Use our parent directory rather than /usr/local as the default prefix
#
ac_default_prefix=`pwd`/..

dnl Begin generic configure.in
ac_os_type="unknown"

if eval "type uname"; then
   ac_uname_stuff=`uname -a`
   AC_MSG_CHECKING(for OS in $ac_uname_stuff)

   if eval "echo -n \"$ac_uname_stuff\" | grep \"SunOS\" >/dev/null"; then
      ac_os_type="SunOS"
      AC_MSG_RESULT(IT'S $ac_os_type)

   elif eval "echo -n \"$ac_uname_stuff\" | grep \"IRIX\" >/dev/null"; then
      ac_os_type="IRIX"
      AC_MSG_RESULT(IT'S $ac_os_type)

   elif eval "echo -n \"$ac_uname_stuff\" | grep \"HP\-UX\" >/dev/null"; then
      ac_os_type="HPUX"
      AC_MSG_RESULT(IT'S $ac_os_type)

   elif eval "echo -n \"$ac_uname_stuff\" | grep \"Linux\" >/dev/null"; then
      ac_os_type="Linux"
      AC_MSG_RESULT(IT'S $ac_os_type)
   elif eval "echo -n \"$ac_uname_stuff\" | grep \"Darwin\" >/dev/null"; then
      ac_os_type="Darwin"
      AC_MSG_RESULT(IT'S $ac_os_type)
   else		  
      AC_MSG_RESULT(found NO match for OS names!)
#      exit 1
   fi
else
    echo "CAN'T DETERMINE OS TYPE!"
#    exit 1
fi

dnl Check for sys/mtio.h
AC_CHECK_HEADERS(sys/mtio.h)

dnl Checks for programs.
#c-----------------------------------------------------
AC_PROG_CC
#c-----------------------------------------------------
AC_PROG_CPP
#c-----------------------------------------------------
AC_PROG_INSTALL
#c-----------------------------------------------------
AC_PROG_LN_S
#c-----------------------------------------------------
AC_PROG_MAKE_SET
AC_CHECK_LIB(m, cos)

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL

AC_C_BIGENDIAN
LOCAL_DEFS="-DUNIX"
if test $ac_cv_c_bigendian != yes; then
   LOCAL_DEFS="$LOCAL_DEFS -DLITTLENDIAN"
fi
#c---- AC_PATH_XTRA -----------------------------------
AC_PATH_XTRA
#c-----------------------------------------------------

SOLODIR=`pwd`
SOLOLIBDIR='${LIBDIR}'
SOLOBINDIR='${BINDIR}'

LOCAL_INCLUDES='-I./ -I${SOLODIR}/include'

XLIBDIR=

AC_DEFUN( AC_FIND_LOCALHDR, [
dnl echo "find_localhdr $1,$2"
for ac_incdir in $2
do
if test -r $ac_incdir/$1; then
   LOCAL_INCLUDES="$LOCAL_INCLUDES -I$ac_incdir"
   AC_MSG_RESULT(found in $ac_incdir)
fi
done
] )

dnl AC_MSG_CHECKING("for gtk.h ")
dnl AC_FIND_LOCALHDR(gtk/gtk.h,/usr/include /usr/include/gtk-1.2 /usr/local/include /usr/local/include/gtk-1.2)

dnl AC_MSG_CHECKING("for glib.h ")
dnl AC_FIND_LOCALHDR(glib.h,/usr/include /usr/include/glib-1.2 /usr/local/include /usr/local/include/glib-1.2)

AC_MSG_CHECKING("for netcdf.h ")
AC_FIND_LOCALHDR(netcdf.h,/usr/include /usr/include/netcdf /usr/local/include /usr/local/include/netcdf /usr/lib /usr/lib/netcdf /usr/local/lib/netcdf /usr/include/netcdf-3)

AC_MSG_CHECKING("for libnetcdf.a ")
NETCDFLIBDIR=
for ac_xdir in /usr/lib /lib /local/lib /usr/local/lib /usr/lib/netcdf /lib/netcdf /local/lib/netcdf /usr/local/lib/netcdf /usr/lib/netcdf-3
do

if test -r $ac_xdir/libnetcdf.a; then
   NETCDFLIBDIR=$ac_xdir
   AC_MSG_RESULT(found in $ac_xdir )
fi
done

AC_MSG_CHECKING("for FORTRAN ")
WHICHFORTRAN=
FCSTATICFLAG=
FCFLAGS="-g"

if test -n "$FC"; then
   set dummy $FC
   shift
   WHICHFORTRAN="$1"
elif eval "type gfortran"; then
   WHICHFORTRAN="gfortran";
elif eval "type g77"; then
   WHICHFORTRAN="g77"
elif eval "type f77"; then
   WHICHFORTRAN="f77"
else
   AC_MSG_ERROR("No FORTRAN compiler found!")
fi
AC_MSG_RESULT( WHICHFORTRAN: $WHICHFORTRAN )

if test "$WHICHFORTRAN" = "g77" || test "$WHICHFORTRAN" = "gfortran"; then
   FCSTATICFLAG="-static"
elif test "$WHICHFORTRAN" = "f77"; then
   FCSTATICFLAG="-Bstatic"
fi

DAPLIB=libdap.a
FLIBS="-lm"

if test -n "$NETCDFLIBDIR"; then
   LOCAL_DEFS="$LOCAL_DEFS -DNETCDF"
   LIBS="$LIBS -L$NETCDFLIBDIR -lnetcdf"

   if test "$ac_os_type" = "HPUX" -a "$WHICHFORTRAN" = "f77"; then
      FLIBS="$FLIBS -Wl,-L$NETCDFLIBDIR -lnetcdf"
      FCFLAGS="$FCFLAGS +ppu"
   else
      FLIBS="$FLIBS -L$NETCDFLIBDIR -lnetcdf"
   fi
fi

REODORADE_LIBS=

for ac_dir in $SOLOLIBDIR $SOLODIR/translate
do
    if test -d $ac_dir; then
    echo "$ac_dir"
    if test -r $ac_dir/libdap.a -a -r $ac_dir/libs2.a; then
       REODORADE_LIBS=$ac_dir
       break;
    fi
    fi
done

WHICH_INSTALL=
if eval "type install"; then
    ac_sh_test=`type install`
    for ii in $ac_sh_test
    do
	if eval "echo -n \"$ii\" | grep '/'"; then
	   WHICH_INSTALL="$ii"
	   echo " "
	fi
    done
fi

# force it to use supplied install
WHICH_INSTALL="$SOLODIR/install-sh"

set dummy $CC; shift
WHICHCC="$1"


if test "$ac_os_type" = "SunOS"; then
      LOCAL_DEFS="$LOCAL_DEFS -DSVR4 -DSYSV -Dsun"
      LIBS="$LIBS -lgen -ltermcap -lsocket -lnsl"

elif test "$ac_os_type" = "IRIX"; then
      LOCAL_DEFS="$LOCAL_DEFS -DSVR4 -DSYSV -Dsgi"
      LIBS="$LIBS -lgen -ltermcap"
      if test "WHICHCC" = "cc"; then
        if test -n "$CFLAGS"; then
            CFLAGS="$CFLAGS -n32"
        else
            CFLAGS="-n32 "
        fi
      fi
      if test -n "$WHICHFORTRAN" -a "$WHICHFORTRAN" = "f77"; then
        FCFLAGS="$FCFLAGS -n32"
	FCSTATICFLAG=
      fi
      X_LIBS=
      X_CFLAGS=

elif test "$ac_os_type" = "HPUX"; then
     set dummy $CC; shift
     if test "$WHICHCC" = "cc"; then
        if test -n "$CFLAGS"; then
            CFLAGS="$CFLAGS -Ae"
        else
            CFLAGS="-Ae "
        fi
     fi
     LOCAL_DEFS="$LOCAL_DEFS -DSYSV -Dhpux"
     LIBS="$LIBS -ltermcap"
     if test -n "$WHICHFORTRAN" -a "$WHICHFORTRAN" = "f77"; then
	FCSTATICFLAG=
        LOCAL_DEFS="$LOCAL_DEFS -Dhpux_f77"
     fi

elif test "$ac_os_type" = "Linux"; then
     if test -z "$CFLAGS"; then
          CFLAGS=" "
     fi
     # Put on the -m32 flag to force building a 32-bit binary!
     CFLAGS="$CFLAGS -g -m32"
     LIBS="$LIBS -lncurses"
     
     if test -z "$FCFLAGS"; then
          FCFLAGS=" "
     fi
     FCFLAGS="$FCFLAGS -m32"

elif test "$ac_os_type" = "Darwin"; then
     if test -z "$CFLAGS"; then
          CFLAGS=" "
     fi
     # Put on the -m32 flag to force building a 32-bit binary!
     CFLAGS="$CFLAGS -g -m32"
     
     if test -z "$FCFLAGS"; then
          FCFLAGS=" "
     fi
     FCFLAGS="$FCFLAGS -m32"
     
fi

ac_sh_test=`type sh`
for ii in $ac_sh_test
do
    if eval "echo -n \"$ii\" | grep '/'"; then
       WHICH_SHELL="$ii"
       echo " "
    fi
done

GTKLIBS=`gtk-config --libs`
if test $? -ne 0; then
    AC_MSG_ERROR(Failed to run gtk-config. Is gtk+ (gtk+-devel) installed on the system?)
fi
GTKINCLUDES=`gtk-config --cflags`

AC_MSG_RESULT( GTKLIBS: $GTKLIBS )
AC_MSG_RESULT( GTKINCLUDES: $GTKINCLUDES )
AC_MSG_RESULT( CC: $CC )
AC_MSG_RESULT( CFLAGS: $CFLAGS )
AC_MSG_RESULT( FCFLAGS: $FCFLAGS )
AC_MSG_RESULT( LIBS: $LIBS )
AC_MSG_RESULT( FLIBS: $FLIBS )
AC_MSG_RESULT( LOCAL_INCLUDES: $LOCAL_INCLUDES )
AC_MSG_RESULT( DAPLIB: $DAPLIB )
AC_MSG_RESULT( WHICHFORTRAN: $WHICHFORTRAN )
AC_MSG_RESULT( LOCAL_DEFS: $LOCAL_DEFS )
AC_MSG_RESULT( WHICH_SHELL: $WHICH_SHELL )
AC_MSG_RESULT( WHICH_INSTALL: $WHICH_INSTALL )
AC_MSG_RESULT( FCSTATICFLAG: $FCSTATICFLAG )
AC_MSG_RESULT( REODORADE_LIBS: $REODORADE_LIBS )
AC_MSG_RESULT( SOLODIR: $SOLODIR )
AC_MSG_RESULT( SOLOLIBDIR: $SOLOLIBDIR )
AC_MSG_RESULT( SOLOBINDIR: $SOLOBINDIR )
AC_MSG_RESULT( exec_prefix: $exec_prefix )
AC_MSG_RESULT( libdir: $libdir )
AC_MSG_RESULT( bindir: $bindir )
dnl AC_MSG_RESULT( : $ )

AC_SUBST(FCFLAGS)
AC_SUBST(LOCAL_INCLUDES)
AC_SUBST(LOCAL_DEFS)
AC_SUBST(WHICHFORTRAN)
AC_SUBST(DAPLIB)
AC_SUBST(FLIBS)
AC_SUBST(WHICH_SHELL)
AC_SUBST(WHICH_INSTALL)
AC_SUBST(FCSTATICFLAG)
AC_SUBST(REODORADE_LIBS)
AC_SUBST(SOLODIR)
AC_SUBST(SOLOLIBDIR)
AC_SUBST(SOLOBINDIR)
AC_SUBST(GTKLIBS)
AC_SUBST(GTKINCLUDES)

dnl remember for IRIX cc compiler you need a -n32 flag if various libraries
dnl have been built with the same flag

echo "ac_os_type: $ac_os_type"

echo " "
echo "#### Ran configure in `pwd` ####"
echo " "
dnl End generic configure.in

AC_CONFIG_FILES(editor/Makefile perusal/Makefile translate/Makefile Makefile)
AC_OUTPUT
