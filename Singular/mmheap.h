#ifndef MEMHEAP_H
#define MEMHEAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.h,v 1.1 1998-08-05 12:32:47 obachman Exp $ */

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "mod2.h"

// define to enable heap - debugging
// #define HEAP_DEBUG
/*****************************************************************
 *
 * Public Interface
 *
 *****************************************************************/
/* frees all memory associated with heap */
extern void mmh_clear();
/* resets heap, i.e. sets low - mark to the beginning, initializes, if necessary */
extern void mmh_reset();
/* sets mark in heap -- memory allocated from here until mmh_reset()
   is "kept", and not re-used by heap stuff */
extern void mmh_top();

#ifndef HEAP_DEBUG                                                         
/* allocate memory -- ptr should be a simple identifier */                 
#define mmh_alloc(ptr, words)                                               \
do                                                                          \
{                                                                           \
  (void**) ptr =  mmh_current;                                              \
  mmh_current += words;                                                     \
  if (mmh_current >= mmh_current_max) (void**) ptr = mmh_alloc_new(words);  \
}                                                                           \
while(0)
/* Do nevertheless always a free, so that debugging is possible */
#define mmh_free(ptr)   ((void*)0)

/* Alloc and Free for monoms */
#define mmh_alloc_monom(ptr) mmh_alloc(ptr, pMonomSizeW)
#define mmh_free_monom(ptr) ((void*)0)

#else /* HEAP_DEBUG */

#define mmh_alloc(ptr, words) ptr = AllocL(words)
#define mmh_free(ptr)       FreeL(ptr)

#define mmh_alloc_monom(ptr) ptr = pNew()
#define mmh_free_monom(ptr)  pFree1(ptr)

#endif /* HEAP_DEBUG */  


/*****************************************************************
 *
 * Internal stuff 
 *
 *****************************************************************/

/* where we currently stand */
extern void** mmh_current;

/* max value of current */
extern void** mmh_current_max;

/* old page is exceeded, allocate new page */
extern void* mmh_alloc_new(size_t words);

#ifdef __cplusplus
}
#endif

#endif /* MEMHEAP_H */

