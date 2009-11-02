/*******************************************************************
 *  File:    omMmap.c
 *  Purpose: implementing valloc via mmap
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

static void* omVallocMmap(size_t size)
{
  void* addr;
#ifndef MAP_ANONYMOUS
  static int fd = -1;
#endif

#ifdef MAP_ANONYMOUS
#ifndef __CYGWIN__
  /* under cygwin, MAP_PRIVATE|MAP_ANONYMOUS fails */
  addr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#else
  /* however, the following works */
  addr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, -1, 0);
#endif
#else /* !MAP_ANONYMOUS */
  if (fd < 0)
  {
    fd = open("/dev/zero", O_RDWR);
    if (fd < 0) return NULL;
  }
  addr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
#endif

  if (addr == (void*) -1) return NULL;
  return addr;
}

static int omVfreeMmap(void* addr, size_t size)
{
  return munmap(addr, size);
}
