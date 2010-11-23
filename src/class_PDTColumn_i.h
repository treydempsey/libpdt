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

#ifndef _CLASS_PDTCOLUMN_I_H_
#define _CLASS_PDTCOLUMN_I_H_

#include "class_PDTColumn.h"


/* Class Methods */
int                     class_PDTColumn(void);
PDTColumn *             new_PDTColumn(char * column, size_t column_length);
PDTColumn *             alloc_PDTColumn(void);


/* Instance Methods */
static PDTColumn *      PDTColumn_init(PDTColumn * self, char * column, size_t column_length);
static PDTColumn *      PDTColumn_free(PDTColumn * self);

static PDTColumn *      PDTColumn_add_validation(PDTColumn * self, char * validation, size_t column_length);

/* Private Instance Methods */
static void             _PDTValidation_Array_free(void * data);


/* Global Variables */
PDTColumn *             null_PDTColumn = NULL;

static PDTColumn        _null_PDTColumn;
static PDTColumnMethods PDTColumn_methods;

#endif /* not _CLASS_PDTCOLUMN_I_H_ */
