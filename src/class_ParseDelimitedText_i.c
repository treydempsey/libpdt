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

#include "dependencies.h"
#include "class_ParseDelimitedText_i.h"

/*
 * Class Methods
 ***************/

int
class_ParseDelimitedText(void)
{
  if(null_ParseDelimitedText == NULL) {
    /* Dependencies */
    class_String();

    /* Methods */
    ParseDelimitedText_methods.init = ParseDelimitedText_init;
    ParseDelimitedText_methods.free = ParseDelimitedText_free;

    ParseDelimitedText_methods.block_size = ParseDelimitedText_block_size;
    ParseDelimitedText_methods.set_block_size = ParseDelimitedText_set_block_size;
    ParseDelimitedText_methods.delimiter = ParseDelimitedText_delimiter;
    ParseDelimitedText_methods.set_delimiter = ParseDelimitedText_set_delimiter;
    ParseDelimitedText_methods.quote = ParseDelimitedText_quote;
    ParseDelimitedText_methods.set_quote = ParseDelimitedText_set_quote;

    ParseDelimitedText_methods.parse = ParseDelimitedText_parse;
    ParseDelimitedText_methods.finish = ParseDelimitedText_finish;
    ParseDelimitedText_methods.reset_state = ParseDelimitedText_reset_state;
    ParseDelimitedText_methods.fire_field_callback = ParseDelimitedText_fire_field_callback;
    ParseDelimitedText_methods.fire_record_callback = ParseDelimitedText_fire_record_callback;
    ParseDelimitedText_methods.stop = ParseDelimitedText_stop;
    ParseDelimitedText_methods.state_to_s = ParseDelimitedText_state_to_s;
    ParseDelimitedText_methods.status_to_s = ParseDelimitedText_status_to_s;
    ParseDelimitedText_methods.char_class_to_s = ParseDelimitedText_char_class_to_s;

    /* Null Instance */
    null_ParseDelimitedText = &_null_ParseDelimitedText;
    null_ParseDelimitedText->handle = &null_ParseDelimitedText;
    null_ParseDelimitedText->m = &ParseDelimitedText_methods;
    ParseDelimitedText_init(null_ParseDelimitedText, 0);
  }

  return 1;
}


ParseDelimitedText *
new_ParseDelimitedText(unsigned char options)
{
  ParseDelimitedText *self;

  self = alloc_ParseDelimitedText();
  self->m->init(self, options);

  return self;
}


ParseDelimitedText *
alloc_ParseDelimitedText(void)
{
  ParseDelimitedText **handle;

  /* Allocate */
  handle = malloc(sizeof(ParseDelimitedText *));
  if(handle == NULL) {
    goto error;
  }

  *handle = malloc(sizeof(ParseDelimitedText));
  if(*handle == NULL) {
    free(handle);
    goto error;
  }
  memset(*handle, 0, sizeof(ParseDelimitedText));
  (*handle)->handle = handle;
  (*handle)->m = &ParseDelimitedText_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_ParseDelimitedText()");
  }

  return *handle;
}


static ParseDelimitedText *
ParseDelimitedText_init(ParseDelimitedText *self, unsigned char options)
{
  if(self != null_ParseDelimitedText) {
    self->input               = new_String("", 4096, 0);
    self->field               = new_String("", 4096, 0);
    self->lookahead           = new_String("", 10, 0);
    self->trailing_space      = new_String("", 32, 0);
  }
  else {
    self->input               = null_String;
    self->field               = null_String;
    self->lookahead           = null_String;
    self->trailing_space      = null_String;
  }

  self->state               = ST_RECORD_NOT_BEGUN;
  self->status              = PDT_SUCCESS;
  self->options             = options;
  self->block_size          = 4096;
  self->compound_delimiter  = 0;

  self->delimiter           = null_String;
  self->quote               = null_String;

  return self;
}


static ParseDelimitedText *
ParseDelimitedText_free(ParseDelimitedText *self)
{
  if(self != null_ParseDelimitedText) {
    self->input->m->free(self->input);
    self->field->m->free(self->field);
    self->lookahead->m->free(self->lookahead);
    self->trailing_space->m->free(self->trailing_space);
    self->delimiter->m->free(self->delimiter);
    self->quote->m->free(self->quote);

    free(self->handle);
    free(self);
  }

  return null_ParseDelimitedText;
}


/*
 * Properties
 ************/

static size_t
ParseDelimitedText_block_size(ParseDelimitedText *self)
{
  return self->block_size;
}

static ParseDelimitedText *
ParseDelimitedText_set_block_size(ParseDelimitedText *self, size_t new_block_size)
{
  if(self != null_ParseDelimitedText) {
    self->block_size = new_block_size;
  }

  return self;
}


static String *
ParseDelimitedText_delimiter(ParseDelimitedText *self)
{
  return self->delimiter;
}

static ParseDelimitedText *
ParseDelimitedText_set_delimiter(ParseDelimitedText *self, char *new_delimiter, size_t new_delimiter_length)
{
  if(self != null_ParseDelimitedText) {
    if(self->delimiter != null_String) {
      self->delimiter = self->delimiter->m->free(self->delimiter);
    }

    self->delimiter = new_String(new_delimiter, new_delimiter_length + 1, new_delimiter_length);
    self->compound_delimiter = (self->delimiter->length > 1) ? 1 : 0;
  }

  return self;
}


static String *
ParseDelimitedText_quote(ParseDelimitedText *self)
{
  return self->quote;
}

static ParseDelimitedText *
ParseDelimitedText_set_quote(ParseDelimitedText *self, char *new_quote, size_t new_quote_length)
{
  if(self != null_ParseDelimitedText) {
    if(self->quote != null_String) {
      self->quote = self->quote->m->free(self->quote);
    }

    self->quote = new_String(new_quote, new_quote_length + 1, new_quote_length);
  }

  return self;
}


/*
 * Public Instance Methods
 *************************/

static size_t
ParseDelimitedText_parse(ParseDelimitedText *self, char *input, size_t input_length)
{
  if(self != null_ParseDelimitedText) {
    if(self->stop != 0) {
      /* Clear stopped state, but don't reset */
      self->stop = 0;
    }
    else {
      /* New input chunk, reset */
      if(self->input->size < input_length) {
        self->input->m->extend(self->input, input_length - self->input->size);
      }

      self->input->m->truncate(self->input);
      self->input->m->append_cstr(self->input, input, input_length);
    }

    while(self->input->position < self->input->length) {
      if(self->stop != 0) {
        break;
      }

      self->lookahead->m->truncate(self->lookahead);
      self->lookahead->m->slice(self->lookahead, self->input, 9);
      self->input->position++;

      _ParseDelimitedText_identify_character(self);

      /* DEBUG */
      debug("I: >>%s<<\n", self->input->string);
      debug("S: %s, C: %s, P: %zu, L: >>%s<<, F: >>%s<<\n", self->m->state_to_s(self), self->m->char_class_to_s(self), self->input->position, self->lookahead->string, self->field->string);
      debug("-----\n");

      switch(self->state) {
        case ST_RECORD_NOT_BEGUN:
        case ST_FIELD_NOT_BEGUN:
          /* Unquoted Leading White space */
          if(self->character_class == CL_WHITE_SPACE) {
            continue;
          }

          /* Carriage Return or Line Feed */
          if(self->character_class == CL_EOL) {
            if(self->state == ST_FIELD_NOT_BEGUN) {
              debug("parse() 1: fire_field_callback()\n");
              self->m->fire_field_callback(self);
              self->m->fire_record_callback(self);
            } 
            /* ST_RECORD_NOT_BEGUN ignore empty rows by default */
            else if(self->options & PDT_REPALL_NL) {
              debug("parse() 1: append slice self->lookahead\n");
              self->field->m->append_slice(self->field, self->lookahead, 1);
              debug("parse() 2: fire_field_callback()\n");
              self->m->fire_field_callback(self);
              self->m->fire_record_callback(self);
            }
            continue;
          }
          /* Delimiter */
          else if(self->character_class == CL_FS) {
            debug("parse() 3: fire_field_callback()\n");
            self->m->fire_field_callback(self);
            continue;
          }
          /* Quote */
          else if(self->character_class == CL_QUOTE) {
            self->state = ST_QUOTED_FIELD;
          }
          /* Anything else */
          else if(self->character_class == CL_CHAR) {
            self->state = ST_FIELD;
            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              debug("parse() 2: append slice self->trailing_space\n");
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            debug("parse() 3: append slice self->lookahead\n");
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break;

        case ST_FIELD:
          /* Quote */
          if(self->character_class == CL_QUOTE) {
            /* STRICT ERROR - double quote inside non-quoted field */
            if(self->options & PDT_STRICT) {
              self->status = PDT_EPARSE;
              return self->input->position - 1;
            }

            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              debug("parse() 4: append slice self->trailing_space\n");
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            debug("parse() 5: append slice self->lookahead\n");
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
          /* Delimiter */
          else if(self->character_class == CL_FS) {
            debug("parse() 4: fire_field_callback()\n");
            self->m->fire_field_callback(self);
          }
          /* Carriage Return or Line Feed */
          else if(self->character_class == CL_EOL) {
            debug("parse() 5: fire_field_callback()\n");
            self->m->fire_field_callback(self);
            self->m->fire_record_callback(self);
          }
          /* White space for non-quoted field */
          else if(self->character_class == CL_WHITE_SPACE) {
            self->trailing_space->m->append_slice(self->trailing_space, self->lookahead, 1);
          }
          /* Anything else */
          else if(self->character_class == CL_CHAR) {
            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              debug("parse() 6: append slice self->trailing_space\n");
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            debug("parse() 7: append slice self->lookahead\n");
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break;

        case ST_QUOTED_FIELD:
          /* Quote */
          if(self->character_class == CL_QUOTE) {
            self->state = ST_POSSIBLE_QUOTED_END;
          }
          /* Non-quote */
          else if(self->character_class == CL_FS
             || self->character_class == CL_EOL
             || self->character_class == CL_WHITE_SPACE
             || self->character_class == CL_CHAR) {
            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              debug("parse() 8: append slice self->trailing_space\n");
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            debug("parse() 9: append slice self->lookahead\n");
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break;

        /* This only happens when a quote character is encountered in a quoted field */
        case ST_POSSIBLE_QUOTED_END:
          /* Delimiter */
          if(self->character_class == CL_FS) {
            debug("parse() 6: fire_field_callback()\n");
            self->m->fire_field_callback(self);
          }
          /* Carriage Return or Line Feed */
          else if(self->character_class == CL_EOL) {
            debug("parse() 7: fire_field_callback()\n");
            self->m->fire_field_callback(self);
            self->m->fire_record_callback(self);
          }
          /* Quote - Two quotes in a row */
          else if(self->character_class == CL_QUOTE) {
            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              debug("parse() 10: append slice self->trailing_space\n");
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->state = ST_QUOTED_FIELD;
            debug("parse() 11: append slice self->lookahead\n");
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
          /* Anything else */
          else if(self->character_class == CL_WHITE_SPACE
             || self->character_class == CL_CHAR) {
            if(self->options & PDT_STRICT) {
              /* STRICT ERROR - unescaped double quote */
              self->status = PDT_EPARSE;
              return self->input->position - 1;
            }

            /* Add the previous character which is a quote because we're in ST_POSSIBLE_QUOTED_END */
            debug("parse() 12: append slice self->quote\n");
            self->field->m->append_slice(self->field, self->quote, 1);

            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              debug("parse() 13: append slice self->trailing_space\n");
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->state = ST_QUOTED_FIELD;
            debug("parse() 13: append slice self->lookahead\n");
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break; 
      } /* switch */
    } /* while */

    return (self->status == PDT_SUCCESS) ? self->input->position : -1;
  } /* null_ParseDelimitedText */
  return -1;
}


static size_t
ParseDelimitedText_finish(ParseDelimitedText *self)
{
  /* Finalize parsing.  Needed, for example, when file does not end in a newline */
  
  if(self != null_ParseDelimitedText) {
    /* Force them to reset stop */
    if(self->stop != 0) {
      return 0;
    }

    if(self->status != PDT_SUCCESS) {
      return -1;
    }

    /* Current field is quoted, no end-quote was seen, and PDT_STRICT_FINI is set */
    if(self->state == ST_QUOTED_FIELD
       && (self->options & PDT_STRICT)
       && (self->options & PDT_STRICT_FINI)) {
      self->status = PDT_EPARSE;
      return -1;
    }
    /* Handle record not ending in a new line */
    else if(self->state == ST_QUOTED_FIELD
       || self->state == ST_POSSIBLE_QUOTED_END
       || self->state == ST_FIELD_NOT_BEGUN
       || self->state == ST_FIELD) {
      self->lookahead->m->truncate(self->lookahead);
      debug("finish() 1: fire_field_callback()\n");
      self->m->fire_field_callback(self);
      self->m->fire_record_callback(self);
    }

    /* Reset parser */
    self->m->reset_state(self);
  }

  return 0;
}


static ParseDelimitedText *
ParseDelimitedText_reset_state(ParseDelimitedText *self)
{
  if(self != null_ParseDelimitedText) {
    self->state = ST_FIELD_NOT_BEGUN;
    self->field->m->truncate(self->field);
    self->trailing_space->m->truncate(self->trailing_space);
  }

  return self;
}
 

static ParseDelimitedText *
ParseDelimitedText_fire_field_callback(ParseDelimitedText *self)
{
  if(self != null_ParseDelimitedText) {
    /* DEBUG */
    if(getenv("DEBUG") != NULL) {
      debug("\t!! fire_field_callback(~");
      for(self->field->position = 0; self->field->position < self->field->length; self->field->position++) {
        debug("%02x ", *(self->field->string + self->field->position));
      }
      self->field->position = 0;
      debug("~, ~%zu~)\n", self->field->length);
      fflush(stdout);
    }

    if(self->field_callback != NULL) {
      self->field_callback(self, self->field);
    }

    self->m->reset_state(self);
  }

  return self;
}


static ParseDelimitedText *
ParseDelimitedText_fire_record_callback(ParseDelimitedText *self)
{
  if(self != null_ParseDelimitedText) {
    /* DEBUG */
    if(getenv("DEBUG") != NULL) {
      debug("\t!! fire_record_callback(~%02x~)\n", *(self->lookahead->string));
      fflush(stdout);
    }

    if(self->record_callback != NULL) {
      self->record_callback(self, *(self->lookahead->string));
    }

    self->record_count++;
    self->m->reset_state(self);
    self->state = ST_RECORD_NOT_BEGUN;
  }

  return self;
}


static ParseDelimitedText *
ParseDelimitedText_stop(ParseDelimitedText *self)
{
  if(self != null_ParseDelimitedText) {
    self->stop = 1;
  }

  return self;
}


static char *
ParseDelimitedText_state_to_s(ParseDelimitedText *self)
{
  /* Return a textual description of state  */
  if(self->state < 0 || self->state > 6) {
    return "";
  }
  else {
    return ParseDelimitedText_states[self->state];
  }
}


static char *
ParseDelimitedText_status_to_s(ParseDelimitedText *self)
{
  /* Return a textual description of status */
  if(self->status < 0 || self->status >= PDT_EINVALID) {
    return ParseDelimitedText_statuses[PDT_EINVALID];
  }
  else {
    return ParseDelimitedText_statuses[self->status];
  }
}


static char *
ParseDelimitedText_char_class_to_s(ParseDelimitedText *self)
{
  /* Return a textual description of character class */
  if(self->character_class < 0 || self->character_class > CL_WHITE_SPACE) {
    return "";
  }
  else {
    return ParseDelimitedText_character_classes[self->character_class];
  }
}


/*
 * Private Methods
 *****************/

static ParseDelimitedText *
_ParseDelimitedText_identify_character(ParseDelimitedText *self)
{
  size_t delimiter_amt;  /* amount to compare for input and delimiter */
  size_t delimiter_matched;
  delimiter_amt = (self->lookahead->length < self->delimiter->length - self->delimiter->position) ? self->lookahead->length : self->delimiter->length - self->delimiter->position;

  /* DEBUG */
  if(getenv("DEBUG") != NULL) {
    debug("delimiter_amt: %zu, delimiter_position: %zu\n", delimiter_amt, self->delimiter->position);
    debug("comparen: %i %c <=> %c\n", self->lookahead->m->comparen(self->lookahead, self->delimiter, delimiter_amt), *(self->lookahead->string), *(self->delimiter->string + self->delimiter->position));
    fflush(stdout);
  }

  /* Simple FS */
  if(self->compound_delimiter == 0
     && *(self->lookahead->string) == *(self->delimiter->string)) {
    self->character_class = CL_FS;
    return self;
  }
  /* Multichar FS */
  else if(self->compound_delimiter != 0) {
    if(self->lookahead->m->comparen(self->lookahead, self->delimiter, delimiter_amt) == 0) {
      if(delimiter_amt > 0) {
        self->input->position += delimiter_amt - 1;
      }
      /* Have we made a full or partial match? */
      if((self->delimiter->position + delimiter_amt) == self->delimiter->length) {
        self->character_class = CL_FS;
        self->delimiter->position = 0;
      }
      else {
        self->delimiter->position += delimiter_amt;
        self->character_class = CL_PARTIAL_FS;
      }

      return self;
    }
    /* Matched part of a FS but failed */
    else if(self->character_class == CL_PARTIAL_FS) {
      delimiter_matched = self->delimiter->position;
      self->delimiter->position = 0;
      debug("identify_character(): append slice self->delimiter\n");
      self->field->m->append_slice(self->field, self->delimiter, delimiter_matched);

      fflush(stdout);

      if(self->lookahead->m->comparen(self->lookahead, self->delimiter, delimiter_amt) == 0) {
        if(delimiter_amt > 0) {
          self->input->position += delimiter_amt - 1;
          if(self->input->position >= self->input->length) {
            self->input->position = self->input->length;
          }
        }
        /* Have we made a full or partial match? */
        if((self->delimiter->position + delimiter_amt) == self->delimiter->length) {
          self->character_class = CL_FS;
          self->delimiter->position = 0;
        }
        else {
          self->delimiter->position += delimiter_amt;
          self->character_class = CL_PARTIAL_FS;
        }

        return self;
      }
    }
  }

  /* White Space */
  if(*(self->lookahead->string) == CHAR_SPACE
     || *(self->lookahead->string) == CHAR_TAB
     || *(self->lookahead->string) == CHAR_VTAB) {
    self->character_class = CL_WHITE_SPACE;
  }
  /* Quote */
  else if(self->quote != null_String
     && *(self->lookahead->string) == *(self->quote->string)) {
    self->character_class = CL_QUOTE;
  }
  /* EOL */
  else if(*(self->lookahead->string) == CHAR_CR
     || *(self->lookahead->string) == CHAR_LF) {
    self->character_class = CL_EOL;
  }
  /* Regular character data */
  else {
    self->character_class = CL_CHAR;
  }

  return self;
}


static int
debug(const char *format, ...)
{
  int r = 0;
  va_list args;
  
  va_start(args, format);

  if(getenv("DEBUG") != NULL) {
    r = vprintf(format, args);
    fflush(stdout);
  }

  va_end(args);

  return r;
}


