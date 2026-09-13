#include "omalloc/omalloc.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define THREAD_COUNT 4
#define THREAD_ROUNDS 20000

static omBin shared_bin;

static DWORD WINAPI allocator_thread(LPVOID parameter)
{
  uintptr_t thread_no = (uintptr_t)parameter;
  size_t i;
  for (i = 0; i < THREAD_ROUNDS; i++)
  {
    size_t old_size = 17 + ((i + thread_no) % 240);
    size_t new_size = old_size + 39;
    unsigned char* p = (unsigned char*)omAlloc(old_size);
    void* b;
    assert(p != NULL);
    memset(p, (int)(i & 0xff), old_size);
    p = (unsigned char*)omRealloc0Size(p, old_size, new_size);
    assert(p != NULL);
    assert(p[old_size - 1] == (unsigned char)(i & 0xff));
    assert(p[old_size] == 0 && p[new_size - 1] == 0);
    omFree(p);

    b = omAllocBin(shared_bin);
    assert(b != NULL);
    memset(b, (int)thread_no, omSizeWOfBin(shared_bin) * SIZEOF_LONG);
    omFreeBin(b, shared_bin);
  }
  return 0;
}

static void test_general_allocation(void)
{
  unsigned char* p;
  unsigned char* copy;
  size_t i;

  p = (unsigned char*)omAlloc0Aligned(73);
  assert(p != NULL);
  assert(((uintptr_t)p & 15U) == 0);
  assert(omSizeWOfAddr(p) * SIZEOF_LONG >= 73);
  for (i = 0; i < 73; i++) assert(p[i] == 0);
  for (i = 0; i < 73; i++) p[i] = (unsigned char)(i + 1);

  copy = (unsigned char*)omMemDup(p);
  assert(copy != NULL);
  assert(memcmp(copy, p, 73) == 0);
  omFree(copy);

  p = (unsigned char*)omRealloc0Size(p, 73, 211);
  assert(p != NULL);
  for (i = 0; i < 73; i++) assert(p[i] == (unsigned char)(i + 1));
  for (i = 73; i < 211; i++) assert(p[i] == 0);
  p = (unsigned char*)omReallocSize(p, 211, 31);
  for (i = 0; i < 31; i++) assert(p[i] == (unsigned char)(i + 1));
  omFree(p);

  p = (unsigned char*)omCallocFunc(31, 7);
  for (i = 0; i < 31 * 7; i++) assert(p[i] == 0);
  omFreeFunc(p);
}

static void test_bins_and_generic_free(void)
{
  omBin tiny = omGetSpecBin(2);
  omBin bin = omGetSpecBin(37);
  omBin sticky;
  void* p;
  void* reused;

  assert(tiny != NULL && bin != NULL);
  assert(omSizeWOfBin(tiny) * SIZEOF_LONG >= 2);
  p = omAllocBin(tiny);
  ((unsigned char*)p)[0] = 17;
  ((unsigned char*)p)[1] = 23;
  omFreeBin(p, tiny);
  reused = omAlloc0Bin(tiny);
  assert(reused == p);
  assert(((unsigned char*)reused)[0] == 0);
  assert(((unsigned char*)reused)[1] == 0);
  omFree(reused);                 /* bin allocation, general free */

  sticky = omGetStickyBinOfBin(bin);
  assert(sticky != NULL);
  p = omAllocBin(sticky);
  omFreeBin(p, sticky);
  omMergeStickyBinIntoBin(sticky, bin);
  reused = omAllocBin(bin);
  assert(reused == p);
  omFreeBin(reused, bin);

  omUnGetSpecBin(&tiny);
  omUnGetSpecBin(&bin);
  assert(tiny == NULL && bin == NULL);
}

static void test_threads(void)
{
  HANDLE threads[THREAD_COUNT];
  size_t i;

  shared_bin = omGetSpecBin(96);
  assert(shared_bin != NULL);
  for (i = 0; i < THREAD_COUNT; i++)
  {
    threads[i] = CreateThread(NULL, 0, allocator_thread,
                              (LPVOID)(uintptr_t)i, 0, NULL);
    assert(threads[i] != NULL);
  }
  assert(WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE) ==
         WAIT_OBJECT_0);
  for (i = 0; i < THREAD_COUNT; i++) CloseHandle(threads[i]);
  omUnGetSpecBin(&shared_bin);
}

int main(void)
{
  struct omInfo_s before;
  struct omInfo_s during;
  struct omInfo_s after;
  struct omInfo_s final;
  void* p;

#ifndef OMALLOC_USE_WIN64_BACKEND
#error "omtWin64 must test the native Win64 backend"
#endif

  omInitInfo();
  before = omGetInfo();
  p = omAlloc(8192);
  during = omGetInfo();
  assert(during.UsedBytes > before.UsedBytes);
  assert(during.CurrentBytesSystem >= during.UsedBytes);
  assert(during.MaxBytesSystem >= during.CurrentBytesSystem);
  omFree(p);
  after = omGetInfo();
  assert(after.UsedBytes == before.UsedBytes);
  assert(after.MaxBytesSystem >= during.CurrentBytesSystem);

  test_general_allocation();
  p = omAlloc(63);
  omFreeSize(p, 63);
  assert(omGetInfo().UsedBytes == before.UsedBytes);
  test_bins_and_generic_free();
  test_threads();
  final = omGetInfo();
  assert(final.UsedBytes == before.UsedBytes);
  assert(final.InternalUsedBytesMalloc == before.InternalUsedBytesMalloc);
  assert(omTestMemory(2) == omError_NoError);
  assert(strcmp(omError2String(omError_NoError), "no error") == 0);
  puts("OMALLOC_WIN64_OK");
  return 0;
}
