/*******************************************************************
 *  File:    omDebug.c
 *  Purpose: implementation of main omDebug functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omDebug.c,v 1.3 2000-05-31 13:34:31 obachman Exp $
 *******************************************************************/
#include "omConfig.h"

#ifdef OM_HAVE_DEBUG

#include "omAlloc.h"

/*******************************************************************
 *  
 * Declarations
 *  
 *******************************************************************/
/* number of bytes for padding before addr: needs to > 0 and a multiple of OM_SIZEOF_STRICT_ALIGNMENT */
#ifndef OM_SIZEOF_FRONT_PADDING
#define OM_SIZEOF_FRONT_PADDING SIZEOF_STRICT_ALIGNMENT
#endif
/* number of bytes for padding after addr: needs to be a multiple of OM_SIZEOF_STRICT_ALIGNMENT */ 
#ifndef OM_SIZEOF_BACK_PADDING
#define OM_SIZEOF_BACK_PADDING SIZEOF_STRICT_ALIGNMENT
#endif

struct omDebugAddr_s;
typedef struct omDebugAddr_s omDebugAddr_t;
typedef omDebugAddr_t * omDebugAddr;

struct omDebugAddr_s
{
  char              debug;
  char              check;
  short             alloc_line;
  char*             alloc_file;
  /* debug > 1 */
  void*             alloc_frames[OM_MAX_FRAMES];
  /* debug > 2 */
  void*             size_bin;
  omFlags_t         flags;
  omDebugCount_t    debugs;
  /* debug > 3 */
  short             free_line;
  char*             free_file;
  /* debug > 4 */
  void*             free_frames[OM_MAX_FRAMES];
};

/* this is only needed to determine SIZEOF_DEBUG_ADDR_i */
static struct omDebugAddr_s debug_addr;
#define OM_SIZEOF_DEBUG_ADDR_1  OM_STRICT_ALIGN_SIZE(((void*)&debug_addr.alloc_frames-(void*)&debug_addr))
#define OM_SIZEOF_DEBUG_ADDR_2  OM_STRICT_ALIGN_SIZE(((void*)&debug_addr.size_bin-(void*)&debug_addr))
#define OM_SIZEOF_DEBUG_ADDR_3  (OM_STRICT_ALIGN_SIZE(((void*)&debug_addr.free_line-(void*)&debug_addr))+SIZEOF_FRONT_PADDING)
#define OM_SIZEOF_DEBUG_ADDR_4  (OM_STRICT_ALIGN_SIZE(((void*)&debug_addr.free_frames-(void*)&debug_addr))+SIZEOF_FRONT_PADDING)
#define OM_SIZEOF_DEBUG_ADDR_5  OM_SIZEOF_DEBUG_ADDR
#define OM_SIZEOF_DEBUG_ADDR    (OM_STRICT_ALIGN_SIZE(sizeof(struct omDebugAddr_s)) + SIZEOF_FRONT_PADDING)

OM_INLINE_LOCAL omDebugAddr omAddr_2_DebugAddr(void* addr)
{
  void* page = omGetPageOfAddr(addr);
  size_t size = omGetTopeBinOfPage((omBinPage) page)->sizeW << LOG_SIZEOF_LONG;
  page += SIZEOF_OM_BIN_PAGE_HEADER;
  return (omDebugAddr) (page + (void*) (((unsigned long)addr - (unsigned long)page) / size)*size);
}

#define OM_BIN_FLAG     1           /* size_bin is bin, if set, else size */
#define OM_USED_FLAG    2           /* is in use, if set */
#define OM_FREE_FLAG    4           /* had been freed, if set */
#define OM_STATIC_FLAG  8           /* if set, considered to be static, i.e. never be freed */
#define OM_MAX_FLAG             64  /* define, but never use it */
#define SET_FLAG(var, flag)     (var |= (flag))
#define CLEAR_FLAG(var, flag)   (var &= ~(flag))
#define IS_FLAG_SET(var, flag)  (var & (flag))


#define _omDebugAddr_2_OutAddr(d_addr)   (((void*) d_addr) + OM_SIZEOF_DEBUG_ADDR_HEADER)
#define _omOutAddr_2_DebugAddr(addr)     ((omDebugAddr) ((void*) addr - OM_SIZEOF_DEBUG_ADDR_HEADER))
#define _omOutSize_2_DebugSize(size)     (size + OM_SIZEOF_DEBUG_ADDR_HEADER + OM_SIZEOF_BACK_PADDING)
#define _omDebugSize_2_OutSize(size)     (size - OM_SIZEOF_DEBUG_ADDR_HEADER - OM_SIZEOF_BACK_PADDING)

#ifdef OM_INTERNAL_DEBUG
static void* omDebugAddr_2_OutAddr(void* d_addr) {return _omDebugAddr_2_OutAddr(d_addr);}
static void* omOutAddr_2_DebugAddr(void* addr)   {return _omOutAddr_2_DebugAddr(addr);}
static size_t omOutSize_2_DebugSize(size_t size) {return _omOutSize_2_DebugSize(size);} 
static size_t omDebugSize_2_OutSize(size_t size) {return _omOutSize_2_DebugSize(size);}
#else
#define omDebugAddr_2_OutAddr   _omDebugAddr_2_OutAddr
#define omOutAddr_2_DebugAddr   _omOutAddr_2_DebugAddr
#define omOutSize_2_DebugSize   _omOutSize_2_DebugSize
#define omDebugSize_2_OutSize   _omDebugSize_2_OutSize
#endif

static void* om_Frames[OM_MAX_FRAMES];

/*******************************************************************
 *  
 * Public routines --they are just dispatcher to equivalent first-level,
 * i.e.,  _om*  routines
 *  
 *******************************************************************/

void* omTestAllocBin(omBin bin, char check, char debug, char* file, int line)
{
  return _omTestAlloc(bin,
                      OM_BIN_FLAGS,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestAlloc0Bin(omBin bin, char check, char debug, char* file, int line)
{
  return _omTestAlloc(bin, 
                      OM_BIN_FLAG|OM_ZERO_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestReallocBin(void* old_addr, omBin old_bin, omBin new_bin, char check, char debug, char* file, int line)
{
  return _omTestRealloc(old_addr, old_bin, new_bin, 
                        OM_BIN_FLAG,OM_BIN_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}   
void* omTestRealloc0Bin(void* old_addr, omBin old_bin, omBin new_bin, char check, char debug, char* file, int line)
{
  return _omTestRealloc(old_addr, old_bin, new_bin, 
                        OM_BIN_FLAG,OM_BIN_FLAG|OM_ZERO_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void omTestFreeBin(void* addr, omBin bin, char check, char* file, int line)
{
  _omTestFree(addr, bin, 
              OM_BIN_FLAG,MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}

void* omTestAlloc(size_t size, char check, char debug, char* file, int line)
{
  return _omTestAlloc(size, 
                      OM_SIZE_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestAlloc0(size_t size, char check, char debug, char* file, int line)
{
  return _omTestAlloc(size, 
                      OM_SIZE_FLAG|OM_ZERO_FLAG, MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestAllocAligned(size_t size, char check, char debug, char* file, int line)
{
  return _omTestAlloc(size, 
                      OM_SIZE_FLAG|OM_ALIGNED_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestAlloc0Aligned(size_t size, char check, char debug, char* file, int line)
{
  return _omTestAlloc(size, 
                      OM_SIZE_FLAG|OM_ALIGNED_FLAG|OM_ZERO_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}

void* omTestRealloc(void* addr, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, NULL, new_size, 
                        0,OM_SIZE_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestRealloc0(void* addr, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, NULL, new_size, 
                        0, OM_SIZE_FLAG|OM_ZERO_FLAG, MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestReallocAligned(void* addr, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, NULL, new_size, 
                        0,OM_SIZE_FLAG|OM_ALIGNED_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR);
}
void* omTestRealloc0Aligned(void* addr, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, NULL, new_size, 
                        0,OM_SIZE_FLAG|OM_ZERO_FLAG|OM_ALIGNED_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR,OM_REALLOC0_ALIGNED);
}

void* omTestReallocSize(void* addr, size_t old_size, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, old_size, new_size, 
                        OM_SIZE_FLAG,OM_SIZE_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR,OM_REALLOC_SIZE);
}
void* omTestRealloc0Size(void* addr, size_t old_size, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, old_size, new_size, 
                        OM_SIZE_FLAG,OM_SIZE_FLAG|OM_ZERO_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR,OM_REALLOC0_SIZE);
}
void* omTestReallocAlignedSize(void* addr, size_t old_size, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, old_size, new_size, 
                        OM_SIZE_FLAG,OM_SIZE_FLAG|OM_ALIGNED_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR,OM_REALLOC_ALIGNED_SIZE);
}
void* omTestRealloc0AlignedSize(void* addr, size_t old_size, size_t new_size, char check, char debug, char* file, int line)
{
  return _omTestRealloc(addr, old_size, new_size, 
                        OM_SIZE_FLAG,OM_SIZE_FLAG|OM_ZERO_FLAG|OM_ALIGNED_FLAG,MAX(check,om_Opts.MinCheck),MAX(debug,om_Opts.MinDebug),f,l,OM_DEBUG_RETURN_ADDR,OM_REALLOC0_ALIGNED_SIZE);
}

void omTestFreeSize(void* addr, size_t size, char check, char* file, int line)
{
  _omTestFree(addr, size, 
              OM_SIZE_FLAG, MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR,OM_FREE_SIZE);
}
void omTestFree(void* addr, size_t size, char check, char* file, int line)
{
  _omTestFree(addr, NULL, 
              0, MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR,OM_FREE);
}

omError_t omTestAddrBin(void* addr, omBin bin, char check, char* file, int line)
{
  return _omTestAddr(addr, bin, 
                     OM_USED_FLAG|OM_BIN_FLAG|OM_COUNT_FLAG, MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}
omError_t omTestAddrSize(void* addr, size_t size, char check, char* file, int line)
{
  return _omTestAddr(addr, size, 
                     OM_USED_FLAG|OM_SIZE_FLAG|OM_COUNT_FLAG, MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}
omError_t omTestAddr(void* adr, char check, char* file, int line)
{
  return _omTestAddr(addr, NULL, 
                     OM_USED_FLAG|OM_COUNT_FLAG,MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}
omError_t omTestAlignedAddrBin(void* addr, omBin bin, char check, char* file, int line)
{
  return _omTestAddr(addr, bin, 
                     OM_USED_FLAG|OM_BIN_FLAG|OM_ALIGNED_FLAG|OM_COUNT_FLAG, MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}
omError_t omTestAlignedAddr(void* adr, char check, char* file, int line)
{
  return _omTestAddr(addr,  bin, 
                     OM_USED_FLAG|OM_BIN_FLAG|OM_ALIGNED_FLAG|OM_COUNT_FLAG,MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}
omError_t omTestAlignedAddrSize(void* addr, size_t size, char check, char* file, int line)
{
  return _omTestAddr(addr, bin,    
                     OM_USED_FLAG|OM_BIN_FLAG|OM_ALIGNED_FLAG|OM_COUNT_FLAG, MAX(check,om_Opts.MinCheck),f,l,OM_DEBUG_RETURN_ADDR);
}

/* TBC */
omError_t omTestBin(omBin bin, char check, char* file, int line)
{
  return omError_NoError;
}
omError_t omTestMemory(char check, char debug)
{
  return omError_NoError;
}

/*******************************************************************
 *  
 * First level _om* routines: call respective checks and dispatch
 * to second level, i.e., __om routines
 *  
 *******************************************************************/
static void* _omTestAlloc(void* bin_size, 
                          int flags, char check, char debug,char* f,const int l,void* r)
{
  void* addr;
  
  om_ReportError = omError_MemoryCorrupted;
  if (flags & BIN_FLAG)
    (void) _omTestBin((omBin)bin_size,check-1,f,l,r);
  else 
    (void) _omTestMemory(check-2,f,l,r);

  addr = __omTestAlloc(bin_size, flags, debug, f, l, r);
  
#ifdef OM_INTERNAL_DEBUG
  om_ReportError = omError_InternalBug;
  (void) _omTestAddr(addr, bin_size,flags|OM_USED_FLAGS,check, __FILE__,__LINE__,0);
#endif

  om_ReportError = omError_NoError;
  return new_addr;
}


static void* _omTestRealloc(void* old_addr, void* old_bin_size, void* new_bin_size,
                            int old_flags, int new_flags, char check, char debug, char* f, const int l, void* r)
{
  void* new_addr = NULL;
  
  (void*) _omTestAddr(old_addr, old_flags|OM_USED_FLAG, old_bin_size, check, f, l, r);

  new_addr = __omTestRealloc(old_addr, old_bin_size, new_bin_size,
                             old_flags, new_flags, debug, f, l, r);
#ifdef OM_INTERNAL_DEBUG
  om_ReportError = omError_InternalBug;
  (void) _omTestAddr(new_addr, bin_size,flags|OM_USED_FLAGS,check, __FILE__,__LINE__,0);
  om_ReportError = omError_NoError;
#endif

  return new_addr;
}


static void _omTestFree(void* addr, void* size_bin,
                        int flags, char check, char* f, const int l, void* r)
{
  if (! _omTestAddr(addr, size_bin, flags, check, f, l, r))
    __omTestFree(addr,size_bin,flags,f,l,r);
  
#ifdef OM_INTERNAL_DEBUG
  om_ReportError = omError_InternalBug;
  if (flags & BIN_FLAG)
    (void) _omTestBin((omBin)bin_size,check-1,__FILE__,__LINE__,0);
  else 
    (void) _omTestMemory(check-2,__FILE__,__LINE__,0);
  om_ReportError = omError_NoError;
#endif
}


static omError_t _omTestAddr(void* addr, void* size_bin,
                        int flags, char check, char* f, const int l, void* r)
{
  if (check <= 0) return omError_NoError;
  
  if (!__omPrimitiveTestAddr(addr, size_bin, flags, f, l, r) ||
      !__omTestAddr(addr, size_bin, flags, f, l, r))
    return om_ErrorStatus;
  
  if (check > 1) 
  {
    if (omIsDebugAddr(addr))
    {
      if (! _omTestBin(omGetBinOfAddr(addr), 1,check-1, f, l, r))
        return om_ErrorStatus;
      if (flags & OM_BIN_FLAG)
        return _omTestBin((omBin) bin_size, 0, check - 1, f, l, r);
    }
    else
    {
      if (omIsBinAddr(addr))
        return _omTestBin(omGetBinOfAddr(addr), 0, check-1, f, l, r);
      else if (check > 2)
        return _omTestMemory(check-2, f, l, r);
    }
  }

  return om_ErrorStatus = omError_NoError;
}


static omError_t _omTestBin(omBin bin, char is_debug_bin, 
                            char check, char* f, const int l, void* r)
{
  if (check <= 0) return omError_NoError;
  
  if (! __omPrimitiveTestBin(bin, f, l, r) ||
      ! __omTestBin(bin, is_debug_bin, check - 1, f, l, r))
    return om_ErrorStatus;
  
  if (check > 1) 
    return _omTestMemory(check - 1, f, l, r);

  return om_ErrorStatus = omError_NoError;
}


static omError_t _omTestMemory(char check, char* f, const int l, void* r)
{
  int i = 0;
  omSpecBin s_bin;
  void* addr;
  
  if (check <= 0) return omError_NoError;
  
  if (! omTestBinPagesReagions(check, f, l, r)) return om_ErrorStatus;
  
  for (i=0; i<= OM_MAX_BIN_INDEX; i++)
  {
    if (! __omTestBin(&om_StaticBin[i], 0, check - 1, f, l, r))
      return om_ErrorStatus;
  }

  for (i=0; i<= OM_MAX_DEBUG_BIN_INDEX; i++)
  {
    if (! __omTestBin(&om_StaticDebugBin[i], 1, check -1, f, l, r))
      return om_ErrorStatus;
  }
  
  s_bin = om_SpecBin;
  while (s_bin != NULL)
  {
    if (! __omTestBin(s_bin->bin, 0,check - 1, f, l, r)) 
      return om_ErrorStatus;
    s_bin = s_bin->next;
  }
  

  addr = FirstKeptDebugAddr;
  
  while (addr != NULL)
  {
    if (! __omTestAddr(addr, NULL, OM_FREE_FLAG, f, l, r))
      return om_ErrorStatus;
    addr = *((void**) addr);
  }
    
  return om_ErrorStatus = omError_NoError;
}

/*******************************************************************
 *  
 * Second level __om* routines: do the actual work, no checks, 
 * assume that everything is ok
 *  
 *******************************************************************/
static void* __omTestAlloc(void* bin_size, 
                           int flags, char check, char debug,char* f,const int l,void* r)
{
  void* o_addr;
  o_size = (flags & OM_BIN_FLAG ? ((omBin)bin)->sizeW << LOG_SIZEOF_LONG : (size_t) bin_size);
  
  if (debug > 0)
  {
    omDebugAddr d_addr;
    size_t d_size, o_size;

    d_size = omOutSize_2_DebugSize(o_size, debug);
    
    d_addr = omAllocDebugAddr(d_size);
    
    d_addr->next = (void*) -1;
    d_addr->debug = debug;
    d_addr->alloc_file = f;
    d_addr->alloc_line = l;
    
    if (debug > 1)
    {
      memset(&d_addr->alloc_frames, 0, OM_MAX_FRAMES*SIZEOF_VOIDP);
      omGetCurrentBackTrace(&d_addr->alloc_frames, OM_MAX_FRAMES, r);
      
      if (debug > 2)
      {
        char* pattern;
        
        d_addr->size_bin = size_bin;
        d_addr->flags = flags | USED_FLAG;
        d_addr->debugs = 0;

        if (debug > 3)
        {
          d_addr->free_line = -1;
          d_addr->free_file = (char*) -1;
          
          if (debug > 4)
            memset(&d_addr->free_frames, 0, OM_MAX_FRAMES*SIZEOF_VOIDP);
        }

        memset(omDebugAddr_2_FrontPattern(d_addr), OM_FRONT_PATTER, OM_SIZEOF_FRONT_PATTERN);
        memset(omDebugAddr_2_BackPattern(d_addr), OM_BACK_PATTER, omDebugAddr_2_BackPatternSize(d_addr));
      }
    }
    o_addr = omDebugAddr_2_OutAddr(d_addr);
    if (flags & OM_ZERO_FLAG) memset(o_addr, 0, o_size);
  }
  else
  {
    if (flags & OM_BIN_FLAG)
    {
      omBin bin = (omBin) bin_size;

      if (flags & ZERO_FLAG)
        __omTypeAllo0cBin(void*, o_addr, bin);
      else
        __omTypeAllocBin(void*, o_addr, bin);
    }
    else
    {
      if (flags & ZERO_FLAG)
      {
        if (flags & OM_ALIGNED_FLAG)
          __omTypeAlloc0Aligned(void*, o_addr, o_size);
        else
          __omTypeAlloc0(void*, o_addr, o_size);
      }
      else
      {
        if (flags & OM_ALIGNED_FLAG)
          __omTypeAllocAligned(void*, o_addr, o_size);
        else
          __omTypeAlloc(void*, o_addr, o_size);
      }
    }
  }
  
  if (! (flags & OM_ZERO_FLAG)) memset(o_addr, OM_INIT_PATTERN, o_size);
  
  return o_addr;
}
static omDebugAddr omAllocDebugAddr(size_t d_size)
{
  omDebugAddr d_addr;
  omBin bin;
  
  if (d_size <= OM_MAX_DEBUG_BLOCK_SIZE)
    bin = omSize2DebugBin(d_size);
  else
    bin = omGetSpecBin(d_size);
  
  __omTypeAllocBin(omTestAddr, d_addr, bin);
  
  omAssume(bin->current_page == omGetPageOfAddr(d_addr));

  omSetDebugOfUsedBlocks(bin->current_page->used_blocks);
  
  return d_addr;
}


static void* __omTestRealloc(void* old_addr, void* old_bin_size, void* new_bin_size,
                             int old_flags, int new_flags, char debug, char* f,const int l,void* r)
{
  void* new_addr;
  
  if (om_Opts.Keep > 0 || debug > 0 || omIsDebugAddr(old_addr))
  {
    new_addr = __omTestAlloc(new_bin_size, new_flags, debug, f, l, r);
    memcpy(new_addr, old_addr, omSizeOfAddr(old_addr));
    __omTestFree(old_addr, old_bin_size, old_flags, f, l, r);
  }
  else
  {
    if (new_flags & OM_BIN_FLAG) 
    {
      omBin new_bin = (omBin) new_bin_size;
      omBin old_bin = (omBin) old_bin_size;
      
      omAssume(old_flags & OM_BIN_FLAG);
      if (new_flags & OM_ZERO_FLAG)
        __omTypeRealloc0Bin(old_addr, old_bin, void*, new_addr, new_bin);
      else
        __omTypeReallocBin(old_addr, old_bin, void*, new_addr, new_bin);
    }
    else
    {
      size_t new_size = (size_t) new_bin_size;
      omAssume(!(new_flags & OM_BIN_FLAG) && !(old_flags & OM_BIN_FLAG));
      
      if (flags & OM_SIZE_FLAG)
      {
        size_t old_size = (size_t) old_bin_size;
        
        if (new_flags & OM_ZERO_FLAG)
        {
          if (new_flags & OM_ALIGNED_FLAG)
            __omTypeRealloc0AlignedSize(old_addr, old_size, (void*), new_addr, new_size);
          else
            __omTypeRealloc0Size(old_addr, old_size, (void*), new_addr, new_size);
        }
        else
        {
          if (new_flags & OM_ALIGNED_FLAG)
            __omTypeReallocAlignedSize(old_addr, old_size, (void*), new_addr, new_size);
          else
            __omTypeReallocSize(old_addr, old_size, (void*), new_addr, new_size);
        }
      }
      else
      {
        if (new_flags & OM_ZERO_FLAG)
        {
          if (new_flags & OM_ALIGNED_FLAG)
            __omTypeRealloc0Aligned(old_addr, (void*), new_addr, new_size);
          else
            __omTypeRealloc0(old_addr, (void*), new_addr, new_size);
        }
        else
        {
          if (new_flags & OM_ALIGNED_FLAG)
            __omTypeReallocAligned(old_addr, (void*), new_addr, new_size);
          else
            __omTypeRealloc(old_addr, (void*), new_addr, new_size);
        }
      }
    }
  }

  return new_addr;
}


unsigned long om_NumberOfKeptAddrs = 0;
omDebugAddr om_FirstKeptDebugAddr = NULL;
omDebugAddr om_LastKeptDebugAddr = NULL;
static void __omTestFree(void* addr, void* size_bin, int flags, char* f,const int l,void* r)
{
  if (omIsDebugAddr(addr))
  {
    omDebugAddr d_addr = omOutAddr_2_DebugAddr(addr);
    
    d_addr->next = NULL;
    
    if (d_addr->debug > 2)
    {
      d_addr->flags &= ~OM_USED_FLAG;
      d_addr->flags |= OM_FREE_FLAG;
      
      if (d_addr->debug > 3)
      {
        d_addr->free_line = l;
        d_addr->free_file = f;
        
        if (d_addr->debug > 4)
          omGetCurrentBackTrace(&d_addr->free_frames, OM_MAX_FRAMES, r);
      }
    }
  }
  
  if (om_Opts.Keep > 0)
  {
    if (om_NumberOfKeptAddrs)
    {
      omAssume(om_FirstKeptDebugAddr != NULL && om_LastKeptDebugAddr != NULL &&
               ((om_NumberOfKeptAddr == 1 && om_LastKeptDebugAddr == om_FirstKeptDebugAddr) ||
                (om_NumberOfKeptAddr != 1 && om_LastKeptDebugAddr != om_FirstKeptDebugAddr)));
      om_NumberOfKeptAddrs++;
      *((void**) om_LastKeptAddr) = addr;
      om_LastKeptAddr = addr;
    }
    else
    {
      om_NumberOfKeptAddrs = 1;
      om_LastKeptAddr = addr;
      om_FirstKeptAddr = addr;
      *((void**) om_LastKeptAddr) = NULL;
    }
    
    if (om_NumberOfKeptAddrs > om_Opts.Keep)
    {
      addr = om_FirstKeptAddr;
      om_FirstKeptAddr = *((void**) addr);
      om_NumberOfKeptAddrs--;
    }
    else
      return;
  }
  else
  {
    if (! omIsDebugAddr(addr))
    {
      if (flags & OM_BIN_FLAG) 
        ___omFreeBin(addr);
      else if (flags & OM_SIZE_FLAG)
        __omFreeSize(addr, (size_t) bin_size);
      else
        __omFree(addr);

      return;
    }
  }
  
  if (omIsDebugAddr(addr))
    omFreeDebugAddr(addr);
  else
    __omFree(addr);
}
static void omFreeDebugAddr(void* d_addr)
{
  omBinPage page;
  
  omAssume(d_addr != NULL && omIsDebugAddr(d_addr));

  page = (omBinPage) omGetPageOfAddr((void*) d_addr);
  omAssume(omIsBinPage(page));

  omUnsetDebugOfUsedBlocks(page->used_blocks);

  om_JustFreedPage = NULL;

  ___omFreeBin(d_addr);

  if (page != om_JustFreedPage)
    omSetDebugOfUsedBlocks(page->used_blocks);
}

/*******************************************************************
 *  
 * Second level omCheck routines: do the actual checks
 *  
 *******************************************************************/

#define omIsAddrInValidRange(addr) 1

omError_t omCheckAddr(void* addr, void* bin_size, int flags, char level, char* f, const int l, void* r)
{
  if (addr == NULL) 
    return omReportAddrError(omError_NullAddr, addr, bin_size, flags, f, l, r, "");
  
  if (((long) addr  & (SIZEOF_OM_ALIGNMENT - 1)))
    return omReportAddrError(omError_UnalignedAddr, addr, bin_size, flags, f, l, r,  "");

  if ((flags & OM_ALIGN_FLAG) &&  ((long) addr  & (SIZEOF_OM_STRICT_ALIGNMENT - 1)))
    return omReportAddrError(omError_UnalignedAddr, addr, bin_size, flags, f, l, r,  "");

  if (! omIsAddrInValidRange(addr))
    return omReportAddrError(omError_InvalidRange, addr, bin_size, flags, f, l, r,  "");

  if (omIsBinPageAddr(addr))
  {
    if (! omCheckPageHeader(omGetPageOfAddr(addr))) 
      return omReportAddrError(omError_MemoryCorrupted, addr, bin_size, flags, f, l, r,  
                               "Page Header corrupted");

    if (omIsDebugAddr(addr))
    {
      omDebugAddr d_addr = omAddr_2_DebugAddr(addr);
      if (d_addr->debug < 0 || d_addr->debug > OM_DEBUG_MAX)
        return omReportAddrError(omError_MemoryCorrupted, addr, bin_size, flags, f, l, r, 
                                 "debug:%d out of range", d_addr->debug);
      if (omDebugAddr_2_OutAddr(d_addr) != addr)
        return omReportAddrError(omError_FalseAddr, addr, bin_size, flags, f, l, r, 
                                 "omDebugAddr_2_OutAddr(d_addr):%d  != addr:%d",
                                 omDebugAddr_2_OutAddr(d_addr), addr);
      if (! omCheckDebugAddr(d_addr, bin_size, flags, level, f, l, r))
        return om_ErrorStatus;
      
      if (level > 1 && 
          !omCheckBinAddr(d_addr, NULL, (flags &~ OM_SIZE_FLAG) &~ OM_BIN_FLAG , level, f, l, r))
        return om_ErrorStatus;
      else
        return om_ErrorStatus = omError_NoError;
    }
    else
    {
      return omCheckBinAddr(addr, bin_size, flags, level, f, l, r);
    }
  }
  else
  {
    return omCheckLargeAddr(addr, bin_size, flags, level, f, l, r);
  }
}

omError_t omCheckLargeAddr(void* addr, void* bin_size, int flags, char level, char* f, const int l, void* r)
{
  


      
#endif /* OM_HAVE_DEBUG */
