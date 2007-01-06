#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([sobby], [0.4.0rc1], [crew@0x539.de])
AM_INIT_AUTOMAKE(1.9)
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
if test "x$host_os" = "xmingw32" ; then
        win32=true
        AC_MSG_RESULT([yes])
else
        win32=false
        AC_MSG_RESULT([no])
fi
AM_CONDITIONAL(WIN32, test x$win32 = xtrue)
AC_CACHE_SAVE

# Checks for libraries.
PKG_CHECK_MODULES([sobby], [net6-1.3 obby-0.4 glibmm-2.4 >= 2.6.0 libxml++-2.6])

# Check for howl in libobby.
AC_CHECK_LIB(obby, obby_has_zeroconf, [obby_has_zeroconf=yes],
             [obby_has_zeroconf=no], [$pkg_cv_sobby_LIBS])
if test "x$ac_cv_lib_obby_obby_has_zeroconf" = "xyes" ; then
	# obby's pkg-config script takes care of any additional dependencies
	AC_DEFINE([WITH_ZEROCONF], 1, [Define whether to use Zeroconf])
fi
AM_CONDITIONAL(WITH_ZEROCONF, test x$ac_cv_lib_obby_obby_has_zeroconf = xyes)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

