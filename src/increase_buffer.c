
static int
csv_increase_buffer(struct csv_parser *p)
{
  /* Increase the size of the entry buffer.  Attempt to increase size by 
   * p->blk_size, if this is larger than SIZE_MAX try to increase current
   * buffer size to SIZE_MAX.  If allocation fails, try to allocate halve 
   * the size and try again until successful or increment size is zero.
   */

  size_t to_add = p->blk_size;
  void *vp;

  if ( p->entry_size >= SIZE_MAX - to_add )
    to_add = SIZE_MAX - p->entry_size;

  if (!to_add) {
    p->status = CSV_ETOOBIG;
    return -1;
  }

  while ((vp = p->realloc_func(p->entry_buf, p->entry_size + to_add)) == NULL) {
    to_add /= 2;
    if (!to_add) {
      p->status = CSV_ENOMEM;
      return -1;
    }
  }

  /* Update entry buffer pointer and entry_size if successful */
  p->entry_buf = vp;
  p->entry_size += to_add;
  return 0;
}



