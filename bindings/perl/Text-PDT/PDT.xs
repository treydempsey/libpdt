#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <class_ParseDelimitedText.h>


static HV * PDT_contexts;


typedef struct ParseDelimitedTextXS {
  ParseDelimitedText *  self;
  SV *                  field_callback;
  SV *                  record_callback;
} ParseDelimitedTextXS;


typedef struct ParseDelimitedTextCB {
  ParseDelimitedText *  self;
} ParseDelimitedTextCB;


static void
field_callback_shim(ParseDelimitedText * self, String * field)
{
  dSP;

  SV **                   p_xs_context;
  ParseDelimitedTextXS *  xs;
  ParseDelimitedTextCB *  xs_cb;
  SV *                    parser_sv;
  SV *                    field_sv;

  ENTER;
  SAVETMPS;

  p_xs_context = hv_fetch(PDT_contexts, (char *)self, sizeof(ParseDelimitedText *), 0);
  if(p_xs_context == NULL) { 
    croak("Text::PDTXS::field_callback_shim() could not locate context\n");
  }

  xs = (ParseDelimitedTextXS *)SvUV(*p_xs_context);
  if(xs == NULL) {
    croak("Text::PDTXS::field_callback_shim() could not locate context\n");
  }

  PUSHMARK(SP);
  if(xs->field_callback != NULL) {
    Newx(xs_cb, sizeof(ParseDelimitedTextCB), ParseDelimitedTextCB);
    xs_cb->self = xs->self;
    parser_sv = sv_newmortal();
    sv_setref_pv(parser_sv, "Text::PDTCB", (void *)xs_cb);

    field_sv = newSVpv(field->string, field->length);

    XPUSHs(parser_sv);
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

  SV **                   p_xs_context;
  ParseDelimitedTextXS *  xs;
  ParseDelimitedTextCB *  xs_cb;
  SV *                    parser_sv;
  SV *                    eol_sv;

  ENTER;
  SAVETMPS;

  p_xs_context = hv_fetch(PDT_contexts, (char *)self, sizeof(ParseDelimitedText *), 0);
  if(p_xs_context == NULL) { 
    croak("Text::PDTXS::record_callback_shim() could not locate context\n");
  }

  xs = (ParseDelimitedTextXS *)SvUV(*p_xs_context);

  PUSHMARK(SP);
  if(xs->record_callback != NULL) {
    Newx(xs_cb, sizeof(ParseDelimitedTextCB), ParseDelimitedTextCB);
    xs_cb->self = xs->self;
    parser_sv = sv_newmortal();
    sv_setref_pv(parser_sv, "Text::PDTCB", (void *)xs_cb);

    eol_sv = newSVpv(&eol, 1);

    XPUSHs(parser_sv);
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
set_xs_block_size(pTHX_ SV * sv, MAGIC * mg)
{
  if(! SvOK(sv)) {
    croak("only defined values can be assigned to block_size");
  }

  ((ParseDelimitedTextXS *)SvUV(LvTARG(sv)))->self->block_size = SvIV(sv);

  return TRUE;
}

static MGVTBL xs_block_size_vtbl = {
  0, set_xs_block_size,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_xs_delimiter(pTHX_ SV * sv, MAGIC * mg)
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

static MGVTBL xs_delimiter_vtbl = {
  0, set_xs_delimiter,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_xs_quote(pTHX_ SV * sv, MAGIC * mg)
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

static MGVTBL xs_quote_vtbl = {
  0, set_xs_quote,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_xs_field_callback(pTHX_ SV * sv, MAGIC * mg)
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

static MGVTBL xs_field_callback_vtbl = {
  0, set_xs_field_callback,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};


static int
set_xs_record_callback(pTHX_ SV * sv, MAGIC * mg)
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

static MGVTBL xs_record_callback_vtbl = {
  0, set_xs_record_callback,
  0, 0, 0,
#if defined(PERL_REVISION) && PERL_VERSION >= 8
  0, 0,
#endif
};



MODULE = Text::PDT  PACKAGE = Text::PDT

PROTOTYPES: ENABLE

BOOT:
{
  HV *stash;

  stash = gv_stashpvn("Text::PDT", 9, TRUE);

  newCONSTSUB(stash, "PDT_STRICT", newSViv(PDT_STRICT));
  newCONSTSUB(stash, "PDT_REPALL_NL", newSViv(PDT_REPALL_NL));
  newCONSTSUB(stash, "PDT_STRICT_FINI", newSViv(PDT_STRICT_FINI));
  newCONSTSUB(stash, "PDT_SUCCESS", newSViv(PDT_SUCCESS));
  newCONSTSUB(stash, "PDT_EPARSE", newSViv(PDT_EPARSE));
  newCONSTSUB(stash, "PDT_ENOMEM", newSViv(PDT_ENOMEM));
  newCONSTSUB(stash, "PDT_ETOOBIG", newSViv(PDT_ETOOBIG));
  newCONSTSUB(stash, "PDT_EINVALID", newSViv(PDT_EINVALID));
}


ParseDelimitedTextXS *
new(options)
    unsigned char options
  PREINIT:
    ParseDelimitedTextXS *  xs;
    SV *                    xs_context;
  PPCODE:
    if(PDT_contexts == NULL) {
      PDT_contexts = newHV();
    }

    Newx(xs, sizeof(ParseDelimitedTextXS), ParseDelimitedTextXS);
    xs->self = new_ParseDelimitedText(options);
    xs->self->field_callback = field_callback_shim;
    xs->self->record_callback = record_callback_shim;
    xs->field_callback = NULL;
    xs->record_callback = NULL;

    xs_context = newSVuv((UV)xs);
    hv_store(PDT_contexts, (char *)xs->self, sizeof(ParseDelimitedText *), xs_context, 0);

    ST(0) = sv_newmortal();
    sv_setref_pv(ST(0), "Text::PDTXS", (void *)xs);
    XSRETURN(1);



MODULE = Text::PDT  PACKAGE = Text::PDTXS 

PROTOTYPES: ENABLE

BOOT:
{
  HV *  stash;
  SV ** method;
  CV *  cv;

  class_ParseDelimitedText();

  stash = gv_stashpvn("Text::PDTXS", 11, TRUE);

  method = hv_fetch(stash, "block_size", 10, 0);
  if(! method) {
    croak("Text::PDTXS lost method 'block_size'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "delimiter", 9, 0);
  if(! method) {
    croak("Text::PDTXS lost method 'delimiter'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "quote", 5, 0);
  if(! method) {
    croak("Text::PDTXS lost method 'quote'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "field_callback", 14, 0);
  if(! method) {
    croak("Text::PDTXS lost method 'field_callback'");
  }
  cv = GvCV(*method);
  CvLVALUE_on(cv);

  method = hv_fetch(stash, "record_callback", 15, 0);
  if(! method) {
    croak("Text::PDTXS lost method 'record_callback'");
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

    SvMAGIC(sv)->mg_virtual = &xs_block_size_vtbl;
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

    SvMAGIC(sv)->mg_virtual = &xs_delimiter_vtbl;
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

    SvMAGIC(sv)->mg_virtual = &xs_quote_vtbl;
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

    SvMAGIC(sv)->mg_virtual = &xs_field_callback_vtbl;
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

    SvMAGIC(sv)->mg_virtual = &xs_record_callback_vtbl;
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


ParseDelimitedTextXS *
stop(xs)
    ParseDelimitedTextXS *  xs
  CODE:
    xs->self->stop = 1;
    RETVAL = xs;
  OUTPUT:
    RETVAL


ParseDelimitedTextXS *
reset_state(xs)
    ParseDelimitedTextXS *  xs
  CODE:
    xs->self->m->reset_state(xs->self);
    RETVAL = xs;
  OUTPUT:
    RETVAL


char *
state_to_s(xs)
    ParseDelimitedTextXS *  xs
  CODE:
    RETVAL = xs->self->m->state_to_s(xs->self);
  OUTPUT:
    RETVAL


char *
status_to_s(xs)
    ParseDelimitedTextXS *  xs
  CODE:
    RETVAL = xs->self->m->status_to_s(xs->self);
  OUTPUT:
    RETVAL


char *
char_class_to_s(xs)
    ParseDelimitedTextXS *  xs
  CODE:
    RETVAL = xs->self->m->char_class_to_s(xs->self);
  OUTPUT:
    RETVAL


void
DESTROY(xs)
    ParseDelimitedTextXS * xs
  CODE:
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


MODULE = Text::PDT  PACKAGE = Text::PDTCB

PROTOTYPES: ENABLE

size_t
block_size(cb)
    ParseDelimitedTextCB *  cb 
  CODE:
    RETVAL = cb->self->block_size;
  OUTPUT:
    RETVAL


void
delimiter(cb)
    ParseDelimitedTextXS *  cb
  PREINIT:
    SV *                    sv;
  PPCODE:
    sv = newSVpv(cb->self->delimiter->string, cb->self->delimiter->length);
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


void
quote(cb)
    ParseDelimitedTextCB *  cb
  PREINIT:
    SV *                    sv;
  PPCODE:
    sv = newSVpv(cb->self->quote->string, cb->self->quote->length);
    sv_2mortal(sv);
    ST(0) = sv;
    XSRETURN(1);


ParseDelimitedTextCB *
stop(cb)
    ParseDelimitedTextCB *  cb 
  CODE:
    cb->self->stop = 1;
    RETVAL = cb;
  OUTPUT:
    RETVAL


char *
state_to_s(cb)
    ParseDelimitedTextCB *  cb
  CODE:
    RETVAL = cb->self->m->state_to_s(cb->self);
  OUTPUT:
    RETVAL


char *
status_to_s(cb)
    ParseDelimitedTextCB *  cb
  CODE:
    RETVAL = cb->self->m->status_to_s(cb->self);
  OUTPUT:
    RETVAL


char *
char_class_to_s(cb)
    ParseDelimitedTextCB *  cb
  CODE:
    RETVAL = cb->self->m->char_class_to_s(cb->self);
  OUTPUT:
    RETVAL


void
DESTROY(cb)
    ParseDelimitedTextCB *  cb
  CODE:
    Safefree(cb);
