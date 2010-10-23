/*
libcsv - parse and write csv data
Copyright (C) 2008  Robert Gamble

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

    ParseDelimitedText_methods.blk_size = ParseDelimitedText_blk_size;
    ParseDelimitedText_methods.set_blk_size = ParseDelimitedText_set_blk_size;
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

    /* Null String Instance */
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
  self->state               = ST_RECORD_NOT_BEGUN;
  self->input               = new_String("", 4096, 0);
  self->field               = new_String("", 4096, 0);
  self->status              = DELIM_SUCCESS;
  self->options             = options;
  self->blk_size            = 4096;
  self->compound_delimiter  = 0;

  self->lookahead           = new_String("", 10, 0);
  self->trailing_space      = new_String("", 32, 0);
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
    self->delimiter->m->free(self->delimiter);

    free(self->handle);
    free(self);
  }

  return null_ParseDelimitedText;
}


/*
 * Properties
 ************/

static size_t
ParseDelimitedText_blk_size(ParseDelimitedText *self)
{
  return self->blk_size;
}

static ParseDelimitedText *
ParseDelimitedText_set_blk_size(ParseDelimitedText *self, size_t blk_size)
{
  if(self != null_ParseDelimitedText) {
    self->blk_size = blk_size;
  }

  return self;
}


static String *
ParseDelimitedText_delimiter(ParseDelimitedText *self)
{
  return self->delimiter;
}

static ParseDelimitedText *
ParseDelimitedText_set_delimiter(ParseDelimitedText *self, char *delimiter)
{
  if(self != null_ParseDelimitedText) {
    if(self->delimiter != null_String) {
      self->delimiter = self->delimiter->m->free(self->delimiter);
    }

    self->delimiter = new_String(delimiter, strlen(delimiter) + 1, strlen(delimiter));
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
ParseDelimitedText_set_quote(ParseDelimitedText *self, char *quote)
{
  if(self != null_ParseDelimitedText) {
    if(self->quote != null_String) {
      self->quote = self->quote->m->free(self->quote);
    }

    self->quote = new_String(quote, strlen(quote) + 1, strlen(quote));
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
      fflush(stdout);
      printf("I: >>%s<<\n", self->input->string);
      printf("S: %s, C: %s, P: %zu, L: >>%s<<, F: >>%s<<\n", self->m->state_to_s(self), self->m->char_class_to_s(self), self->input->position, self->lookahead->string, self->field->string);
      printf("-----\n");
      fflush(stdout);

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
              self->m->fire_field_callback(self);
              self->m->fire_record_callback(self);
            } 
            /* ST_RECORD_NOT_BEGUN ignore empty rows by default */
            else if(self->options & DELIM_REPALL_NL) {
              self->field->m->append_slice(self->field, self->lookahead, 1);
              self->m->fire_field_callback(self);
              self->m->fire_record_callback(self);
            }
            continue;
          }
          /* Field Separator */
          else if(self->character_class == CL_FS) {
            self->m->fire_field_callback(self);
            continue;
          }
          /* Quote */
          else if(self->character_class == CL_QUOTE) {
            self->state = ST_QUOTED_FIELD;
          }
          /* Anything else */
          else {
            self->state = ST_FIELD;
            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break;

        case ST_FIELD:
          /* Quote */
          if(self->character_class == CL_QUOTE) {
            /* STRICT ERROR - double quote inside non-quoted field */
            if(self->options & DELIM_STRICT) {
              self->status = DELIM_EPARSE;
              return self->input->position - 1;
            }

            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
          /* Field Separator */
          else if(self->character_class == CL_FS) {
            self->m->fire_field_callback(self);
          }
          /* Carriage Return or Line Feed */
          else if(self->character_class == CL_EOL) {
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
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

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
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break;

        /* This only happens when a quote character is encountered in a quoted field */
        case ST_POSSIBLE_QUOTED_END:
          /* Field Separator */
          if(self->character_class == CL_FS) {
            self->m->fire_field_callback(self);
          }
          /* Carriage Return or Line Feed */
          else if(self->character_class == CL_EOL) {
            self->m->fire_field_callback(self);
            self->m->fire_record_callback(self);
          }
          /* Quote - Two quotes in a row */
          else if(self->character_class == CL_QUOTE) {
            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->state = ST_QUOTED_FIELD;
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
          /* Anything else */
          else if(self->character_class == CL_WHITE_SPACE
             || self->character_class == CL_CHAR) {
            if(self->options & DELIM_STRICT) {
              /* STRICT ERROR - unescaped double quote */
              self->status = DELIM_EPARSE;
              return self->input->position - 1;
            }

            /* Add the previous character which is a quote because we're in ST_POSSIBLE_QUOTED_END */
            self->field->m->append_slice(self->field, self->quote, 1);

            /* Add any trailing white space after we've determined there are chars following it. */
            if(self->trailing_space->length != 0) {
              self->field->m->append(self->field, self->trailing_space);
              self->trailing_space->m->truncate(self->trailing_space);
            }

            self->state = ST_QUOTED_FIELD;
            self->field->m->append_slice(self->field, self->lookahead, 1);
          }
        break; 
      } /* switch */
    } /* while */

    return (self->status == DELIM_SUCCESS) ? self->input->position : -1;
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

    if(self->status != DELIM_SUCCESS) {
      return -1;
    }

    /* Current field is quoted, no end-quote was seen, and DELIM_STRICT_FINI is set */
    if(self->state == ST_QUOTED_FIELD
       && (self->options & DELIM_STRICT)
       && (self->options & DELIM_STRICT_FINI)) {
      self->status = DELIM_EPARSE;
      return -1;
    }
    /* Handle record not ending in a new line */
    else if(self->state == ST_QUOTED_FIELD
       || self->state == ST_POSSIBLE_QUOTED_END
       || self->state == ST_FIELD_NOT_BEGUN
       || self->state == ST_FIELD) {
      self->lookahead->m->truncate(self->lookahead);
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
    printf("!! fire_field_callback(~%s~, ~%zu~)\n", self->field->string, self->field->length);

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
    printf("!! Fire record_callback(~%02x~)\n", *(self->lookahead->string));

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
  if(self->status < 0 || self->status >= DELIM_EINVALID) {
    return ParseDelimitedText_statuses[DELIM_EINVALID];
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
  printf("delimiter_amt: %zu, delimiter_position: %zu\n", delimiter_amt, self->delimiter->position);
  printf("comparen: %i %c <=> %c\n", self->lookahead->m->comparen(self->lookahead, self->delimiter, delimiter_amt), *(self->lookahead->string), *(self->delimiter->string + self->delimiter->position));

  /* Simple FS */
  if(self->compound_delimiter == 0
     && *(self->lookahead->string) == *(self->delimiter->string)) {
    self->character_class = CL_FS;
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
      self->field->m->append_slice(self->field, self->delimiter, delimiter_matched);

      printf("comparen: %i %c <=> %c\n", self->lookahead->m->comparen(self->lookahead, self->delimiter, delimiter_amt), *(self->lookahead->string), *(self->delimiter->string + self->delimiter->position));
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
