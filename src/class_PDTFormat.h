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

#ifndef _CLASS_PDTFORMAT_H_
#define _CLASS_PDTFORMAT_H_

#include <class_Array.h>
#include <class_String.h>
#include <class_PDTColumn.h>
#include <class_PDTFormatFile.h>
#include <class_PDTFormat.h>

/* Class Instance */
typedef struct PDTFormat {
  struct PDTFormat **       handle;

  /* Variables */
  int                       allow_fewer_columns;
  Array *                   columns;
  String *                  delimiter;
  String *                  escape;
  String *                  quote;
  int                       skip;
  struct PDTFormatFile *    format_file;

  /* Instance Methods */
  struct PDTFormatMethods * m;
} PDTFormat;


typedef struct PDTFormatMethods {
  PDTFormat *               (*init)(PDTFormat * self);
  PDTFormat *               (*free)(PDTFormat * self);

  /* Properties */
  PDTFormat *               (*set_allow_fewer_columns)(PDTFormat * self, int new_allow_fewer_columns);
  PDTFormat *               (*set_delimiter)(PDTFormat * self, String * new_delimiter);
  PDTFormat *               (*set_escape)(PDTFormat * self, String * new_escape);
  PDTFormat *               (*set_quote)(PDTFormat * self, String * new_quote);
  PDTFormat *               (*set_skip)(PDTFormat * self, int new_skip);

  PDTFormat *               (*add_column)(PDTFormat * self, char * column, size_t column_length);
  PDTFormat *               (*read_file)(PDTFormat *self, char * format_file_path, size_t format_file_length);
} PDTFormatMethods;


#ifndef _CLASS_PDTFORMAT_I_H_
extern int                  class_PDTFormat(void);
extern PDTFormat *          new_PDTFormat(void);
extern PDTFormat *          alloc_PDTFormat(void);

extern PDTFormat *          null_PDTFormat;
#endif /* not _CLASS_PDTFORMAT_I_H_ */

#endif /* not _CLASS_PDTFORMAT_H_ */
