AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libpdt.la

AM_CFLAGS = -g -std=c99 -Wall -arch x86_64
AM_LDFLAGS = -version-info 0:0:0

libpdt_la_SOURCES = class_ParseDelimitedText_i.c class_PDTFormatFile_i.c class_PDTFormat_i.c class_PDTColumn_i.c class_PDTValidation_i.c class_PDTValidator_i.c
libpdt_la_includedir = $(includedir)/tgcl
libpdt_la_include_HEADERS = class_ParseDelimitedText.h class_PDTFormatFile.h class_PDTFormat.h class_PDTColumn.h class_PDTValidation.h class_PDTValidator.h pdt.h
libpdt_la_CPPFLAGS = $(libtgcl_CFLAGS)
libpdt_la_LIBS = $(libtgcl_LIBS)
noinst_HEADERS = class_ParseDelimitedText_i.h class_PDTFormatFile_i.h class_PDTFormat_i.h class_PDTColumn_i.h class_PDTValidation_i.h class_PDTValidator_i.h dependencies.h

bin_PROGRAMS = dump_format_file
dump_format_file_SOURCES = dump_format_file.c
dump_format_file_LDADD = libpdt.la
dump_format_file_CPPFLAGS = $(libtgcl_CFLAGS)
dump_format_file_LDFLAGS = $(libtgcl_LIBS)

