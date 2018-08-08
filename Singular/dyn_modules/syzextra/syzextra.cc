// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file syzextra.cc
 *
 * New implementations for the computation of syzygies and resolutions
 *
 * ABSTRACT: Computation of Syzygies due to Schreyer
 *
 * @author Oleksandr Motsak
 *
 **/
/*****************************************************************************/
// include header file
#include "kernel/mod2.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE  /*for qsort_r on cygwin, must be before system includes*/
#endif

#include <string.h>


#include "syzextra.h"

#include "misc/intvec.h"

#include "coeffs/coeffs.h"

#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"
#include "polys/simpleideals.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"

#include <stdio.h>
#include <stdlib.h>

poly p_Tail(const poly p, const ring r)
{
  if( UNLIKELY(p == NULL) )
    return NULL;
  else
    return p_Copy( pNext(p), r );
}

ideal id_Tail(const ideal id, const ring r)
{
  if( UNLIKELY(id == NULL) )
    return NULL;

  const ideal newid = idInit(IDELEMS(id),id->rank);

  for (int i=IDELEMS(id) - 1; i >= 0; i--)
    newid->m[i] = p_Tail( id->m[i], r );

  newid->rank = id_RankFreeModule(newid, currRing);

  return newid;
}

