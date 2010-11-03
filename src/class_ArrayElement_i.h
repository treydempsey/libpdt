/*
 *  Copyright 2010  Trey Dempsey
 *
 *  libparse_delimited_text is distributed under the terms of the GNU
 *  Lesser General Public License.
 * 
 *  This file is part of libparse_delimited_text.
 *
 *  libparse_delimited_text is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  libparse_delimited_text is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with libparse_delimited_text.  If not, see
 *  <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#ifndef _CLASS_ARRAYELEMENT_I_H_
#define _CLASS_ARRAYELEMENT_I_H_

#include "class_ArrayElement.h"


/* Class Methods */
int                         class_ArrayElement(void);
ArrayElement *              new_ArrayElement(void * data);
ArrayElement *              alloc_ArrayElement(void);

/* Instance Methods */
static ArrayElement *       ArrayElement_init(ArrayElement * self, void * data);
static ArrayElement *       ArrayElement_free(ArrayElement * self);

static int                  ArrayElement_compare(ArrayElement * self, ArrayElement * other);
static ArrayElement *       ArrayElement_dup(ArrayElement * self);

/* Global Variables */
ArrayElement *              null_ArrayElement = NULL;

static ArrayElement         _null_ArrayElement;
static ArrayElementMethods  ArrayElement_methods;

#endif /* not _CLASS_ARRAYELEMENT_I_H_ */
