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
#include "class_PDTColumn_i.h"

/*
 * Class Methods
 ***************/

int
class_PDTColumn(void)
{
  if(null_PDTColumn == NULL) {
    /* Dependencies */
    class_String();
    class_Array();
    class_PDTValidation();

    /* Methods */
    PDTColumn_methods.init = PDTColumn_init;
    PDTColumn_methods.free = PDTColumn_free;

    PDTColumn_methods.add_validation = PDTColumn_add_validation;

    /* Null PDTColumn Instance */
    null_PDTColumn = &_null_PDTColumn;
    null_PDTColumn->handle = &null_PDTColumn;
    null_PDTColumn->m = &PDTColumn_methods;
    PDTColumn_init(null_PDTColumn, "", 0);
  }

  return 1;
}


PDTColumn *
new_PDTColumn(char * column, size_t column_length)
{
  PDTColumn * self;

  if(null_PDTColumn == NULL) {
    class_PDTColumn();
  }

  self = alloc_PDTColumn();
  self->m->init(self, column, column_length);

  return self;
}


PDTColumn *
alloc_PDTColumn(void)
{
  PDTColumn ** handle;

  /* Allocate */
  handle = malloc(sizeof(PDTColumn *));
  if(handle == NULL) {
    handle = null_PDTColumn->handle;
    goto error;
  }

  *handle = malloc(sizeof(PDTColumn));
  if(*handle == NULL) {
    free(handle);
    handle = null_PDTColumn->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(PDTColumn));
  (*handle)->handle = handle;
  (*handle)->m = &PDTColumn_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_PDTColumn()");
  }

  return *handle;
}


static PDTColumn *
PDTColumn_init(PDTColumn * self, char * column, size_t column_length)
{
  /* Variables */
  if(self != null_PDTColumn) {
    self->column_name = new_String(column, column_length);
  }
  else {
    self->column_name = null_String;
  }

  self->validations = null_Array;

  return self;
}


static PDTColumn *
PDTColumn_free(PDTColumn * self)
{
  if(self != null_PDTColumn) {
    self->validations->m->free(self->validations);
    self->column_name->m->free(self->column_name);

    free(self->handle);
    free(self);
  }

  return null_PDTColumn;
}


/*
 * Public Instance Methods
 *************************/

static PDTColumn *
PDTColumn_add_validation(PDTColumn * self, char * validation, size_t validation_length)
{
  PDTValidation * pdt_validation;

  if(self != null_PDTColumn) {
    if(self->validations == null_Array) {
      self->validations = new_Array();
      self->validations->auto_free = _PDTValidation_Array_free;
    }

    pdt_validation = new_PDTValidation(validation, validation_length);
    self->validations->m->append(self->validations, pdt_validation);
  }

  return self;
}


/*
 * Private Instance Methods
 **************************/

static void
_PDTValidation_Array_free(void * data)
{
  PDTValidation * pdt_validation;

  pdt_validation = (PDTValidation *)data;
  pdt_validation->m->free(pdt_validation);
}
