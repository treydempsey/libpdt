#ifndef _CLASS_ARRAYELEMENT_I_H_
#define _CLASS_ARRAYELEMENT_I_H_

#include "class_ArrayElement.h"


/* Class Methods */
int                   class_ArrayElement(void);
ArrayElement *        new_ArrayElement(void *data);
ArrayElement *        alloc_ArrayElement(void);

/* Instance Methods */
static ArrayElement * ArrayElement_init(ArrayElement *self, void *data);
static ArrayElement * ArrayElement_free(ArrayElement *self);

static int            ArrayElement_compare(ArrayElement *self, ArrayElement *other);
static ArrayElement * ArrayElement_dup(ArrayElement *self);

/* Global Variables */
ArrayElement          *null_ArrayElement = NULL;
ArrayElement          _null_ArrayElement;
ArrayElementMethods   ArrayElement_methods;

#endif /* not _CLASS_ARRAYELEMENT_I_H_ */
