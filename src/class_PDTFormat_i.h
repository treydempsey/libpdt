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

#ifndef _CLASS_PDTFORMAT_I_H_
#define _CLASS_PDTFORMAT_I_H_

#include "class_PDTFormat.h"


/* Class Methods */
int                     class_PDTFormat(void);
PDTFormat *             new_PDTFormat(void);
PDTFormat *             alloc_PDTFormat(void);


/* Instance Methods */
static PDTFormat *      PDTFormat_init(PDTFormat * self);
static PDTFormat *      PDTFormat_free(PDTFormat * self);

static PDTFormat *      PDTFormat_set_allow_fewer_columns(PDTFormat * self, int new_allow_fewer_columns);
static PDTFormat *      PDTFormat_set_delimiter(PDTFormat * self, String *new_delimiter);
static PDTFormat *      PDTFormat_set_escape(PDTFormat * self, String * new_escape);
static PDTFormat *      PDTFormat_set_quote(PDTFormat * self, String * new_quote);
static PDTFormat *      PDTFormat_set_skip(PDTFormat * self, int new_skip);

static PDTFormat *      PDTFormat_add_column(PDTFormat * self, char * column, size_t column_length);
static PDTFormat *      PDTFormat_read_file(PDTFormat *self, char * format_file_path, size_t format_file_length);
  

/* Private Instance Methods */
static void             _PDTColumn_Array_free(void * data);


/* Global Variables */
PDTFormat *             null_PDTFormat = NULL;

static PDTFormat        _null_PDTFormat;
static PDTFormatMethods PDTFormat_methods;

#endif /* not _CLASS_PDTFORMAT_I_H_ */
