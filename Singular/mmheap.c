/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmheap.c,v 1.5 1998-08-05 12:32:46 obachman Exp $ */

#include "mod2.h"
#include "mmheap.h"

#define HEAP_SIZE 4092
#define HEAP_SIZE_W 1023

/* Define malloc/free function used for allocating/freeing pages */
#define RAW_ALLOC   malloc
#define RAW_FREE    free

/**********************************************************
 * Global Variables 
 */

/* where we currently stand */
void** mmh_current = NULL;
/* max value of current */
void** mmh_current_max = NULL;
/* pointer to first heap */
static void** mmh_first_heap = NULL;
/* pointer to current heap */
static void** mmh_current_heap = NULL;
/**********************************************************
 * Implementation of the routines
 */

void mmh_clear()
{
  void** current = mmh_first_heap;
  void** temp;
  
  if (current != NULL)
  {
    while (*current != NULL)
    {
      temp = *current;
      RAW_FREE(current);
      current = temp;
    }
    free(current);
  }
  mmh_first_heap = NULL;
  mmh_current_heap = NULL;
  mmh_current = NULL;
  mmh_current_max = NULL;
}

void mmh_reset()
{

  if (mmh_first_heap == NULL)
  {
    mmh_first_heap = RAW_ALLOC(HEAP_SIZE);
    *mmh_first_heap = NULL;
  }
  mmh_current_heap = mmh_first_heap;
  mmh_current = mmh_first_heap + 1;
  mmh_current_max = mmh_first_heap + HEAP_SIZE_W;
}

void* mmh_alloc_new(size_t words)
{
  void** tmp = *mmh_current_heap;
  
  if (tmp == NULL)
  {
    tmp = RAW_ALLOC(HEAP_SIZE);
    *tmp = NULL;
    *mmh_current_heap = tmp;
  }
  
  mmh_current_heap = tmp;
  mmh_current_max = tmp + HEAP_SIZE_W;
  mmh_current = tmp + 1 + words;
  return (tmp + 1);
}

  
  
  
    



