/*******************************************************************
 *  File:    omMallocSystem.h
 *  Purpose: declaration of macros for malloc to be used from the system
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_MALLOC_H
#define OM_MALLOC_H

#include <omalloc/omConfig.h>

#ifdef OMALLOC_USES_SYSTEM_MALLOC
#include "omalloc/omMallocSystem.h"
#endif

#ifdef OMALLOC_USES_EXTERNAL_MALLOC
#include EXTERNAL_MALLOC_HEADER
#endif

#ifdef OMALLOC_USES_PMALLOC
#include "omalloc/pmalloc.h"
#endif

#ifdef OMALLOC_USES_GMALLOC
#include "omalloc/gmalloc.h"
#endif

#ifdef OMALLOC_USES_DLMALLOC
#include "omalloc/dlmalloc.h"
#endif

#endif /* OM_MALLOC_H */

