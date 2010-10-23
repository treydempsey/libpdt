#include "dependencies.h"
#include "class_Array_i.h"

/*
 * Class Methods
 ***************/

int
class_Array(void)
{
  /* Dependencies */
  class_ArrayElement();

  if(null_Array == NULL) {
    /* Methods */
    Array_methods.init = Array_init;
    Array_methods.free = Array_free;

    Array_methods.append = Array_append;
    Array_methods.compare = Array_compare;
    Array_methods.concat = Array_concat;
    Array_methods.difference = Array_difference;
    Array_methods.dup = Array_dup;
    Array_methods.each = Array_each;
    Array_methods.get = Array_get;
    Array_methods.include = Array_include;
    Array_methods.intersection = Array_intersection;
    Array_methods.reset_each = Array_reset_each;
    Array_methods.repetition = Array_repetition;
    Array_methods.set = Array_set;

    /* Special vars */
    null_Array = &_null_Array;
    null_Array->handle = &null_Array;
    null_Array->m = &Array_methods;
    Array_init(null_Array);
  }

  return 1;
}


Array *
new_Array()
{
  Array *self;

  self = alloc_Array();
  self->m->init(self);

  return self;
}


Array *
alloc_Array(void)
{
  Array **handle;

  /* Allocate */
  handle = malloc(sizeof(Array *));
  if(handle == NULL) {
    handle = null_Array->handle;
    goto error;
  }

  *handle = malloc(sizeof(Array));
  if(*handle == NULL) {
    free(handle);
    handle = null_Array->handle;
    goto error;
  }
  memset(*handle, 0, sizeof(Array));
  (*handle)->handle = handle;
  (*handle)->m = &Array_methods;

error:

  if(errno == ENOMEM) {
    perror("alloc_Array()");
  }

  return *handle;
}


static Array *
Array_init(Array *self)
{
  /* Variables */
  self->current_element = null_ArrayElement;
  self->current_index = 0;
  self->chunk_size = ARRAY_CHUNK_SIZE;
  self->chunks = 1;
  self->length = 0;

  if(self != null_Array) {
    self->elements = malloc(self->chunk_size * sizeof(ArrayElement *));
    if(self->elements == NULL) {
      free(self->handle);
      free(self);
      goto error;
    }
    _Array_null_elements(self, 0, _Array_size_elements(self) - 1);
  }
  else {
    self->chunk_size = 1;
    self->elements = null_ArrayElement->handle;
  }


error:

  if(errno == ENOMEM) {
    perror("Array_init()");
  }

  return self;
}


static Array *
Array_free(Array *self)
{
  int i;

  if(self != null_Array) {
    for(i = 0; i < self->length; i++) {
      self->elements[i]->m->free(self->elements[i]);
    }
    free(self->elements);
    free(self->handle);
    free(self);
  }

  return null_Array;
}


/*
 * Public Instance Methods
 *************************/

static Array *
Array_append(Array *self, ArrayElement *element)
{
  if(self != null_Array) {
    if((self->length + 1) >= _Array_size_elements(self)) {
      _Array_extend_elements(self, 1);
    }

    self->elements[self->length] = element;
    self->length += 1;
  }

  return self;
}


static int
Array_compare(Array *self, Array *other)
{
  /* TODO Complete */
  return (self == other) ? 1 : 0;
}


static Array *
Array_concat(Array *self, Array *other)
{
  Array *new_array;

  if(self != null_Array) {
    new_array = self->m->dup(self);

    other->m->reset_each(other);
    while(other->m->each(other) != null_ArrayElement) {
      new_array->m->append(new_array, other->current_element->m->dup(other->current_element));
    }

    return new_array;
  }
  else {
    return null_Array;
  }
}


static Array *
Array_difference(Array *self, Array *other)
{
  Array *new_array;

  if(self != null_Array) {
    new_array = new_Array();

    self->m->reset_each(self);
    while(self->m->each(self) != null_ArrayElement) {
      if(other->m->include(other, self->current_element) == null_ArrayElement) {
        new_array->m->append(new_array, self->current_element->m->dup(self->current_element));
      }
    }

    return new_array;
  }
  else {
    return null_Array;
  }
}


static Array *
Array_dup(Array *self)
{
  Array *new_array;
  new_array = new_Array();

  if(self != null_Array) {
    self->m->reset_each(self);
    while(self->m->each(self) != null_ArrayElement) {
      new_array->m->append(new_array, self->current_element->m->dup(self->current_element));
    }

    return new_array;
  }
  else {
    return null_Array;
  }
}


static ArrayElement *
Array_each(Array *self)
{
  if(self != null_Array) {
    if(self->current_index >= self->length) {
      self->current_index = -1;
      self->current_element = null_ArrayElement;
    }
    else {
      self->current_index++;
      self->current_element = self->elements[self->current_index];
    }
  }

  return self->current_element;
}


static ArrayElement *
Array_get(Array *self, size_t index)
{
  if(self != null_Array) {
    return self->elements[index];
  }
  else {
    return null_ArrayElement;
  }
}


static ArrayElement *
Array_include(Array *self, ArrayElement *element)
{
  if(self != null_Array) {
    self->m->reset_each(self);
    while(self->m->each(self) != null_ArrayElement) {
      if(self->current_element->m->compare(self->current_element, element)) {
        break;
      }
    }
  }

  return self->current_element;
}


static Array *
Array_reset_each(Array *self)
{
  if(self != null_Array) {
    self->current_element = null_ArrayElement;
    self->current_index = -1;
  }

  return self;
}


static Array *
Array_intersection(Array *self, Array *other)
{
  Array *new_array;

  if(self != null_Array) {
    new_array = new_Array();

    self->m->reset_each(self);
    while(self->m->each(self) != null_ArrayElement) {
      if(other->m->include(other, self->current_element) != null_ArrayElement) {
        new_array->m->append(new_array, self->current_element->m->dup(self->current_element));
      }
    }

    return new_array;
  }
  else {
    return null_Array;
  }
}


static Array *
Array_repetition(Array *self, int times)
{
  Array *new_array;
  ArrayElement *element;
  int i, new_length;

  if(self != null_Array) {
    new_array = new_Array();
    new_length = self->length * times;

    self->m->reset_each(self);
    for(i = 0; i < new_length; i++) {
      element = self->m->each(self);
      if(element == null_ArrayElement) {
        element = self->m->each(self);
      }
      new_array->m->append(new_array, element->m->dup(element));
    }

    return new_array;
  }
  else {
    return null_Array;
  }
}


static Array *
Array_set(Array *self, size_t index, ArrayElement *element)
{
  int chunks;

  if(self != null_Array) {
    if(index >= _Array_size_elements(self)) {
      chunks = ((index + 1) / self->chunk_size);
      if(((index + 1) % self->chunk_size) > 0) {
        chunks++;
      }

      _Array_extend_elements(self, chunks - self->chunks);
    }

    if((index - 1) > (self->length - 1)) {
      _Array_null_elements(self, (self->length - 1), (index - 1));
    }

    *(self->elements + index) = element;
  }

  return self;
}


/* 
 * Private Methods
 *****************/

static inline size_t
_Array_size_elements(Array *self)
{
  return self->chunks * self->chunk_size;
}


static Array *
_Array_extend_elements(Array *self, int add_chunks)
{
  ArrayElement **more_elements;
  int old_chunks;

  if(self != null_Array) {
    old_chunks = self->chunks;
    self->chunks += add_chunks;
    /* TODO Use realloc() if we've got it */
    more_elements = malloc(self->chunks * self->chunk_size * sizeof(ArrayElement *));
    if(more_elements == NULL) {
      goto error;
    }
    free(self->elements);
    memset(more_elements, 0, self->chunks * self->chunk_size * sizeof(ArrayElement *));
    memcpy(more_elements, self->elements, old_chunks * self->chunk_size * sizeof(ArrayElement *));

    self->elements = more_elements;

    _Array_null_elements(self, self->length, _Array_size_elements(self) - 1);

  error:

    if(errno == ENOMEM) {
      perror("_Array_extend_elements()");
    }
  }

  return self;
}


static Array *
_Array_null_elements(Array *self, size_t start, size_t end)
{
  int i, j;

  if(self != null_Array) {
    j = _Array_size_elements(self);
    j = (end < j) ? end : j - 1;

    for(i = start; i <= j; i++) {
      *(self->elements + i) = null_ArrayElement;
    }
  }

  return self;
}
