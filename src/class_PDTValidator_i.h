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

#ifndef _CLASS_PDTVALIDATOR_I_H_
#define _CLASS_PDTVALIDATOR_I_H_

#include "class_PDTValidator.h"


/* Class Methods */
int                         class_PDTValidator(void);
PDTValidator *              new_PDTValidator(void);
PDTValidator *              alloc_PDTValidator(void);


/* Instance Methods */
static PDTValidator *       PDTValidator_init(PDTValidator * self);
static PDTValidator *       PDTValidator_free(PDTValidator * self);

static int                  PDTValidator_dispatch(PDTValidator * self, PDTValidation * validation, String * subject);

/* Validations */
static int                  PDTValidator_is_integer(PDTValidator * self, Array * arguments, String * subject);
static int                  PDTValidator_is_timestamp(PDTValidator * self, Array * arguments, String * subject);
static int                  PDTValidator_length(PDTValidator * self, Array * arguments, String * subject);
static int                  PDTValidator_not_null(PDTValidator * self, Array * arguments, String * subject);
static int                  PDTValidator_utf8_length(PDTValidator * self, Array * arguments, String * subject);
static int                  PDTValidator_utf8_valid(PDTValidator * self, Array * arguments, String * subject);
static int                  PDTValidator_value_in(PDTValidator * self, Array * arguments, String * subject);


/* Global Variables */
PDTValidator *              null_PDTValidator = NULL;

static PDTValidator         _null_PDTValidator;
static PDTValidatorMethods  PDTValidator_methods;

#endif /* not _CLASS_PDTVALIDATOR_I_H_ */
