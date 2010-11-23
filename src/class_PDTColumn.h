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

#ifndef _CLASS_PDTCOLUMN_H_
#define _CLASS_PDTCOLUMN_H_

#include <class_Array.h>
#include <class_ArrayElement.h>
#include <class_String.h>
#include <class_PDTValidation.h>

/* Class Instance */
typedef struct PDTColumn {
  struct PDTColumn **       handle;

  String *                  column_name;
  Array *                   validations;

  /* Instance Methods */
  struct PDTColumnMethods * m;
} PDTColumn;


typedef struct PDTColumnMethods {
  PDTColumn * (*init)(PDTColumn * self, char * column, size_t column_length);
  PDTColumn * (*free)(PDTColumn * self);

  /* Methods */
  PDTColumn * (*add_validation)(PDTColumn * self, char * validation, size_t validation_length);
} PDTColumnMethods;


#ifndef _CLASS_PDTCOLUMN_I_H_
extern int         class_PDTColumn(void);
extern PDTColumn * new_PDTColumn(char * column, size_t column_length);
extern PDTColumn * alloc_PDTColumn(void);

extern PDTColumn * null_PDTColumn;
#endif /* not _CLASS_PDTCOLUMN_I_H_ */

#endif /* not _CLASS_PDTCOLUMN_H_ */
