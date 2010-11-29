AUTOMAKE_OPTIONS = gnu

AM_CPPFLAGS = $(libtgcl_CFLAGS)
AM_CFLAGS = -g -std=c99 -Wall -arch x86_64 -fnested-functions
AM_LDFLAGS = $(libtgcl_LIBS)
LDADD = ../src/libpdt.la

bin_PROGRAMS = add_constants standard_form validate_content validate_structure

add_constants_SOURCES = add_constants.c

standard_form_SOURCES = standard_form.c

validate_content_SOURCES = validate_content.c

validate_structure_SOURCES = validate_structure.c
