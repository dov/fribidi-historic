noinst_PROGRAMS = test_fribidi fribidi_benchmark

INCLUDES = $(GLIB_CFLAGS)

lib_LTLIBRARIES = libfribidi.la

libfribidi_la_SOURCES = \
	fribidi.c	    \
	fribidi_tables.i    \
	fribidi_char_sets.c \
	fribidi_get_type.c  \
	fribidi_utils.c

libfribidi_la_LIBADD = $(GLIB_LIBS)

libfribidiincdir = $(includedir)/fribidi

libfribidiincdir = $(includedir)/fribidi
libfribidiinc_HEADERS = \
	fribidi.h \
	fribidi_char_sets.h \
	fribidi_types.h

EXTRA_DIST = \
        CreateGetType.pl \
	tests/test-hebrew \
	tests/test-capital-rtl \
	run.tests \
	test.reference \
	ANNOUNCE

test_fribidi_SOURCES = test_fribidi.c
test_fribidi_LDADD = libfribidi.la $(GLIB_LIBS)

fribidi_benchmark_SOURCES = fribidi_benchmark.c
fribidi_benchmark_LDADD = libfribidi.la $(GLIB_LIBS)

bin_PROGRAMS = fribidi
fribidi_SOURCES = fribidi_main.c
fribidi_LDADD = libfribidi.la $(GLIB_LIBS)

test.reference : tests/test-capital-rtl
	test_fribidi -capital_rtl tests/test-capital-rtl > test.reference

test : 	test_fribidi 
	test_fribidi -capital_rtl tests/test-capital-rtl

TESTS = run.tests

bin_SCRIPTS = fribidi-config
