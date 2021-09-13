#include "omtTest.h"
#define OM_CHECK 1
#include "omalloc.h"

#ifdef HAVE_OMALLOC
struct LongSpec
{
  int MaxSize;
  int MinSize;
  int MinTrack;
  int NotIsBin;
  int NoTrack;
  int NotZero;
};

omMemCell omFindCell(struct LongSpec spec)
{
  int i;
  for (i=0; i<MAX_CELLS; i++)
  {
    if ((cells[i].addr != NULL) &&
        (spec.MinTrack == 0 || (DO_CHECK(cells[i].spec) &&
                                DO_TRACK(cells[i].spec) &&
                                GET_TRACK(cells[i].spec) >= spec.MinTrack)) &&
        (spec.MinSize == 0  || GET_SIZE(cells[i].spec) >= spec.MinSize) &&
        (spec.MaxSize == 0  || GET_SIZE(cells[i].spec) <= spec.MaxSize) &&
        (spec.NotIsBin == 0 || cells[i].bin == NULL) &&
        (spec.NotZero == 0  || !IS_ZERO(cells[i].spec)) &&
        (spec.NoTrack == 0  || !DO_CHECK(cells[i].spec) || !DO_TRACK(cells[i].spec)))
    {
      return &cells[i];
    }
  }
  return NULL;
}

int omtTestErrors()
{
#ifndef OM_NDEBUG
  int level = om_Opts.MinCheck;
  omError_t error;
  struct LongSpec spec;
  int missed = 0, alloc;
  omMemCell cell = NULL;

  printf("omtTestErrors: Start\n");
  om_Opts.MinCheck = 2;
  for (error = omError_MemoryCorrupted; error < omError_MaxError; error++)
  {
    om_InternalErrorStatus = omError_NoError;
    om_ErrorStatus = omError_NoError;
    printf("!!!expect %s\n", omError2Serror(error));
    memset(&spec, 0, sizeof(struct LongSpec));
    cell = NULL;
    alloc = 0;
    switch (error)
    {
        case omError_MemoryCorrupted:
        {
          spec.MaxSize = OM_MAX_BLOCK_SIZE;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omBin bin = omGetTopBinOfAddr(cell->addr);
            omBinPage last_page = bin->last_page;
            omAssume(last_page != NULL);
            bin->last_page = NULL;
            omDebugBin(cell->bin);
            bin->last_page = last_page;
          }
          else printf("cell not found\n");
          break;
        }

        case omError_NullAddr:
        {
          omDebugAddr(NULL);
          break;
        }

        case omError_InvalidRangeAddr:
        {
          omDebugAddr((void*) om_MaxAddr);
          break;
        }

        case omError_FalseAddr:
        {
          spec.MinSize = 8;
          spec.MaxSize = OM_MAX_BLOCK_SIZE;
          spec.NoTrack = 1;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugAddr(cell->addr + SIZEOF_VOIDP);
          }
          else printf("cell not found\n");
          break;
        }

        case omError_FalseAddrOrMemoryCorrupted:
        {
          spec.MinSize = 8;
          spec.MinTrack = 3;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugAddr(cell->addr + SIZEOF_VOIDP);
          }
          else printf("cell not found\n");
          break;
        }

        case omError_WrongSize:
        {
          spec.MaxSize = OM_MAX_BLOCK_SIZE;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugAddrSize(cell->addr, OM_MAX_BLOCK_SIZE + 1);
            if (om_ErrorStatus != omError_NoError)
            {
              om_ErrorStatus = omError_NoError;
              spec.MaxSize = 0;
              spec.MinTrack = 3;
              spec.NotIsBin = 1;
              spec.MinSize = 2;
              cell = omFindCell(spec);
              if (cell != NULL)
              {
                omDebugAddrSize(cell->addr, GET_SIZE(cell->spec) + SIZEOF_OM_ALIGNMENT);
                if (om_ErrorStatus != omError_NoError)
                {
                  om_ErrorStatus = omError_NoError;
                  spec.MaxSize = OM_MAX_BLOCK_SIZE;
                  spec.MinTrack = 0;
                  spec.NotIsBin = 0;
                  cell = omFindCell(spec);
                  if (cell != NULL)
                    omDebugBinAddrSize(cell->addr, GET_SIZE(cell->spec) - 1);
                  else printf("cell not found");
                }
              }
              else printf("cell not found\n");
            }
          }
          else printf("cell not found\n");
          break;
        }

        case omError_FreedAddr:
        {
#if KEEP_LEVEL > 0
          void* addr = om_AlwaysKeptAddrs;
          while (addr != NULL && omIsTrackAddr(addr))
          {
            addr = *((void**) addr);
          }
          if (addr != NULL)
          {
            omFree(addr);
            if (om_ErrorStatus == omError_FreedAddr)
            {
              om_ErrorStatus = omError_NoError;
              addr = om_AlwaysKeptAddrs;
               while (addr != NULL && ! omIsTrackAddr(addr))
               {
                 addr = *((void**) addr);
               }
               if (addr != NULL)
               {
                 addr = omAddr_2_OutAddr(addr);
                 omFree(addr);
               }
            }
          }
          if (addr == NULL)
          {
            printf("addr not found\n");
            break;
          }
          if (om_ErrorStatus != omError_FreedAddr)
            break;
#endif
          spec.MinTrack = 5;
          spec.NotIsBin = 1;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omFree(cell->addr);
            omFree(cell->addr);
            alloc = 1;
            cell->addr=NULL;
          }
          else printf("cell not found\n");
          break;
        }

        case omError_FreedAddrOrMemoryCorrupted:
        {
          spec.MaxSize = OM_MAX_BLOCK_SIZE;
          spec.NoTrack = 1;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omBinPage page = omGetBinPageOfAddr(cell->addr);
            omBinPageRegion region = page->region;
            page->region = NULL;
            om_Opts.MinCheck = 1;
            omDebugAddr(cell->addr);
            om_Opts.MinCheck = 2;
            page->region = region;
          }
          else printf("cell not found\n");
          break;
        }

        case omError_WrongBin:
        {
          spec.MaxSize = 32;
          spec.NoTrack = 1;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugAddrBin(cell->addr, &om_StaticBin[OM_MAX_BIN_INDEX]);
          }
          else printf("cell not found\n");
          break;
        }

        case omError_UnknownBin:
        {
          spec.MaxSize = OM_MAX_BLOCK_SIZE;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugAddrBin(cell->addr, (void*) omGetTopBinOfAddr(cell->addr) + SIZEOF_VOIDP);
          }
          else printf("cell not found\n");
          break;
        }

        case omError_NotBinAddr:
        {
          spec.NotIsBin = 1;
          spec.MinSize = OM_MAX_BLOCK_SIZE + 1;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugBinAddr(cell->addr);
          }
          else printf("cell not found");
          break;
        }

        case omError_UnalignedAddr:
        {
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            omDebugAddr(cell->addr + 1);
          }
          else printf("cell not found\n");
          break;
        }

        case omError_NullSizeAlloc:
        {
          void* addr = omAlloc(0);
          addr = omRealloc(addr, 0);
          omFree(addr);
          break;
        }

        case omError_ListCycleError:
        {
          void* last = omListLast(om_SpecBin);
          if (last != NULL)
          {
            *((void**)last) = om_SpecBin;
            omCheckList(om_SpecBin, 5, omError_NoError, OM_FLR);
            *((void**)last) = NULL;
          } else printf("last == NULL\n");
          break;
        }

        case omError_SortedListError:
        {
          if (om_SpecBin != NULL && om_SpecBin->next != NULL)
          {
            int max_blocks = om_SpecBin->max_blocks;
            om_SpecBin->max_blocks = om_SpecBin->next->max_blocks + 1;
            omCheckSortedList(om_SpecBin, max_blocks, 5, omError_NoError, OM_FLR);
            om_SpecBin->max_blocks = max_blocks;
          } else printf("om_SpecBin->next == NULL\n");
          break;
        }

        case omError_KeptAddrListCorrupted:
        {
          if (om_KeptAddr != NULL)
          {
            void* last = omListLast(om_KeptAddr);
            *((void**)last) = om_KeptAddr;
            om_Opts.MinCheck = 5;
            omDebugMemory();
            om_Opts.MinCheck = 2;
            *((void**)last) = NULL;
          }
          else printf("om_KeptAddr == NULL\n");
          break;
        }

        case omError_FreePattern:
        {
          if (om_Opts.Keep > 0)
          {
            spec.MinTrack=3;
            spec.NotIsBin = 1;
            cell = omFindCell(spec);
            if (cell != NULL)
            {
              void* value;
              omFree(cell->addr);
              value = *((void**) cell->addr);
              *((void**) cell->addr) = value -1;
              omDebugMemory();
              *((void**) cell->addr) = value;
              alloc = 1;
              cell->addr = NULL;
            }
            else printf("cell not found\n");
          }
          else printf("om_Opts.Keep == 0");
          break;
        }

        case omError_BackPattern:
        {
          spec.MinTrack = 3;
          spec.NotIsBin = 1;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            void* addr = cell->addr + omSizeOfAddr(cell->addr);
            void* value = *((void**) addr);
            *((void**) addr) = value -1;
            omDebugAddr(cell->addr);
            *((void**) addr) = value;
          }
          else printf("cell not found\n");
          break;
        }

        case omError_FrontPattern:
        {
          spec.MinTrack=3;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            void* addr = cell->addr - SIZEOF_VOIDP;
            void* value = *((void**) addr);
            *((void**) addr) = value -1;
            omDebugAddr(cell->addr);
            *((void**) addr) = value;
          }
          else printf("cell not found\n");
          break;
        }

        case omError_NotString:
        {
/* can only test for NULL string */
#if 0
          spec.MaxSize = OM_MAX_BLOCK_SIZE;
          cell = omFindCell(spec);
          if (cell != NULL)
          {
            char* addr = (char*) cell->addr;
            char* s;
            memset(cell->addr, 1, omSizeOfAddr(cell->addr));
            omDebugAddr(cell->addr);
            s = omStrDup(addr);
            omFree(s);
            InitCellAddrContent(cell);
          }
          else printf("cell not found\n");
          break;
#endif
          omStrDup(NULL);
          break;
        }

        case omError_StickyBin:
        {
          omMergeStickyBinIntoBin(NULL, NULL);
          break;
        }

        default:
          printf("No Error test implemented\n");
    }

    if (om_ErrorStatus != error)
    {
      printf("---missed %s\n", omError2Serror(error));
      missed++;
    }
    else
    {
      printf("+++ok  %s\n", omError2Serror(error));
    }

    om_ErrorStatus = omError_NoError;
    if (cell != NULL)
    {
      if (alloc) TestAlloc(cell, cell->spec);
      omtTestDebug(cell);
    }
    else
    {
      omDebugMemory();
    }
    if (om_ErrorStatus != omError_NoError)
    {
      printf("omtTest panik: memory corrupted\n\n");
      return -1;
    }
    printf("\n");
  }
  printf("omtTestErrors: Summary: missed = %d\n\n", missed);
  om_Opts.MinCheck = level;
  return missed;
#else
  return 0;
#endif
}
#endif
