/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_io.h,v 1.2 1997-09-29 13:09:43 schmidt Exp $ */

#ifndef INCL_FTEST_IO_H
#define INCL_FTEST_IO_H

//{{{ docu
//
// ftest_io.h - header to ftest_io.cc.
//
//}}}

#include <factory.h>

CanonicalForm ftestGetCanonicalForm ( const char * stringF );
Variable ftestGetVariable ( const char * stringVariable );

void ftestPrintResult ( const char * resultName, const CanonicalForm & result );
void ftestPrintResult ( const char * resultName, const int result );

#endif /* ! INCL_FTEST_IO_H */
