// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_factory.h,v 1.0 1996-05-17 10:59:38 stobbe Exp $

#ifndef INCL_CFFACTORY_H
#define INCL_CFFACTORY_H

/*
$Log: not supported by cvs2svn $
*/

#include "cf_gmp.h"

#include "cf_defs.h"
#include "variable.h"

class InternalCF;
class CanonicalForm;

class CFFactory
{
private:
    static int currenttype;
public:
    static int gettype () { return currenttype; }
    static void settype ( int type );
    static InternalCF * basic ( int value );
    static InternalCF * basic ( int type, int value );
    static InternalCF * basic ( const char * str );
    static InternalCF * basic ( int type, const char * str );
    static InternalCF * basic ( int type, int value, bool nonimm );
    static InternalCF * basic ( const MP_INT & num );
    static InternalCF * rational ( int num, int den );
    static InternalCF * rational ( const MP_INT & num, const MP_INT & den, bool normalize );
    static InternalCF * poly ( const Variable & v, int exp, const CanonicalForm & c );
    static InternalCF * poly ( const Variable & v, int exp = 1 );
};

MP_INT getmpi ( InternalCF * value, bool symmetric = true );

#endif
