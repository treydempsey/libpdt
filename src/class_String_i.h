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

#ifndef _CLASS_STRING_I_H_
#define _CLASS_STRING_I_H_

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "class_String.h"


/* Class Methods */
int             class_String(void);
String *        new_String(char *cstr, size_t size, size_t length);
String *        alloc_String(size_t size);

/* Instance Methods */
static String * String_init(String *self, char *cstr, size_t size, size_t length);
static String * String_free(String *self);

static size_t   String_blk_size(String *self);
static String * String_set_blk_size(String *self, size_t new_blk_size);

static String * String_append(String *self, String *other);
static String * String_append_cstr(String *self, char *cstr, size_t append_length);
static String * String_append_slice(String *self, String *other, size_t slice_length);
static size_t   String_chomp(String *self);
static int      String_compare(String *self, String *other);
static int      String_comparen(String *self, String *other, size_t compare_length);
static String * String_dup(String *self);
static String * String_each_line(String *self);
static size_t   String_eol(String *self);
static String * String_ltrim(String *self);
static String * String_extend(String *self, size_t add);
static String * String_slice(String *self, String *other, size_t slice_length);
static String * String_truncate(String *self);

/* Private Methods */

/* Global Variables */
String          *null_String = NULL;
String          _null_String;
StringMethods   String_methods;

#endif /* not _CLASS_STRING_I_H_ */
