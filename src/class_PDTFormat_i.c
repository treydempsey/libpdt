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

#include "dependencies.h"
#include "class_PDTFormat_i.h"

/*
 * Class Methods
 ***************/

int
class_PDTFormat(void)
{
  if(null_PDTFormat == NULL) {
    /* Dependencies */
    class_Array();
    class_String();
    class_PDTColumn();
    class_PDTFormatFile();

    /* Methods */
    PDTFormat_methods.init                    = PDTFormat_init;
    PDTFormat_methods.free                    = PDTFormat_free;

    PDTFormat_methods.set_allow_fewer_columns = PDTFormat_set_allow_fewer_columns;
    PDTFormat_methods.set_delimiter           = PDTFormat_set_delimiter;
    PDTFormat_methods.set_escape              = PDTFormat_set_escape;
    PDTFormat_methods.set_quote               = PDTFormat_set_quote;
    PDTFormat_methods.set_skip                = PDTFormat_set_skip;

    PDTFormat_methods.add_column              = PDTFormat_add_column;
    PDTFormat_methods.read_file               = PDTFormat_read_file;

    /* Null PDTFormat Instance */
    null_PDTFormat                            = &_null_PDTFormat;
    null_PDTFormat->handle                    = &null_PDTFormat;
    null_PDTFormat->m                         = &PDTFormat_methods;
    PDTFormat_init(null_PDTFormat);
  }

  return 1;
}


PDTFormat *
new_PDTFormat(void)
{
  PDTFormat * self;

  if(null_PDTFormat == NULL) {
    class_PDTFormat();
  }

  self = alloc_PDTFormat();
  self->m->init(self);

  return self;
}


PDTFormat *
alloc_PDTFormat(void)
{
  PDTFormat ** handle;

  /* Allocate */
  handle = malloc(sizeof(PDTFormat *));
  if(handle == NULL) {
    handle = null_PDTFormat->handle;
    goto error;
  }

  *handle = malloc(sizeof(PDTFormat));
  if(*handle == NULL) {
    free(handle);
    handle = null_PDTFormat->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(PDTFormat));
  (*handle)->handle = handle;
  (*handle)->m = &PDTFormat_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_PDTFormat()");
  }

  return *handle;
}


static PDTFormat *
PDTFormat_init(PDTFormat * self)
{
  /* Variables */
  self->allow_fewer_columns = 0;
  self->columns             = null_Array;
  self->delimiter           = null_String;
  self->escape              = null_String;
  self->quote               = null_String;
  self->skip                = 0;
  self->format_file         = null_PDTFormatFile;

  return self;
}


static PDTFormat *
PDTFormat_free(PDTFormat * self)
{
  if(self != null_PDTFormat) {
    self->columns->m->free(self->columns);
    self->delimiter->m->free(self->delimiter);
    self->escape->m->free(self->escape);
    self->quote->m->free(self->quote);
    self->format_file->m->free(self->format_file);

    free(self->handle);
    free(self);
  }

  return null_PDTFormat;
}


/*
 * Properties
 ************/

static PDTFormat *
PDTFormat_set_allow_fewer_columns(PDTFormat * self, int new_allow_fewer_columns)
{
  if(self != null_PDTFormat) {
    self->allow_fewer_columns = new_allow_fewer_columns;
  }

  return self;
}


static PDTFormat *
PDTFormat_set_delimiter(PDTFormat * self, String * new_delimiter)
{
  if(self != null_PDTFormat) {
    self->delimiter->m->free(self->delimiter);
    self->delimiter = new_delimiter;
  }

  return self;
}


static PDTFormat *
PDTFormat_set_escape(PDTFormat * self, String * new_escape)
{
  if(self != null_PDTFormat) {
    self->escape->m->free(self->escape);
    self->escape = new_escape;
  }

  return self;
}


static PDTFormat *
PDTFormat_set_quote(PDTFormat * self, String * new_quote)
{
  if(self != null_PDTFormat) {
    self->quote->m->free(self->quote);
    self->quote = new_quote;
  }

  return self;
}


static PDTFormat *
PDTFormat_set_skip(PDTFormat * self, int new_skip)
{
  if(self != null_PDTFormat) {
    self->skip = new_skip;
  }

  return self;
}


/*
 * Public Instance Methods
 *************************/

static PDTFormat *
PDTFormat_add_column(PDTFormat * self, char * column, size_t column_length)
{
  PDTColumn * pdt_column;

  if(self != null_PDTFormat) {
    if(self->columns == null_Array) {
      self->columns = new_Array();
      self->columns->auto_free = _PDTColumn_Array_free;
    }

    pdt_column = new_PDTColumn(column, column_length);
    self->columns->m->append(self->columns, pdt_column);
  }

  return self;
}


static PDTFormat *
PDTFormat_read_file(PDTFormat * self, char * format_file_path, size_t format_file_length)
{
  if(self != null_PDTFormat) {
    self->format_file->m->free(self->format_file);
    self->format_file = new_PDTFormatFile(format_file_path, format_file_length);
    self->format_file->format = self;
    self->format_file->m->parse(self->format_file);
  }

  return self;
}


/*
 * Private Instance Methods
 **************************/

static void
_PDTColumn_Array_free(void * data)
{
  PDTColumn * pdt_column;

  pdt_column = (PDTColumn *)data;
  pdt_column->m->free(pdt_column);
}
