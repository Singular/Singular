/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#define MORE_ZEROES

#include <config.h>

#include "cf_assert.h"

#include "cf_defs.h"
#include "cf_reval.h"


REvaluation::REvaluation( const REvaluation & e )
{
    if ( e.gen == 0 )
        gen = 0;
    else
        gen = e.gen->clone();
    values = e.values;
}

REvaluation::~REvaluation()
{
    if ( gen != 0 )
        delete gen;
}

REvaluation&
REvaluation::operator= ( const REvaluation & e )
{
    if ( this != &e ) {
        if (gen!=0)
          delete gen;
        values = e.values;
        if ( e.gen == 0 )
            gen = 0;
        else
            gen = e.gen->clone();
    }
    return *this;
}

void
REvaluation::nextpoint()
{
    int n = values.max();
    for ( int i = values.min(); i <= n; i++ )
        values[i] = gen->generate();
}

void
REvaluation::nextpoint (int n)
{
  int m= values.max();
  int t= values.min();
  for (int i= t; i <= m; i++)
    values [i]= 0;

  if (m == t)
  {
    values [t]= gen->generate();
    return;
  }
  for (int i= 0; i < n; i++)
  {
    int l= factoryrandom (m - t + 1) + t;
    values [l]= gen->generate();
  }
}
