/*******************************************************************
 *  File:    om_Alloc.h
 *  Purpose: declaration of public routines for omalloc  
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: om_Alloc.h,v 1.2 2001-01-27 17:03:41 obachman Exp $
 *******************************************************************/
#ifndef OM_ALLOC_H
#define OM_ALLOC_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "omConfig.h"
#include "omDerivedConfig.h"
#include "omError.h"
#include "omStructs.h"
#include "omAllocDecl.h"
#include "omInlineDecl.h"
#include "omBin.h"
#include "omMemOps.h"
#include "omList.h"
#include "omFindExec.h"
#include "omGetBackTrace.h"
#include "omRet2Info.h"
#include "omStats.h"
#include "omOpts.h"
#include "omBinPage.h"
#include "omAllocSystem.h"
#include "omTables.h"
#include "omAllocPrivate.h"
#include "omDebug.h"
#include "omInline.h"
#include "omAllocFunc.h"

#ifdef __cplusplus
}
#endif

#endif /* OM_ALLOC_H */
