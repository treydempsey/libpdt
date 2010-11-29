AC_INIT([src/class_ParseDelimitedText_i.c])
AM_INIT_AUTOMAKE([libpdt],[0.0.1])
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

AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h locale.h stdint.h stdlib.h string.h])

AC_ARG_ENABLE(perl-bindings,
  [AS_HELP_STRING([--enable-perl-bindings], [build perl bindings for libpdt])],
  [case "${enableval}" in
    yes) perl_bindings=true ;;
    no)  perl_bindings=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-perl-bindings) ;;
  esac],[perl_bindings=false]
)
AM_CONDITIONAL(PERL_BINDINGS, test x$perl_bindings = xtrue)
  
AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug], [Turn on debugging])],
  [case "${enableval}" in
    yes) debug=true ;;
    no)  debug=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
  esac],[debug=false]
)
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

AC_ARG_WITH([tgcl-include-dir],
  [AS_HELP_STRING([--with-tgcl-include-dir], [location of the libtgcl header files, defaults to /usr/local/include/tgcl])],
  [TGCL_CFLAGS="-I$withvalue"],
  [AX_EXT_HAVE_HEADER([tgcl.h], [/usr/include/tgcl /usr/local/include/tgcl /opt/local/include/tgcl])]
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
  bindings/perl/Makefile
  bindings/perl/Text-ParseDelimited/Makefile.PL
  tools/Makefile
]) 
AC_OUTPUT
