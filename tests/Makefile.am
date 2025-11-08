TESTS = test_basic test_entities test_polylines test_text test_tables test_blocks test_versions test_errors
check_PROGRAMS = test_basic test_entities test_polylines test_text test_tables test_blocks test_versions test_errors

test_basic_SOURCES = test_basic.cpp test_interface.h
test_basic_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_basic_LDADD = $(top_builddir)/src/libdxfrw.la

test_entities_SOURCES = test_entities.cpp test_interface.h
test_entities_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_entities_LDADD = $(top_builddir)/src/libdxfrw.la

test_polylines_SOURCES = test_polylines.cpp test_interface.h
test_polylines_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_polylines_LDADD = $(top_builddir)/src/libdxfrw.la

test_text_SOURCES = test_text.cpp test_interface.h
test_text_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_text_LDADD = $(top_builddir)/src/libdxfrw.la

test_tables_SOURCES = test_tables.cpp test_interface.h
test_tables_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_tables_LDADD = $(top_builddir)/src/libdxfrw.la

test_blocks_SOURCES = test_blocks.cpp test_interface.h
test_blocks_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_blocks_LDADD = $(top_builddir)/src/libdxfrw.la

test_versions_SOURCES = test_versions.cpp test_interface.h
test_versions_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_versions_LDADD = $(top_builddir)/src/libdxfrw.la

test_errors_SOURCES = test_errors.cpp test_interface.h
test_errors_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/tests
test_errors_LDADD = $(top_builddir)/src/libdxfrw.la

CLEANFILES = test_output.dxf test_binary.dxf test_*.dxf *.dxf
