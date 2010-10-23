#ifndef _CLASS_ARRAY_I_H_
#define _CLASS_ARRAY_I_H_

#include "class_Array.h"


/* Class Methods */
int      class_Array(void);
Array *  new_Array(void);
Array *  alloc_Array(void);

/* Instance Methods */
static Array *        Array_init(Array *self);
static Array *        Array_free(Array *self);

static Array *        Array_append(Array *self, ArrayElement *element);
static int            Array_compare(Array *self, Array *other);
static Array *        Array_concat(Array *self, Array *other);
static Array *        Array_difference(Array *self, Array *other);
static Array *        Array_dup(Array *self);
static ArrayElement * Array_each(Array *self);
static ArrayElement * Array_get(Array *self, size_t index);
static ArrayElement * Array_include(Array *self, ArrayElement *element);
static Array *        Array_intersection(Array *self, Array *other);
static Array *        Array_reset_each(Array *self);
static Array *        Array_repetition(Array *self, int times);
static Array *        Array_set(Array *self, size_t index, ArrayElement *element);

/* Private Methods */
static Array *        _Array_extend_elements(Array *self, int add_chunks);
static Array *        _Array_null_elements(Array *self, size_t start, size_t end);
inline static size_t  _Array_size_elements(Array *self);

/* Global Variables */
Array                 *null_Array = NULL;
Array                 _null_Array;
ArrayMethods          Array_methods;

#endif /* not _CLASS_ARRAY_I_H_ */
