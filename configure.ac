AC_INIT([src/class_ParseDelimitedText_i.c])
AM_INIT_AUTOMAKE([libpdt],[0.0.1])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_C
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h locale.h stdint.h stdlib.h string.h])

AC_ARG_WITH([tgcl-include-dir],
  [AS_HELP_STRING([--with-tgcl-include-dir], [location of the libtgcl header files, defaults to /usr/local/include/tgcl])],
  [TGCL_CFLAGS="-I$withvalue"],
  [
    AX_EXT_HAVE_HEADER([tgcl.h], [/usr/include/tgcl /usr/local/include/tgcl /opt/local/include/tgcl])
  ]
)
AC_SUBST([TGCL_CFLAGS])

AC_ARG_WITH([tgcl-lib-dir],
  [AS_HELP_STRING([--with-tgcl-lib-dir], [location of the libtgcl library])],
  [TGCL_LIBS="-L$withval -ltgcl"],
  [TGCL_LIBS="-L/usr/local/lib -ltgcl"]
)
AC_SUBST([TGCL_LIBS])

AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strchr])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/tests/Makefile
]) 
AC_OUTPUT
