/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libpdt is distributed under the terms of the GNU
 *  Lesser General Public License.
 * 
 *  This file is part of libpdt.
 *
 *  libpdt is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  libpdt is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with libpdt.  If not, see
 *  <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#ifndef _CLASS_PARSEDELIMITEDTEXT_I_H_
#define _CLASS_PARSEDELIMITEDTEXT_I_H_

#include "class_ParseDelimitedText.h"

int                         class_ParseDelimitedText(void);
ParseDelimitedText *        new_ParseDelimitedText(unsigned char options);
ParseDelimitedText *        alloc_ParseDelimitedText(void);

static ParseDelimitedText * ParseDelimitedText_init(ParseDelimitedText *self, unsigned char options);
static ParseDelimitedText * ParseDelimitedText_free(ParseDelimitedText *self);

static size_t               ParseDelimitedText_block_size(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_set_block_size(ParseDelimitedText *self, size_t new_block_size);
static String *             ParseDelimitedText_delimiter(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_set_delimiter(ParseDelimitedText *self, char *new_delimiter);
static String *             ParseDelimitedText_quote(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_set_quote(ParseDelimitedText *self, char *new_quote);

static size_t               ParseDelimitedText_parse(ParseDelimitedText *self, char *input, size_t input_length);
static size_t               ParseDelimitedText_finish(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_reset_state(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_fire_field_callback(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_fire_record_callback(ParseDelimitedText *self);
static ParseDelimitedText * ParseDelimitedText_stop(ParseDelimitedText *self);
static char *               ParseDelimitedText_state_to_s(ParseDelimitedText *self);
static char *               ParseDelimitedText_status_to_s(ParseDelimitedText *self);
static char *               ParseDelimitedText_char_class_to_s(ParseDelimitedText *self);

static ParseDelimitedText * _ParseDelimitedText_identify_character(ParseDelimitedText *self);
static int                  debug(const char *format, ...);

/* Global Variables */
ParseDelimitedText        *null_ParseDelimitedText = NULL;

static ParseDelimitedText _null_ParseDelimitedText;
static ParseDelimitedTextMethods ParseDelimitedText_methods;

static char               *ParseDelimitedText_states[] = {
  "record not begun",
  "field not begun",
  "field",
  "quoted field",
  "leading white space",
  "possible quoted end",
  "possible delimiter end"
};

static char               *ParseDelimitedText_statuses[] = {
  "success",
  "error parsing data while strict checking enabled",
  "memory exhausted while increasing buffer size",
  "data size too large",
  "invalid status code"
};

static char               *ParseDelimitedText_character_classes[] = {
  "char",
  "delim",
  "partial delim",
  "quote",
  "eol",
  "white space",
};

#endif /* not _CLASS_PARSEDELIMITEDTEXT_I_H_ */
