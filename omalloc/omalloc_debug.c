/*******************************************************************
 *  File:    omalloc_debug.c
 *  Purpose: implementation of ANSI-C conforming malloc functions
 *           -- debug versions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#include <omalloc/omDefaultConfig.h>

#define OM_TRACK OM_DEFAULT_MIN_TRACK
#define OM_CHECK OM_DEFAULT_MIN_CHECK

#if OM_CHECK <= 0
#undef OM_CHECK
#define OM_CHECK 1
#endif

#include <omalloc/omalloc.c>
