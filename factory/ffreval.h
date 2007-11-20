#ifndef FFREVAL_H
#define FFREVAL_H

#include <config.h>

#include "canonicalform.h"
#include "cf_eval.h"


class FFREvaluation : public REvaluation
{
  private:
    CFArray offset; // random point
  public:
    FFREvaluation( ) : REvaluation() {}
    FFREvaluation( int min, int max ) : offset( min, max ) {}
    FFREvaluation( int min, int max, const FFRandom & sample ) : REvaluation( min, max, sample ) {}
    void init();
    bool step();
    FFREvaluation& operator= ( const FFREvaluation & e );
};

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

CanonicalForm fin_ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG );

#endif
