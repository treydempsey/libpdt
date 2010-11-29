AC_INIT([libpdt], [0.0.2])
AC_CONFIG_SRCDIR([src/class_ParseDelimitedText_i.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_C
AC_PROG_AWK
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROG([PERL], [perl], [perl], )
PKG_PROG_PKG_CONFIG

AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h locale.h stdint.h stdlib.h string.h])

AC_ARG_ENABLE(perl-bindings,
  [AS_HELP_STRING([--enable-perl-bindings], [build perl bindings for libpdt])],
  [case "${enableval}" in
    yes) perl_bindings=true ;;
    no)  perl_bindings=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-perl-bindings]) ;;
  esac],[perl_bindings=false]
)
AM_CONDITIONAL(PERL_BINDINGS, test x$perl_bindings = xtrue)
  
AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug], [Turn on debugging])],
  [case "${enableval}" in
    yes) debug=true ;;
    no)  debug=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
  esac],[debug=false]
)
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

PKG_CHECK_MODULES([libtgcl], [libtgcl],,AC_MSG_ERROR([Missing required library libtgcl]))

AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strchr])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/tests/Makefile
  bindings/perl/Makefile
  bindings/perl/Text-ParseDelimited/Makefile.PL
  tools/Makefile
]) 
AC_OUTPUT
