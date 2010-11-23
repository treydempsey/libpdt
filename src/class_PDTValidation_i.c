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
#include "class_PDTValidation_i.h"

/*
 * Class Methods
 ***************/

int
class_PDTValidation(void)
{
  if(null_PDTValidation == NULL) {
    /* Dependencies */
    class_String();

    /* Methods */
    PDTValidation_methods.init = PDTValidation_init;
    PDTValidation_methods.free = PDTValidation_free;

    /* Null PDTValidation Instance */
    null_PDTValidation = &_null_PDTValidation;
    null_PDTValidation->handle = &null_PDTValidation;
    null_PDTValidation->m = &PDTValidation_methods;
    PDTValidation_init(null_PDTValidation, "", 0);
  }

  return 1;
}


PDTValidation *
new_PDTValidation(char * validation, size_t validation_length)
{
  PDTValidation * self;

  if(null_PDTValidation == NULL) {
    class_PDTValidation();
  }

  self = alloc_PDTValidation();
  self->m->init(self, validation, validation_length);

  return self;
}


PDTValidation *
alloc_PDTValidation(void)
{
  PDTValidation ** handle;

  /* Allocate */
  handle = malloc(sizeof(PDTValidation *));
  if(handle == NULL) {
    handle = null_PDTValidation->handle;
    goto error;
  }

  *handle = malloc(sizeof(PDTValidation));
  if(*handle == NULL) {
    free(handle);
    handle = null_PDTValidation->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(PDTValidation));
  (*handle)->handle = handle;
  (*handle)->m = &PDTValidation_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_PDTValidation()");
  }

  return *handle;
}


static PDTValidation *
PDTValidation_init(PDTValidation * self, char * validation, size_t validation_length)
{
  /* Variables */
  if(self != null_PDTValidation) {
    self->validation_name = new_String(validation, validation_length);
    self->arguments = null_Array;
    _PDTValidation_parse_arguments(self);
  }
  else {
    self->validation_name = null_String;
    self->arguments = null_Array;
  }

  return self;
}


static PDTValidation *
PDTValidation_free(PDTValidation * self)
{
  if(self != null_PDTValidation) {
    self->validation_name->m->free(self->validation_name);
    self->arguments->m->free(self->arguments);

    free(self->handle);
    free(self);
  }

  return null_PDTValidation;
}


/*
 * Private Instance Methods
 *************************/

static PDTValidation *
_PDTValidation_parse_arguments(PDTValidation * self)
{
  int             argument_start = -1;
  int             argument_end = -1;
  String *        args = null_String;
  String *        comma = null_String;

  if(self != null_PDTValidation) {
    comma = new_String(",", 1);
    self->validation_name->position = 0;
    while(self->validation_name->position < self->validation_name->length) {
      if(*(self->validation_name->string + self->validation_name->position) == '(') {
        argument_start = self->validation_name->position + 1;
        break;
      }
      self->validation_name->position++;
    }

    if(argument_start != -1) {
      while(self->validation_name->position < self->validation_name->length) {
        if(*(self->validation_name->string + self->validation_name->position) == ')') {
        argument_end = self->validation_name->position - 1;
          break;
        }
        self->validation_name->position++;
      }
    }

    if(argument_start != -1 && argument_end != -1) {
      self->validation_name->position = argument_start;
      args = new_String("", 0);
      args->m->extend(args, 1023);
      args->m->slice(args, self->validation_name, (argument_end - argument_start) + 1);
      self->arguments = args->m->split(args, comma);

      self->arguments->m->reset_each(self->arguments);
      while(self->arguments->m->each(self->arguments) != null_ArrayElement) {
        ((String *)self->arguments->current_element->data)->m->ltrim((String *)self->arguments->current_element->data);
        ((String *)self->arguments->current_element->data)->m->rtrim((String *)self->arguments->current_element->data);
      }

      args->m->free(args);

      argument_start--;
      *(self->validation_name->string + argument_start) = '\0';
      self->validation_name->length = argument_start;
    }

    comma->m->free(comma);
  }

  return self;
}
