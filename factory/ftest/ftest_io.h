/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_io.h,v 1.7 1998-04-06 11:08:44 schmidt Exp $ */

#ifndef INCL_FTEST_IO_H
#define INCL_FTEST_IO_H

//{{{ docu
//
// ftest_io.h - header to ftest_io.cc.
//
//}}}

#include <factory.h>

// functions to read objects from strings
void ftestRead ( const char *, CanonicalForm & );
void ftestRead ( const char *, Variable & );
void ftestRead ( const char *, int & );
void ftestRead ( const char *, bool & );

void ftestPrintResult ( const char * resultName, const CanonicalForm & result );
void ftestPrintResult ( const char * resultName, const CFFList & result );
void ftestPrintResult ( const char * resultName, const int result );
void ftestPrintResult ( const char * resultName, const bool result );

#endif /* ! INCL_FTEST_IO_H */
