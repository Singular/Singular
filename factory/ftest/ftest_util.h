/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_util.h,v 1.1 1997-09-11 08:31:26 schmidt Exp $ */

//{{{ docu
//
// ftest_util.h - header to ftest_util.cc.
//
//}}}

#include <factory.h>

//{{{ constants
const int Passed = 0;
const int Failed = 1;
const int UndefinedResult = 2;
const int Error = 3;
//}}}

//{{{ variable declarations
extern long timing_ftest_timer_time;
extern int circle;
//}}}

int ftestGetOpts ( int argc, char ** argv );

int ftestGetEnv ( int argc, char ** argv, const int optind );

CanonicalForm ftestGetCanonicalForm ( const char * stringF );
Variable ftestGetVariable ( const char * stringVariable );

void ftestPrintTimer ( const char * algorithm );
void ftestPrintCheck ( const char * algorithm, int check );
void ftestPrintCanonicalForm ( const char * algorithm, const char * result, const CanonicalForm & f );
void ftestPrintExit ();
