/*******************************************************************
 *  File:    omalloc_provide.c
 *  Purpose: implementation of ANSI-C conforming malloc functions
 *            which are to be provided by omalloc library
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omalloc_provide.c,v 1.5 2001-04-30 09:02:13 Singular Exp $
 *******************************************************************/
#include "omConfig.h"

#if OM_PROVIDE_MALLOC > 0
#if OM_PROVIDE_MALLOC > 1
#include "omalloc_debug.c"
#else
#include "omalloc.c"
#endif
#endif
