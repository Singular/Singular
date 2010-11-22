#ifndef FFREVAL_H
#define FFREVAL_H

#include <config.h>

#include "canonicalform.h"
#include "cf_reval.h"

class FFREvaluation : public REvaluation
{
  private:
    CFArray start; // random point
  public:
    FFREvaluation( ) : REvaluation(), start() {}
    FFREvaluation( int min, int max, const FFRandom & sample ) : REvaluation( min, max, sample ), start( min, max )
    {
      for( int i=min; i<=max; i++ )
        values[i] = start[i] = 0; // start with 0

      nextpoint();
    }
    FFREvaluation( int min, int max, const GFRandom & sample ) : REvaluation( min, max, sample ), start( min, max )
    {
      for( int i=min; i<=max; i++ )
        values[i] = start[i] = 0; // start with 0

      nextpoint();
    }
    FFREvaluation( int min, int max, const AlgExtRandomF & sample ) : REvaluation( min, max, sample ), start( min, max )
    {
      for( int i=min; i<=max; i++ )
        values[i] = start[i] = 0; // start with 0

      nextpoint();
    }
    FFREvaluation& operator= ( const FFREvaluation & e );

    void nextpoint();
    bool hasNext();
};

#endif
