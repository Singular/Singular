/*******************************************************************
 *  File:    omTrack.c
 *  Purpose: routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omTrack.c,v 1.3 2000-05-31 13:34:32 obachman Exp $
 *******************************************************************/
#include <limits.h>
#include "omConfig.h"
#include "omFindExec.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

/* This is for nice alignment of omPrintBackTrace */
#define OM_MAX_PROC_NAME_LENGTH 20
/* if you make this larger than 11, extend OM_GET_RETURN_ADDR and 
   OM_GET_FRAME_ADDR */
#define OM_MAX_BT_FRAMES 11

#ifdef OM_RETURN_ADDR_RVALUE
#define OM_GET_RETURN_ADDR OM_RETURN_ADDR
#else
#define OM_GET_RETURN_ADDR(addr, i)             \
switch(i)                                       \
{                                               \
    case 0:                                     \
      addr = OM_RETURN_ADDR(0); break;          \
    case 1:                                     \
      addr = OM_RETURN_ADDR(1); break;          \
    case 2:                                     \
      addr = OM_RETURN_ADDR(2); break;          \
    case 3:                                     \
      addr = OM_RETURN_ADDR(3); break;          \
    case 4:                                     \
      addr = OM_RETURN_ADDR(4); break;          \
    case 5:                                     \
      addr = OM_RETURN_ADDR(5); break;          \
    case 6:                                     \
      addr = OM_RETURN_ADDR(6); break;          \
    case 7:                                     \
      addr = OM_RETURN_ADDR(7); break;          \
    case 8:                                     \
      addr = OM_RETURN_ADDR(8); break;          \
    case 9:                                     \
      addr = OM_RETURN_ADDR(9); break;          \
    case 10:                                    \
      addr = OM_RETURN_ADDR(10); break;         \
    default:                                    \
      addr = NULL;                              \
}
#endif /* OM_RETURN_ADDR_RVALUE */

#ifdef OM_FRAME_ADDR_RVALUE
#define OM_GET_FRAME_ADDR OM_FRAME_ADDR
#else
#define OM_GET_FRAME_ADDR(addr, i)              \
switch(i)                                       \
{                                               \
    case 0:                                     \
      addr = OM_FRAME_ADDR(0); break;           \
    case 1:                                     \
      addr = OM_FRAME_ADDR(1); break;           \
    case 2:                                     \
      addr = OM_FRAME_ADDR(2); break;           \
    case 3:                                     \
      addr = OM_FRAME_ADDR(3); break;           \
    case 4:                                     \
      addr = OM_FRAME_ADDR(4); break;           \
    case 5:                                     \
      addr = OM_FRAME_ADDR(5); break;           \
    case 6:                                     \
      addr = OM_FRAME_ADDR(6); break;           \
    case 7:                                     \
      addr = OM_FRAME_ADDR(7); break;           \
    case 8:                                     \
      addr = OM_FRAME_ADDR(8); break;           \
    case 9:                                     \
      addr = OM_FRAME_ADDR(9); break;           \
    case 10:                                    \
      addr = OM_FRAME_ADDR(10); break;          \
    default:                                    \
      addr = NULL;                              \
}
#endif /* OM_FRAME_ADDR_RVALUE */

static char om_this_prog[MAXPATHLEN] = "";
static void* om_this_main_frame_addr = NULL;
static void* om_this_prog_min_return_addr = ((void*) 1023);
static void* om_this_prog_max_return_addr = ((void*) ULONG_MAX -1);

void omInitTrack(const char* argv0)
{
  char buf[MAXPATHLEN];
  
  if (argv0 != NULL && omFindExec(argv0, buf))
  {
    strcpy(om_this_prog, buf);
  }
#if defined(OM_FRAME_ADDR_WORKS) 
  om_this_main_frame_addr = OM_FRAME_ADDR(1);
#endif
#if defined(OM_PROG_NM) && defined(HAVE_POPEN)
  if (*om_this_prog != '\0')
  {
    char command[MAXPATHLEN + 30];
    FILE *pipe;
    sprintf(command, "%s -n %s", OM_PROG_NM, om_this_prog);
    
    pipe = popen(command, "r");
    if (pipe != NULL)
    {
      /* serach for first address */
      int c;
      void* nm_addr;
      while ( (c=fgetc(pipe)) !=  EOF)
      {
        if (c == '\n')
        {
          if (fscanf(pipe, "%p", &nm_addr) && 
              (unsigned long) nm_addr > 
              (unsigned long) om_this_prog_min_return_addr)
          {
            om_this_prog_min_return_addr = nm_addr;
            break;
          }
        }
      }
      om_this_prog_max_return_addr = nm_addr;
      while ( (c=fgetc(pipe)) !=  EOF)
      {
        if (c == '\n')
        {
          if (fscanf(pipe, "%p", &nm_addr) && nm_addr != NULL &&
              (unsigned long) nm_addr  > 
              (unsigned long) om_this_prog_max_return_addr)
          {
            om_this_prog_max_return_addr = nm_addr;
          }
        }
      }
      pclose(pipe);
    }
  }
#endif /* defined(OM_NM) && defined(HAVE_POPEN) */
}


int omGetCurrentBackTrace(void** addr, int max_frames)
{
  int i = 0;
#if defined(OM_RETURN_ADDR_WORKS)
#if defined(OM_FRAME_ADDR_WORKS)
  if (om_this_main_frame_addr != NULL)
  {
    void* this_frame = OM_FRAME_ADDR(0);
    if (this_frame == 0) return 0;
    else
    {
#endif /* ! defined(OM_RETURN_ADDR_WORKS) */
      void* r_addr;
      if (max_frames > OM_MAX_BT_FRAMES) max_frames = OM_MAX_BT_FRAMES;
      for (i=0; i< max_frames; i++)
      {
        OM_GET_RETURN_ADDR(r_addr, i);
        if (r_addr > om_this_prog_min_return_addr &&
            r_addr < om_this_prog_max_return_addr)
          addr[i] = r_addr;
        else
        {
          addr[i] = NULL;
          return i;
        }
#ifdef OM_FRAME_ADDR_WORKS
        OM_GET_FRAME_ADDR(r_addr, i + 1);
        /* check that next frame is really in between main and this_frame */
        if ((r_addr >= om_this_main_frame_addr && 
             om_this_main_frame_addr >= this_frame) ||
            (r_addr <= om_this_main_frame_addr && 
             om_this_main_frame_addr <= this_frame))
        {
          if (i+1 < max_frames) addr[i+1] = NULL;
          return i + 1;
        }
      }
    }
#endif /* OM_FRAME_ADDR_WORKS */
  }
#endif /* defined(OM_RETURN_ADDR_WORKS) */
  return i;
}

int omPrintBackTrace(void** addr, int max_frames, FILE* fd)
{
  int i = 0;
  if (max_frames > OM_MAX_BT_FRAMES) max_frames = OM_MAX_BT_FRAMES;
#if defined(HAVE_POPEN) && defined(OM_PROG_ADDR2LINE)
  if (*om_this_prog != '\0')
  {
    char command[2*MAXPATHLEN + 15 + OM_MAX_BT_FRAMES*(2*SIZEOF_VOIDP + 4)];
    FILE *pipe;
    int l;
    l = sprintf(command, "%s -s -C -f -e %s", 
                     OM_PROG_ADDR2LINE, om_this_prog);

    while (i<max_frames && addr[i] != NULL)
    {
      l += sprintf(&command[l], " %p", addr[i]);
      i++;
    }
    
    if (i > 0)
    {
      pipe = popen(command, "r");
      if (pipe != NULL)
      {
        int nl = 0;
        int j = 0;
        int k=0;
        while ((l=fgetc(pipe)) != EOF)
        {
          /* An output entry of addr2line looks as follows:
FunctionName
File:Line
             The above is pretty ugly, huh? 
          */
          if (nl == 0) 
          {
            fprintf(fd, "  #%d %p in ", j, addr[j]);
            nl = 1;
            j++;
            k=0;
          }
          if (l == '\n')
          {
            if (nl == 1)
            {
              while (k<OM_MAX_PROC_NAME_LENGTH)
              {
                fprintf(fd, " ");
                k++;
              }
              fprintf(fd, " at ");
              nl = 2;
            }
            else
            {
              fputc('\n', fd);
              nl = 0;
            }
          }
          else
          {
            k++;
            fputc(l, fd);
          }
        }
        pclose(pipe);
        fflush(fd);
        return i;
      }
      i=0;
    }
  }
#endif
#if OM_RETURN_ADDR_WORKS
  while (i<max_frames && addr[i] != NULL)
  {
    fprintf(fd, "  #%d %p in ??\n", i, addr[i]);
    i++;
  }
#endif
  if (i == 0) 
  {
    fprintf(fd, "??");
    fputc('\n', fd);
  }
  fflush(fd);
  return i;
}

int omPrintCurrentBackTrace(int from_frame, int max_frames, FILE *fd)
{
  void* addr[OM_MAX_BT_FRAMES];
  int got_frames = omGetCurrentBackTrace(addr, from_frame + max_frames + 1);
  return omPrintBackTrace(&addr[from_frame + 1], 
                          got_frames - from_frame + 1, fd);
}

  

    
    
      
    

