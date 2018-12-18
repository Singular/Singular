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

#include "omalloc/omConfig.h"
#ifndef HAVE_OMALLOC
#include "omalloc/xalloc.h"
#else

#ifdef __cplusplus
extern "C" {
  #if __cplusplus >= 201402L
  /* clang 3.7, gcc 5.1 sets 201402L */
  #define REGISTER
  #elif defined(__clang__)
  #define REGISTER
  #else
  #define REGISTER register
  #endif
#else
  #define REGISTER register
#endif


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
#include "omalloc/omTables.h"
#include "omAllocPrivate.h"
#include "omDebug.h"
#include "omInline.h"
#include "omAllocFunc.h"

#ifdef __cplusplus
}
#endif

#endif /* HAVE_OMALLOC */
#endif /* OM_ALLOC_H */
