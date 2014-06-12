/*******************************************************************
 *  File:    omalloc.h
 *  Purpose: declaration of public routines for omalloc
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#ifndef OM_ALLOC_H
#define OM_ALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <omalloc/omConfig.h>

#if defined(OM_NDEBUG) && !defined(OM_ALLOC_INTERNAL)
#if (SIZEOF_LONG == 8)
#define OM_T_FREE1
#define OM_T_FREE3
#define OM_T_STR
#define OM_T_ALLOC
#define OM_T_REALLOC
#endif
#endif

#include "omDerivedConfig.h"
#include "omError.h"
#include "omStructs.h"
#include "omAllocDecl.h"
#include "omInlineDecl.h"
#include "omBin.h"
#include "omMemOps.h"
#include "omList.h"
#include "omGetBackTrace.h"
#include "omRet2Info.h"
#include "omStats.h"
#include "omOpts.h"
#include "omBinPage.h"
#include "omAllocSystem.h"
#include <omalloc/omTables.h>
#include "omAllocPrivate.h"
#include "omDebug.h"
#include "omInline.h"
#include "omAllocFunc.h"

#ifdef __cplusplus
}
#endif

#endif /* OM_ALLOC_H */
