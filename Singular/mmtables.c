/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmtables.c,v 1.4 1999-10-18 11:19:30 obachman Exp $ */

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
#define HALF_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 8)*4)
#define QUARTER_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 16)*4)
#define EIGHTS_MAX_BLOCK_SIZE (((MAX_BLOCK_SIZE) / 32)*4)

const size_t mm_mcbSizesAlign8 [] = 
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
const size_t mm_mcbSizesAlign4 [] = 
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

char mm_IndiciesAlign8[MAX_INDEX + 1];
char mm_IndiciesAlign4[MAX_INDEX + 1];

int mmGetBinaryIndexAlign8( size_t size )
{
  int j;

  if (size<81)
  {
    if (size<41)
    {
      if (size<25)
      {
        if(size<17)
        {
          if (size<9) j= 0;
          else        j= 1;
        }
        else /*17..24*/
        {
          j= 2;
        }
      }
      else/*25..40*/
      {
        if(size<33)
        {
          j= 3;
        }
        else /*33..40*/
        {
          j= 4;
        }
      }
    }
    else/*41..80*/
    {
      if(size<57)
      {
        if (size<49) j= 5;
        else         j= 6;
      }
      else /*57..80*/
      {
        if (size<65) j= 7;
        else         j= 8;
      }
    }
  }
  else/*81..1024*/
  {
    if(size<161)
    {
      if (size<113)
      {
        if (size<97) j= 9;
        else         j= 10;
      }
      else /*113..160*/
      {
        if (size<129) j= 11;
        else          j= 12;
      }
    }
    else/*161..1024*/
    {
      if (size<257)
      {
        if (size<225)
        {
          if (size<193)  j= 13;
          else           j= 14;
        }
        else             j= 15;
      }
      else/*257..1024*/
      {
        if(size<=MAX_BLOCK_SIZE)
        {
          if(size<=HALF_MAX_BLOCK_SIZE) j= 16;
          else         j= 17;
        }
        else
          j= -1;
      }
    }
  }
  return j;
}

int mmGetBinaryIndex( size_t size )
{
  int j;
  if (size<161)
  {
    if (size<41)
    {
      if (size<21)
      {
        if(size<13)
        {
          if (size<9) j= 0;
          else        j= 1;
        }
        else /*13..20*/
        {
          if (size<17) j= 2;
          else         j= 3;
        }
      }
      else/*21..40*/
      {
        if(size<29)
        {
          if (size<25) j= 4;
          else         j= 5;
        }
        else /*29..40*/
        {
          if (size<33) j= 6;
          else         j= 7;
        }
      }
    }
    else/*41..160*/
    {
      if (size<81)
      {
        if(size<57)
        {
          if (size<49) j= 8;
          else         j= 9;
        }
        else /*57..80*/
        {
          if (size<65) j= 10;
          else         j= 11;
        }
      }
      else/*81..160*/
      {
        if(size<113)
        {
          if (size<97) j= 12;
          else         j= 13;
        }
        else /*113..160*/
        {
          if (size<129) j= 14;
          else          j= 15;
        }
      }
    }
  }
  else/*161..1024*/
  {
    if (size<257)
    {
      if (size<225)
      {
        if (size<193)  j= 16;
        else           j= 17;
      }
      else             j= 18;
    }
    else/*257..1024*/
    {
      if(size<=MAX_BLOCK_SIZE)
      {
        if(size<=HALF_MAX_BLOCK_SIZE) j= 19;
        else         j= 20;
      }
      else
        j= -1;
    }
  }
  return j;
}

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


void mmInitIndexTable(char* index_table, const size_t* size_table,
                      int (*GetIndexFnc)(size_t size))
{
  unsigned int i;

  /* Fill in IndexTable */
  for (i=0; i<=MAX_INDEX; i++)
    index_table[i] = GetIndex((size_t) i*4 + 1, size_table);

  /* Check what we did */
  if (GetIndexFnc != NULL)
  {
    for (i=1; i<=MAX_BLOCK_SIZE + 5; i++)
    {
      unsigned int t_index, b_index;
      t_index = (i <= MAX_BLOCK_SIZE ? index_table[(i-1) >> 2] : -1);
      b_index = GetIndexFnc(i);

      if (t_index != b_index)
      {
        fprintf(stderr, "Error in mmInitIndexTable for size %d\n", i);
        GetIndexFnc(i);
        exit(1);
      }
    }
  }
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

#if 0
void OutputIndexTable(char* index_table)
{
  int i = -1;
  int l= -1;
  printf("const char mm_IndexTable[] = {\n");
  do
  {
    i++;
    l++;
    printf("%d,", index_table[i]);
    if (l==15) { printf("\n"); l=0; }
  }
  while (mmGetSize(i) < MAX_BLOCK_SIZE)
  printf("%d};\n", index_table[i]);
}
#endif

void OutputTheListTable(const size_t *mm_mcbSizes)
{
  int i;
  printf("struct sip_memHeap mm_theList[] = {\n");
  printf("#ifndef MDEBUG\n");
  for (i=0;  mm_mcbSizes[i] < MAX_BLOCK_SIZE; i++)
    printf("{NULL, NULL, NULL, %d},\n", mm_mcbSizes[i]);
  printf("{NULL, NULL, NULL, %d}\n};\n", mm_mcbSizes[i]);
  printf("#else /* MDEBUG */\n");
  for (i=0; mm_mcbSizes[i] < MAX_BLOCK_SIZE; i++)
    printf("{NULL, NULL, NULL, %d},\n", RealSizeFromSize(mm_mcbSizes[i]));
  printf("{NULL, NULL, NULL, %d}\n};\n", RealSizeFromSize(mm_mcbSizes[i]));
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
#if 0
  mmInitIndexTable(mm_IndiciesAlign8, mm_mcbSizesAlign8,
                   mmGetBinaryIndexAlign8);
  mmInitIndexTable(mm_IndiciesAlign4, mm_mcbSizesAlign4,
                   mmGetBinaryIndex);
#else
  mmInitIndexTable(mm_IndiciesAlign8, mm_mcbSizesAlign8,
                   NULL);
  mmInitIndexTable(mm_IndiciesAlign4, mm_mcbSizesAlign4,
                   NULL);
#endif

  printf("#ifndef MM_TABLES_INC\n#define MM_TABLES_INC\n");
  printf("
#include \"mod2.h\"
#include \"mmemory.h\"
#include \"mmprivate.h\"
");
  printf("#ifdef ALIGN_8\n");
  OutputIndexTable(mm_IndiciesAlign8);
  OutputSizeTable(mm_mcbSizesAlign8);
  OutputTheListTable(mm_mcbSizesAlign8);
  printf("#else /* ! ALIGN_8 */\n");
  OutputIndexTable(mm_IndiciesAlign4);
  OutputSizeTable(mm_mcbSizesAlign4);
  OutputTheListTable(mm_mcbSizesAlign4);
  printf("#endif /* ALIGN_8 */\n");
  printf("#endif /* MM_TABLES_INC*/\n");
  return 0;
}

#else /* ! GENERATE_INDEX_TABLE */

#include "mod2.h"
#include "mmemory.h"
#include "mmprivate.h"
#include "mmtables.inc"

#endif /* GENERATE_INDEX_TABLE */

