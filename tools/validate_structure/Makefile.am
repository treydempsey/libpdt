AUTOMAKE_OPTIONS = gnu

bin_PROGRAMS = validate_structure

AM_CPPFLAGS = -I../../src
AM_CFLAGS = -g -std=c99 -Wall -arch x86_64

validate_structure_SOURCES = validate_structure.c validate_structure.h
validate_structure_LDADD = -L$(top_builddir)/src -lparse_delimited_text
