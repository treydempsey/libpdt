#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include "const-c.inc"

#include <class_ParseDelimitedText.h>


static HV * PDT_contexts;


typedef struct ParseDelimitedTextXS {
  ParseDelimitedText *  self;
  SV *                  field_callback;
  SV *                  record_callback;
  int                   retain;
} ParseDelimitedTextXS;


static void
field_callback_shim(ParseDelimitedText * self, String * field)
{
  dSP;

  SV **                   p_xs_context;
  ParseDelimitedTextXS *  xs;
  SV *                    field_sv;
  HV *                    stash;
  SV *                    xs_rv;

  ENTER;
  SAVETMPS;

  p_xs_context = hv_fetch(PDT_contexts, (char *)self, sizeof(ParseDelimitedText *), 0);
  if(p_xs_context == NULL) { 
    croak("ParseDelimitedTextXS::field_callback_shim() could not locate context\n");
  }

  xs = (ParseDelimitedTextXS *)SvUV(*p_xs_context);
  if(xs == NULL) {
    croak("ParseDelimitedTextXS::field_callback_shim() could not locate context\n");
  }
  //xs->retain = 1;

  stash = gv_stashpvn("ParseDelimitedTextXS", 20, TRUE);

  PUSHMARK(SP);
  if(xs->field_callback != NULL) {
    //xs_rv = newRV_noinc(*p_xs_context);
    //sv_bless(xs_rv, stash);
    field_sv = newSVpv(field->string, field->length);
    XPUSHs(*p_xs_context);
    XPUSHs(sv_2mortal(field_sv));
    PUTBACK;

    call_sv(xs->field_callback, G_DISCARD);
  }
  else {
    PUTBACK;
  }

  FREETMPS;
  LEAVE;
}


static void
record_callback_shim(ParseDelimitedText * self, char eol)
{
  dSP;

  SV **                   p_xs_sv;
  SV *                    xs_sv;
  ParseDelimitedTextXS *  xs;
  SV *                    eol_sv;

  ENTER;
  SAVETMPS;

  p_xs_sv = hv_fetch(PDT_contexts, (char *)self, sizeof(ParseDelimitedText *), 0);
  if(p_xs_sv == NULL) { 
    croak("ParseDelimitedTextXS::record_callback_shim() could not locate context\n");
  }

  xs_sv = *p_xs_sv;
  xs = (ParseDelimitedTextXS *)SvUV(xs_sv);

  PUSHMARK(SP);
  if(xs->record_callback != NULL) {
    eol_sv = newSVpv(&eol, 1);

    XPUSHs(xs_sv);
    XPUSHs(sv_2mortal(eol_sv));
    PUTBACK;

    call_sv(xs->record_callback, G_DISCARD);
  }
  else {
    PUTBACK;
  }

  FREETMPS;
  LEAVE;
}


static int
set_block_size(pTHX_ SV * sv, MAGIC * mg)
{
  if(! SvOK(sv)) {
    croak("only defined values can be assigned to block_size");
  }

  ((ParseDelimitedTextXS *)SvUV(LvTARG(sv)))->self->block_size = SvIV(sv);

  return TRUE;
}

static MGVTBL block_size_vtbl = {
  0, set_block_size,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_delimiter(pTHX_ SV * sv, MAGIC * mg)
{
  ParseDelimitedTextXS *  xs;
  char *                  new_delimiter;
  STRLEN                  new_delimiter_length;

  if(! SvOK(sv)) {
    croak("only defined values can be assigned to delimiter");
  }

  xs = (ParseDelimitedTextXS *)SvUV(LvTARG(sv));
  new_delimiter = SvPV(sv, new_delimiter_length);
  xs->self->m->set_delimiter(xs->self, new_delimiter, new_delimiter_length);

  return TRUE;
}

static MGVTBL delimiter_vtbl = {
  0, set_delimiter,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_quote(pTHX_ SV * sv, MAGIC * mg)
{
  ParseDelimitedTextXS *  xs;
  char *                  new_quote;
  STRLEN                  new_quote_length;

  xs = (ParseDelimitedTextXS *)SvUV(LvTARG(sv));

  if(! SvOK(sv)) {
    xs->self->quote = xs->self->quote->m->free(xs->self->quote);
  }
  else {
    new_quote = SvPV(sv, new_quote_length);
    xs->self->m->set_quote(xs->self, new_quote, new_quote_length);
  }

  return TRUE;
}

static MGVTBL quote_vtbl = {
  0, set_quote,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_field_callback(pTHX_ SV * sv, MAGIC * mg)
{
  ParseDelimitedTextXS *  xs;

  if(! SvOK(sv)) {
    // TODO Check sv is a sub routine reference, CV?
    croak("only defined values can be assigned to field_callback");
  }

  xs = (ParseDelimitedTextXS *)SvUV(LvTARG(sv));
  if(xs->field_callback != NULL) {
    SvREFCNT_dec(xs->field_callback);
  }

  xs->field_callback = newSVsv(sv);

  return TRUE;
}

static MGVTBL field_callback_vtbl = {
  0, set_field_callback,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_record_callback(pTHX_ SV * sv, MAGIC * mg)
{
  ParseDelimitedTextXS *  xs;

  if(! SvOK(sv)) {
    // TODO Check sv is a sub routine reference, CV?
    croak("only defined values can be assigned to record_callback");
  }

  xs = (ParseDelimitedTextXS *)SvUV(LvTARG(sv));
  if(xs->record_callback != NULL) {
    SvREFCNT_dec(xs->record_callback);
  }

  xs->record_callback = newSVsv(sv);

  return TRUE;
}

static MGVTBL record_callback_vtbl = {
  0, set_record_callback,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


MODULE = PDT  PACKAGE = ParseDelimitedText

PROTOTYPES: ENABLE

INCLUDE: const-xs.inc

ParseDelimitedTextXS *
new(options)
    unsigned char options
  INIT:
    ParseDelimitedTextXS *  xs;
    SV *                    xs_sv;
  CODE:
    if(PDT_contexts == NULL) {
      PDT_contexts = newHV();
    }

    Newx(xs, sizeof(ParseDelimitedTextXS), ParseDelimitedTextXS);
    xs->self = new_ParseDelimitedText(options);
    xs->self->field_callback = field_callback_shim;
    xs->self->record_callback = record_callback_shim;
    xs->field_callback = NULL;
    xs->record_callback = NULL;
    xs->retain = 0;

    xs_sv = newSVuv((UV)xs);
    hv_store(PDT_contexts, (char *)xs->self, sizeof(ParseDelimitedText *), xs_sv, 0);

    printf("new() xs %p\n", xs);
    printf("new() xs_sv %p\n", xs_sv);

    ST(0) = sv_newmortal();
    sv_setref_pv(ST(0), "ParseDelimitedTextXS", (void *)xs);
    XSRETURN(1);


MODULE = PDT  PACKAGE = ParseDelimitedTextXS

PROTOTYPES: ENABLE

BOOT:
{
  HV *  stash;
  SV ** method;
  CV *  cv;

  class_ParseDelimitedText();

  stash = gv_stashpvn("ParseDelimitedTextXS", 20, TRUE);

  method = hv_fetch(stash, "block_size", 10, 0);
  if(! method) {
    croak("ParseDelimitedTextXS lost method 'block_size'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "delimiter", 9, 0);
  if(! method) {
    croak("ParseDelimitedTextXS lost method 'delimiter'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "quote", 5, 0);
  if(! method) {
    croak("ParseDelimitedTextXS lost method 'quote'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "field_callback", 14, 0);
  if(! method) {
    croak("ParseDelimitedTextXS lost method 'field_callback'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "record_callback", 15, 0);
  if(! method) {
    croak("ParseDelimitedTextXS lost method 'record_callback'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);
}


void
block_size(xs)
    ParseDelimitedTextXS *  xs
  PREINIT:
    SV *                    sv;
  PPCODE:
    sv = newSViv(xs->self->block_size);
    sv_upgrade(sv, SVt_PVLV);
    sv_magic(sv, Nullsv, PERL_MAGIC_ext, Nullch, 0);
    SvSMAGICAL_on(sv);
    LvTYPE(sv) = PERL_MAGIC_ext;
    LvTARG(sv) = SvREFCNT_inc(SvRV(ST(0)));

    SvMAGIC(sv)->mg_virtual = &block_size_vtbl;
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


void
delimiter(xs)
    ParseDelimitedTextXS *  xs
  PREINIT:
    SV *                    sv;
  PPCODE:
    sv = newSVpv(xs->self->delimiter->string, xs->self->delimiter->length);
    sv_upgrade(sv, SVt_PVLV);
    sv_magic(sv, Nullsv, PERL_MAGIC_ext, Nullch, 0);
    SvSMAGICAL_on(sv);
    LvTYPE(sv) = PERL_MAGIC_ext;
    LvTARG(sv) = SvREFCNT_inc(SvRV(ST(0)));

    SvMAGIC(sv)->mg_virtual = &delimiter_vtbl;
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


void
quote(xs)
    ParseDelimitedTextXS *  xs
  PREINIT:
    SV *                    sv;
  PPCODE:
    sv = newSVpv(xs->self->quote->string, xs->self->quote->length);
    sv_upgrade(sv, SVt_PVLV);
    sv_magic(sv, Nullsv, PERL_MAGIC_ext, Nullch, 0);
    SvSMAGICAL_on(sv);
    LvTYPE(sv) = PERL_MAGIC_ext;
    LvTARG(sv) = SvREFCNT_inc(SvRV(ST(0)));

    SvMAGIC(sv)->mg_virtual = &quote_vtbl;
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


void
field_callback(xs)
    ParseDelimitedTextXS *  xs
  PREINIT:
    SV *                    sv;
  PPCODE:
    if(xs->field_callback == NULL) {
      sv = newSV(0);
    }
    else {
      sv = newSVsv(xs->field_callback);
    }
    sv_upgrade(sv, SVt_PVLV);
    sv_magic(sv, Nullsv, PERL_MAGIC_ext, Nullch, 0);
    SvSMAGICAL_on(sv);
    LvTYPE(sv) = PERL_MAGIC_ext;
    LvTARG(sv) = SvREFCNT_inc(SvRV(ST(0)));

    SvMAGIC(sv)->mg_virtual = &field_callback_vtbl;
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


void
record_callback(xs)
    ParseDelimitedTextXS *  xs
  PREINIT:
    SV *                    sv;
  PPCODE:
    if(xs->field_callback == NULL) {
      sv = newSV(0);
    }
    else {
      sv = newSVsv(xs->field_callback);
    }
    sv_upgrade(sv, SVt_PVLV);
    sv_magic(sv, Nullsv, PERL_MAGIC_ext, Nullch, 0);
    SvSMAGICAL_on(sv);
    LvTYPE(sv) = PERL_MAGIC_ext;
    LvTARG(sv) = SvREFCNT_inc(SvRV(ST(0)));

    SvMAGIC(sv)->mg_virtual = &record_callback_vtbl;
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


size_t
parse(xs, input)
    ParseDelimitedTextXS *  xs
    SV *                    input
  INIT:
    char *                  input_cstr;
    size_t                  bytes_parsed;
    STRLEN                  input_length;
  CODE:
    input_cstr = SvPV(input, input_length);
    bytes_parsed = xs->self->m->parse(xs->self, input_cstr, input_length);

    RETVAL = bytes_parsed;
  OUTPUT:
    RETVAL


size_t
finish(xs)
    ParseDelimitedTextXS *  xs
  INIT:
    size_t                  result;
  CODE:
    result = xs->self->m->finish(xs->self);
    RETVAL = result;
  OUTPUT:
    RETVAL


void
stop(xs)
    ParseDelimitedTextXS *  xs
  CODE:
    xs->self->stop = 1;


void
DESTROY(xs)
    ParseDelimitedTextXS * xs
  INIT:
    SV **                  p_xs_context;
  CODE:
    printf("DESTROY() pdt\n");

    if(xs->retain == 0) {
      printf("Freeing\n");
      hv_delete(PDT_contexts, (char *)xs->self, sizeof(ParseDelimitedText *), G_DISCARD);

      if(xs->self != NULL && xs->self != null_ParseDelimitedText) {
        xs->self->m->free(xs->self);
      }
      if(xs->field_callback != NULL) {
         SvREFCNT_dec(xs->field_callback);
      }
      if(xs->record_callback != NULL) {
         SvREFCNT_dec(xs->record_callback);
      }
      Safefree(xs);
    }
    else {
      printf("Retaining\n");
      xs->retain = 0;
    }
