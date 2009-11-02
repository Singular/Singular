/*******************************************************************
 *  File:    omStructs.h
 *  Purpose: declaration of structs and typedefs of omalloc
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_STRUCTS_H
#define OM_STRUCTS_H

struct omBin_s;
typedef struct omBin_s      omBin_t;
typedef omBin_t*            omBin;

struct omBinPage_s;
typedef struct omBinPage_s  omBinPage_t;
typedef omBinPage_t*        omBinPage;

struct omBinPageRegion_s;
typedef struct omBinPageRegion_s omBinPageRegion_t;
typedef omBinPageRegion_t* omBinPageRegion;

struct omOpts_s;
typedef struct omOpts_s omOpts_t;

struct omInfo_s;
typedef struct omInfo_s omInfo_t;

struct omSpecBin_s;
typedef struct omSpecBin_s omSpecBin_t;
typedef omSpecBin_t*        omSpecBin;

struct omRetInfo_s;
typedef struct omRetInfo_s omRetInfo_t;
typedef omRetInfo_t*        omRetInfo;

#endif /* OM_STRUCTS_H */
