/*******************************************************************
 *  File:    omalloc_provide.c
 *  Purpose: implementation of ANSI-C conforming which are to be provided
 *           by omalloc library
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omalloc_provide.c,v 1.3 2000-08-14 12:26:51 obachman Exp $
 *******************************************************************/
#include "omConfig.h"

#if OM_PROVIDE_MALLOC > 0
#if OM_PROVIDE_MALLOC > 1
#include "omalloc_debug.c"
#else
#include "omalloc.c"
#endif
#endif

