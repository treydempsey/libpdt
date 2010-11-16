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

#ifndef _CLASS_PARSEDELIMITEDTEXT_H_
#define _CLASS_PARSEDELIMITEDTEXT_H_

/* Dependencies */
#include <class_String.h>
#include <class_PDTFormat.h>


/* Version */
#define CLASS_PARSEDELIMITEDTEXT_MAJOR   4
#define CLASS_PARSEDELIMITEDTEXT_MINOR   0
#define CLASS_PARSEDELIMITEDTEXT_RELEASE 0


/* ParseDelimitedText Instance */
typedef struct ParseDelimitedText {
  struct ParseDelimitedText **        handle;

  /* Properties */
  size_t                              block_size;
  String *                            delimiter;
  String *                            quote;
  size_t                              record_count;

  /* Callbacks */
  void                                (*field_callback)(struct ParseDelimitedText * parser, String * field);
  void                                (*record_callback)(struct ParseDelimitedText * parser, char eol);
  
  /* Instants Variables */
  String *                            input;
  String *                            field;
  String *                            lookahead;
  String *                            trailing_space;
  int                                 options;
  int                                 character_class;
  int                                 compound_delimiter;
  int                                 state;
  int                                 status;
  int                                 stop;

  struct ParseDelimitedTextMethods *  m;
} ParseDelimitedText;

/* ParseDelimitedText Instance Methods */
typedef struct ParseDelimitedTextMethods {
  ParseDelimitedText *                (*init)(ParseDelimitedText * self, unsigned char options);
  ParseDelimitedText *                (*free)(ParseDelimitedText * self);

  /* Properties */
  ParseDelimitedText *                (*set_block_size)(ParseDelimitedText * self, size_t new_block_size);
  ParseDelimitedText *                (*set_delimiter)(ParseDelimitedText * self, char * new_delimiter, size_t new_delimiter_length);
  ParseDelimitedText *                (*set_quote)(ParseDelimitedText * self, char * new_quote, size_t new_quote_length);
  ParseDelimitedText *                (*set_field_callback)(ParseDelimitedText * self, void (*new_field_callback)(struct ParseDelimitedText * parser, String * field));
  ParseDelimitedText *                (*set_record_callback)(ParseDelimitedText * self, void (*new_record_callback)(struct ParseDelimitedText * parser, char eol));

  /* Instance Methods */
  size_t                              (*parse)(ParseDelimitedText * self, char * input, size_t input_length);
  size_t                              (*finish)(ParseDelimitedText * self);
  ParseDelimitedText *                (*apply_format)(ParseDelimitedText * self, PDTFormat * format);
  ParseDelimitedText *                (*reset_state)(ParseDelimitedText * self);
  ParseDelimitedText *                (*fire_field_callback)(ParseDelimitedText * self);
  ParseDelimitedText *                (*fire_record_callback)(ParseDelimitedText * self);
  ParseDelimitedText *                (*stop)(ParseDelimitedText * self);
  char *                              (*state_to_s)(ParseDelimitedText * self);
  char *                              (*status_to_s)(ParseDelimitedText * self);
  char *                              (*char_class_to_s)(ParseDelimitedText * self);
} ParseDelimitedTextMethods;


#ifndef _CLASS_PARSEDELIMITEDTEXT_I_H_
/* Class Methods */
extern int                            class_ParseDelimitedText(void);
extern ParseDelimitedText *           new_ParseDelimitedText(unsigned char options);
extern ParseDelimitedText *           alloc_ParseDelimitedText(void);

/* Global Variables */
extern ParseDelimitedText *           null_ParseDelimitedText;
#endif /* not _CLASS_PARSEDELIMITEDTEXT_I_H_ */


/* Parser Options */
#define PDT_STRICT                    1 /* Enable strict mode */
#define PDT_REPALL_NL                 2 /* Report all unquoted carriage returns and linefeeds */
#define PDT_STRICT_FINI               4 /* Causes csv_fini to return CSV_EPARSE if last
                                           field is quoted and doesn't containg ending quote */


/* Parse States */
#define ST_RECORD_NOT_BEGUN           0 /* There have not been any fields encountered for this row */
#define ST_FIELD_NOT_BEGUN            1 /* There have been fields but we are currently not in one */
#define ST_FIELD                      2 /* We are in a field */
#define ST_QUOTED_FIELD               3 /* We are in a field */
#define ST_LEADING_WHITE_SPACE        4 /* Unquoted leading white space */
#define ST_POSSIBLE_QUOTED_END        5 /* We encountered a double quote inside a quoted field, the field is either ended or the quote is literal */
#define ST_POSSIBLE_FS_END            6 /* We encountered a part of the delimiter and are not inside a quoted string */


/* Character Values */
#define CHAR_TAB                      0x09
#define CHAR_VTAB                     0x0B
#define CHAR_SPACE                    0x20
#define CHAR_CR                       0x0d
#define CHAR_LF                       0x0a
#define CHAR_COMMA                    0x2C
#define CHAR_QUOTE                    0x22
#define CSTR_TAB                      "\x09"
#define CSTR_VTAB                     "\x0B"
#define CSTR_SPACE                    "\x20"
#define CSTR_CR                       "\x0d"
#define CSTR_LF                       "\x0a"
#define CSTR_COMMA                    "\x2C"
#define CSTR_QUOTE                    "\x22"

/* Classes of characters */
#define CL_CHAR                       0
#define CL_FS                         1
#define CL_PARTIAL_FS                 2
#define CL_QUOTE                      3
#define CL_EOL                        4
#define CL_WHITE_SPACE                5

/* Error Codes */
#define PDT_SUCCESS                   0
#define PDT_EPARSE                    1 /* Parse error in strict mode */
#define PDT_ENOMEM                    2 /* Out of memory while increasing buffer size */
#define PDT_ETOOBIG                   3 /* Buffer larger than SIZE_MAX needed */
#define PDT_EINVALID                  4 /* Invalid code,should never be received from csv_error*/

#endif /* not _CLASS_PARSEDELIMITEDTEXT_H_ */
