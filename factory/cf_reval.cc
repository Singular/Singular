/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_reval.cc,v 1.6 2005-08-24 13:31:55 Singular Exp $ */

#define MORE_ZEROES

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_reval.h"


REvaluation::REvaluation( const REvaluation & e )
{
    if ( e.gen == 0 )
	gen = 0;
    else
	gen = e.gen->clone();
    values = e.values;
    cnt = e.cnt;
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
        cnt = e.cnt;
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
REvaluation::nextpoint_0()
{
    int n = values.max();
#ifdef MORE_ZEROES
   // for ( int i = values.min(); i <= n; i++ )
//	values[i] = gen->generate();
  if (cnt<=n /* values.max() */ )
  {
    cnt++;
    int t;
    int m=values.min();
    if (m<n)
    {
      t=factoryrandom(n-m+1)+m;
    }
    else t=n;
    values[t]=gen->generate();
  }
  else
#endif
  {
    for ( int i = values.min(); i <= n; i++ )
	values[i] = gen->generate();
  }
}
