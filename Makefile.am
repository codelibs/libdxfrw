# -*- Makefile -*-

AM_CPPFLAGS = ${my_CPPFLAGS} -Wall
ACLOCAL_AMFLAGS = -I m4

library_includedir=$(includedir)/libdxfrw$(LIBRARY_AGE)
library_include_HEADERS = src/drw_base.h src/drw_entities.h src/drw_interface.h \
	src/drw_objects.h src/drw_header.h src/drw_classes.h src/libdxfrw.h src/libdwgr.h
dist_noinst_HEADERS = src/intern/dxfreader.h src/intern/dxfwriter.h src/intern/drw_dbg.h \
	src/intern/dwgutil.h src/intern/dwgreader.h src/intern/dwgreader15.h \
	src/intern/dwgreader18.h src/intern/dwgreader21.h src/intern/dwgreader24.h \
	src/intern/dwgreader27.h  src/intern/dwgbuffer.h src/intern/drw_cptable932.h \
	src/intern/drw_cptable936.h src/intern/drw_cptable949.h src/intern/drw_cptable950.h \
	src/intern/drw_cptables.h src/intern/drw_textcodec.h src/intern/rscodec.h

lib_LTLIBRARIES = libdxfrw.la

libdxfrw_la_SOURCES = src/drw_entities.cpp src/drw_objects.cpp src/drw_header.cpp \
		      src/drw_classes.cpp src/libdwgr.cpp src/libdxfrw.cpp src/intern/dwgutil.cpp \
		      src/intern/dxfreader.cpp src/intern/dwgreader15.cpp src/intern/dwgreader18.cpp src/intern/dwgreader21.cpp \
		      src/intern/dwgreader24.cpp src/intern/dwgreader27.cpp src/intern/dxfwriter.cpp src/intern/dwgreader.cpp \
		      src/intern/dwgbuffer.cpp src/intern/drw_textcodec.cpp src/intern/rscodec.cpp

libdxfrw_la_LDFLAGS = -no-undefined -version-number $(LIBRARY_AGE):$(LIBRARY_CURRENT):$(LIBRARY_REVISION)

libdxfrw_la_LIBADD =

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libdxfrw$(LIBRARY_AGE).pc

EXTRA_DIST = autogen.sh makefile.mingw

libdxfrw.pc: ${top_builddir}/config.status
