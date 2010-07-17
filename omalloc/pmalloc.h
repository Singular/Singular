/*******************************************************************
 *  File:    omMallocSystem.h
 *  Purpose: declaration of macros for malloc to be used from the system
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_MALLOC_SYSTEM_H
#define OM_MALLOC_SYSTEM_H

#include <omalloc/omConfig.h>

/* define to -1 if you want that this implementation provides
   malloc/calloc/realloc/free funcs */
#ifndef OM_PROVIDE_MALLOC
#define OM_PROVIDE_MALLOC 0
#endif

#if OM_PROVIDE_MALLOC != -1
#define calloc  cALLOc		
#define free    fREe		
#define malloc  mALLOc		
#define realloc rEALLOc
#endif

/* this is the minimal set of OM_MALLOC_* macros which must be defined */
#define OM_MALLOC_MALLOC   malloc
#define OM_MALLOC_REALLOC  realloc
#define OM_MALLOC_FREE     free
#define OM_MALLOC_VFREE(addr, size) OM_MALLOC_FREE(addr)

#endif /* OM_MALLOC_SYSTEM_H */
