/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_io.h,v 1.8 1998-04-06 11:30:29 schmidt Exp $ */

#ifndef INCL_FTEST_IO_H
#define INCL_FTEST_IO_H

//{{{ docu
//
// ftest_io.h - header to ftest_io.cc.
//
//}}}

#include <factory.h>

// functions to read objects from strings
void ftestReadString ( const char *, CanonicalForm & );
void ftestReadString ( const char *, Variable & );
void ftestReadString ( const char *, int & );
void ftestReadString ( const char *, bool & );

void ftestPrintResult ( const char * resultName, const CanonicalForm & result );
void ftestPrintResult ( const char * resultName, const CFFList & result );
void ftestPrintResult ( const char * resultName, const int result );
void ftestPrintResult ( const char * resultName, const bool result );

#endif /* ! INCL_FTEST_IO_H */
