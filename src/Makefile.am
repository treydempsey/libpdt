AUTOMAKE_OPTIONS = gnu
AM_CFLAGS = -g -Wall -arch x86_64
AM_LDFLAGS = -version-info 0:0:0
lib_LTLIBRARIES = libparse_delimited_text.la

libparse_delimited_text_la_SOURCES = class_ArrayElement_i.c class_Array_i.c class_ParseDelimitedText_i.c class_String_i.c
libparse_delimited_text_la_includedir = $(includedir)/parse_delimited_text
libparse_delimited_text_la_include_HEADERS = class_Array.h class_ArrayElement.h class_ParseDelimitedText.h class_String.h
noinst_HEADERS = class_ArrayElement_i.h class_Array_i.h class_ParseDelimitedText_i.h class_String_i.h dependencies.h
