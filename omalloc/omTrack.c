/*******************************************************************
 *  File:    omTrack.c
 *  Purpose: routines for getting Backtraces of stack
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omTrack.c,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/
#include <limits.h>

#include "omConfig.h"
#include "omFindExec.h"
#include "omPrivate.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

static char* om_this_prog = NULL;
static void* om_this_main_frame_addr = NULL;
static void* om_this_prog_min_return_addr = ((void*) 1023);
static void* om_this_prog_max_return_addr = ((void*) ULONG_MAX -1);


void omInitTrack(const char* argv0)
{
  char buf[MAXPATHLEN];
  
  if (argv0 != NULL && omFindExec(argv0, buf))
  {
    __omTypeAllocChunk(char*, om_this_prog, strlen(buf));
    strcpy(om_this_prog, buf);
  }
#if defined(OM_FRAME_ADDR_WORKS) 
  om_this_main_frame_addr = OM_FRAME_ADDR(1);
#endif
#if defined(OM_PROG_NM) && defined(HAVE_POPEN)
  if (om_this_prog != NULL)
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

#define OM_MAX_TRACK_FRAMES 11
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


int omGetCurrentBackTrace(void** addr, int max_frames)
{
#if defined(OM_RETURN_ADDR_WORKS)
#if defined(OM_FRAME_ADDR_WORKS)
  if (om_this_main_frame_addr != NULL)
  {
    void* this_frame = OM_FRAME_ADDR(0);
    if (this_frame == 0) return 0;
    else
    {
#endif /* ! defined(OM_RETURN_ADDR_WORKS) */
      int i;
      void* r_addr;
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
        /* check that next frame is in really between main and this_frame */
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
  return max_frames;
#endif /* defined(OM_RETURN_ADDR_WORKS) */
  return 0;
}

int omPrintBackTrace(void** addr, int max_frames, FILE* fd)
{
  int i = 0;
#if defined(HAVE_POPEN) && defined(OM_PROG_ADDR2LINE)
  if (om_this_prog != NULL)
  {
    char command[2*MAXPATHLEN + 15 + OM_MAX_TRACK_FRAMES*(2*SIZEOF_VOIDP + 4)];
    FILE *pipe;
    int l;

    l = sprintf(command, "%s -s -C -f -e %s", 
                     OM_PROG_ADDR2LINE, om_this_prog);

    while (i<max_frames && addr[i] != NULL)
    {
      l += sprintf(&command[l], " %p", addr[i]);
      i++;
    }
    
    if (i==0) return 0;
    pipe = popen(command, "r");
    if (pipe != NULL)
    {
      int nl = '@';
      while ((l=fgetc(pipe)) != EOF)
      {
        if (l == '\n')
        {
          l =fgetc(pipe);
          if (l == EOF) break;
          fputc(nl, fd);
          if (nl == '@') nl = '#';
          else nl = '@';
        }
        fputc(l, fd);
      }
      pclose(pipe);
      if (i > 0)
      {
        fprintf(fd, "%d", i);
        fputc('\n', fd);
        fflush(fd);
      }
      return i;
    }
    i=0;
  }
#endif
  while (i<max_frames && addr[i] != NULL)
  {
    fprintf(fd, "%p#", addr[i]);
    i++;
  }
  if (i > 0)
  {
    fputc('\n', fd);
    fflush(fd);
  }
  return i;
}

int omPrintCurrentBackTrace(int from_frame, int max_frames, FILE *fd)
{
#if defined(OM_RETURN_ADDR_WORKS)
  if (from_frame >= 0)
  {
    void* addr[OM_MAX_TRACK_FRAMES];
    int got_frames = omGetCurrentBackTrace(addr, from_frame + max_frames + 1);
    return omPrintBackTrace(&addr[from_frame + 1], got_frames, fd);
  }
#endif
  return 0;
}

  

    
    
      
    

