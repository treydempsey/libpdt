AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libpdt.la

AM_CFLAGS = -g -std=c99 -Wall -arch x86_64
AM_LDFLAGS = -version-info 0:0:0

libpdt_la_SOURCES = class_ParseDelimitedText_i.c
libpdt_la_includedir = $(includedir)/parse_delimited_text
libpdt_la_include_HEADERS = class_ParseDelimitedText.h
libpdt_la_CPPFLAGS = $(TGCL_CFLAGS)
libpdt_la_LIBS = $(TGCL_LIBS)
noinst_HEADERS = class_ParseDelimitedText_i.h dependencies.h
