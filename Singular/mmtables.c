/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmtables.c,v 1.7 1999-11-15 17:20:26 obachman Exp $ */

/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#ifdef GENERATE_INDEX_TABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod2.h"
#include "mmemory.h"
#include "mmprivate.h"
#include "mmpage.h"

#define MAX_INDEX ((MAX_BLOCK_SIZE - 1) >> 2)

#ifdef ALIGN_8

/* make sure these are all divisible by 8 */
#define HALF_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 16)*8)
#define QUARTER_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 32)*8)
#define EIGHTS_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 64)*8)

const size_t mm_mcbSizesAlign [] = 
{   8,  16,  24,  32,
    40,  48,  56,  64,
    80,  96, 112, 128,
    160, 192, 224, 
    QUARTER_MAX_BLOCK_SIZE,
    QUARTER_MAX_BLOCK_SIZE + EIGHTS_MAX_BLOCK_SIZE,
    HALF_MAX_BLOCK_SIZE, 
    HALF_MAX_BLOCK_SIZE + EIGHTS_MAX_BLOCK_SIZE,
    HALF_MAX_BLOCK_SIZE + QUARTER_MAX_BLOCK_SIZE,
    HALF_MAX_BLOCK_SIZE + QUARTER_MAX_BLOCK_SIZE + EIGHTS_MAX_BLOCK_SIZE,
    MAX_BLOCK_SIZE};

#else

#define HALF_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 8)*4)
#define QUARTER_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 16)*4)
#define EIGHTS_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 32)*4)

const size_t mm_mcbSizesAlign [] = 
{   8,  12,  16,  20,
    24,  28,  32,
    40,  48,  56,  64,
    80,  96, 112, 128,
    160, 192, 224, 
    QUARTER_MAX_BLOCK_SIZE,
    QUARTER_MAX_BLOCK_SIZE + EIGHTS_MAX_BLOCK_SIZE,
    HALF_MAX_BLOCK_SIZE, 
    HALF_MAX_BLOCK_SIZE + EIGHTS_MAX_BLOCK_SIZE,
    HALF_MAX_BLOCK_SIZE + QUARTER_MAX_BLOCK_SIZE,
    HALF_MAX_BLOCK_SIZE + QUARTER_MAX_BLOCK_SIZE + EIGHTS_MAX_BLOCK_SIZE,
    MAX_BLOCK_SIZE};

#endif


int GetIndex(size_t size, const size_t* size_table)
{
  int i;

  if (size > MAX_BLOCK_SIZE)
  {
    printf("GetIndex: %d size is too large\n", size);
    exit(1);
  }

  for (i=0; size_table[i] < size; i++)
  {};

  return i;
}


void mmInitIndexTable(char* index_table, const size_t* size_table)
{
  unsigned int i;

  /* Fill in IndexTable */
  for (i=0; i<=MAX_INDEX; i++)
    index_table[i] = GetIndex((size_t) i*4 + 1, size_table);
}

void OutputIndexTable(char* index_table)
{
  int i;
  int l=0;
  printf("const char mm_IndexTable[/*%d*/] = {\n",MAX_INDEX+1);
  for (i=0;i<MAX_INDEX; i++,l++)
  {
    printf("%d,", index_table[i]);
    if (l==15) { printf("\n"); l=0; }
  }
  printf("%d};\n", index_table[MAX_INDEX]);
}

void OutputTheListTable(const size_t *mm_mcbSizes)
{
  int i;
  printf("struct sip_memHeap mm_theList[] = {\n");
  printf("#ifndef MDEBUG\n");
  for (i=0;  mm_mcbSizes[i] < MAX_BLOCK_SIZE; i++)
  {
#ifndef HAVE_AUTOMATIC_GC
    printf("{NULL, NULL, NULL, %d},\n", mm_mcbSizes[i]);
#else
    printf("{mmZeroPage, NULL, %d, %d},\n", mm_mcbSizes[i], 
           SIZE_OF_HEAP_PAGE/mm_mcbSizes[i]);
#endif
  }
#ifndef HAVE_AUTOMATIC_GC
    printf("{NULL, NULL, NULL, %d}\n};\n", mm_mcbSizes[i]);
#else
    printf("{mmZeroPage, NULL, %d, %d}\n};\n", mm_mcbSizes[i], 
           SIZE_OF_HEAP_PAGE/mm_mcbSizes[i]);
#endif
  printf("#else /* MDEBUG */\n");
  for (i=0; mm_mcbSizes[i] < MAX_BLOCK_SIZE; i++)
  {
#ifndef HAVE_AUTOMATIC_GC
    printf("{NULL, NULL, NULL, %d},\n", RealSizeFromSize(mm_mcbSizes[i]));
#else
    printf("{mmZeroPage, NULL, %d, %d},\n", RealSizeFromSize(mm_mcbSizes[i]), 
           SIZE_OF_HEAP_PAGE/RealSizeFromSize(mm_mcbSizes[i]));
#endif
  }
#ifndef HAVE_AUTOMATIC_GC
  printf("{NULL, NULL, NULL, %d}\n};\n", RealSizeFromSize(mm_mcbSizes[i]));
#else
  printf("{mmZeroPage, NULL, %d, %d}\n};\n", RealSizeFromSize(mm_mcbSizes[i]), 
           SIZE_OF_HEAP_PAGE/RealSizeFromSize(mm_mcbSizes[i]));
#endif
  printf("#endif /* ! MDEBUG */\n");
}
 
void OutputSizeTable( const size_t *mm_mcbSizes)
{
  int i;
  printf("const size_t mm_mcbSizes[] = {");
  for (i=0; mm_mcbSizes[i] < MAX_BLOCK_SIZE; i++)
    printf("%d, ",  mm_mcbSizes[i]);
  printf("%d};\n",  mm_mcbSizes[i]);
}
  
int main()
{
  char mm_IndiciesAlign[MAX_INDEX + 1];
  mmInitIndexTable(mm_IndiciesAlign, mm_mcbSizesAlign);

  printf("#ifndef MM_TABLES_INC\n#define MM_TABLES_INC\n");
  printf("
#include \"mod2.h\"
#include \"mmemory.h\"
#include \"mmprivate.h\"
");
  printf("\n\n");
#ifdef HAVE_AUTOMATIC_GC
  printf("struct sip_memHeapPage mmZeroPage[] = {{0, NULL, NULL, NULL}};\n");
  printf("\n");
#endif
  OutputIndexTable(mm_IndiciesAlign);
  printf("\n");
  OutputSizeTable(mm_mcbSizesAlign);
  printf("\n");
  OutputTheListTable(mm_mcbSizesAlign);
  printf("\n");
  printf("#endif /* MM_TABLES_INC*/\n");
  return 0;
}

#else /* ! GENERATE_INDEX_TABLE */

#include "mod2.h"
#include "mmemory.h"
#include "mmprivate.h"
#include "mmtables.inc"

#endif /* GENERATE_INDEX_TABLE */

