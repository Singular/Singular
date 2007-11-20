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

CanonicalForm fin_ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG );

#endif
