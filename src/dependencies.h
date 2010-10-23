#ifndef _DEPENDENCIES_H_
#define _DEPENDENCIES_H_

#if ___STDC_VERSION__ >= 199901L
#  include <stdint.h>
#else
#  define SIZE_MAX ((size_t)-1) /* C89 doesn't have stdint.h or SIZE_MAX */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#endif /* not _DEPENDENCIES_H_ */
