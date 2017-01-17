# -*- Makefile.am -*-
ACLOCAL_AMFLAGS = -I m4

dwg2textdir = $(srcdir)

bin_PROGRAMS = dwg2text

AM_LDFLAGS = -L../src -ldxfrw

dist_noinst_HEADERS = dx_iface.h dx_data.h

dwg2text_SOURCES = dx_iface.cpp main.cpp

AM_CPPFLAGS = ${my_CPPFLAGS} -Wall -I$(top_srcdir)/src

man_MANS = dwg2text.1

EXTRA_DIST = $(dwg2text_HEADERS) $(dwg2text_SOURCES) $(man_MANS) ./vs2013/*
