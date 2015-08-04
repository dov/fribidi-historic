LIBS = -lglib

noinst_PROGRAMS = test_fribidi

lib_LIBRARIES = libfribidi.a
libfribidi_a_SOURCES = \
	fribidi.c	    \
	fribidi_tables.i    \
	fribidi_char_sets.c \
	fribidi_get_type.c  \
	test_fribidi.c

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

TESTS = run.tests
