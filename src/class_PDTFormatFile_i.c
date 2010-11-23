/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libpdt is distributed under the terms of the GNU Lesser General Public
 *  License.
 * 
 *  This file is part of libpdt.
 *
 *  libpdt is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  libpdt is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with libpdt.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

/* Sample File
 * +----------------------------------------------------------------------
 * |# Comment
 * |Key1: Value1
 * |Key2:
 * |  Multiline Value2
 * |  - sub values
 * |  - of Multiline
 * |  Multiline Value3
 * |Key3: Value4
 * +----------------------------------------------------------------------
 *
 * Maps to JSON Pseudo Structure
 * -----------------------------
 *  
 *  {
 *    'Key1': [ { 'Value1': [] } ],
 *    'Key2':
 *    [
 *      { 'Multiline Value2': ['sub values', 'of Multiline'] }
 *      { 'Multiline Value3': [] }
 *    ],
 *    'Key3': [ { 'Value4': [] } ]
 *  }
 *************************************************************************/


#include "dependencies.h"
#include "class_PDTFormatFile_i.h"

/*
 * Class Methods
 ***************/

int
class_PDTFormatFile(void)
{
  if(null_PDTFormatFile == NULL) {
    /* Dependencies */
    class_String();
    class_Array();

    /* Methods */
    PDTFormatFile_methods.init = PDTFormatFile_init;
    PDTFormatFile_methods.free = PDTFormatFile_free;

    PDTFormatFile_methods.set_format_file_path = PDTFormatFile_set_format_file_path;

    PDTFormatFile_methods.parse = PDTFormatFile_parse;

    /* Null PDTFormatFile Instance */
    null_PDTFormatFile = &_null_PDTFormatFile;
    null_PDTFormatFile->handle = &null_PDTFormatFile;
    null_PDTFormatFile->m = &PDTFormatFile_methods;
    PDTFormatFile_init(null_PDTFormatFile, "", 0);
  }

  return 1;
}


PDTFormatFile *
new_PDTFormatFile(char * format_file_path, size_t format_file_path_length)
{
  PDTFormatFile * self;

  if(null_PDTFormatFile == NULL) {
    class_PDTFormatFile();
  }

  self = alloc_PDTFormatFile();
  self->m->init(self, format_file_path, format_file_path_length);

  return self;
}


PDTFormatFile *
alloc_PDTFormatFile(void)
{
  PDTFormatFile ** handle;

  /* Allocate */
  handle = malloc(sizeof(PDTFormatFile *));
  if(handle == NULL) {
    handle = null_PDTFormatFile->handle;
    goto error;
  }

  *handle = malloc(sizeof(PDTFormatFile));
  if(*handle == NULL) {
    free(handle);
    handle = null_PDTFormatFile->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(PDTFormatFile));
  (*handle)->handle = handle;
  (*handle)->m = &PDTFormatFile_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_PDTFormatFile()");
  }

  return *handle;
}


static PDTFormatFile *
PDTFormatFile_init(PDTFormatFile * self, char * format_file_path, size_t format_file_path_length)
{
  if(self != null_PDTFormatFile) {
    /* Variables */
    self->format            = null_PDTFormat;
    self->format_file_path  = new_String(format_file_path, format_file_path_length);

    self->_chunk            = new_String("", 0);
    self->_chunk->m->extend(self->_chunk, 4095);
    self->_key              = new_String("", 0);
    self->_key->m->extend(self->_key, 1023);
    self->_value            = new_String("", 0);
    self->_value->m->extend(self->_value, 1023);
  }
  else {
    self->format            = null_PDTFormat;
    self->format_file_path  = null_String;
    self->_chunk            = null_String;
    self->_key              = null_String;
    self->_value            = null_String;
  }

  /* Private Variables */
  self->_char_class         = CL_NONE;
  self->_format_file        = -1;
  self->_state              = ST_NONE;

  return self;
}


static PDTFormatFile *
PDTFormatFile_free(PDTFormatFile * self)
{
  if(self != null_PDTFormatFile) {
    self->format_file_path->m->free(self->format_file_path);

    self->_chunk->m->free(self->_chunk);
    self->_key->m->free(self->_key);
    self->_value->m->free(self->_value);

    free(self->handle);
    free(self);
  }

  return null_PDTFormatFile;
}


/*
 * Properties
 ************/

static PDTFormatFile *
PDTFormatFile_set_format_file_path(PDTFormatFile * self, String * new_format_file_path)
{
  if(self != null_PDTFormatFile) {
    self->format_file_path->m->free(self->format_file_path);
    self->format_file_path = new_format_file_path->m->dup(new_format_file_path);
  }

  return self;
}


/*
 * Public Instance Methods
 *************************/

static PDTFormatFile *
PDTFormatFile_parse(PDTFormatFile * self)
{
  String * line;
  String * prepend;
  size_t   bytes_read;
  size_t   offset;

  if(self != null_PDTFormatFile) {
    _PDTFormatFile_open_format_file(self);

    if(self->_format_file != -1) {
      prepend = new_String("", 0);
      prepend->m->extend(prepend, 1023);

      do {
        if(prepend->length > 0) {
          self->_chunk->m->append(self->_chunk, prepend);
        }

        offset = self->_chunk->length;
        if(offset > 0) {
          offset--;
        }

        bytes_read = read(self->_format_file, self->_chunk->string + offset, self->_chunk->size - self->_chunk->length);
        self->_chunk->length += bytes_read;

        while((line = self->_chunk->m->each_line(self->_chunk)) != null_String) {
          if(line->m->eol(line) > 0 || bytes_read == 0) {
            if(bytes_read == 0 && line->m->eol(line) == 0) {
              line->position = line->length - 1;
              line->m->append_cstr(line, "\x0a", 1);
            }
            _PDTFormatFile_parse_line(self, line);
            _PDTFormatFile_set_key_value(self);
          }
          else {
            prepend->m->append(prepend, line);
          }
        }

        self->_chunk->m->truncate(self->_chunk);
      } while(bytes_read > 0);

      prepend->m->free(prepend);

      _PDTFormatFile_close_format_file(self);
    }
  }

  return self;
}


/*
 * Private Methods
 *****************/

static PDTFormatFile *
_PDTFormatFile_close_format_file(PDTFormatFile * self)
{
  if(self != null_PDTFormatFile
     && self->_format_file != -1) {
    close(self->_format_file); 
    self->_format_file = -1;
  }

  return self;
}


static PDTFormatFile *
_PDTFormatFile_identify_char(PDTFormatFile * self, char c)
{
  if(self != null_PDTFormatFile) {
    switch(c) {
      case CHAR_SPACE:
      case CHAR_TAB:
      case CHAR_VTAB:
        self->_char_class = CL_WHITESPACE;
        break;
      case CHAR_CR:
      case CHAR_LF:
        self->_char_class = CL_EOL;
        break;
      case CHAR_COMMENT:
        self->_char_class = CL_COMMENT;
        break;
      case CHAR_COLON:
        self->_char_class = CL_COLON;
        break;
      case CHAR_HYPHEN:
        self->_char_class = CL_HYPHEN;
        break;
      default:
        self->_char_class = CL_CHAR;
    }
  }

  return self;
}


static PDTFormatFile *
_PDTFormatFile_open_format_file(PDTFormatFile * self)
{
  if(self != null_PDTFormatFile
     && self->format_file_path != null_String) {
    self->_format_file = open(self->format_file_path->string, O_RDONLY);

    if(self->_format_file == -1) {
      perror("_PDTFormatFile_open_format_file()\n");
      exit(1);
    }
  }

  return self;
}


static PDTFormatFile *
_PDTFormatFile_parse_line(PDTFormatFile * self, String * line)
{
  size_t line_offset = 0;

  while(line_offset < line->length) {
    _PDTFormatFile_identify_char(self, *(line->string + line_offset));
    _PDTFormatFile_state(self);

    switch(self->_state) {
      case ST_START_KEY:
      case ST_START_VALUE:
      case ST_START_MULTILINE_VALUE0:
      case ST_START_MULTILINE_VALUE1:
        line->position = line_offset;
        break;

      case ST_END_KEY:
        self->_value->m->truncate(self->_value);
        self->_key->m->slice(self->_key, line, line_offset - line->position);
        self->_key->m->ltrim(self->_key);
        break;

      case ST_END_VALUE:
      case ST_END_MULTILINE_VALUE0:
      case ST_END_MULTILINE_VALUE1:
        self->_value->m->slice(self->_value, line, line_offset - line->position);
        self->_value->m->ltrim(self->_value);
        break;
    }

    line_offset++;
  }
  line->position = 0;

  return self;
}


static PDTFormatFile *
_PDTFormatFile_set_key_value(PDTFormatFile * self)
{
  String *    key;
  int         ival;
  PDTColumn * column;

  if(self != null_PDTFormatFile) {
    key = self->_key->m->dup(self->_key);
    key->m->upcase(key);

    if(self->_state == ST_END_VALUE) {
      if(key->m->compare_cstr(key, "ALLOW FEWER COLUMNS", 19) == 0) {
        ival = self->_value->m->to_i(self->_value);
        self->format->m->set_allow_fewer_columns(self->format, (ival == 0) ? 0 : 1);
      }

      else if(key->m->compare_cstr(key, "DELIMITER", 9) == 0) {
        self->format->delimiter = self->format->delimiter->m->free(self->format->delimiter);
        if(self->_value->m->ishex(self->_value)) {
          self->format->delimiter = new_String(" ", 1);
          *(self->format->delimiter->string) = self->_value->m->hex_to_byte(self->_value);
        }
        else {
          self->format->delimiter = self->_value->m->dup(self->_value);
        }
      }

      else if(key->m->compare_cstr(key, "ESCAPE", 6) == 0) {
        self->format->escape = self->format->escape->m->free(self->format->escape);
        if(self->_value->m->ishex(self->_value)) {
          self->format->escape = new_String(" ", 1);
          *(self->format->escape->string) = self->_value->m->hex_to_byte(self->_value);
        }
        else {
          self->format->escape = self->_value->m->dup(self->_value);
        }
      }

      else if(key->m->compare_cstr(key, "QUOTE", 5) == 0) {
        self->format->quote = self->format->quote->m->free(self->format->quote);
        if(self->_value->m->ishex(self->_value)) {
          self->format->quote = new_String(" ", 1);
          *(self->format->quote->string) = self->_value->m->hex_to_byte(self->_value);
        }
        else {
          self->format->quote = self->_value->m->dup(self->_value);
        }
      }

      else if(key->m->compare_cstr(key, "SKIP", 4) == 0) {
        ival = self->_value->m->to_i(self->_value);
        self->format->m->set_skip(self->format, (ival == 0) ? 0 : 1);
      }

      else {
        printf("ERROR: %s, unknown key.\n", self->_key->string);
      }
    }
    else if(self->_state == ST_END_MULTILINE_VALUE0) {
      if(key->m->compare_cstr(key, "COLUMN LIST", 11) == 0) {
        self->format->m->add_column(self->format, self->_value->string, self->_value->length);
      }
      else {
        printf("ERROR: %s, unknown multi-line key.\n", self->_key->string);
      }
    }
    else if(self->_state == ST_END_MULTILINE_VALUE1) {
      column = (PDTColumn *)self->format->columns->m->last(self->format->columns)->data;
      column->m->add_validation(column, self->_value->string, self->_value->length);
    }

    key = key->m->free(key);
  }

  return self;
}


static PDTFormatFile *
_PDTFormatFile_state(PDTFormatFile * self)
{
  if(self != null_PDTFormatFile) {
    switch(self->_state) {
      case ST_NONE:
      case ST_BEFORE_KEY:
        switch(self->_char_class) {
          case CL_COMMENT:
            self->_state = ST_COMMENT;
            break;
          case CL_COLON:
            self->_state = ST_COMMENT;
            fprintf(stderr, "ERROR: _PDTFormatFile_state() found empty key\n");
            break;
          case CL_CHAR:
          case CL_HYPHEN:
            self->_state = ST_START_KEY;
            break;
          case CL_EOL:
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_NONE, ST_BEFORE_KEY: */

      case ST_COMMENT:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_BEFORE_KEY;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            break;
        }
        break; /* case ST_COMMENT: */

      case ST_MULTILINE_VALUE_COMMENT0:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_BEFORE_MULTILINE_VALUE0;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            break;
        }
        break; /* case ST_MULTILINE_VALUE_COMMENT0: */

      case ST_START_KEY:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_KEY;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_WHITESPACE:
          case CL_HYPHEN:
            self->_state = ST_KEY;
            break;
        }
        break; /* case ST_START_KEY: */

      case ST_KEY:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_KEY;
            break;
          case CL_COLON:
            self->_state = ST_END_KEY;
          case CL_CHAR:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            break;
        }
        break; /* case ST_KEY: */

      case ST_END_KEY:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_BEFORE_MULTILINE_VALUE0;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
            self->_state = ST_START_VALUE;
            break;
          case CL_WHITESPACE:
            self->_state = ST_BEFORE_VALUE;
            break;
        }
        break; /* case ST_END_KEY: */

      case ST_BEFORE_VALUE:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_BEFORE_MULTILINE_VALUE0;
            break;
          case CL_COLON:
          case CL_CHAR:
          case CL_COMMENT:
          case CL_HYPHEN:
            self->_state = ST_START_VALUE;
            break;
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_BEFORE_VALUE: */

      case ST_START_VALUE:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_VALUE;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            self->_state = ST_VALUE;
            break;
        } /* _char_class */
        break; /* case ST_START_VALUE: */

      case ST_VALUE:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_VALUE;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_VALUE: */

      case ST_END_VALUE:
        switch(self->_char_class) {
          case CL_COMMENT:
            self->_state = ST_COMMENT;
            break;
          case CL_COLON:
            self->_state = ST_COMMENT;
            fprintf(stderr, "ERROR: _PDTFormatFile_state() found empty key\n");
            break;
          case CL_CHAR:
          case CL_HYPHEN:
            self->_state = ST_START_KEY;
            break;
          case CL_EOL:
          case CL_WHITESPACE:
            self->_state = ST_BEFORE_KEY;
            break;
        } /* _char_class */
        break; /* case ST_END_VALUE: */

      case ST_BEFORE_MULTILINE_VALUE0:
      case ST_END_MULTILINE_VALUE0:
      case ST_END_MULTILINE_VALUE1:
        switch(self->_char_class) {
          case CL_CHAR:
          case CL_HYPHEN:
            self->_state = ST_START_KEY;
            break;
          case CL_COLON:
            self->_state = ST_MULTILINE_VALUE_COMMENT0;
            fprintf(stderr, "ERROR: _PDTFormatFile_state() found empty key\n");
            break;
          case CL_COMMENT:
            self->_state = ST_MULTILINE_VALUE_COMMENT0;
            break;
          case CL_EOL:
            self->_state = ST_BEFORE_KEY;
            break;
          case CL_WHITESPACE:
            self->_state = ST_CONTINUE_MULTILINE_VALUE0;
            break;
        } /* _char_class */
        break; /* case ST_BEFORE_MULTILINE_VALUE0, ST_END_MULTILINE_VALUE0, ST_END_MULTILINE_VALUE1: */

      case ST_CONTINUE_MULTILINE_VALUE0:
        switch(self->_char_class) {
          case CL_CHAR:
            self->_state = ST_START_MULTILINE_VALUE0;
            break;
          case CL_COLON:
            self->_state = ST_MULTILINE_VALUE_COMMENT0;
            fprintf(stderr, "ERROR: _PDTFormatFile_state() found empty key\n");
            break;
          case CL_COMMENT:
            self->_state = ST_MULTILINE_VALUE_COMMENT0;
            break;
          case CL_EOL:
            self->_state = ST_BEFORE_KEY;
            break;
          case CL_HYPHEN:
            self->_state = ST_BEFORE_MULTILINE_VALUE1;
            break;
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_CONTINUE_MULTILINE_VALUE0: */

      case ST_START_MULTILINE_VALUE0:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_MULTILINE_VALUE0;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            self->_state = ST_MULTILINE_VALUE0;
            break;
        } /* _char_class */
        break; /* case ST_START_MULTILINE_VALUE0: */

      case ST_MULTILINE_VALUE0:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_MULTILINE_VALUE0;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_MULTILINE_VALUE0: */

      case ST_BEFORE_MULTILINE_VALUE1:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_MULTILINE_VALUE1;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
            self->_state = ST_START_MULTILINE_VALUE1;
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_BEFORE_MULTILINE_VALUE1: */

      case ST_START_MULTILINE_VALUE1:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_MULTILINE_VALUE1;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            self->_state = ST_MULTILINE_VALUE1;
            break;
        } /* _char_class */
        break; /* case ST_START_MULTILINE_VALUE1: */

      case ST_MULTILINE_VALUE1:
        switch(self->_char_class) {
          case CL_EOL:
            self->_state = ST_END_MULTILINE_VALUE1;
            break;
          case CL_CHAR:
          case CL_COLON:
          case CL_COMMENT:
          case CL_HYPHEN:
          case CL_WHITESPACE:
            break;
        } /* _char_class */
        break; /* case ST_START_MULTILINE_VALUE1: */

    }
  }

  return self;
}
