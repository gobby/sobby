#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([sobby], [0.2.0], [crew@0x539.de])
AM_INIT_AUTOMAKE(1.9 check-news)
AC_CONFIG_SRCDIR([inc/server.hpp])
AC_CONFIG_HEADER([inc/config.hpp])

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
PKG_CHECK_MODULES([sobby], [net6-1.1 obby-0.2 glibmm-2.4 >= 2.6.0])

# Check for howl in libobby.
AC_CHECK_LIB(obby, obby_has_howl)
if test "x$ac_cv_lib_obby_obby_has_howl" = "xyes" ; then
	# We do not need any new flags as obby should take care of it.
	PKG_CHECK_MODULES([howl], [howl])
	AC_DEFINE([WITH_HOWL], 1, [Define whether to use howl support])
fi
AM_CONDITIONAL(WITH_HOWL, test x$ac_cv_lib_obby_obby_has_howl = xyes)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

