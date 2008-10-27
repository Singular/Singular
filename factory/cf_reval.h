/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_reval.h,v 1.7 2008-10-27 10:16:09 Singular Exp $ */

#ifndef INCL_CF_REVAL_H
#define INCL_CF_REVAL_H

#include <config.h>

#include "canonicalform.h"
#include "cf_eval.h"
#include "cf_random.h"

/*BEGINPUBLIC*/

class REvaluation : public Evaluation
{
protected: // neeeded in FFREvaluation
    CFRandom * gen;
public:
    REvaluation() : Evaluation(), gen(0) {}
    REvaluation( int min0, int max0, const CFRandom & sample ) : Evaluation( min0, max0 ), gen( sample.clone() ) {}
    REvaluation( const REvaluation & e );
    ~REvaluation();
    REvaluation& operator= ( const REvaluation & e );
    void nextpoint();
};

/*ENDPUBLIC*/

#endif /* ! INCL_CF_REVAL_H */
