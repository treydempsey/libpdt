/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libparse_delimited_text is distributed under the terms of the GNU
 *  Lesser General Public License.
 * 
 *  This file is part of libparse_delimited_test.
 *
 *  libparse_delimited_test is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  libparse_delimited_test is distributed in the hope that it will be
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
    /* Methods */
    String_methods.init = String_init;
    String_methods.free = String_free;

    String_methods.blk_size = String_blk_size;
    String_methods.set_blk_size = String_set_blk_size;

    String_methods.append = String_append;
    String_methods.append_cstr = String_append_cstr;
    String_methods.append_slice = String_append_slice;
    String_methods.chomp = String_chomp;
    String_methods.compare = String_compare;
    String_methods.comparen = String_comparen;
    String_methods.dup = String_dup;
    String_methods.eol = String_eol;
    String_methods.each_line = String_each_line;
    String_methods.extend = String_extend;
    String_methods.ltrim = String_ltrim;
    String_methods.slice = String_slice;
    String_methods.truncate = String_truncate;

    /* Null String Instance */
    null_String = &_null_String;
    null_String->handle = &null_String;
    null_String->m = &String_methods;
    String_init(null_String, NULL, 0, 0);
  }

  return 1;
}


String *
new_String(char *cstr, size_t size, size_t length)
{
  String *self;

  self = alloc_String(size);
  self->m->init(self, cstr, size, length);

  return self;
}


String *
alloc_String(size_t size)
{
  String **handle;

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
String_init(String *self, char *cstr, size_t size, size_t length)
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
  strncpy(self->string, cstr, self->size);

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
static size_t
String_blk_size(String *self)
{
  return self->blk_size;
}


static String *
String_set_blk_size(String *self, size_t new_blk_size)
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
String_append(String *self, String *other)
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
String_append_cstr(String *self, char *cstr, size_t append_length)
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
String_append_slice(String *self, String *other, size_t slice_length)
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
String_chomp(String *self)
{
  size_t chomped = 0;
  size_t eol_chars = 0;

  if(self != null_String && self->length > 0) {
    eol_chars = self->m->eol(self);
    while(eol_chars > 0 && self->length > 0) {
      self->length--;
      *(self->string + self->length) = '\x00';
      eol_chars--;
      chomped++;
    }
  }

  return chomped;
}


static int
String_compare(String *self, String *other)
{
  size_t compare_length;
  int r = 0;

  if(self != null_String) {
    compare_length = (self->length < other->length) ? self->length : other->length;
    r = self->m->comparen(self, other, compare_length);

    if(r == 0) {
      if(self->length > other->length) {
        r = 1;
      }
      else if(self->length < other->length) {
        r = -1;
      }
    }
  }

  return r;
}


static int
String_comparen(String *self, String *other, size_t compare_length)
{
  int r = 0;

  if(self != null_String) {
    r = strncmp(self->string + self->position, other->string + other->position, compare_length);
  }

  return r;
}


static String *
String_dup(String *self)
{
  return new_String(self->string, self->size, self->length);
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
String_eol(String *self)
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
String_extend(String *self, size_t add)
{
  size_t new_size;
  char *extended;

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


static String *
String_ltrim(String *self)
{
  size_t ws_chars = 0;
  size_t offset = 0;

  if(self != null_String && self->length > 0) {
    offset = self->length;
    offset--;

    while(offset >= 0
          && (*(self->string + offset) == '\x20'
              || *(self->string + offset) == '\x09'
              || *(self->string + offset) == '\x0b'
             )) {
      *(self->string + offset) = '\x00';
      self->length--;
      offset--;
    }
  }

  return self;
}


static String *
String_slice(String *self, String *other, size_t slice_length)
{
  if(self != null_String) {
    self->m->truncate(self);
    self->m->append_slice(self, other, slice_length);
  }

  return self;
}


static String *
String_truncate(String *self)
{
  if(self != null_String) {
    self->length = 0;
    self->position = 0;
    *(self->string) = '\x00';
  }

  return self;
}
