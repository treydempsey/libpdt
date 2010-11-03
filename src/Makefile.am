AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libparse_delimited_text.la

AM_CFLAGS = -g -std=c99 -Wall -arch x86_64
AM_LDFLAGS = -version-info 0:0:0

libparse_delimited_text_la_SOURCES = class_ParseDelimitedText_i.c
libparse_delimited_text_la_includedir = $(includedir)/parse_delimited_text
libparse_delimited_text_la_include_HEADERS = class_ParseDelimitedText.h
libparse_delimited_text_la_CPPFLAGS = $(TGCL_CFLAGS)
libparse_delimited_text_la_LIBS = $(TGCL_LIBS)
noinst_HEADERS = class_ParseDelimitedText_i.h dependencies.h
