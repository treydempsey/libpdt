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

#ifndef _CLASS_ARRAY_H_
#define _CLASS_ARRAY_H_

#include <class_ArrayElement.h>

#define ARRAY_CHUNK_SIZE 10

/* Class Instance */
typedef struct Array {
  struct Array **         handle;

  struct ArrayElement **  elements;
  struct ArrayElement *   current_element;
  int                     length;
  int                     current_index;
  size_t                  chunk_size;
  size_t                  chunks;

  /* Instance Methods */
  struct ArrayMethods *   m;
} Array;


/* Class Methods */
typedef struct ArrayMethods {
  Array *                 (*init)(Array * self);
  Array *                 (*free)(Array * self);

  Array *                 (*append)(Array * self, void * data);
  Array *                 (*append_element)(Array * self, ArrayElement * element);
  int                     (*compare)(Array * self, Array * other);
  Array *                 (*concat)(Array * self, Array * other);
  Array *                 (*difference)(Array * self, Array * other);
  Array *                 (*dup)(Array * self);
  ArrayElement *          (*each)(Array * self);
  ArrayElement *          (*get)(Array * self, size_t index);
  ArrayElement *          (*include)(Array * self, ArrayElement * element);
  Array *                 (*intersection)(Array * self, Array * other);
  ArrayElement *          (*last)(Array * self);
  Array *                 (*reset_each)(Array * self);
  Array *                 (*repetition)(Array * self, int times);
  Array *                 (*set)(Array * self, size_t index, ArrayElement * element);
} ArrayMethods;


#ifndef _CLASS_ARRAY_I_H_
extern int                class_Array(void);
extern Array *            new_Array(void);
extern Array *            alloc_Array(void);

extern Array *            null_Array;
#endif /* not _CLASS_ARRAY_I_H_ */

#endif /* not _CLASS_ARRAY_H_ */
