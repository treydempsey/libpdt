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

#ifndef _CLASS_PDTVALIDATOR_H_
#define _CLASS_PDTVALIDATOR_H_

#include <class_Array.h>
#include <class_ArrayElement.h>
#include <class_String.h>
#include <class_PDTValidation.h>

/* Class Instance */
typedef struct PDTValidator {
  struct PDTValidator **        handle;

  /* Instance Methods */
  struct PDTValidatorMethods *  m;
} PDTValidator;


typedef struct PDTValidatorMethods {
  PDTValidator *                (*init)(PDTValidator * self);
  PDTValidator *                (*free)(PDTValidator * self);

  /* Methods */
  int                           (*dispatch)(PDTValidator * self, PDTValidation * validation, String * subject);

  /* Validations */
  int                           (*is_integer)(PDTValidator * self, Array * arguments, String * subject);
  int                           (*is_timestamp)(PDTValidator * self, Array * arguments, String * subject);
  int                           (*length)(PDTValidator * self, Array * arguments, String * subject);
  int                           (*not_null)(PDTValidator * self, Array * arguments, String * subject);
  int                           (*utf8_length)(PDTValidator * self, Array * arguments, String * subject);
  int                           (*utf8_valid)(PDTValidator * self, Array * arguments, String * subject);
  int                           (*value_in)(PDTValidator * self, Array * arguments, String * subject);
} PDTValidatorMethods;


#ifndef _CLASS_PDTVALIDATOR_I_H_
extern int                      class_PDTValidator(void);
extern PDTValidator *           new_PDTValidator(void);
extern PDTValidator *           alloc_PDTValidator(void);

extern PDTValidator *           null_PDTValidator;
#endif /* not _CLASS_PDTVALIDATOR_I_H_ */

#endif /* not _CLASS_PDTVALIDATOR_H_ */
