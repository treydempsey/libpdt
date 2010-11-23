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

#ifndef _CLASS_PDTFORMATFILE_H_
#define _CLASS_PDTFORMATFILE_H_

#include <class_Array.h>
#include <class_String.h>
#include <class_PDTFormat.h>
#include <class_PDTFormatFile.h>

/* Class Instance */
typedef struct PDTFormatFile {
  struct PDTFormatFile **       handle;

  struct PDTFormat *            format;
  String *                      format_file_path;

  /* Private Variables */
  int                           _char_class;
  String *                      _chunk;
  int                           _format_file;
  String *                      _key;
  int                           _state;
  String *                      _value;


  /* Instance Methods */
  struct PDTFormatFileMethods * m;
} PDTFormatFile;


typedef struct PDTFormatFileMethods {
  PDTFormatFile * (*init)(PDTFormatFile * self, char * format_file_path, size_t format_file_path_length);
  PDTFormatFile * (*free)(PDTFormatFile * self);

  PDTFormatFile * (*set_format_file_path)(PDTFormatFile * self, String * new_format_file_path);

  /* Methods */
  PDTFormatFile * (*parse)(PDTFormatFile * self);
} PDTFormatFileMethods;


#ifndef _CLASS_PDTFORMATFILE_I_H_
extern int             class_PDTFormatFile(void);
extern PDTFormatFile * new_PDTFormatFile(char * format_file_path, size_t format_file_path_length);
extern PDTFormatFile * alloc_PDTFormatFile(void);

extern PDTFormatFile * null_PDTFormatFile;
#endif /* not _CLASS_PDTFORMATFILE_I_H_ */

#endif /* not _CLASS_PDTFORMATFILE_H_ */
