/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_iter.cc,v 1.4 2007-08-07 14:11:01 Singular Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_iter.h"
#include "int_cf.h"
#include "int_poly.h"


CFIterator::CFIterator()
{
    data = 0; cursor = 0;
    ispoly = false; hasterms = false;
}

CFIterator::CFIterator( const CFIterator & i )
{
    data = i.data;
    cursor = i.cursor;
    ispoly = i.ispoly;
    hasterms = i.hasterms;
}

CFIterator::CFIterator( const CanonicalForm & f )
{
    if ( f.inBaseDomain() || f.inQuotDomain() )
    {
        data = f; cursor = 0;
        ispoly = false; hasterms = true;
    }
    else
    {
        data = f;
        cursor = ((InternalPoly*)(f.value))->firstTerm;
        ispoly = true; hasterms = true;
    }
}

CFIterator::CFIterator( const CanonicalForm & f, const Variable & v )
{
    ASSERT( !f.inQuotDomain(), "illegal iterator" );
    ASSERT( v.level() > 0, "illegal iterator" );
    if ( f.inBaseDomain() )
    {
        data = f; cursor = 0;
        ispoly = false; hasterms = true;
    }
    else
    {
        if ( f.mvar() == v )
        {
            data = f;
            cursor = ((InternalPoly*)(f.value))->firstTerm;
            ispoly = true; hasterms = true;
        }
        else  if ( v > f.mvar() )
        {
            data = f; cursor = 0;
            ispoly = false; hasterms = true;
        }
        else
        {
            data = swapvar( f, v, f.mvar().next() );
            if ( data.mvar() == f.mvar().next() )
            {
                cursor = ((InternalPoly*)(data.value))->firstTerm;
                ispoly = true; hasterms = true;
            }
            else
            {
                cursor = 0;
                ispoly = false; hasterms = true;
            }
        }
    }
}

CFIterator::~CFIterator()
{
    data = 0; cursor = 0;
}

CFIterator&
CFIterator::operator= ( const CFIterator & i )
{
    if ( this != &i )
    {
        data = i.data;
        cursor = i.cursor;
        ispoly = i.ispoly;
        hasterms = i.hasterms;
    }
    return *this;
}

CFIterator&
CFIterator::operator= ( const CanonicalForm & f )
{
    if ( f.inBaseDomain() || f.inQuotDomain() )
    {
        data = f; cursor = 0;
        ispoly = false; hasterms = true;
    }
    else
    {
        data = f;
        cursor = ((InternalPoly*)(f.value))->firstTerm;
        ispoly = true; hasterms = true;
    }
    return *this;
}
