LIBS = `gtk-config --libs`
CFLAGS = -g -Wall `gtk-config --cflags`

noinst_PROGRAMS = test_fribidi fribidi_benchmark

lib_LIBRARIES = libfribidi.a
libfribidi_a_SOURCES = \
	fribidi.c	    \
	fribidi_tables.i    \
	fribidi_char_sets.c \
	fribidi_get_type.c  

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
test_fribidi_LDADD= libfribidi.a 

fribidi_benchmark_SOURCES = fribidi_benchmark.c
fribidi_benchmark_LDADD= libfribidi.a 

bin_PROGRAMS = fribidi
fribidi_SOURCES = fribidi_main.c
fribidi_LDADD = libfribidi.a

test.reference : tests/test-capital-rtl
	test_fribidi -capital_rtl tests/test-capital-rtl > test.reference

test : 	test_fribidi 
	test_fribidi -capital_rtl tests/test-capital-rtl

TESTS = run.tests
