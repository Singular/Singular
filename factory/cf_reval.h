// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_reval.h,v 1.0 1996-05-17 10:59:39 stobbe Exp $

#ifndef INCL_R_EVALUATION_H
#define INCL_R_EVALUATION_H

/*
$Log: not supported by cvs2svn $
*/

#include <iostream.h>
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

#endif /* INCL_R_EVALUATION_H */
