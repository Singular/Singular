/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_util.cc,v 1.2 1997-09-12 10:03:41 schmidt Exp $ */

//{{{ docu
//
// ftest_util.cc - some utilities for the factory test environment.
//
//}}}

#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include <GetOpt.h>

// we include timing.h for calculation of HZ only
#define TIMING
#include <timing.h>

#include <factory.h>

#include "ftest_util.h"

//{{{ external variables
//{{{ docu
//
// - external variables.
//
// ftestCircle: set by ftestGetOpts() from commandline, read by
//   main().  Number of test circles.
//
//}}}
int ftestCircle = 1;
//}}}

//{{{ static variables
//{{{ docu
//
// - static variables.
//
// writtenToStream: set and read by output routines.
//   True iff any of the routines has written something
//   to cout without terminating it with a CR.
// outputType: set by ftestGetOpts() from commandline, read by
//   output routines. Things to print/not to print.
//
//}}}
static bool writtenToStream = false;
static char * outputType = 0;
//}}}

//{{{ int ftestGetOpts ( int argc, char ** argv )
//{{{ docu
//
// ftestGetOpts() - read options from commandline.
//
// Returns new index into commandline.
//
//}}}
int
ftestGetOpts ( int argc, char ** argv )
{
    GetOpt getopt( argc, argv, "o:c:s:" );
    int optionChar;

    while ( (optionChar = getopt()) != EOF ) {
	switch ( optionChar ) {
	case 'o': outputType = getopt.optarg; break;
	case 'c': ftestCircle = atoi( getopt.optarg ); break;
	case 's': factoryseed( atoi ( getopt.optarg ) ); break;
	case '?': cerr << "Usage: " << argv[0]
		       << " [o<outputType>c<circles>s<seed>]" << endl; exit( Error );
	}
    }
    return getopt.optind;
}
//}}}

//{{{ int ftestGetEnv ( int argc, char ** argv, const int optind )
//{{{ docu
//
// ftestGetEnv() - read a factory environment specification
//   from commandline.
//
// Returns new index into commandline.
//
//}}}
int
ftestGetEnv ( int argc, char ** argv, const int optind )
{
    return optind;
}
//}}}

//{{{ CanonicalForm ftestGetCanonicalForm ( const char * stringF )
//{{{ docu
//
// ftestGetCanonicalForm() - read a canonical form from stringF,
//   return it.
//
//}}}
CanonicalForm
ftestGetCanonicalForm ( const char * stringF )
{
    CanonicalForm f;

    // create terminated CanonicalForm
    int i = strlen( stringF );
    char * terminatedStringF = new char[i+2];
    strcpy( terminatedStringF, stringF );
    terminatedStringF[i] = ';';
    terminatedStringF[i+1] = '\0';

    // read f
    istrstream( terminatedStringF ) >> f;

    delete [] terminatedStringF;
    return f;
}
//}}}

//{{{ Variable ftestGetVariable ( const char * stringVariable )
//{{{ docu
//
// ftestGetVariable() - read a variable from stringVariable,
//   return it.
//
//}}}
Variable
ftestGetVariable ( const char * stringVariable )
{
    CanonicalForm vf;

    // create terminated CanonicalForm
    int i = strlen( stringVariable );
    char * terminatedStringF = new char[i+2];
    strcpy( terminatedStringF, stringVariable );
    terminatedStringF[i] = ';';
    terminatedStringF[i+1] = '\0';

    // read f
    istrstream( terminatedStringF ) >> vf;

    delete [] terminatedStringF;
    return vf.mvar();
}
//}}}

//{{{ void ftestPrintTimer ( const char * algorithm )
//{{{ docu
//
// ftestPrintTimer() - print value of timer.
//
//}}}
void
ftestPrintTimer ( const char * algorithm )
{
    if ( strchr( outputType, 't' ) ) {
	// print time in seconds
	if ( strchr( outputType, '@' ) ) {
	    // print in reduced format
	    cout << "time: " << timing_ftestTimer_time / HZ << "; ";
	    writtenToStream = true;
	}
	else
	    // print in full format
	    cout << "Time used by " << algorithm << ": " << timing_ftestTimer_time / HZ << endl;
    } else if ( strchr( outputType, 'T' ) ) {
	// print time in tics
	if ( strchr( outputType, '@' ) ) {
	    // print in reduced format
	    cout << "time: " << timing_ftestTimer_time << "; ";
	    writtenToStream = true;
	}
	else
	    // print in full format
	    cout << "Time used by " << algorithm << ": " << timing_ftestTimer_time << endl;
    }
}
//}}}

//{{{ void ftestPrintCheck ( const char * algorithm, const int check )
//{{{ docu
//
// ftestPrintCheck() - print result of checks.
//
//}}}
void
ftestPrintCheck ( const char * algorithm, const int check )
{
    char * checkStr = 0;

    switch (check) {
    case Passed: checkStr = "Passed"; break;
    case Failed: checkStr = "Failed"; break;
    case UndefinedResult: checkStr = "Undef.";
    }

    if ( strchr( outputType, 'c' ) ) {
	if ( strchr( outputType, '@' ) ) {
	    // print in reduced format
	    cout << "check: " << checkStr << "; ";
	    writtenToStream = true;
	}
	else
	    // print in full format
	    cout << "Check state of " << algorithm << ": " << checkStr << endl;
    }
}
//}}}

//{{{ void ftestPrintCanonicalForm ( const char * algorithm, const char * result, const CanonicalForm & f )
//{{{ docu
//
// ftestPrintCanonicalForm() - print a canonical form.
//
//}}}
void
ftestPrintCanonicalForm ( const char * algorithm, const char * result, const CanonicalForm & f )
{
    if ( strchr( outputType, 'r' ) ) {
	if ( strchr( outputType, '@' ) ) {
	    // print in reduced format
	    cout << "result: " << result << endl;
	    cout << f;
	    writtenToStream = true;
	}
	else {
	    // print in full format
	    cout << "Result of " << algorithm << ": " << result << endl;
	    cout << f << endl;
	}
    }
}
//}}}

//{{{ void ftestPrintExit ()
//{{{ docu
//
// ftestPrintExit() - clean up output.
//
//}}}
void
ftestPrintExit ()
{
    if ( writtenToStream )
	cout << endl;
}
//}}}
