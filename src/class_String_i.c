/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libparse_delimited_text is distributed under the terms of the GNU
 *  Lesser General Public License.
 * 
 *  This file is part of libparse_delimited_text.
 *
 *  libparse_delimited_text is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  libparse_delimited_text is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with libparse_delimited_text.  If not, see
 *  <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "dependencies.h"
#include "class_String_i.h"

/*
 * Class Methods
 ***************/

int
class_String(void)
{
  if(null_String == NULL) {
    /* Dependencies */
    class_Array();

    /* Methods */
    String_methods.init         = String_init;
    String_methods.free         = String_free;

    String_methods.set_blk_size = String_set_blk_size;

    String_methods.append       = String_append;
    String_methods.append_cstr  = String_append_cstr;
    String_methods.append_slice = String_append_slice;
    String_methods.chomp        = String_chomp;
    String_methods.compare      = String_compare;
    String_methods.compare_cstr = String_compare_cstr;
    String_methods.comparen     = String_comparen;
    String_methods.dup          = String_dup;
    String_methods.downcase     = String_downcase;
    String_methods.eol          = String_eol;
    String_methods.each_line    = String_each_line;
    String_methods.extend       = String_extend;
    String_methods.hex_to_byte  = String_hex_to_byte;
    String_methods.ishex        = String_ishex;
    String_methods.ltrim        = String_ltrim;
    String_methods.rtrim        = String_rtrim;
    String_methods.slice        = String_slice;
    String_methods.split        = String_split;
    String_methods.to_i         = String_to_i;
    String_methods.truncate     = String_truncate;
    String_methods.upcase       = String_upcase;

    /* Null String Instance */
    null_String                 = &_null_String;
    null_String->handle         = &null_String;
    null_String->m              = &String_methods;
    null_String->string         = _null_String_string;
    String_init(null_String, "", 1, 0);
  }

  return 1;
}


String *
new_String(char * cstr, size_t size, size_t length)
{
  String * self;

  self = alloc_String(size);
  self->m->init(self, cstr, size, length);

  return self;
}


String *
alloc_String(size_t size)
{
  String ** handle;

  /* Allocate */
  handle = malloc(sizeof(String *));
  if(handle == NULL) {
    handle = null_String->handle;
    goto error;
  }

  *handle = malloc(sizeof(String));
  if(*handle == NULL) {
    free(handle);
    goto error;
  }
  memset(*handle, 0, sizeof(String));
  (*handle)->handle = handle;
  (*handle)->m = &String_methods;

  (*handle)->string = malloc(sizeof(char) * size);
  if(*handle == NULL) {
    free(*handle);
    free(handle);
    goto error;
  }

error:

  if(errno == ENOMEM) {
    perror("alloc_String()");
  }

  return *handle;
}


static String *
String_init(String * self, char * cstr, size_t size, size_t length)
{
  /* Variables */
  self->size = size;
  self->length = length;
  self->position = 0;
  self->blk_size = 0;

  self->_line = null_String;

  if(length > size) {
    self->m->extend(self, (length - size) + 1);
  }
  strncpy(self->string, cstr, self->length);
  *(self->string + self->length) = '\0';

  return self;
}


static String *
String_free(String *self)
{
  if(self != null_String) {
    if(self->_line != null_String) {
      self->_line = self->_line->m->free(self->_line);
    }
    free(self->string);
    free(self->handle);
    free(self);
  }

  return null_String;
}


/* Properties
 ************/

static String *
String_set_blk_size(String * self, size_t new_blk_size)
{
  if(self != null_String) {
    self->blk_size = new_blk_size;
  }

  return self;
}


/*
 * Public Instance Methods
 *************************/

static String *
String_append(String * self, String * other)
{
  size_t new_size;
  size_t append_length;
  size_t other_length;
  size_t other_position;

  if(self != null_String) {
    other_length = other->length;
    other_position = other->position;

    if(other_length > other->size) {
      other_length = other->size;
    }
    if(other_position > other_length) {
      other_position = other_length;
    }

    append_length = other_length - other_position;
    if((other_position + append_length) > other_length) {
      append_length = other_length - other_position;
    }

    new_size = self->length + append_length;
    if(new_size > self->size) {
      self->m->extend(self, (new_size - self->size) + 1);
    }

    strncpy(self->string + self->length, other->string + other_position, append_length);
    self->length = self->length + append_length;
    *(self->string + self->length) = '\0';
  }

  return self;
}


static String *
String_append_cstr(String * self, char * cstr, size_t append_length)
{
  size_t new_size;
  size_t cpy_size;

  if(self != null_String) {
    new_size = self->length + append_length;
    if(new_size > self->size) {
      self->m->extend(self, (new_size - self->size) + 1);
    }

    cpy_size = append_length;
    if(self->size - self->length > append_length) {
      cpy_size = self->size - self->length;
    }

    strncpy(self->string + self->length, cstr, cpy_size);
    self->length = self->length + append_length;
  }

  return self;
}


static String *
String_append_slice(String * self, String * other, size_t slice_length)
{
  size_t new_size;
  size_t append_length;
  size_t other_length;
  size_t other_position;

  if(self != null_String) {
    other_length = other->length;
    other_position = other->position;

    if(other_length > other->size) {
      other_length = other->size;
    }
    if(other_position > other_length) {
      other_position = other_length;
    }

    append_length = slice_length;
    if((other_position + append_length) > other_length) {
      append_length = other_length - other_position;
    }

    new_size = self->length + append_length;
    if(new_size >= self->size) {
      self->m->extend(self, (new_size - self->size) + 1);
    }

    strncpy(self->string + self->length, other->string + other_position, append_length);
    self->length = self->length + append_length;
    *(self->string + self->length) = '\0';
  }

  return self;
}


static size_t
String_chomp(String * self)
{
  size_t chomped = 0;
  size_t eol_chars = 0;

  if(self != null_String && self->length > 0) {
    eol_chars = self->m->eol(self);
    while(eol_chars > 0 && self->length > 0) {
      self->length--;
      *(self->string + self->length) = '\0';
      eol_chars--;
      chomped++;
    }
  }

  return chomped;
}


static int
String_compare(String * self, String * other)
{
  size_t compare_length;
  int r = 0;

  if(self != null_String) {
    compare_length = ((self->length - self->position) < (other->length - self->position)) ? self->length - self->position : other->length - other->position;
    r = strncmp(self->string + self->position, other->string + other->position, compare_length);

    if(r == 0) {
      if((self->length - self->position) > (other->length - other->position)) {
        r = 1;
      }
      else if((self->length - self->position) < (other->length - other->position)) {
        r = -1;
      }
    }
  }

  return r;
}


static int
String_compare_cstr(String * self, char * other, size_t other_length)
{
  size_t compare_length;
  int r = 0;

  if(self != null_String) {
    compare_length = ((self->length - self->position) < other_length) ? self->length - self->position : other_length;
    r = strncmp(self->string + self->position, other, compare_length);

    if(r == 0) {
      if((self->length - self->position) > other_length) {
        r = 1;
      }
      else if((self->length - self->position) < other_length) {
        r = -1;
      }
    }
  }

  return r;
}


static int
String_comparen(String * self, String * other, size_t compare_length)
{
  int r = 0;
  size_t safe_compare_length;

  if(self != null_String) {
    safe_compare_length = ((self->length - self->position) < (other->length - other->position)) ? self->length - self->position : other->length - other->position;
    safe_compare_length = (safe_compare_length < compare_length) ? safe_compare_length : compare_length;
    r = strncmp(self->string + self->position, other->string + other->position, safe_compare_length);
  }

  return r;
}


static String *
String_dup(String * self)
{
  return new_String(self->string, self->size, self->length);
}


static String *
String_downcase(String * self)
{
  size_t i;

  if(self != null_String) {
    for(i = 0; i < self->length; i++) {
      *(self->string + i) = tolower(*(self->string + i));
    }
  }

  return self;
}


static String *
String_each_line(String *self)
{
  size_t cur_position;

  if(self != null_String) {
    if(self->_line == null_String) {
      self->_line = new_String("", 1024, 0);
      self->_line->blk_size = 1024;
    }

    if(self->position >= self->length) {
      self->_line->m->truncate(self->_line);
      self->position = 0;
      return null_String;
    }

    cur_position = self->position;
    while(cur_position < self->length) {
      /* CR */
      if(*(self->string + cur_position) == '\x0d') {
        /* CR+LF */
        if((cur_position + 1) < self->length && *(self->string + cur_position + 1) == '\x0a') {
          cur_position++;
        }
        break;
      }
      /* LF */
      if(*(self->string + cur_position) == '\x0a') {
        break;
      }

      cur_position++;
    }

    cur_position++;
    self->_line->m->slice(self->_line, self, cur_position - self->position);
    self->position = cur_position;

    return self->_line;
  }
  else {
    return self;
  }
}


static size_t
String_eol(String * self)
{
  size_t eol_chars = 0;
  size_t offset = 0;

  if(self != null_String && self->length > 0) {
    offset = self->length;
    offset--;

    /* CR */
    if(*(self->string + offset) == '\x0d') {
      eol_chars++;

      /* CR+LF */
      if(*(self->string + offset) == '\x0d') {
        eol_chars++;
      }
    }
    /* LF */
    else if(*(self->string + offset) == '\x0a') {
      eol_chars++;
    }
  }

  return eol_chars;
}


static String *
String_extend(String * self, size_t add)
{
  size_t new_size;
  char * extended;

  if(add < 1) {
    return self;
  }

  if(self != null_String) {
    if(self->blk_size == 0) {
      new_size = self->size + add;
    }
    else {
      new_size = (self->size + add) / self->blk_size;
      new_size = (((self->size + add) % self->blk_size) == 0) ? new_size : new_size + 1;
      new_size = new_size * self->blk_size;
    }

    extended = realloc(self->string, new_size);
    if(self->string == NULL) {
      perror("String_extend()\n");
    }
    else {
      self->string = extended;
      self->size = new_size;
    }
  }

  return self;
}


static unsigned char
String_hex_to_byte(String * self)
{
  unsigned long lbyte = 0;
  unsigned char cbyte = 0;
  char **       endval = NULL;

  if(self != null_String) {
    if(self->length - self->position >= 4) {
      lbyte = strtoul(self->string + self->position, endval, 16);
      if(endval == '\0') {
        cbyte = (unsigned char)(lbyte & 0xff);
      }
      else {
        fprintf(stderr, "ERROR: Error String_hex_to_byte could not convert string to byte\n");
      }
    }
  }

  return cbyte;
}


static int
String_ishex(String * self)
{
  int     r = 0;
  char *  cstr;

  if(self != null_String) {
    cstr = self->string + self->position;
    if((self->length - self->position) >= 4
       && *cstr == '0'
       && (*(cstr + 1) == 'x' || *(cstr + 1) == 'X')
       && (   (*(cstr + 2) >= '0' && *(cstr + 2) <= '9')
           || (*(cstr + 2) >= 'a' && *(cstr + 2) <= 'f')
           || (*(cstr + 2) >= 'A' && *(cstr + 2) <= 'F')
          )
       && (   (*(cstr + 3) >= '0' && *(cstr + 3) <= '9')
           || (*(cstr + 3) >= 'a' && *(cstr + 3) <= 'f')
           || (*(cstr + 3) >= 'A' && *(cstr + 3) <= 'F')
          )) {
      r = 1;
    }
  }

  return r;
}


static String *
String_ltrim(String * self)
{
  size_t offset = 0;

  if(self != null_String && self->length > 0) {
    offset = self->length;
    offset--;

    while(offset >= 0
          && (*(self->string + offset) == '\x20'
              || *(self->string + offset) == '\x09'
              || *(self->string + offset) == '\x0b'
             )) {
      *(self->string + offset) = '\0';
      self->length--;
      offset--;
    }
  }

  return self;
}


static String *
String_rtrim(String * self)
{
  size_t ws = 0;

  if(self != null_String && self->length > 0) {

    self->position = 0;
    while(ws < self->length
          && (   *(self->string + self->position) == '\x20'
              || *(self->string + self->position) == '\x09'
              || *(self->string + self->position) == '\x0b'
             )
         ) {
      self->position++;
      ws++;
    }

    if(ws > 0) {
      self->position = 0;
      while(self->position < (self->length - ws)) {
        *(self->string + self->position) = *(self->string + self->position + ws);
        self->position++;
      }
      self->length = self->length - ws;
      *(self->string + self->length) = '\0';
    }
  }

  return self;
}


static String *
String_slice(String * self, String * other, size_t slice_length)
{
  if(self != null_String) {
    self->m->truncate(self);
    self->m->append_slice(self, other, slice_length);
  }

  return self;
}


static Array *
String_split(String * self, String * delimiter)
{
  Array *   r = null_Array;
  String *  e;
  size_t    self_position;
  size_t    start_mark;
  size_t    end_mark;
  
  if(self != null_String) {
    self->position = 0;
    delimiter->position = 0;
    start_mark = 0;
    end_mark = 0;

    while(self->position < self->length) {
      if(*(self->string + self->position) == *(delimiter->string + delimiter->position)) {

        if(delimiter->position == 0) {
          end_mark = self->position - 1;
        }

        if(delimiter->position == (delimiter->length - 1)) {
          delimiter->position = 0;
          if(r == null_Array) {
            r = new_Array();
            r->auto_free = _String_Array_free;
          }

          self_position = self->position;
          self->position = start_mark;
          e = new_String(self->string + start_mark, (self_position - start_mark) + 1, self_position - start_mark);
          r->m->append(r, e);

          delimiter->position = 0;
          self->position = self_position;
          start_mark = self->position + 1;
        }

        self->position++;
      }
      else if(delimiter->position > 0
              && *(self->string + self->position) != *(delimiter->string + delimiter->position)) {
        delimiter->position = 0;
      }
      else {
        self->position++;
      }
    }

    if(r == null_Array) {
      r = new_Array();
      r->auto_free = _String_Array_free;
    }

    self_position = self->position;
    self->position = start_mark;
    e = new_String(self->string + start_mark, (self_position - start_mark) + 1, self_position - start_mark);
    r->m->append(r, e);
  }

  return r;
}


static int
String_to_i(String * self)
{
  long    l;
  int     i = 0;
  char ** endval = NULL;

  if(self != null_String) {
    if(self->position > self->length) {
      l = strtol(self->string + self->position, endval, 10);
      if(endval == '\0') {
        i = (int)l;
      }
      else {
        fprintf(stderr, "ERROR: Error String_to_i could not convert string to integer\n");
      }
    }
  }

  return i;
}


static String *
String_truncate(String * self)
{
  if(self != null_String) {
    self->length = 0;
    self->position = 0;
    *(self->string) = '\0';
  }

  return self;
}


static String *
String_upcase(String * self)
{
  size_t i;

  if(self != null_String) {
    for(i = 0; i < self->length; i++) {
      *(self->string + i) = toupper(*(self->string + i));
    }
  }

  return self;
}


/*
 * Private Instance Methods
 **************************/

static void
_String_Array_free(void *data)
{
  String *s;

  s = (String *)data;
  s->m->free(s);
}
