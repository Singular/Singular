// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_reval.h,v 1.2 1997-04-07 16:11:56 schmidt Exp $

#ifndef INCL_R_EVALUATION_H
#define INCL_R_EVALUATION_H

/*
$Log: not supported by cvs2svn $
Revision 1.1  1997/03/26 16:53:10  schmidt
spurious #include <iostream.h> removed

Revision 1.0  1996/05/17 10:59:39  stobbe
Initial revision

*/

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

#endif /* INCL_R_EVALUATION_H */
