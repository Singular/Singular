/*******************************************************************
 *  File:    om_Alloc.h
 *  Purpose: declaration of public routines for omalloc
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: om_Alloc.h,v 1.5 2006-06-14 14:28:19 Singular Exp $
 *******************************************************************/
#ifndef OM_ALLOC_H
#define OM_ALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*MAKEHEADER*/
#include "omConfig.h"
/*MAKEHEADER*/
#include "omDerivedConfig.h"
/*MAKEHEADER*/
#include "omError.h"
/*MAKEHEADER*/
#include "omStructs.h"
/*MAKEHEADER*/
#include "omAllocDecl.h"
/*MAKEHEADER*/
#include "omInlineDecl.h"
/*MAKEHEADER*/
#include "omBin.h"
/*MAKEHEADER*/
#include "omMemOps.h"
/*MAKEHEADER*/
#include "omList.h"
/*MAKEHEADER*/
#include "omFindExec.h"
/*MAKEHEADER*/
#include "omGetBackTrace.h"
/*MAKEHEADER*/
#include "omRet2Info.h"
/*MAKEHEADER*/
#include "omStats.h"
/*MAKEHEADER*/
#include "omOpts.h"
/*MAKEHEADER*/
#include "omBinPage.h"
/*MAKEHEADER*/
#include "omAllocSystem.h"
/*MAKEHEADER*/
#include "omTables.h"
/*MAKEHEADER*/
#include "omAllocPrivate.h"
/*MAKEHEADER*/
#include "omDebug.h"
/*MAKEHEADER*/
#include "omInline.h"
/*MAKEHEADER*/
#include "omAllocFunc.h"

#ifdef __cplusplus
}
#endif

#endif /* OM_ALLOC_H */
