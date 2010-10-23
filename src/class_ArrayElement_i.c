#include "dependencies.h"
#include "class_ArrayElement_i.h"

/*
 * Class Methods
 ***************/

int
class_ArrayElement(void)
{
  if(null_ArrayElement == NULL) {
    /* Methods */
    ArrayElement_methods.init = ArrayElement_init;
    ArrayElement_methods.free = ArrayElement_free;

    ArrayElement_methods.compare = ArrayElement_compare;
    ArrayElement_methods.dup = ArrayElement_dup;

    /* Null ArrayElement Instance */
    null_ArrayElement = &_null_ArrayElement;
    null_ArrayElement->handle = &null_ArrayElement;
    null_ArrayElement->m = &ArrayElement_methods;
    ArrayElement_init(null_ArrayElement, NULL);
  }

  return 1;
}


ArrayElement *
new_ArrayElement(void *data)
{
  ArrayElement *self;

  self = alloc_ArrayElement();
  self->m->init(self, data);

  return self;
}


ArrayElement *
alloc_ArrayElement(void)
{
  ArrayElement **handle;

  /* Allocate */
  handle = malloc(sizeof(ArrayElement *));
  if(handle == NULL) {
    handle = null_ArrayElement->handle;
    goto error;
  }

  *handle = malloc(sizeof(ArrayElement));
  if(*handle == NULL) {
    free(handle);
    handle = null_ArrayElement->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(ArrayElement));
  (*handle)->handle = handle;
  (*handle)->m = &ArrayElement_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_ArrayElement()");
  }

  return *handle;
}


static ArrayElement *
ArrayElement_init(ArrayElement *self, void *data)
{
  /* Variables */
  self->data = data;

  return self;
}


static ArrayElement *
ArrayElement_free(ArrayElement *self)
{
  if(self != null_ArrayElement) {
    free(self->handle);
    free(self);
  }

  return null_ArrayElement;
}


/*
 * Public Instance Methods
 *************************/

static int
ArrayElement_compare(ArrayElement *self, ArrayElement *other)
{
  return (self->data == other->data);
}


static ArrayElement *
ArrayElement_dup(ArrayElement *self)
{
  ArrayElement *new_array_element;

  new_array_element = new_ArrayElement(self->data);

  return new_array_element;
}
