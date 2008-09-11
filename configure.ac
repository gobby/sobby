#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([sobby], [0.4.5], [crew@0x539.de])
AM_INIT_AUTOMAKE(1.9 check-news)
AM_MAINTAINER_MODE
AC_CONFIG_SRCDIR([inc/server.hpp])
AC_CONFIG_HEADER([inc/features.hpp])

# Extract host information.
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CACHE_SAVE

AC_MSG_CHECKING([whether to enable WIN32 specific flags])
case "$host_os" in
*mingw*)
	win32=true
	AC_MSG_RESULT([yes])
	;;
*)
	win32=false
	AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL(WIN32, test x$win32 = xtrue)
AC_CACHE_SAVE

# Checks for libraries.
PKG_CHECK_MODULES([sobby], [net6-1.3 obby-0.4 >= 0.4.3
                            glibmm-2.4 >= 2.6.0 libxml++-2.6])

# Check for Zeroconf support in libobby.
AC_CHECK_LIB(obby, obby_has_zeroconf, [obby_has_zeroconf=yes],
             [obby_has_zeroconf=no], [$pkg_cv_sobby_LIBS])
AC_CHECK_LIB(obby, obby_has_avahi, [obby_has_avahi=yes],
             [obby_has_avahi=no], [$pkg_cv_sobby_LIBS])

AC_ARG_ENABLE([zeroconf],
              AS_HELP_STRING([--disable-zeroconf],
                             [disable Zeroconf support (if present in obby)]),
              [zeroconf=$withval], [zeroconf=yes])
zeroconf_enabled=no
if test "x$ac_cv_lib_obby_obby_has_zeroconf" = "xyes" ; then
  if test "x$zeroconf" = "xyes" ; then
    # obby's pkg-config control file takes care of any additional dependencies
    AC_DEFINE([WITH_ZEROCONF], 1, [Define whether to use Zeroconf])
    zeroconf_enabled=yes
    if test "x$ac_cv_lib_obby_obby_has_avahi" = "xyes" ; then
      AC_DEFINE([WITH_AVAHI], 1,
                [Define whether Avahi is used as Zeroconf backend])
      PKG_CHECK_MODULES([avahi], [avahi-glib])
    fi
  fi
fi
AM_CONDITIONAL(WITH_ZEROCONF, test x$zeroconf_enabled = xyes)
AM_CONDITIONAL(WITH_AVAHI, test x$ac_cv_lib_obby_obby_has_avahi = xyes)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

