/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_reval.h,v 1.3 1997-06-19 12:23:56 schmidt Exp $ */

#ifndef INCL_CF_REVAL_H
#define INCL_CF_REVAL_H

#include <config.h>

#include "canonicalform.h"
#include "cf_eval.h"
#include "cf_random.h"

/*BEGINPUBLIC*/

class REvaluation : public Evaluation
{
private:
    CFRandom * gen;
public:
    REvaluation() : Evaluation(), gen(0) {}
    REvaluation( int min, int max, const CFRandom & sample ) : Evaluation( min, max ), gen( sample.clone() ) {}
    REvaluation( const REvaluation & e );
    ~REvaluation();
    REvaluation& operator= ( const REvaluation & e );
    void nextpoint();
};

/*ENDPUBLIC*/

#endif /* ! INCL_CF_REVAL_H */
