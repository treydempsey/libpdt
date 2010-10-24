AC_INIT(src/class_ParseDelimitedText_i.c)
AM_INIT_AUTOMAKE(libparse_delimited_text,0.0.1)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_C
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

AC_HEADER_STDC

AC_CHECK_HEADERS(ctype.h stdio.h stdlib.h string.h errno.h,,AC_MSG_ERROR([
required header file missing]))

AC_CHECK_FUNCS(malloc realloc,,AC_MSG_ERROR([
required standard library function missing]))

AC_OUTPUT(Makefile src/Makefile src/tests/Makefile)

AX_PKG_SWIG(2.0.1,,AC_MSG_WARN([Not building perl extension]))
