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

#ifndef _CLASS_ARRAYELEMENT_H_
#define _CLASS_ARRAYELEMENT_H_

/* Class Instance */
typedef struct ArrayElement {
  struct ArrayElement **        handle;
  void *                        data;

  /* Instance Methods */
  struct ArrayElementMethods *  m;
} ArrayElement;


/* Class Methods */
typedef struct ArrayElementMethods {
  ArrayElement *                (*init)(ArrayElement * self, void * data);
  ArrayElement *                (*free)(ArrayElement * self);

  int                           (*compare)(ArrayElement * self, ArrayElement * other);
  ArrayElement *                (*dup)(ArrayElement * self);
} ArrayElementMethods;


#ifndef _CLASS_ARRAYELEMENT_I_H_
extern int                      class_ArrayElement(void);
extern ArrayElement *           new_ArrayElement(void * data);
extern ArrayElement *           alloc_ArrayElement(void);

extern ArrayElement *           null_ArrayElement;
#endif /* not _CLASS_ARRAYELEMENT_I_H_ */

#endif /* not _CLASS_ARRAYELEMENT_H_ */
