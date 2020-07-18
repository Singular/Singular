/*******************************************************************
 *  File:    omTables.c
 *  Purpose: program which generates omTables.inc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/

#ifndef MH_TABLES_C
#define MH_TABLES_C

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "omalloc/omConfig.h"
#include "omalloc/omDerivedConfig.h"
#include "omalloc/omStructs.h"
#include "omalloc/omAllocPrivate.h"

/* Specify the minimal number of blocks which should go into a bin */
#if SIZEOF_SYSTEM_PAGE > 4096
#define MIN_BIN_BLOCKS 8
#define INCR_FACTOR     2
#else
#define MIN_BIN_BLOCKS 4
#define INCR_FACTOR 1
#endif


#define OM_MAX_BLOCK_SIZE ((SIZEOF_OM_BIN_PAGE / MIN_BIN_BLOCKS) & ~(SIZEOF_STRICT_ALIGNMENT - 1))

/* Specify sizes of static bins */
#ifdef OM_ALIGN_8

size_t om_BinSize [SIZEOF_OM_BIN_PAGE / MIN_BIN_BLOCKS] =
{   8,  16,  24,  32,
    40,  48,  56,  64, 72,
    80,  96, 112, 128, 144,
    160, 192, 224,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*9)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*6)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*4)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*2)) / 8)*8,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR))   / 8)*8,
    OM_MAX_BLOCK_SIZE};

#else /* ! OM_ALIGN_8 */

size_t om_BinSize [SIZEOF_OM_BIN_PAGE / MIN_BIN_BLOCKS] =
{   8,  12,  16,  20,
    24,  28,  32,
    40,  48,  56,  64,
    80,  96, 112, 128,
    160, 192, 224,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*9)) / SIZEOF_STRICT_ALIGNMENT)*SIZEOF_STRICT_ALIGNMENT,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*6)) / SIZEOF_STRICT_ALIGNMENT)*SIZEOF_STRICT_ALIGNMENT,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*4)) / SIZEOF_STRICT_ALIGNMENT)*SIZEOF_STRICT_ALIGNMENT,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR*2)) / SIZEOF_STRICT_ALIGNMENT)*SIZEOF_STRICT_ALIGNMENT,
    ((SIZEOF_OM_BIN_PAGE / (MIN_BIN_BLOCKS + INCR_FACTOR))   / SIZEOF_STRICT_ALIGNMENT)*SIZEOF_STRICT_ALIGNMENT,
    OM_MAX_BLOCK_SIZE};

#endif /* OM_ALIGN_8 */

void OutputSize2Bin(size_t *binSize, size_t max_block_size, int track)
{
  long i, j;
  printf("omBin om_Size2%sBin[/*%ld*/] = {\n",
         (track? "Track" : ""), (long)(max_block_size / SIZEOF_OM_ALIGNMENT));
  i=0;
  j=SIZEOF_OM_ALIGNMENT;
  while (j < max_block_size)
  {
    printf("&om_Static%sBin[%ld], /* %ld */ \n", (track? "Track" : ""), i, j);
    if (binSize[i] == j) i++;
    j += SIZEOF_OM_ALIGNMENT;
  }
  printf("&om_Static%sBin[%ld] /* %ld */};\n\n", (track? "Track" : ""), i, j);
}

void OutputSize2AlignedBin(size_t *binSize, size_t max_block_size, int track)
{
  long i, j;
  if (OM_MAX_BLOCK_SIZE % 8 != 0)
  {
    fprintf(stderr, "OM_MAX_BLOCK_SIZE == %d not divisible by 8\n", OM_MAX_BLOCK_SIZE);fflush(stdout);
    _exit(1);
  }
  printf("omBin om_Size2%sBin[/*%ld*/] = {\n",
         (track ? "Track" : "Aligned"), (long)(max_block_size / SIZEOF_OM_ALIGNMENT));
  i=0;
  while (binSize[i] % SIZEOF_STRICT_ALIGNMENT != 0) i++;
  j=SIZEOF_OM_ALIGNMENT;
  while (j < max_block_size)
  {
    printf("&om_Static%sBin[%ld], /* %ld */ \n", (track ? "Track" : ""), i, j);
    if (binSize[i] == j)
    {
      i++;
      while (binSize[i] % SIZEOF_STRICT_ALIGNMENT != 0) i++;
    }
    j += SIZEOF_OM_ALIGNMENT;
  }
  printf("&om_Static%sBin[%ld] /* %ld */};\n\n", (track ? "Track" : ""), i, j);
}

void OutputStaticBin(size_t *binSize, int max_bin_index, int track)
{
  long i;
  printf("omBin_t om_Static%sBin[/*%d*/] = {\n", (track ? "Track" : ""), max_bin_index+1);

  for (i=0;  i< max_bin_index; i++)
  {
    printf("{om_ZeroPage, NULL, NULL, %ld, %ld, 0},\n",
           (long)(binSize[i] / SIZEOF_LONG),
           (long)(SIZEOF_OM_BIN_PAGE/binSize[i]));
  }
  printf("{om_ZeroPage, NULL, NULL, %ld, %ld, 0}\n};\n\n",
         (long)(binSize[i] / SIZEOF_LONG),
         (long)(SIZEOF_OM_BIN_PAGE/binSize[i]));
}

int GetMaxBlockThreshold()
{
  int i;
  for (i=SIZEOF_OM_ALIGNMENT; i < OM_MAX_BLOCK_SIZE; i += SIZEOF_OM_ALIGNMENT)
  {
    if ((SIZEOF_OM_BIN_PAGE/i) == SIZEOF_OM_BIN_PAGE/(i + SIZEOF_OM_ALIGNMENT))
      return i;
  }
  /* should never get here */
  printf("error");fflush(stdout);
  _exit(1);
}

void CreateDenseBins()
{
  size_t size, align_size = SIZEOF_OM_ALIGNMENT;
  int i = 1;
#ifdef OM_ALIGNMENT_NEEDS_WORK
  int n = GetMaxBlockThreshold();
#endif

  size = align_size;
  om_BinSize[0] = align_size;
  i = 1;
  while (size < OM_MAX_BLOCK_SIZE)
  {
    size += align_size;
#ifdef OM_ALIGNMENT_NEEDS_WORK
    if (size >= n && align_size != SIZEOF_STRICT_ALIGNMENT)
    {
      align_size = SIZEOF_STRICT_ALIGNMENT;
      size= OM_STRICT_ALIGN_SIZE(size);
    }
#endif
    om_BinSize[i] = size;
    if ((SIZEOF_OM_BIN_PAGE / (size + align_size)) < (SIZEOF_OM_BIN_PAGE /size))
    {
      i++;
    }
  }
}

int main(int argc, char* argv[])
{
  int max_bin_index = 0;
  /* determine max_bin_index */
#ifdef OM_HAVE_DENSE_BIN_DISTRIBUTION
  CreateDenseBins();
#endif
  for(;;)
  {
    max_bin_index++;
    if (om_BinSize[max_bin_index] == OM_MAX_BLOCK_SIZE) break;
  }

  printf(
"#ifndef OM_TABLES_INC\n"
"#define OM_TABLES_INC\n"
);

  /* Output om_StaticBin */
  OutputStaticBin(om_BinSize, max_bin_index, 0);
  /* Output om_Size2Bin */
  OutputSize2Bin(om_BinSize, OM_MAX_BLOCK_SIZE, 0);

#ifdef OM_ALIGNMENT_NEEDS_WORK
  OutputSize2AlignedBin(om_BinSize, OM_MAX_BLOCK_SIZE, 0);
#endif

  printf("\n#ifdef OM_HAVE_TRACK\n");
  /* Output om_StaticBin */
  OutputStaticBin(om_BinSize, max_bin_index, 1);
  /* Output om_Size2Bin */
#ifdef OM_ALIGNMENT_NEEDS_WORK
  OutputSize2AlignedBin(om_BinSize, OM_MAX_BLOCK_SIZE, 1);
#else
  OutputSize2Bin(om_BinSize, OM_MAX_BLOCK_SIZE, 1);
#endif
  printf("\n#endif /* OM_HAVE_TRACK */\n");

  printf("\n#endif /* OM_TABLES_INC */\n");
  return 0;
}
#endif /* MH_TABLES_C */
