#ifndef _CLASS_PARSEDELIMITEDTEXT_H_
#define _CLASS_PARSEDELIMITEDTEXT_H_

#include <class_String.h>

/* Version */
#define CLASS_PARSEDELIMITEDTEXT_MAJOR   4
#define CLASS_PARSEDELIMITEDTEXT_MINOR   0
#define CLASS_PARSEDELIMITEDTEXT_RELEASE 0


/* ParseDelimitedText Instance */
typedef struct ParseDelimitedText {
  struct ParseDelimitedText **handle;

  int    options;

  String *input;
  String *field;
  String *lookahead;

  int    character_class;
  int    compound_delimiter;
  int    state;
  int    status;
  int    stop;

  size_t blk_size;
  size_t record_count;

  String *delimiter;
  String *quote;

  String *trailing_space;

  void (*field_callback)(struct ParseDelimitedText *parser, String *field);
  void (*record_callback)(struct ParseDelimitedText *parser, char eol);
  
  struct ParseDelimitedTextMethods *m;
} ParseDelimitedText;

/* ParseDelimitedText Instance Methods */
typedef struct ParseDelimitedTextMethods {
  ParseDelimitedText * (*init)(ParseDelimitedText *self, unsigned char options);
  ParseDelimitedText * (*free)(ParseDelimitedText *self);

  /* Properties */
  size_t               (*blk_size)(ParseDelimitedText *self);
  ParseDelimitedText * (*set_blk_size)(ParseDelimitedText *self, size_t blk_size);
  String*              (*delimiter)(ParseDelimitedText *self);
  ParseDelimitedText * (*set_delimiter)(ParseDelimitedText *self, char *delimiter);
  String *             (*quote)(ParseDelimitedText *self);
  ParseDelimitedText * (*set_quote)(ParseDelimitedText *self, char *quote);

  /* Instance Methods */
  size_t               (*parse)(ParseDelimitedText *self, char *input, size_t input_length);
  size_t               (*finish)(ParseDelimitedText *self);
  ParseDelimitedText * (*reset_state)(ParseDelimitedText *self);
  ParseDelimitedText * (*fire_field_callback)(ParseDelimitedText *self);
  ParseDelimitedText * (*fire_record_callback)(ParseDelimitedText *self);
  ParseDelimitedText * (*stop)(ParseDelimitedText *self);
  char *               (*state_to_s)(ParseDelimitedText *self);
  char *               (*status_to_s)(ParseDelimitedText *self);
  char *               (*char_class_to_s)(ParseDelimitedText *self);
} ParseDelimitedTextMethods;


#ifndef _CLASS_PARSEDELIMITEDTEXT_I_H_
/* Class Methods */
extern int                      class_ParseDelimitedText(void);
extern ParseDelimitedText *     new_ParseDelimitedText(unsigned char options);
extern ParseDelimitedText *     alloc_ParseDelimitedText(void);

/* Global Variables */
extern ParseDelimitedText       *null_ParseDelimitedText;
#endif /* not _CLASS_PARSEDELIMITEDTEXT_I_H_ */


/* Parse States */
#define ST_RECORD_NOT_BEGUN     0 /* There have not been any fields encountered for this row */
#define ST_FIELD_NOT_BEGUN      1 /* There have been fields but we are currently not in one */
#define ST_FIELD                2 /* We are in a field */
#define ST_QUOTED_FIELD         3 /* We are in a field */
#define ST_LEADING_WHITE_SPACE  4 /* Unquoted leading white space */
#define ST_POSSIBLE_QUOTED_END  5 /* We encountered a double quote inside a quoted field, the field is either ended or the quote is literal */
#define ST_POSSIBLE_FS_END      6 /* We encountered a part of the delimiter and are not inside a quoted string */


/* Character Values */
#define CHAR_TAB                0x09
#define CHAR_VTAB               0x0B
#define CHAR_SPACE              0x20
#define CHAR_CR                 0x0d
#define CHAR_LF                 0x0a
#define CHAR_COMMA              0x2C
#define CHAR_QUOTE              0x22
#define CSTR_TAB                "\x09"
#define CSTR_VTAB               "\x0B"
#define CSTR_SPACE              "\x20"
#define CSTR_CR                 "\x0d"
#define CSTR_LF                 "\x0a"
#define CSTR_COMMA              "\x2C"
#define CSTR_QUOTE              "\x22"

/* Classes of characters */
#define CL_CHAR                 0
#define CL_FS                   1
#define CL_PARTIAL_FS           2
#define CL_QUOTE                3
#define CL_EOL                  4
#define CL_WHITE_SPACE          5

/* Error Codes */
#define DELIM_SUCCESS           0
#define DELIM_EPARSE            1 /* Parse error in strict mode */
#define DELIM_ENOMEM            2 /* Out of memory while increasing buffer size */
#define DELIM_ETOOBIG           3 /* Buffer larger than SIZE_MAX needed */
#define DELIM_EINVALID          4 /* Invalid code,should never be received from csv_error*/


/* Parser Options */
#define DELIM_STRICT            1 /* Enable strict mode */
#define DELIM_REPALL_NL         2 /* Report all unquoted carriage returns and linefeeds */
#define DELIM_STRICT_FINI       4 /* Causes csv_fini to return CSV_EPARSE if last
                                       field is quoted and doesn't containg ending quote */

#endif /* not _CLASS_PARSEDELIMITEDTEXT_H_ */
