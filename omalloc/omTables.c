/*******************************************************************
 *  File:    omTables.c
 *  Purpose: program which generates omTables.inc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omTables.c,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/

#ifndef MH_TABLE_C
#define MH_TABLES_C

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omConfig.h"
#include "omPrivate.h"

/* Specify the minimal number of blocks which should go into a bin */
#if SIZEOF_SYSTEM_PAGE > 4096
#define MIN_BIN_BLOCKS 8
#define INCR_FACTOR     2
#else
#define MIN_BIN_BLOCKS 4
#define INCR_FACTOR 1
#endif


/* Specify sizes of static bins */
#ifdef OM_ALIGN_8

#define OM_MAX_BLOCK_SIZE ((SIZEOF_OM_BIN_PAGE / MIN_BIN_BLOCKS) / 8)*8
size_t om_BinSize [] = 
{   8,  16,  24,  32,
    40,  48,  56,  64, 72
    80,  96, 112, 128, 144
    160, 192, 224, 
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*9)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*6)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*4)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*2)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR))   / 8)*8,
    OM_MAX_BLOCK_SIZE};

#else /* ! OM_ALIGN_8 */

#define OM_MAX_BLOCK_SIZE ((SIZEOF_OM_BIN_PAGE / MIN_BIN_BLOCKS) / 4)*4

size_t om_BinSize [] = 
{   8,  12,  16,  20,
    24,  28,  32,
    40,  48,  56,  64,
    80,  96, 112, 128,
    160, 192, 224, 
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*9)) / 4)*4,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*6)) / 4)*4,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*4)) / 4)*4,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*2)) / 4)*4,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR))   / 4)*4,
    OM_MAX_BLOCK_SIZE};

#endif /* OM_ALIGN_8 */

void OutputSize2Bin(size_t *binSize, size_t max_block_size)
{
  int i, j;
  printf("omBin om_Size2Bin[/*%d*/] = {\n",
         max_block_size / 4);
  i=0;
  j=SIZEOF_OM_ALIGNMENT;
  while (j < max_block_size)
  {
    printf("&om_StaticBin[%d],\n", i);
    if (binSize[i] == j) i++;
    j += SIZEOF_OM_ALIGNMENT;
  }
  printf("&om_StaticBin[%d]};\n\n", i);
}

void OutputStaticBin(size_t *binSize, int max_bin_index)
{
  int i;
  printf("omBin_t om_StaticBin[/*%d*/] = {\n", max_bin_index+1);

  for (i=0;  i< max_bin_index; i++)
  {
    printf("{om_ZeroPage, NULL, NULL, %d, %d, 0},\n", 
           binSize[i] / SIZEOF_LONG, 
           SIZEOF_OM_BIN_PAGE/binSize[i]);
  }
  printf("{om_ZeroPage, NULL, NULL, %d, %d, 0}\n};\n\n", 
         binSize[i] / SIZEOF_LONG, 
         SIZEOF_OM_BIN_PAGE/binSize[i]);
}
 
int main()
{
  int max_bin_index = 0;

  /* determine max_bin_index */
  for(;;)
  {
    max_bin_index++;
    if (om_BinSize[max_bin_index] == OM_MAX_BLOCK_SIZE) break;
  }
  /* output what goes into omPrivate.h */
  printf("
#ifndef OM_TABLES_INC
#define OM_TABLES_INC

#ifdef OM_PRIVATE_H
#define OM_MAX_BLOCK_SIZE %d
#define OM_MAX_BIN_INDEX %d
#endif /* OM_PRIVATE_H */

#ifdef OM_ALLOC_C
extern omBinPage_t om_ZeroPage[];
",  OM_MAX_BLOCK_SIZE, max_bin_index);
  /* Output om_StaticBin */
  OutputStaticBin(om_BinSize, max_bin_index);
  /* Output om_Size2Bin */
  OutputSize2Bin(om_BinSize, OM_MAX_BLOCK_SIZE);
  
  printf("
#endif /* OM_ALLOC_C */
#endif /* OM_TABLES_INC */
");
  return 0;
}

#endif /* MH_TABLES_C */

