/*******************************************************************
 *  File:    om_Alloc.h
 *  Purpose: declaration of public routines for omalloc
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
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
#include <omalloc/omConfig.h>
/*MAKEHEADER*/
#include <omalloc/omDerivedConfig.h>
/*MAKEHEADER*/
#include <omalloc/omError.h>
/*MAKEHEADER*/
#include <omalloc/omStructs.h>
/*MAKEHEADER*/
#include <omalloc/omAllocDecl.h>
/*MAKEHEADER*/
#include <omalloc/omInlineDecl.h>
/*MAKEHEADER*/
#include <omalloc/omBin.h>
/*MAKEHEADER*/
#include <omalloc/omMemOps.h>
/*MAKEHEADER*/
#include <omalloc/omList.h>
/*MAKEHEADER*/
#include <omalloc/omFindExec.h>
/*MAKEHEADER*/
#include <omalloc/omGetBackTrace.h>
/*MAKEHEADER*/
#include <omalloc/omRet2Info.h>
/*MAKEHEADER*/
#include <omalloc/omStats.h>
/*MAKEHEADER*/
#include <omalloc/omOpts.h>
/*MAKEHEADER*/
#include <omalloc/omBinPage.h>
/*MAKEHEADER*/
#include <omalloc/omAllocSystem.h>
/*MAKEHEADER*/
#include <omalloc/omTables.h>
/*MAKEHEADER*/
#include <omalloc/omAllocPrivate.h>
/*MAKEHEADER*/
#include <omalloc/omDebug.h>
/*MAKEHEADER*/
#include <omalloc/omInline.h>
/*MAKEHEADER*/
#include <omalloc/omAllocFunc.h>

#ifdef __cplusplus
}
#endif

#endif /* OM_ALLOC_H */
