/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_io.h,v 1.4 1997-10-01 12:29:56 schmidt Exp $ */

#ifndef INCL_FTEST_IO_H
#define INCL_FTEST_IO_H

//{{{ docu
//
// ftest_io.h - header to ftest_io.cc.
//
//}}}

#include <factory.h>

CanonicalForm ftestGetCanonicalForm ( const char * canFormSpec );
Variable ftestGetVariable ( const char * stringVariable );
int ftestGetint ( const char * stringInt );

void ftestPrintResult ( const char * resultName, const CanonicalForm & result );
void ftestPrintResult ( const char * resultName, const int result );

#endif /* ! INCL_FTEST_IO_H */
