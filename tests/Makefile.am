TESTS = test_basic
check_PROGRAMS = test_basic

test_basic_SOURCES = test_basic.cpp test_interface.h
test_basic_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_basic_LDADD = $(top_builddir)/src/libdxfrw.la

CLEANFILES = test_output.dxf test_binary.dxf
