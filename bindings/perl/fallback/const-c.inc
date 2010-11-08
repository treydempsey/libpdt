#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_10 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     PDT_ENOMEM PDT_EPARSE PDT_STRICT */
  /* Offset 5 gives the best switch position.  */
  switch (name[5]) {
  case 'N':
    if (memEQ(name, "PDT_ENOMEM", 10)) {
    /*                    ^           */
#ifdef PDT_ENOMEM
      *iv_return = PDT_ENOMEM;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'P':
    if (memEQ(name, "PDT_EPARSE", 10)) {
    /*                    ^           */
#ifdef PDT_EPARSE
      *iv_return = PDT_EPARSE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "PDT_STRICT", 10)) {
    /*                    ^           */
#ifdef PDT_STRICT
      *iv_return = PDT_STRICT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!perl -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(IV)};
my @names = (qw(PDT_EINVALID PDT_ENOMEM PDT_EPARSE PDT_ETOOBIG PDT_REPALL_NL
	       PDT_STRICT PDT_STRICT_FINI PDT_SUCCESS));

print constant_types(), "\n"; # macro defs
foreach (C_constant ("PDT", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "\n#### XS Section:\n";
print XS_constant ("PDT", $types);
__END__
   */

  switch (len) {
  case 10:
    return constant_10 (aTHX_ name, iv_return);
    break;
  case 11:
    /* Names all of length 11.  */
    /* PDT_ETOOBIG PDT_SUCCESS */
    /* Offset 5 gives the best switch position.  */
    switch (name[5]) {
    case 'T':
      if (memEQ(name, "PDT_ETOOBIG", 11)) {
      /*                    ^            */
#ifdef PDT_ETOOBIG
        *iv_return = PDT_ETOOBIG;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'U':
      if (memEQ(name, "PDT_SUCCESS", 11)) {
      /*                    ^            */
#ifdef PDT_SUCCESS
        *iv_return = PDT_SUCCESS;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    }
    break;
  case 12:
    if (memEQ(name, "PDT_EINVALID", 12)) {
#ifdef PDT_EINVALID
      *iv_return = PDT_EINVALID;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 13:
    if (memEQ(name, "PDT_REPALL_NL", 13)) {
#ifdef PDT_REPALL_NL
      *iv_return = PDT_REPALL_NL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 15:
    if (memEQ(name, "PDT_STRICT_FINI", 15)) {
#ifdef PDT_STRICT_FINI
      *iv_return = PDT_STRICT_FINI;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

