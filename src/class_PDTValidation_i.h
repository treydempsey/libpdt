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

#ifndef _CLASS_PDTVALIDATION_I_H_
#define _CLASS_PDTVALIDATION_I_H_

#include "class_PDTValidation.h"


/* Class Methods */
int                     class_PDTValidation(void);
PDTValidation *         new_PDTValidation(char * validation, size_t validation_length);
PDTValidation *         alloc_PDTValidation(void);


/* Instance Methods */
static PDTValidation * PDTValidation_init(PDTValidation * self, char * validation, size_t validation_length);
static PDTValidation * PDTValidation_free(PDTValidation * self);


/* Private Instance Methods */

static PDTValidation * _PDTValidation_parse_arguments(PDTValidation * self);


/* Global Variables */
PDTValidation *        null_PDTValidation = NULL;
PDTValidation          _null_PDTValidation;
PDTValidationMethods   PDTValidation_methods;

#endif /* not _CLASS_PDTVALIDATION_I_H_ */
