/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

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
