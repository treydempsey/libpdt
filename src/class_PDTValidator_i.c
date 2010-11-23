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
#include "class_PDTValidator_i.h"

/*
 * Class Methods
 ***************/

int
class_PDTValidator(void)
{
  if(null_PDTValidator == NULL) {
    /* Dependencies */
    class_String();
    class_Array();

    /* Methods */
    PDTValidator_methods.init         = PDTValidator_init;
    PDTValidator_methods.free         = PDTValidator_free;

    PDTValidator_methods.dispatch     = PDTValidator_dispatch;

    /* Validators */
    PDTValidator_methods.is_integer   = PDTValidator_is_integer;
    PDTValidator_methods.is_timestamp = PDTValidator_is_timestamp;
    PDTValidator_methods.length       = PDTValidator_length;
    PDTValidator_methods.not_null     = PDTValidator_not_null;
    PDTValidator_methods.utf8_length  = PDTValidator_utf8_length;
    PDTValidator_methods.utf8_valid   = PDTValidator_utf8_valid;
    PDTValidator_methods.value_in     = PDTValidator_value_in;

    /* Null PDTValidator Instance */
    null_PDTValidator = &_null_PDTValidator;
    null_PDTValidator->handle = &null_PDTValidator;
    null_PDTValidator->m = &PDTValidator_methods;
    PDTValidator_init(null_PDTValidator);
  }

  return 1;
}


PDTValidator *
new_PDTValidator(void)
{
  PDTValidator * self;

  if(null_PDTValidator == NULL) {
    class_PDTValidator();
  }

  self = alloc_PDTValidator();
  self->m->init(self);

  return self;
}


PDTValidator *
alloc_PDTValidator(void)
{
  PDTValidator ** handle;

  /* Allocate */
  handle = malloc(sizeof(PDTValidator *));
  if(handle == NULL) {
    handle = null_PDTValidator->handle;
    goto error;
  }

  *handle = malloc(sizeof(PDTValidator));
  if(*handle == NULL) {
    free(handle);
    handle = null_PDTValidator->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(PDTValidator));
  (*handle)->handle = handle;
  (*handle)->m = &PDTValidator_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_PDTValidator()");
  }

  return *handle;
}


static PDTValidator *
PDTValidator_init(PDTValidator * self)
{
  /* Variables */

  return self;
}


static PDTValidator *
PDTValidator_free(PDTValidator * self)
{
  if(self != null_PDTValidator) {
    free(self->handle);
    free(self);
  }

  return null_PDTValidator;
}


/*
 * Public Instance Methods
 *************************/

static int
PDTValidator_dispatch(PDTValidator * self, PDTValidation * validation, String * subject)
{ /* {{{ */
  int valid = 1;

  if(self != null_PDTValidator && validation != null_PDTValidation) {
    /* Switch on length then key character */
    switch(validation->validation_name->length) {
      /* length */
      case 6:
        valid = self->m->length(self, validation->arguments, subject);
      break;

      /* not_null, value_in */
      case 8:
        switch(*(validation->validation_name->string)) {
          /* not_null */
          case 'n':
          case 'N':
            valid = self->m->not_null(self, validation->arguments, subject);
          break;

          /* value_in */
          case 'v':
          case 'V':
            valid = self->m->value_in(self, validation->arguments, subject);
          break;

          default:
            fprintf(stderr, "ERROR: unknown validation %s\n", validation->validation_name->string);
            valid = 0;
        }
      break;

      /* is_integer, utf8_valid */
      case 10:
        switch(*(validation->validation_name->string)) {
          /* is_integer */
          case 'i':
          case 'I':
            valid = self->m->is_integer(self, validation->arguments, subject);
          break;

          /* utf8_valid */
          case 'u':
          case 'U':
            valid = self->m->utf8_valid(self, validation->arguments, subject);
          break;

          default:
            fprintf(stderr, "ERROR: unknown validation %s\n", validation->validation_name->string);
            valid = 0;
        }
      break;

      /* utf8_length */
      case 11:
        valid = self->m->utf8_length(self, validation->arguments, subject);
      break;

      /* is_timestamp */
      case 12:
        valid = self->m->is_timestamp(self, validation->arguments, subject);
      break;

      default:
        fprintf(stderr, "ERROR: unknown validation %s\n", validation->validation_name->string);
        valid = 0;
    }
  }

  return valid;
} /* }}} */


static int
PDTValidator_is_integer(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int valid = 1;

  if(arguments->length == 0) {
    for(subject->position = 0; valid != 0 && subject->position < subject->length; subject->position++) {
      valid = isdigit(*(subject->string + subject->position));
    }
    subject->position = 0;
  }
  else {
    fprintf(stderr, "ERROR: invalid numnber of arguments for is_integer() validation, expected 0 arguments.\n");
  }

  return valid;
} /* }}} */


static int
PDTValidator_is_timestamp(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int valid           = 1;
  int matched_year    = 0;
  int matched_month   = 0;
  int matched_day     = 0;
  int matched_hour    = 0;
  int matched_minute  = 0;
  int matched_second  = 0;

  subject->position = 0;
  while(valid != 0 && subject->position < subject->length) {
    /* Year */
    if(matched_year < 4) {
      if(   *(subject->string + subject->position) == '\x20'      /* <SP> */  /* ^\s* */
         || *(subject->string + subject->position) == '\x09'      /* <HT> */
         || *(subject->string + subject->position) == '\x0b') {   /* <VT> */
        ; /* Consume leading white space */
      }
      else {                                                                  /* \d{4} */
        valid = isdigit(*(subject->string + subject->position));
        if(valid != 0) {
          matched_year++;
        }
      }
    }
    else if(matched_year == 4) {                                              /* - */
      valid = (*(subject->string + subject->position) == '-') ? 1 : 0;
      if(valid != 0) {
        matched_year = 5;
      }
    }
    /* Month */
    else if(matched_month < 2) {                                              /* \d{1,2} */
      if(matched_month == 1 && *(subject->string + subject->position) == '-') {
        matched_month = 3;
      }
      else {
        valid = isdigit(*(subject->string + subject->position));
        if(valid != 0) {
          matched_month++;
        }
      }
    }
    else if(matched_month == 2) {                                             /* - */
      valid = (*(subject->string + subject->position) == '-') ? 1 : 0;
      if(valid != 0) {
        matched_month = 3;
      }
    }
    /* Day */
    else if(matched_day < 2) {                                                /* \d{1,2} */
      if(matched_day == 1 && *(subject->string + subject->position) == ' ') {
        matched_day = 3;
      }
      else {
        valid = isdigit(*(subject->string + subject->position));
        if(valid != 0) {
          matched_day++;
        }
      }
    }
    else if(matched_day == 2) {                                               /* <SP> */
      valid = (*(subject->string + subject->position) == ' ') ? 1 : 0;
      if(valid != 0) {
        matched_day = 3;
      }
    }
    /* Hour */
    else if(matched_hour < 2) {                                               /* \d{1,2} */
      if(matched_hour == 1 && *(subject->string + subject->position) == ':') {
        matched_hour = 3;
      }
      else {
        valid = isdigit(*(subject->string + subject->position));
        if(valid != 0) {
          matched_hour++;
        }
      }
    }
    else if(matched_hour == 2) {                                              /* : */
      valid = (*(subject->string + subject->position) == ':') ? 1 : 0;
      if(valid != 0) {
        matched_hour = 3;
      }
    }
    /* Minute */
    else if(matched_minute < 2) {                                             /* \d{1,2} */
      if(matched_minute == 1 && *(subject->string + subject->position) == ':') {
        matched_minute = 3;
      }
      else {
        valid = isdigit(*(subject->string + subject->position));
        if(valid != 0) {
          matched_minute++;
        }
      }
    }
    else if(matched_minute == 2) {                                            /* : */
      valid = (*(subject->string + subject->position) == ':') ? 1 : 0;
      matched_minute = 3;
    }
    /* Second */
    else if(matched_second < 2) {
      valid = isdigit(*(subject->string + subject->position));
      if(valid != 0) {
        matched_second++;
      }
    }
    else if(matched_second == 2) {                                            /* \s*$ */
      if(   *(subject->string + subject->position) != '\x20'      /* <SP> */
         && *(subject->string + subject->position) != '\x09'      /* <HT> */
         && *(subject->string + subject->position) != '\x0a'      /* <LF> */
         && *(subject->string + subject->position) != '\x0b'      /* <VT> */
         && *(subject->string + subject->position) != '\x0d') {   /* <CR> */
        valid = 0;
      }
    }

    subject->position++;
  }
  subject->position = 0;

  return valid;
} /* }}} */


static int
PDTValidator_length(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int       valid = 1;
  int       length = 0;
  String *  length_str;

  if(arguments->length == 1) {
    length_str = (String *)arguments->elements[0]->data;
    length = length_str->m->to_i(length_str);
    if(subject->length > length) {
      valid = 0;
    }
  }
  else {
    fprintf(stderr, "ERROR: invalid numnber of arguments for length() validation, expected 1 argument.\n");
    return 0;
  }

  return valid;
} /* }}} */


static int
PDTValidator_not_null(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int valid = 1;

  if(arguments->length == 0) {
    if(subject->length == 0) {
      valid = 0;
    }
  }
  else {
    fprintf(stderr, "ERROR: invalid numnber of arguments for not_null() validation, expected 0 arguments.\n");
  }

  return valid;
} /* }}} */


static int
PDTValidator_utf8_length(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int       valid = 1;
  int       utf8_length = 0;
  String *  utf8_length_str;

  if(arguments->length == 1) {
    utf8_length_str = (String *)arguments->elements[0]->data;
    utf8_length = utf8_length_str->m->to_i(utf8_length_str);
    if(subject->m->utf8_length(subject) > utf8_length) {
      valid = 0;
    }
  }
  else {
    fprintf(stderr, "ERROR: invalid numnber of arguments for utf8_length() validation, expected 1 argument.\n");
    return 0;
  }

  return valid;
} /* }}} */


static int
PDTValidator_utf8_valid(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int       valid = 1;

  if(arguments->length == 0) {
    if(subject->m->utf8_valid(subject) == 0) {
      valid = 0;
    }
  }
  else {
    fprintf(stderr, "ERROR: invalid numnber of arguments for utf8_valid() validation, expected 0 arguments.\n");
    return 0;
  }

  return valid;
} /* }}} */


static int
PDTValidator_value_in(PDTValidator * self, Array * arguments, String * subject)
{ /* {{{ */
  int       valid = 1;
  String *  argument;

  valid = 0;
  arguments->m->reset_each(arguments);
  while(arguments->m->each(arguments) != null_ArrayElement) {
    argument = (String *)arguments->current_element->data;
    if(argument->m->compare(argument, subject) == 0) {
      valid = 1;
      break;
    }
  }

  return valid;
} /* }}} */
