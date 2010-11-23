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

#ifndef _CLASS_PDTVALIDATION_H_
#define _CLASS_PDTVALIDATION_H_

#include <class_String.h>
#include <class_PDTValidation.h>

/* Class Instance */
typedef struct PDTValidation {
  struct PDTValidation **       handle;

  String *                      validation_name;
  Array *                       arguments;

  /* Instance Methods */
  struct PDTValidationMethods * m;
} PDTValidation;


typedef struct PDTValidationMethods {
  PDTValidation * (*init)(PDTValidation * self, char * validation, size_t validation_length);
  PDTValidation * (*free)(PDTValidation * self);
} PDTValidationMethods;


#ifndef _CLASS_PDTVALIDATION_I_H_
extern int              class_PDTValidation(void);
extern PDTValidation *  new_PDTValidation(char * validation, size_t validation_length);
extern PDTValidation *  alloc_PDTValidation(void);

extern PDTValidation *  null_PDTValidation;
#endif /* not _CLASS_PDTVALIDATION_I_H_ */

#endif /* not _CLASS_PDTVALIDATION_H_ */
