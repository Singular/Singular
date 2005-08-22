/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_reval.h,v 1.4 2005-08-22 17:24:01 Singular Exp $ */

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
    int cnt;
public:
    REvaluation() : Evaluation(), gen(0) {}
    REvaluation( int min, int max, const CFRandom & sample ) : Evaluation( min, max ), gen( sample.clone() ) { cnt=1;}
    REvaluation( const REvaluation & e );
    ~REvaluation();
    REvaluation& operator= ( const REvaluation & e );
    void nextpoint();
    void nextpoint_0();
};

/*ENDPUBLIC*/

#endif /* ! INCL_CF_REVAL_H */
