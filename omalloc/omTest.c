#include "omConfig.h"
#include "omPrivate.h"
#include "omDebug.h"
#include "omLocal.h"
#include "omList.h"
#include "omTrack.h"
#include "omFindExec.h"

#define MAX_SIZE 1024
#define D_LEVEL 10

struct TestAddr_s;
typedef struct TestAddr_s TestAddr_t;
typedef TestAddr_t* TestAddr;

struct TestAddr_s
{
  TestAddr next;
  size_t   size;
};

TestAddr bin_addr_list[MAX_SIZE >> 2];
TestAddr block_addr_list[MAX_SIZE >> 2];
TestAddr chunk_addr_list[MAX_SIZE >> 2];
omBin    bin_list[MAX_SIZE >> 2];

void InitLists()
{
  int i;
  for (i=0; i<MAX_SIZE >> 2; i++)
  {
    bin_addr_list[i] = NULL;
    block_addr_list[i] = NULL;
    chunk_addr_list[i] = NULL;
/*    bin_list[i] = omGetSpecHeap(i*4); */
  }
}
  
#if D_LEVEL == 0
#define ALLOC_CHUNK(size)       _omAllocChunk(size)
#define ALLOC_BLOCK(size)       _omAllocBlock(size)
#define ALLOC_BIN(size)         _omAllocBin(omGetSpecBin(size))
#define FREE_BIN(addr)          _omFreeBin(addr)
#define FREE_BLOCK(addr, size)  _omFreeBlock(addr, size)
#define FREE_CHUNK(addr)        _omFreeChunk(addr)
#define omPrintBinStats(fd)     ((void)0)
#elif D_LEVEL > 0
#define ALLOC_CHUNK(size)       omdCheckAllocChunk(size, 0, D_LEVEL)
#define ALLOC_BLOCK(size)       omdCheckAllocBlock(size, 0, D_LEVEL)
#define ALLOC_BIN(size)         omdCheckAllocBin(omGetSpecBin(size), 0, D_LEVEL)
#define FREE_BIN(addr)          omdCheckFreeBin(addr, D_LEVEL)
#define FREE_BLOCK(addr, size)  omdCheckFreeBlock(addr, size, D_LEVEL)
#define FREE_CHUNK(addr)        omdCheckFreeChunk(addr, D_LEVEL)
#else
#define ALLOC_CHUNK(size)       malloc(size)
#define ALLOC_BLOCK(size)       malloc(size)
#define ALLOC_BIN(size)         malloc(size)
#define FREE_BIN(addr)          free(addr)
#define FREE_BLOCK(addr, size)  free(addr)
#define FREE_CHUNK(addr)        free(addr)
#define omPrintBinStats(fd)     ((void)0)
#endif

#if 0
void TestAddrs()
{
  TestAddr addr;
  
  addr = bin_addr_list;
  while (addr != NULL)
  {
    omdCheckBinAddr(addr, omGetSpecBin(addr->size), 2);
    if (omIsOnList(addr->next, addr)) omError("addr on own list");
    if (omIsOnList(block_addr_list, addr)) omError("addr on block_list");
    if (omIsOnList(chunk_addr_list, addr)) omError("addr on chunck_list");
    addr = addr->next;
  }
  addr = block_addr_list;
  while (addr != NULL)
  {
    omdCheckBlockAddr(addr, addr->size, 2);
    if (omIsOnList(addr->next, addr)) omError("addr on own list");
    if (omIsOnList(chunk_addr_list, addr)) omError("addr on chunck_list");
    addr = addr->next;
  }
  addr = chunk_addr_list;
  while (addr != NULL)
  {
    omdCheckChunkAddr(addr, 2);
    if (omIsOnList(addr->next, addr)) omError("addr on own list");
    addr = addr->next;
  }
}
#else
#define TestAddrs() ((void)0)
#endif
  
  
void TestAllocChunk(size_t size)
{
  TestAddr addr = 
    (TestAddr) ALLOC_CHUNK(size);
  addr->size = size;
  addr->next = chunk_addr_list[size >> 2];
  chunk_addr_list[size >> 2] = addr;
}

void TestFreeChunk(size_t size)
{
  TestAddr addr = chunk_addr_list[size >> 2];
  TestAddr a2;
  if (addr != NULL) 
  {
    if (addr->next != NULL)
    {
      while (addr->next->next != NULL) addr = addr->next;
      a2 = addr->next;
      addr->next = NULL;
      FREE_CHUNK((void*)a2);
    }
    else
    {
      FREE_CHUNK(addr);
      chunk_addr_list[size >> 2] = NULL;
    }
  }
}

void FreeAllChunk()
{
  int i;
  for (i=0; i<MAX_SIZE >> 2; i++)
  {
    while (chunk_addr_list[i] != NULL) TestFreeChunk(i*4);
  }
}

void TestAllAddrs()
{
  int i;
  TestAddr addr;
  for (i=0; i<MAX_SIZE >> 2; i++)
  {
    addr = chunk_addr_list[i];
    while (addr != NULL)
    {
      omdCheckChunkAddr(addr, D_LEVEL);
      addr = addr->next;
    }
  }
}
  
  
#if 0
void TestAllocBlock(size_t size)
{
  TestAddr addr = 
    (TestAddr) ALLOC_BLOCK(size);
  addr->size = size;
  block_addr_list = omInsertInSortedList(block_addr_list, size, addr);
}

void TestFreeBlock(size_t size)
{
  TestAddr addr = omFindInSortedList(block_addr_list, size, size);
  if (addr != NULL) 
  {
    block_addr_list = omRemoveFromList(block_addr_list, addr);
    FREE_BLOCK(addr, addr->size);
  }
}

void FreeAllBlock()
{
  TestAddr addr;
  while (block_addr_list != NULL)
  {
    addr = block_addr_list;
    block_addr_list = block_addr_list->next;
    FREE_BLOCK(addr, addr->size);
  }
}

void TestAllocBin(size_t size)
{
  TestAddr addr = 
    (TestAddr) ALLOC_BIN(size);
  addr->size = size;
  bin_addr_list = omInsertInSortedList(bin_addr_list, size, addr);
}

void TestFreeBin(size_t size)
{
  TestAddr addr = omFindInSortedList(bin_addr_list, size, size);
  if (addr != NULL) 
  {
    bin_addr_list = omRemoveFromList(bin_addr_list, addr);
    FREE_BIN(addr);
  }
}
  
void FreeAllBin()
{
  TestAddr addr;
  while (bin_addr_list != NULL)
  {
    addr = bin_addr_list;
    bin_addr_list = bin_addr_list->next;
    FREE_BIN(addr);
  }
}
#else
#define TestAllocBin(p) TestAllocChunk(p)
#define TestFreeBin(p)  TestFreeChunk(p)
#define FreeAllBin() FreeAllChunk()

#define TestAllocBlock(p) TestAllocChunk(p)
#define TestFreeBlock(p)  TestFreeChunk(p)
#define FreeAllBlock() FreeAllChunk()
#endif 


int InitSizeGen(int i)
{
  int size;
  srand(1);
  size = 1 + (int) ( ((double) MAX_SIZE)* rand()/(RAND_MAX + 1.0));
  if (size < 8) return 8;
  return size;
}

int NextSizeGen(int prev)
{
  int size;
  size = 1 + (int) ( ((double) MAX_SIZE) * rand()/(RAND_MAX + 1.0));
#if D_LEVEL > 4
  printf("%d:", size);
#endif
  fflush(stdout);
  if (size < 8) return 8;
  return size;
}

void PrintTest()
{
  omError("sub");
}

int main(int argc, char* argv[])
{
  int limit, i, size, free_all;
  omInitTrack(argv[0]);
#if 0
  void *a1, *a2, *a3, *a4, *a5;
  omBin bin = omGetSpecBin(5000);
  
  
  a1 = omdCheckAllocBin(bin, 0, D_LEVEL);
  a2 = omdCheckAllocBin(bin, 0, D_LEVEL);
  a3 = omdCheckAllocBin(bin, 0, D_LEVEL);
  a4 = omdCheckAllocBin(bin, 0, D_LEVEL);
  a5 = omdCheckAllocBin(bin, 0, D_LEVEL);
  
  omPrintBinStats(stdout);
  omdCheckFreeBin(a1, bin, D_LEVEL);
  omPrintBinStats(stdout);
  omdCheckFreeBin(a5, bin, D_LEVEL);
  omPrintBinStats(stdout);
  omdCheckFreeBin(a2, bin, D_LEVEL);
  omPrintBinStats(stdout);
  omdCheckFreeBin(a3, bin, D_LEVEL);
  omPrintBinStats(stdout);
  omdCheckFreeBin(a4, bin, D_LEVEL);
  omPrintBinStats(stdout);

  return(0);
#endif
  InitLists();
  if (argc > 1) 
    sscanf(argv[1], "%d", &limit);
  else
    limit = 200;

  if (argc > 2)
    sscanf(argv[2], "%d", &free_all);
  else
    free_all = 10000;
  
  size = InitSizeGen(limit);
  i = 0;
  
  for (i=1; i<= limit; i++)
  {
    size = NextSizeGen(size);
    TestAllocBin(size);
    TestAddrs();
    TestAllocBlock(size);
    TestAddrs();
    TestAllocChunk(size);
    TestAddrs();
    size = NextSizeGen(size);
    TestAllocBin(size);
    TestAddrs();
    TestAllocBlock(size);
    TestAddrs();
    TestAllocChunk(size);
    TestAddrs();
    TestFreeBin(size);
    TestAddrs();
    TestFreeBlock(size);
    TestAddrs();
    TestFreeChunk(size);
    TestAddrs();
    
    if (i % 300 == 0)
    {
      omdCheckBins(D_LEVEL);
      TestAllAddrs();
      omDeleteStickyAllBinTag(1);
      printf("DeleteSticky\n");
      omdCheckBins(D_LEVEL);
      TestAllAddrs();
    }
    else if (i % 200 == 0)
    {
      omdCheckBins(D_LEVEL);
      TestAllAddrs();
      omUnSetStickyAllBinTag(1);
      printf("UnSetSticky");
      TestAllAddrs();
      omdCheckBins(D_LEVEL);
    }
    else if (i % 100 == 0)
    {
      omdCheckBins(D_LEVEL);
      TestAllAddrs();
      omSetStickyAllBinTag(1);
      printf("SetSticky");
      omdCheckBins(D_LEVEL);
      TestAllAddrs();
    }
    
    if (i % 100 == 0)
    {
      printf("i=%d\n",i);
      omPrintBinStats(stdout);
      if (i % free_all == 0)
      {
        printf("\nFreeAllChunk\n");
        FreeAllChunk();
#if 0
        omPrintBinStats(stdout);
        if (i % 20000 == 0)
        {
          printf("\nFreeAllBlock\n");
          FreeAllBlock();
          omPrintBinStats(stdout);
          if (i % 40000 == 0)
          {
            printf("\nFreeAllBin\n");
            FreeAllBin();
            omPrintBinStats(stdout);
          }
        }
#endif
      }
    }
  }
  omPrintBinStats(stdout);
  FreeAllBlock();
  omPrintBinStats(stdout);
  FreeAllChunk();
  omPrintBinStats(stdout);
  FreeAllBin();
  omPrintBinStats(stdout);
  return 0;
}

