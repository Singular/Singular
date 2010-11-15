#ifndef CF_MAP_EXT_H
#define CF_MAP_EXT_H
// -*- c++ -*-
//*****************************************************************************
/** @file cf_map_ext.h
 *
 * @author Martin Lee
 * @date   16.11.2009
 *
 * This file implements functions to map between extensions of finite fields
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************

#include <config.h>

CanonicalForm GFMapUp (const CanonicalForm & F, int k);

CanonicalForm GFMapDown (const CanonicalForm & F, int k);

CanonicalForm
mapUp (const CanonicalForm& F, const Variable& alpha, const Variable& beta,
        const CanonicalForm& prim_elem, const CanonicalForm& im_prim_elem,
        CFList& source, CFList& dest);

CanonicalForm
mapDown (const CanonicalForm& F, const CanonicalForm& prim_elem, const
          CanonicalForm& im_prim_elem, const Variable& alpha, CFList& source,
          CFList& dest);

CanonicalForm
primitiveElement (const Variable& alpha, Variable& beta, bool fail);

CanonicalForm
mapPrimElem (const CanonicalForm& prim_elem, const Variable& alpha,
             const Variable& beta);

CanonicalForm GF2FalphaRep (const CanonicalForm& F, const Variable& alpha);

CanonicalForm Falpha2GFRep (const CanonicalForm& F);
#endif
