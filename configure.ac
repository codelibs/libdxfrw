#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libdxfrw], [0.2.2], [https://sourceforge.net/projects/libdxfrw/])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/drw_interface.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
LT_INIT
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile libdxfrw.pc])
AC_OUTPUT
