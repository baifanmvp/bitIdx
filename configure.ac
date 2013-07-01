#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/bIdxFile.h])
AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
# Checks for libraries.
AC_CONFIG_MACRO_DIR([m4])
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
PKG_CHECK_MODULES([glib], [glib-2.0], [],
    AC_MSG_ERROR(["Error! You need to have glib2.0 installed.(glib2.0-dev)"])
    ])

bitidx_version="bitidx"
AC_SUBST(bitidx_version)


AC_CONFIG_FILES([Makefile
                 utest/Makefile
                 src/Makefile
                 bin/Makefile
                 src/lib/Makefile])
AC_OUTPUT
