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

#ifndef _CLASS_STRING_H_
#define _CLASS_STRING_H_

/* Class Instance */
typedef struct String {
  struct String **handle;
  char   *string;
  size_t size;
  size_t length;
  size_t position;
  size_t blk_size;

  /* Instance Methods */
  struct StringMethods *m;
} String;


/* Class Instance Methods */
typedef struct StringMethods {
  String * (*init)(String *self, char *cstr, size_t size, size_t length);
  String * (*free)(String *self);

  /* Properties */
  size_t   (*blk_size)(String *self);
  String * (*set_blk_size)(String *self, size_t new_blk_size);

  /* Instance Methods */
  String * (*append)(String *self, String *other);
  String * (*append_cstr)(String *self, char *cstr, size_t append_length);
  String * (*append_slice)(String *self, String *other, size_t slice_length);
  int      (*compare)(String *self, String *other);
  int      (*comparen)(String *self, String *other, size_t compare_length);
  String * (*dup)(String *self);
  String * (*extend)(String *self, size_t add);
  String * (*slice)(String *self, String *other, size_t slice_length);
  String * (*truncate)(String *self);
} StringMethods;


#ifndef _CLASS_STRING_I_H_
extern int       class_String(void);
extern String *  new_String(char *cstr, size_t size, size_t length);
extern String *  alloc_String(size_t size);

extern String *null_String;
#endif /* not _CLASS_STRING_I_H_ */

#endif /* not _CLASS_STRING_H_ */
