#include "omConfig.h"
#include "omPrivate.h"
#include "omDebug.h"
#include "omLocal.h"
#include "omList.h"
#include "omTrack.h"
#include "omFindExec.h"

struct TestAddr_s;
typedef struct TestAddr_s TestAddr_t;
typedef TestAddr_t* TestAddr;

struct TestAddr_s
{
  TestAddr next;
  size_t   size;
};

#define D_LEVEL 10
TestAddr bin_addr_list = NULL;
TestAddr block_addr_list = NULL;
TestAddr chunk_addr_list = NULL;

void TestAllocChunk(size_t size)
{
  TestAddr addr = 
    (TestAddr) omdCheckAllocChunk(size, 0, D_LEVEL, __FILE__, __LINE__);
  addr->size = size;
  chunk_addr_list = omInsertInSortedList(chunk_addr_list, size, addr);
}

void TestFreeChunk(size_t size)
{
  TestAddr addr = omFindInSortedList(chunk_addr_list, size, size);
  if (addr != NULL) 
    chunk_addr_list = omRemoveFromList(chunk_addr_list, addr);
  omdCheckFreeChunk(addr, D_LEVEL, __FILE__, __LINE__);
}

void FreeAllChunk()
{
  TestAddr addr;
  while (chunk_addr_list != NULL)
  {
    addr = chunk_addr_list;
    chunk_addr_list = chunk_addr_list->next;
    omdCheckFreeChunk(addr, D_LEVEL, __FILE__, __LINE__);
  }
}

void TestAllocBlock(size_t size)
{
  TestAddr addr = 
    (TestAddr) omdCheckAllocBlock(size, 0, D_LEVEL, __FILE__, __LINE__);
  addr->size = size;
  block_addr_list = omInsertInSortedList(block_addr_list, size, addr);
}

void TestFreeBlock(size_t size)
{
  TestAddr addr = omFindInSortedList(block_addr_list, size, size);
  if (addr != NULL) 
    block_addr_list = omRemoveFromList(block_addr_list, addr);
  omdCheckFreeBlock(addr, addr->size,D_LEVEL, __FILE__, __LINE__);
}

void FreeAllBlock()
{
  TestAddr addr;
  while (block_addr_list != NULL)
  {
    addr = block_addr_list;
    block_addr_list = block_addr_list->next;
    omdCheckFreeBlock(addr, addr->size, D_LEVEL, __FILE__, __LINE__);
  }
}

void TestAllocBin(size_t size)
{
  omBin bin = omGetSpecBin(size<5?5:size);
  TestAddr addr = 
    (TestAddr) omdCheckAllocBin(bin, 0, D_LEVEL, __FILE__, __LINE__);
  addr->size = size;
  bin_addr_list = omInsertInSortedList(bin_addr_list, size, addr);
}

void TestFreeBin(size_t size)
{
  omBin bin;
  TestAddr addr = omFindInSortedList(bin_addr_list, size, size);
  if (addr != NULL) 
    bin_addr_list = omRemoveFromList(bin_addr_list, addr);
  bin = omGetSpecBin(addr->size);
  omdCheckFreeBin(addr, bin, D_LEVEL, __FILE__, __LINE__);
}
  
void FreeAllBin()
{
  TestAddr addr;
  omBin bin;
  while (bin_addr_list != NULL)
  {
    addr = bin_addr_list;
    bin_addr_list = bin_addr_list->next;
    bin = omGetSpecBin(addr->size);
    omdCheckFreeBin(addr, bin, D_LEVEL, __FILE__, __LINE__);
  }
}

int count = 0;
int InitSizeGen(int i)
{
  return 0;
}

int NextSizeGen(int prev)
{
  return ++count % 1024;
}

void PrintTest()
{
  omPrintCurrentBackTrace(0, 10, stdout);
}

int main(int argc, char* argv[])
{
  int limit, i, size;
  omInitTrack(argv[0]);
  
  omPrintCurrentBackTrace(0, 10, stdout);
  PrintTest();
  return 0;
  
  if (argc > 1) 
    sscanf(argv[1], "%d", &limit);
  else
    limit = 200;
  
  size = InitSizeGen(limit);
  i = 0;
  
  for (i=0; i<= limit; i++)
  {
    size = NextSizeGen(size);
    TestAllocBin(size);
    TestAllocBlock(size);
    TestAllocChunk(size);
    size = NextSizeGen(size);
    TestAllocBin(size);
    TestAllocBlock(size);
    TestAllocChunk(size);
    TestFreeBin(size);
    TestFreeBlock(size);
    TestFreeChunk(size);
    
    if (i % 100 == 0)
    {
      printf("i=%d\n",i);
      omPrintBinStats(stdout);
    }
  }
  omPrintBinStats(stdout);
  FreeAllBin();
  omPrintBinStats(stdout);
  FreeAllBlock();
  omPrintBinStats(stdout);
  FreeAllChunk();
  omPrintBinStats(stdout);
  return 0;
}

