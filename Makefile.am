# -*- Makefile -*-

AM_CPPFLAGS = ${my_CPPFLAGS}
ACLOCAL_AMFLAGS = -I m4

library_includedir=$(includedir)/libdxfrw$(LIBRARY_AGE)
library_include_HEADERS = src/drw_base.h src/drw_entities.h src/drw_interface.h src/drw_objects.h src/drw_header.h src/libdxfrw.h
dist_noinst_HEADERS = src/intern/dxfreader.h src/intern/dxfwriter.h src/intern/drw_cptable932.h src/intern/drw_cptable936.h \
  src/intern/drw_cptable949.h src/intern/drw_cptable950.h src/intern/drw_cptables.h src/intern/drw_textcodec.h

lib_LTLIBRARIES = libdxfrw.la

libdxfrw_la_SOURCES = src/drw_entities.cpp src/drw_objects.cpp src/drw_header.cpp src/libdxfrw.cpp src/intern/dxfreader.cpp \
			src/intern/dxfwriter.cpp src/intern/drw_textcodec.cpp
libdxfrw_la_LDFLAGS = -version-number $(LIBRARY_AGE):$(LIBRARY_CURRENT):$(LIBRARY_REVISION)
libdxfrw_la_LIBADD =

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libdxfrw$(LIBRARY_AGE).pc

EXTRA_DIST = autogen.sh makefile.mingw

libdxfrw.pc: ${top_builddir}/config.status
