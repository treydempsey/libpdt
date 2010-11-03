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

#ifndef _CLASS_ARRAY_I_H_
#define _CLASS_ARRAY_I_H_

#include "class_Array.h"


/* Class Methods */
int                   class_Array(void);
Array *               new_Array(void);
Array *               alloc_Array(void);

/* Instance Methods */
static Array *        Array_init(Array * self);
static Array *        Array_free(Array * self);

static Array *        Array_append(Array * self, void * data);
static Array *        Array_append_element(Array * self, ArrayElement * element);
static int            Array_compare(Array * self, Array * other);
static Array *        Array_concat(Array * self, Array * other);
static Array *        Array_difference(Array * self, Array * other);
static Array *        Array_dup(Array * self);
static ArrayElement * Array_each(Array * self);
static ArrayElement * Array_get(Array * self, size_t index);
static ArrayElement * Array_include(Array * self, ArrayElement * element);
static Array *        Array_intersection(Array * self, Array * other);
static ArrayElement * Array_last(Array * self);
static Array *        Array_reset_each(Array * self);
static Array *        Array_repetition(Array * self, int times);
static Array *        Array_set(Array * self, size_t index, ArrayElement * element);

/* Private Methods */
static Array *        _Array_extend_elements(Array * self, int add_chunks);
static Array *        _Array_null_elements(Array * self, size_t start, size_t end);
inline static size_t  _Array_size_elements(Array * self);

/* Global Variables */
Array *               null_Array = NULL;

static Array          _null_Array;
static ArrayMethods   Array_methods;

#endif /* not _CLASS_ARRAY_I_H_ */
