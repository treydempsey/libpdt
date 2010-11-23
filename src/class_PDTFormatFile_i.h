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

#ifndef _CLASS_PDTFORMATFILE_I_H_
#define _CLASS_PDTFORMATFILE_I_H_

#include "class_PDTFormatFile.h"


/* Class Methods */
int                    class_PDTFormatFile(void);
PDTFormatFile *        new_PDTFormatFile(char * format_file_path, size_t file_format_path_length);
PDTFormatFile *        alloc_PDTFormatFile(void);


/* Instance Methods */
static PDTFormatFile * PDTFormatFile_init(PDTFormatFile * self, char * format_file_path, size_t file_format_path_length);
static PDTFormatFile * PDTFormatFile_free(PDTFormatFile * self);

static PDTFormatFile * PDTFormatFile_set_format_file_path(PDTFormatFile * self, String * new_format_file_path);

static PDTFormatFile * PDTFormatFile_parse(PDTFormatFile * self);


/* Private Methods */
static PDTFormatFile * _PDTFormatFile_close_format_file(PDTFormatFile * self);
static PDTFormatFile * _PDTFormatFile_identify_char(PDTFormatFile * self, char c);
static PDTFormatFile * _PDTFormatFile_open_format_file(PDTFormatFile * self);
static PDTFormatFile * _PDTFormatFile_parse_line(PDTFormatFile * self, String * line);
static PDTFormatFile * _PDTFormatFile_set_key_value(PDTFormatFile * self);
static PDTFormatFile * _PDTFormatFile_state(PDTFormatFile * self);


/* Global Variables */
PDTFormatFile *         null_PDTFormatFile = NULL;
PDTFormatFile           _null_PDTFormatFile;
PDTFormatFileMethods    PDTFormatFile_methods;


/* Constants */
#define CHAR_COLON          '\x3A'
#define CHAR_COMMENT        '\x23'
#define CHAR_CR             '\x0D'
#define CHAR_HYPHEN         '\x2D'
#define CHAR_LF             '\x0A'
#define CHAR_SPACE          '\x20'
#define CHAR_TAB            '\x09'
#define CHAR_VTAB           '\x0B'

#define CL_NONE             0
#define CL_CHAR             1
#define CL_COLON            2
#define CL_COMMENT          3
#define CL_EOL              4
#define CL_HYPHEN           5
#define CL_WHITESPACE       6

#define ST_NONE                       0
#define ST_COMMENT                    1
#define ST_MULTILINE_VALUE_COMMENT0   2
#define ST_MULTILINE_VALUE_COMMENT1   3
#define ST_BEFORE_KEY                 4
#define ST_START_KEY                  5
#define ST_KEY                        6
#define ST_END_KEY                    7
#define ST_BEFORE_VALUE               8
#define ST_START_VALUE                9
#define ST_VALUE                      10
#define ST_END_VALUE                  11
#define ST_BEFORE_MULTILINE_VALUE0    12
#define ST_CONTINUE_MULTILINE_VALUE0  13
#define ST_START_MULTILINE_VALUE0     14
#define ST_MULTILINE_VALUE0           15
#define ST_END_MULTILINE_VALUE0       16
#define ST_BEFORE_MULTILINE_VALUE1    17
#define ST_CONTINUE_MULTILINE_VALUE1  18
#define ST_START_MULTILINE_VALUE1     19
#define ST_MULTILINE_VALUE1           20
#define ST_END_MULTILINE_VALUE1       21

#endif /* not _CLASS_PDTFORMATFILE_I_H_ */
