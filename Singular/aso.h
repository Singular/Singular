/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  aso.h:  Header file for generation of ALLOC_SIZE_OF_ macros
 *  
 *  Version: $Id: aso.h,v 1.1 1999-10-14 14:46:48 obachman Exp $
 *******************************************************************/
#ifndef ASO_H
#define ASO_H

/* define HAVE_ASO to 0, which means ASO generation */
#undef HAVE_ASO
#define HAVE_ASO 0

#include <stdio.h>
#include "mod2.h"
#include "mmtables.inc"

#define ASO(x)                                                                                                    \
do                                                                                                                \
{                                                                                                                 \
  int index = mmGetIndex(sizeof(x));                                                                              \
  if (index < 0)                                                                                                  \
  {                                                                                                               \
    fprintf(stderr, "sizeof(%s) == %d larger than MAX_BLOCK_SIZE\n",                                              \
            #x, sizeof(x));                                                                                       \
      exit(1);                                                                                                    \
  }                                                                                                               \
  printf("#define ALLOC_SIZE_OF_%s      AllocHeapSizeOf(&mm_theList[%d], sizeof(%s))\n", #x, index, #x);           \
  printf("#define ALLOC0_SIZE_OF_%s     Alloc0HeapSizeOf(&mm_theList[%d], sizeof(%s))\n", #x, index, #x);         \
  printf("#define FREE_SIZE_OF_%s(addr) FreeHeapSizeOf(addr, &mm_theList[%d], sizeof(%s))\n\n", #x, index, #x); \
}                                                                                                                 \
while (0)

#endif /* ASO_H  */
