/*******************************************************************
 * Native Win64 implementation of the omalloc public interface.
 *******************************************************************/
#include "omalloc/omalloc.h"

#include <limits.h>
#include <stdint.h>
#include <windows.h>

#define OM_WIN_ALIGNMENT 16U
#define OM_WIN_MAX_CACHED_SIZE 4096U
#define OM_WIN_CLASS_COUNT (OM_WIN_MAX_CACHED_SIZE / OM_WIN_ALIGNMENT)
#define OM_WIN_BIN_DYNAMIC 1UL
#define OM_WIN_BIN_STICKY 2UL

typedef char omWinSrwlockMustFitPointer[
  (sizeof(SRWLOCK) == sizeof(void*)) ? 1 : -1];

static INIT_ONCE omWinInitOnce = INIT_ONCE_STATIC_INIT;
static SRWLOCK omWinStatsLock = SRWLOCK_INIT;
static omBin_t omWinSizeBins[OM_WIN_CLASS_COUNT];
static HANDLE omWinHeap = NULL;
static volatile LONG64 omWinCurrentBytes = 0;
static volatile LONG64 omWinUsedBytes = 0;
static volatile LONG64 omWinMaxBytes = 0;
static volatile LONG64 omWinInternalBytes = 0;
static volatile LONG64 omWinLastReportedBytes = 0;

struct omInfo_s om_Info;
struct omOpts_s om_Opts = {
  0, 0, 5, 10, 100, 1, 0, 512, NULL, NULL, omErrorBreak
};
omError_t om_ErrorStatus = omError_NoError;
omError_t om_InternalErrorStatus = omError_NoError;
int om_sing_opt_show_mem = 0;

static long omWinLongValue(LONGLONG value)
{
  if (value > LONG_MAX) return LONG_MAX;
  if (value < LONG_MIN) return LONG_MIN;
  return (long)value;
}

static LONG64 omWinRead64(volatile LONG64* value)
{
  return InterlockedCompareExchange64(value, 0, 0);
}

static void omWinUpdateMax(LONG64 current)
{
  LONG64 previous = omWinRead64(&omWinMaxBytes);
  while (current > previous)
  {
    LONG64 observed = InterlockedCompareExchange64(&omWinMaxBytes,
                                                    current, previous);
    if (observed == previous) break;
    previous = observed;
  }
}

static void omWinPublishInfoLocked(void)
{
  LONG64 current = omWinRead64(&omWinCurrentBytes);
  LONG64 used = omWinRead64(&omWinUsedBytes);
  LONG64 maximum = omWinRead64(&omWinMaxBytes);
  LONG64 internal = omWinRead64(&omWinInternalBytes);
  LONG64 available = current - used;
  if (available < 0) available = 0;

  om_Info.MaxBytesSystem = omWinLongValue(maximum);
  om_Info.CurrentBytesSystem = omWinLongValue(current);
  om_Info.MaxBytesSbrk = 0;
  om_Info.CurrentBytesSbrk = 0;
  om_Info.MaxBytesMmap = 0;
  om_Info.CurrentBytesMmap = 0;
  om_Info.UsedBytes = omWinLongValue(used);
  om_Info.AvailBytes = omWinLongValue(available);
  om_Info.UsedBytesMalloc = omWinLongValue(used);
  om_Info.InternalUsedBytesMalloc = omWinLongValue(internal);
  om_Info.AvailBytesMalloc = omWinLongValue(available);
  om_Info.MaxBytesFromMalloc = omWinLongValue(maximum);
  om_Info.CurrentBytesFromMalloc = omWinLongValue(current);
  om_Info.MaxBytesFromValloc = 0;
  om_Info.CurrentBytesFromValloc = 0;
  om_Info.UsedBytesFromValloc = 0;
  om_Info.AvailBytesFromValloc = 0;
  om_Info.MaxPages = 0;
  om_Info.UsedPages = 0;
  om_Info.AvailPages = 0;
  om_Info.MaxRegionsAlloc = 0;
  om_Info.CurrentRegionsAlloc = 0;
}

static void omWinChangeStats(LONGLONG current_delta,
                             LONGLONG used_delta,
                             LONGLONG internal_delta)
{
  LONG64 current = 0;
  LONG64 last = 0;
  int report = 0;

  if (used_delta != 0)
    InterlockedAdd64(&omWinUsedBytes, used_delta);
  if (internal_delta != 0)
    InterlockedAdd64(&omWinInternalBytes, internal_delta);
  if (current_delta != 0)
  {
    current = InterlockedAdd64(&omWinCurrentBytes, current_delta);
    omWinUpdateMax(current);
    if (om_sing_opt_show_mem)
    {
      last = omWinRead64(&omWinLastReportedBytes);
      if ((current - last >= 1024 * 1024 ||
           last - current >= 1024 * 1024) &&
          InterlockedCompareExchange64(&omWinLastReportedBytes,
                                       current, last) == last)
        report = 1;
    }
  }

  if (report)
    fprintf(stderr, "// **  redefining memory: %lld bytes\n", current);
}

static BOOL CALLBACK omWinInitialize(PINIT_ONCE once, PVOID parameter,
                                     PVOID* context)
{
  size_t i;
  (void)once;
  (void)parameter;
  (void)context;

  omWinHeap = GetProcessHeap();
  for (i = 0; i < OM_WIN_CLASS_COUNT; i++)
  {
    size_t bytes = (i + 1) * OM_WIN_ALIGNMENT;
    omWinSizeBins[i].sizeW = (bytes + SIZEOF_LONG - 1) / SIZEOF_LONG;
    omWinSizeBins[i].cache_limit = 256 * 1024 / bytes;
    if (omWinSizeBins[i].cache_limit < 32)
      omWinSizeBins[i].cache_limit = 32;
    if (omWinSizeBins[i].cache_limit > 4096)
      omWinSizeBins[i].cache_limit = 4096;
  }
  return TRUE;
}

static void omWinEnsureInitialized(void)
{
  InitOnceExecuteOnce(&omWinInitOnce, omWinInitialize, NULL, NULL);
}

static size_t omWinBinBytes(const omBin bin)
{
  size_t bytes;
  if (bin == NULL) return 0;
  if (bin->sizeW > SIZE_MAX / SIZEOF_LONG) return SIZE_MAX;
  bytes = bin->sizeW * SIZEOF_LONG;
  return bytes < sizeof(void*) ? sizeof(void*) : bytes;
}

static size_t omWinHeapSize(const void* addr)
{
  SIZE_T size;
  if (addr == NULL) return 0;
  omWinEnsureInitialized();
  size = HeapSize(omWinHeap, 0, addr);
  return size == (SIZE_T)-1 ? 0 : (size_t)size;
}

static void omWinOutOfMemory(void)
{
  if (om_Opts.OutOfMemoryFunc != NULL)
    om_Opts.OutOfMemoryFunc();
  fprintf(stderr, "omalloc: out of memory\n");
  fflush(stderr);
  ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
}

static void* omWinHeapAlloc(size_t size, DWORD flags, int internal)
{
  void* addr;
  omWinEnsureInitialized();
  addr = HeapAlloc(omWinHeap, flags, size);
  if (addr == NULL && om_Opts.MemoryLowFunc != NULL)
  {
    om_Opts.MemoryLowFunc();
    addr = HeapAlloc(omWinHeap, flags, size);
  }
  if (addr == NULL)
    omWinOutOfMemory();
  omWinChangeStats((LONGLONG)size, internal ? 0 : (LONGLONG)size,
                   internal ? (LONGLONG)size : 0);
  return addr;
}

static void omWinHeapFree(void* addr, size_t size, int active, int internal)
{
  if (addr == NULL) return;
  if (!HeapFree(omWinHeap, 0, addr))
  {
    om_ErrorStatus = omError_FalseAddrOrMemoryCorrupted;
    om_InternalErrorStatus = om_ErrorStatus;
    if (om_Opts.ErrorHook != NULL) om_Opts.ErrorHook();
    return;
  }
  omWinChangeStats(-(LONGLONG)size,
                   active ? -(LONGLONG)size : 0,
                   internal ? -(LONGLONG)size : 0);
}

static omBin omWinClassForRequest(size_t size)
{
  size_t rounded;
  if (size == 0 || size > OM_WIN_MAX_CACHED_SIZE) return NULL;
  rounded = (size + OM_WIN_ALIGNMENT - 1) & ~(OM_WIN_ALIGNMENT - 1);
  return &omWinSizeBins[rounded / OM_WIN_ALIGNMENT - 1];
}

static omBin omWinClassForBlock(size_t actual)
{
  if (actual < OM_WIN_ALIGNMENT || actual > OM_WIN_MAX_CACHED_SIZE ||
      (actual & (OM_WIN_ALIGNMENT - 1)) != 0)
    return NULL;
  return &omWinSizeBins[actual / OM_WIN_ALIGNMENT - 1];
}

static void* omWinPopBin(omBin bin, int zero)
{
  void* addr = NULL;
  size_t bytes;

  AcquireSRWLockExclusive((PSRWLOCK)&bin->win_lock);
  if (bin->free_list != NULL)
  {
    addr = bin->free_list;
    bin->free_list = *(void**)addr;
    bin->cached--;
  }
  ReleaseSRWLockExclusive((PSRWLOCK)&bin->win_lock);

  if (addr == NULL) return NULL;
  bytes = omWinBinBytes(bin);
  omWinChangeStats(0, (LONGLONG)bytes, 0);
  if (zero)
    memset(addr, 0, bytes);
  return addr;
}

static int omWinPushBin(omBin bin, void* addr, size_t actual)
{
  int cached = 0;
  if (bin == NULL || actual < omWinBinBytes(bin)) return 0;

  AcquireSRWLockExclusive((PSRWLOCK)&bin->win_lock);
  if (bin->cached < bin->cache_limit)
  {
    *(void**)addr = bin->free_list;
    bin->free_list = addr;
    bin->cached++;
    cached = 1;
  }
  ReleaseSRWLockExclusive((PSRWLOCK)&bin->win_lock);
  if (cached)
    omWinChangeStats(0, -(LONGLONG)actual, 0);
  return cached;
}

void* omWinAlloc(size_t size, int zero)
{
  omBin bin;
  void* addr;
  size_t allocation_size;

  if (size == 0) return NULL;
  omWinEnsureInitialized();
  bin = omWinClassForRequest(size);
  if (bin != NULL)
  {
    addr = omWinPopBin(bin, zero);
    if (addr != NULL) return addr;
    allocation_size = omWinBinBytes(bin);
  }
  else
    allocation_size = size;
  return omWinHeapAlloc(allocation_size, zero ? HEAP_ZERO_MEMORY : 0, 0);
}

void omWinFree(void* addr)
{
  size_t actual;
  omBin bin;
  if (addr == NULL) return;
  actual = omWinHeapSize(addr);
  if (actual == 0)
  {
    om_ErrorStatus = omError_FalseAddrOrMemoryCorrupted;
    om_InternalErrorStatus = om_ErrorStatus;
    if (om_Opts.ErrorHook != NULL) om_Opts.ErrorHook();
    return;
  }
  bin = omWinClassForBlock(actual);
  if (bin == NULL || !omWinPushBin(bin, addr, actual))
    omWinHeapFree(addr, actual, 1, 0);
}

void omWinFreeSize(void* addr, size_t size)
{
  omBin bin;
  size_t bytes;
  if (addr == NULL) return;
  bin = omWinClassForRequest(size);
  bytes = bin == NULL ? size : omWinBinBytes(bin);
  if (bin == NULL || !omWinPushBin(bin, addr, bytes))
    omWinHeapFree(addr, bytes, 1, 0);
}

void* omWinAllocBin(omBin bin, int zero)
{
  void* addr;
  size_t bytes;
  if (bin == NULL)
  {
    om_ErrorStatus = omError_UnknownBin;
    return NULL;
  }
  addr = omWinPopBin(bin, zero);
  if (addr != NULL) return addr;
  bytes = omWinBinBytes(bin);
  return omWinHeapAlloc(bytes, zero ? HEAP_ZERO_MEMORY : 0, 0);
}

void omWinFreeBin(void* addr, omBin bin)
{
  size_t bytes;
  if (addr == NULL) return;
  if (bin == NULL)
  {
    om_ErrorStatus = omError_UnknownBin;
    return;
  }
  bytes = omWinBinBytes(bin);
  if (!omWinPushBin(bin, addr, bytes))
    omWinHeapFree(addr, bytes, 1, 0);
}

void* omWinRealloc(void* addr, size_t old_size, size_t new_size, int zero)
{
  void* result;
  omBin old_bin;
  omBin new_bin;
  size_t old_bytes;
  size_t new_bytes;

  if (addr == NULL) return omWinAlloc(new_size, zero);
  if (new_size == 0)
  {
    omWinFree(addr);
    return NULL;
  }
  old_bin = omWinClassForRequest(old_size);
  new_bin = omWinClassForRequest(new_size);
  old_bytes = old_bin == NULL ? old_size : omWinBinBytes(old_bin);
  new_bytes = new_bin == NULL ? new_size : omWinBinBytes(new_bin);
  if (old_bytes == new_bytes)
  {
    if (zero && new_size > old_size)
      memset((char*)addr + old_size, 0, new_size - old_size);
    return addr;
  }

  result = HeapReAlloc(omWinHeap, 0, addr, new_bytes);
  if (result == NULL && om_Opts.MemoryLowFunc != NULL)
  {
    om_Opts.MemoryLowFunc();
    result = HeapReAlloc(omWinHeap, 0, addr, new_bytes);
  }
  if (result == NULL)
    omWinOutOfMemory();
  omWinChangeStats((LONGLONG)new_bytes - (LONGLONG)old_bytes,
                   (LONGLONG)new_bytes - (LONGLONG)old_bytes, 0);
  if (zero && new_size > old_size)
    memset((char*)result + old_size, 0, new_size - old_size);
  return result;
}

void* omWinReallocBin(void* addr, omBin old_bin, omBin new_bin, int zero)
{
  void* result;
  size_t old_bytes;
  size_t new_bytes;
  size_t copy_bytes;
  if (addr == NULL) return omWinAllocBin(new_bin, zero);
  if (old_bin == NULL || new_bin == NULL) return NULL;
  old_bytes = omWinBinBytes(old_bin);
  new_bytes = omWinBinBytes(new_bin);
  if (old_bytes == new_bytes) return addr;
  result = omWinAllocBin(new_bin, 0);
  copy_bytes = old_bytes < new_bytes ? old_bytes : new_bytes;
  memcpy(result, addr, copy_bytes);
  if (zero && new_bytes > copy_bytes)
    memset((char*)result + copy_bytes, 0, new_bytes - copy_bytes);
  omWinFreeBin(addr, old_bin);
  return result;
}

omBin _omGetSpecBin(size_t size, int align, int track)
{
  omBin bin;
  size_t bytes;
  (void)align;
  (void)track;
  if (size == 0) size = 1;
  bin = (omBin)omWinHeapAlloc(sizeof(*bin), HEAP_ZERO_MEMORY, 1);
  bin->sizeW = (size + SIZEOF_LONG - 1) / SIZEOF_LONG;
  bytes = omWinBinBytes(bin);
  bin->cache_limit = 256 * 1024 / bytes;
  if (bin->cache_limit == 0) bin->cache_limit = 1;
  if (bytes <= 8192 && bin->cache_limit < 32) bin->cache_limit = 32;
  if (bin->cache_limit > 4096) bin->cache_limit = 4096;
  bin->flags = OM_WIN_BIN_DYNAMIC;
  return bin;
}

static void omWinFlushBin(omBin bin)
{
  void* list;
  void* next;
  size_t bytes = omWinBinBytes(bin);
  AcquireSRWLockExclusive((PSRWLOCK)&bin->win_lock);
  list = bin->free_list;
  bin->free_list = NULL;
  bin->cached = 0;
  ReleaseSRWLockExclusive((PSRWLOCK)&bin->win_lock);

  while (list != NULL)
  {
    next = *(void**)list;
    omWinHeapFree(list, bytes, 0, 0);
    list = next;
  }
}

void _omUnGetSpecBin(omBin* bin_ptr, int force)
{
  omBin bin;
  (void)force;
  if (bin_ptr == NULL || *bin_ptr == NULL) return;
  bin = *bin_ptr;
  if ((bin->flags & OM_WIN_BIN_DYNAMIC) == 0)
  {
    om_ErrorStatus = omError_WrongBin;
    return;
  }
  omWinFlushBin(bin);
  omWinHeapFree(bin, sizeof(*bin), 0, 1);
  *bin_ptr = NULL;
}

long omGetUsedBinBytes(void)
{
  return omWinLongValue(omWinRead64(&omWinUsedBytes));
}

omBin omGetStickyBinOfBin(omBin bin)
{
  omBin sticky;
  if (bin == NULL) return NULL;
  sticky = _omGetSpecBin(omWinBinBytes(bin), 0, 0);
  sticky->flags |= OM_WIN_BIN_STICKY;
  sticky->sticky = 1;
  return sticky;
}

void omMergeStickyBinIntoBin(omBin sticky_bin, omBin into_bin)
{
  void* list;
  size_t count;
  if (sticky_bin == NULL || into_bin == NULL || sticky_bin == into_bin ||
      (sticky_bin->flags & OM_WIN_BIN_STICKY) == 0 ||
      omWinBinBytes(sticky_bin) != omWinBinBytes(into_bin))
  {
    om_ErrorStatus = omError_StickyBin;
    return;
  }

  AcquireSRWLockExclusive((PSRWLOCK)&sticky_bin->win_lock);
  list = sticky_bin->free_list;
  count = sticky_bin->cached;
  sticky_bin->free_list = NULL;
  sticky_bin->cached = 0;
  ReleaseSRWLockExclusive((PSRWLOCK)&sticky_bin->win_lock);

  if (list != NULL)
  {
    void* last = list;
    while (*(void**)last != NULL) last = *(void**)last;
    AcquireSRWLockExclusive((PSRWLOCK)&into_bin->win_lock);
    *(void**)last = into_bin->free_list;
    into_bin->free_list = list;
    into_bin->cached += count;
    ReleaseSRWLockExclusive((PSRWLOCK)&into_bin->win_lock);
  }
  omWinHeapFree(sticky_bin, sizeof(*sticky_bin), 0, 1);
}

unsigned long omGetNewStickyBinTag(omBin bin)
{
  if (bin != NULL) bin->sticky = 1;
  return 1;
}

void omSetStickyBinTag(omBin bin, unsigned long sticky)
{
  if (bin != NULL) bin->sticky = sticky;
}

void omUnSetStickyBinTag(omBin bin, unsigned long sticky)
{
  if (bin != NULL && bin->sticky == sticky) bin->sticky = 0;
}

void omDeleteStickyBinTag(omBin bin, unsigned long sticky)
{
  omUnSetStickyBinTag(bin, sticky);
}

unsigned long omGetNewStickyAllBinTag(void) { return 1; }
void omSetStickyAllBinTag(unsigned long sticky) { (void)sticky; }
void omUnSetStickyAllBinTag(unsigned long sticky) { (void)sticky; }
void omDeleteStickyAllBinTag(unsigned long sticky) { (void)sticky; }

size_t omSizeOfAddr(const void* addr) { return omWinHeapSize(addr); }
size_t omSizeWOfAddr(const void* addr) { return omWinHeapSize(addr) / SIZEOF_LONG; }

char* omWinStrDup(const char* addr)
{
  size_t size;
  char* result;
  if (addr == NULL) return NULL;
  size = strlen(addr) + 1;
  result = (char*)omWinAlloc(size, 0);
  memcpy(result, addr, size);
  return result;
}

void* omWinMemDup(const void* addr)
{
  size_t size;
  void* result;
  if (addr == NULL) return NULL;
  size = omWinHeapSize(addr);
  if (size == 0) return NULL;
  result = omWinAlloc(size, 0);
  memcpy(result, addr, size);
  return result;
}

struct omInfo_s omGetInfo(void)
{
  struct omInfo_s result;
  omUpdateInfo();
  AcquireSRWLockShared(&omWinStatsLock);
  result = om_Info;
  ReleaseSRWLockShared(&omWinStatsLock);
  return result;
}

void omUpdateInfo(void)
{
  AcquireSRWLockExclusive(&omWinStatsLock);
  omWinPublishInfoLocked();
  ReleaseSRWLockExclusive(&omWinStatsLock);
}

void omInitInfo(void) { omWinEnsureInitialized(); omUpdateInfo(); }

void omPrintInfo(FILE* fd)
{
  struct omInfo_s info = omGetInfo();
  if (fd == NULL) fd = stdout;
  fprintf(fd, "Win64 process heap: current=%ld used=%ld available=%ld peak=%ld\n",
          info.CurrentBytesSystem, info.UsedBytes, info.AvailBytes,
          info.MaxBytesSystem);
}

void omPrintStats(FILE* fd) { omPrintInfo(fd); }

void omPrintBinStats(FILE* fd)
{
  size_t i;
  size_t cached = 0;
  omWinEnsureInitialized();
  for (i = 0; i < OM_WIN_CLASS_COUNT; i++)
  {
    AcquireSRWLockShared((PSRWLOCK)&omWinSizeBins[i].win_lock);
    cached += omWinSizeBins[i].cached;
    ReleaseSRWLockShared((PSRWLOCK)&omWinSizeBins[i].win_lock);
  }
  if (fd == NULL) fd = stdout;
  fprintf(fd, "Win64 omalloc cached blocks: %zu\n", cached);
}

static const char* const omWinErrorStrings[omError_MaxError] = {
  "no error", "unknown error", "internal bug", "memory corrupted",
  "null address", "invalid range address", "false address",
  "false address or memory corrupted", "wrong size", "freed address",
  "freed address or memory corrupted", "wrong bin", "unknown bin",
  "not a bin address", "unaligned address", "zero-size allocation",
  "list cycle", "sorted-list error", "kept-address list corrupted",
  "free pattern corrupted", "back pattern corrupted",
  "front pattern corrupted", "not a string", "sticky-bin error"
};

const char* omError2String(omError_t error)
{
  if (error < 0 || error >= omError_MaxError) return "invalid omalloc error";
  return omWinErrorStrings[error];
}

const char* omError2Serror(omError_t error) { return omError2String(error); }
void omErrorBreak(void) {}

omError_t omWinTestAddr(const void* addr)
{
  if (addr == NULL) return omError_NullAddr;
  omWinEnsureInitialized();
  return HeapValidate(omWinHeap, 0, addr) ? omError_NoError : omError_FalseAddr;
}

omError_t omWinTestAddrSize(const void* addr, size_t size)
{
  size_t actual;
  omError_t error = omWinTestAddr(addr);
  if (error != omError_NoError) return error;
  actual = omWinHeapSize(addr);
  return actual >= size ? omError_NoError : omError_WrongSize;
}

omError_t omWinTestAddrBin(const void* addr, omBin bin)
{
  if (bin == NULL) return omError_UnknownBin;
  return omWinTestAddrSize(addr, omWinBinBytes(bin));
}

omError_t omTestBinAddrSize(void* addr, size_t size, int check_level)
{
  (void)check_level;
  return omWinTestAddrSize(addr, size);
}

omError_t omTestMemory(int check_level)
{
  (void)check_level;
  omWinEnsureInitialized();
  return HeapValidate(omWinHeap, 0, NULL) ? omError_NoError
                                         : omError_MemoryCorrupted;
}

void omPrintAddrInfo(FILE* fd, void* addr, const char* text)
{
  size_t size = omWinHeapSize(addr);
  if (fd == NULL) fd = stderr;
  fprintf(fd, "%s address=%p size=%zu status=%s\n",
          text == NULL ? "" : text, addr, size,
          omError2String(omWinTestAddr(addr)));
}

void* omCallocFunc(size_t nmemb, size_t size)
{
  if (size != 0 && nmemb > SIZE_MAX / size)
    omWinOutOfMemory();
  return omWinAlloc(nmemb * size, 1);
}

void* omMallocFunc(size_t size) { return omWinAlloc(size, 0); }
void omFreeFunc(void* addr) { omWinFree(addr); }
void* omVallocFunc(size_t size) { return omWinAlloc(size, 0); }
void* omReallocFunc(void* addr, size_t size)
{
  return omWinRealloc(addr, omSizeOfAddr(addr), size, 0);
}
char* omStrdupFunc(const char* addr) { return omWinStrDup(addr); }
void* omReallocSizeFunc(void* addr, size_t old_size, size_t new_size)
{
  return omWinRealloc(addr, old_size, new_size, 0);
}
void omFreeSizeFunc(void* addr, size_t size) { omWinFreeSize(addr, size); }
