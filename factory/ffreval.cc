#include <config.h>

#include "cf_defs.h"
#include "canonicalform.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "cf_reval.h"
#include "cf_random.h"
#include "cf_primes.h"
#include "fac_distrib.h"
#include "templates/ftmpl_functions.h"
#include "ffreval.h"
#include "cf_binom.h"
#include "fac_iterfor.h"
#include "cf_iter.h"

void FFREvaluation::nextpoint()
{
  // enumerates the points stored in values
  int n = values.max();
  int p = getCharacteristic();
  for( int i=values.min(); i<=n; i++ )
  {
    if( values[i] != p-1 )
    {
      values[i] += 1;
      break;
    }
    values[i] += 1; // becomes 0
  }
}

bool FFREvaluation::hasNext()
{
  int n = values.max();

  for( int i=values.min(); i<=n; i++ )
  {
    if( values[i]!=start[i] )
      return true;
  }
  return false;
}

FFREvaluation& FFREvaluation::operator= ( const FFREvaluation & e )
{
  if( this != &e )
  {
    if( gen != NULL )
      delete gen;
    values = e.values;
    start = e.start;
    if( e.gen == NULL )
      gen = NULL;
    else
      gen = e.gen->clone();
  }
  return *this;
}

