#ifndef _CLASS_ARRAY_H_
#define _CLASS_ARRAY_H_

#include <class_ArrayElement.h>

#define ARRAY_CHUNK_SIZE 10

/* Class Instance */
typedef struct Array {
  struct Array **handle;
  struct ArrayElement **elements;
  struct ArrayElement *current_element;
  int    length;
  int    current_index;
  size_t chunk_size;
  size_t chunks;

  /* Instance Methods */
  struct ArrayMethods *m;
} Array;


/* Class Methods */
typedef struct ArrayMethods {
  Array *         (*init)(Array *self);
  Array *         (*free)(Array *self);

  Array *         (*append)(Array *self, ArrayElement *element);
  int             (*compare)(Array *self, Array *other);
  Array *         (*concat)(Array *self, Array *other);
  Array *         (*difference)(Array *self, Array *other);
  Array *         (*dup)(Array *self);
  ArrayElement *  (*each)(Array *self);
  ArrayElement *  (*get)(Array *self, size_t index);
  ArrayElement *  (*include)(Array *self, ArrayElement *element);
  Array *         (*intersection)(Array *self, Array *other);
  Array *         (*reset_each)(Array *self);
  Array *         (*repetition)(Array *self, int times);
  Array *         (*set)(Array *self, size_t index, ArrayElement *element);
} ArrayMethods;


#ifndef _CLASS_ARRAY_I_H_
extern int        class_Array(void);
extern Array *    new_Array(void);
extern Array *    alloc_Array(void);

extern Array *null_Array;
#endif /* not _CLASS_ARRAY_I_H_ */

#endif /* not _CLASS_ARRAY_H_ */
