#ifndef _CLASS_ARRAYELEMENT_H_
#define _CLASS_ARRAYELEMENT_H_

/* Class Instance */
typedef struct ArrayElement {
  struct ArrayElement **handle;
  void *data;

  /* Instance Methods */
  struct ArrayElementMethods *m;
} ArrayElement;


/* Class Methods */
typedef struct ArrayElementMethods {
  ArrayElement * (*init)(ArrayElement *self, void *data);
  ArrayElement * (*free)(ArrayElement *self);

  int            (*compare)(ArrayElement *self, ArrayElement *other);
  ArrayElement * (*dup)(ArrayElement *self);
} ArrayElementMethods;


#ifndef _CLASS_ARRAYELEMENT_I_H_
extern int            class_ArrayElement(void);
extern ArrayElement * new_ArrayElement(void *data);
extern ArrayElement * alloc_ArrayElement(void);

extern ArrayElement *null_ArrayElement;
#endif /* not _CLASS_ARRAYELEMENT_I_H_ */

#endif /* not _CLASS_ARRAYELEMENT_H_ */
