# -*- Makefile -*-

AM_CPPFLAGS = ${my_CPPFLAGS}
ACLOCAL_AMFLAGS = -I m4

include_HEADERS = src/drw_base.h src/drw_entities.h src/drw_interface.h src/drw_objects.h src/libdxfrw.h
pkginclude_HEADERS = src/dxfreader.h src/dxfwriter.h src/drw_cptable932.h src/drw_cptable936.h src/drw_cptable949.h \
  src/drw_cptable950.h src/drw_cptables.h src/drw_textcodec.h

lib_LTLIBRARIES = libdxfrw.la

libdxfrw_la_SOURCES = src/drw_entities.cpp src/drw_objects.cpp src/libdxfrw.cpp src/dxfreader.cpp src/dxfwriter.cpp \
  src/drw_textcodec.cpp
libdxfrw_la_LDFLAGS = -release ${PACKAGE_VERSION}
libdxfrw_la_LIBADD =

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libdxfrw.pc

EXTRA_DIST = autogen.sh

libdxfrw.pc: ${top_builddir}/config.status
