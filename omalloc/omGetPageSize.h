/*******************************************************************
 *  File:    omGetPageSize.h
 *  Purpose: figure out how to get the pagesize
 *  This is adapted from dlmalloc's mechanisms which in fact derived it from
 *  bsd/gnu getpagesize.h
 *  Version: $Id$
 *******************************************************************/
#include <unistd.h>

#ifndef omalloc_getpagesize
#  ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
#    ifndef _SC_PAGE_SIZE
#      define _SC_PAGE_SIZE _SC_PAGESIZE
#    endif
#  endif
#  ifdef _SC_PAGE_SIZE
#    define omalloc_getpagesize sysconf(_SC_PAGE_SIZE)
#  else
#    if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
       extern size_t getpagesize();
#      define omalloc_getpagesize getpagesize()
#    else
#      include <sys/param.h>
#      ifdef EXEC_PAGESIZE
#        define omalloc_getpagesize EXEC_PAGESIZE
#      else
#        ifdef NBPG
#          ifndef CLSIZE
#            define omalloc_getpagesize NBPG
#          else
#            define omalloc_getpagesize (NBPG * CLSIZE)
#          endif
#        else
#          ifdef NBPC
#            define omalloc_getpagesize NBPC
#          else
#            ifdef PAGESIZE
#              define omalloc_getpagesize PAGESIZE
#            else
#              define omalloc_getpagesize (4096) /* just guess */
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
