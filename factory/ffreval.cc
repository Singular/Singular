#include <config.h>

#include "assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "cf_reval.h"
#include "cf_random.h"
#include "cf_primes.h"
#include "fac_distrib.h"
#include "ftmpl_functions.h"
#include "ffreval.h"

void FFREvaluation::init()
{
  int n = values.max();

  for( int i=values.min(); i<=n; i++ )
    values[i] = offset[i] = gen->generate(); // generate random point
}

bool FFREvaluation::step()
{
  // enumerates the points stored in values
  int n = values.max();
  int p = getCharacteristic();

  int i;
  for(i=values.min(); i<=n; i++ )
  {
    if( values[i] != p-1 )
    {
      values[i] += 1;
      break;
    }
    values[i] += 1; // becomes 0
  }
  for(i=values.min();i<=n;i++)
  {
    if(values[i]!=offset[i]) return true;
  }
  return false;
}

FFREvaluation& FFREvaluation::operator= ( const FFREvaluation & e )
{
  if( this != &e )
  {
    if( gen != 0 )
      delete gen;
    values = e.values;
    offset = e.offset;
    if( e.gen == 0 )
      gen = 0;
    else
      gen = e.gen->clone();
  }
  return *this;
}

