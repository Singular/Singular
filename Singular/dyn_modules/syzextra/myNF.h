// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file myNF.h
 * 
 * NF which uses pLength instead of pSize!
 *
 * ABSTRACT: NF using length
 *
 * @author Oleksandr Motsak
 *
 *
 **/
/*****************************************************************************/

#ifndef MYNF_H
#define MYNF_H

// include basic definitions
#include "singularxx_defs.h"

struct  spolyrec;
typedef struct spolyrec    polyrec;
typedef polyrec *          poly;

struct ip_sring;
typedef struct ip_sring *         ring;

struct sip_sideal;
typedef struct sip_sideal *       ideal;

BEGIN_NAMESPACE_SINGULARXX    BEGIN_NAMESPACE(NF)

/// high-level functio, which calls kNF2Length(redNFLength)
poly kNFLength(ideal F, ideal Q, poly p, int syzComp, int lazyReduce);

END_NAMESPACE               END_NAMESPACE_SINGULARXX

#endif 
/* #ifndef MYNF_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab




