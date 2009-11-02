/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// ftest_io.cc - io utilities for the factory test environment.
//
//}}}

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>

#include <factory.h>

#include "ftest_util.h"

//
// - external functions.
//

//{{{ void ftestReadString ( const char * canFormSpec, CanonicalForm & f )
//{{{ docu
//
// ftestReadString() - read canonical form `f' from `canFormSpec'.
//
//}}}
void
ftestReadString ( const char * canFormSpec, CanonicalForm & f )
{
    canFormSpec = ftestSkipBlancs( canFormSpec );

    // check for a single minus and read from stdin
    if ( *canFormSpec == '-' ) {
	const char * tokenCursor = ftestSkipBlancs( canFormSpec+1 );
	if ( ! *tokenCursor ) {
	    cin >> f;
	    return;
	}
    }

    // create terminated CanonicalForm
    char * terminatedSpec = new char[ strlen( canFormSpec )+2 ];
    char * cursor = terminatedSpec;
    while ( *canFormSpec ) {
	switch ( *canFormSpec ) {
	case '.': *cursor = '*'; break;
	case '{': *cursor = '('; break;
	case '}': *cursor = ')'; break;
	default: *cursor = *canFormSpec; break;
	}
	canFormSpec++; cursor++;
    }
    *cursor++ = ';';
    *cursor = '\0';

    // read f from string
    istrstream( terminatedSpec ) >> f;
    delete [] terminatedSpec;
}
//}}}

//{{{ void ftestReadString ( const char * varSpec, Variable & v )
//{{{ docu
//
// ftestReadString() - read variable `v' from `varSpec'.
//
//}}}
void
ftestReadString ( const char * varSpec, Variable & v )
{
    varSpec = ftestSkipBlancs( varSpec );

    if ( isalpha( *varSpec ) )
	v = Variable( *varSpec );
    else if ( isdigit( *varSpec ) )
	v = Variable();
    else
	ftestError( CommandlineError,
		    "variable expected at `%s'\n", varSpec );

    varSpec = ftestSkipBlancs( varSpec+1 );
    if ( *varSpec )
	ftestError( CommandlineError,
		    "extra characters after var spec `%s'\n", varSpec );
}
//}}}

//{{{ void ftestReadString ( const char * intSpec, int & i )
//{{{ docu
//
// ftestReadString() - read integer `i' from `intSpec'.
//
//}}}
void
ftestReadString ( const char * intSpec, int & i )
{
    const char * tokenCursor;

    i = (int)strtol( intSpec, (char**)&tokenCursor, 0 );

    // do error checks
    if ( intSpec == tokenCursor )
	ftestError( CommandlineError,
		    "integer expected at `%s'\n", intSpec );

    // check for extra characters after skipping blancs
    intSpec = ftestSkipBlancs( tokenCursor );
    if ( *intSpec )
	ftestError( CommandlineError,
		    "extra characters after int spec `%s'\n", intSpec );
}
//}}}

//{{{ void ftestReadString ( const char * boolSpec, bool & b )
//{{{ docu
//
// ftestReadString() - read boolean `b' from `boolSpec'.
//
//}}}
void
ftestReadString ( const char * boolSpec, bool & b )
{
    // skip blancs
    boolSpec = ftestSkipBlancs( boolSpec );

    // look for "true" or "false"
    const char * tokenCursor = ftestSubStr( "true", boolSpec );
    if ( boolSpec != tokenCursor )
	b = true;
    else {
	tokenCursor = ftestSubStr( "false", boolSpec );
	b = false;
    }

    // do error checks
    if ( boolSpec == tokenCursor )
	ftestError( CommandlineError,
		    "bool expected at `%s'\n", boolSpec );

    // check for extra characters after skipping blancs
    boolSpec = ftestSkipBlancs( tokenCursor );
    if ( *boolSpec )
	ftestError( CommandlineError,
		    "extra characters after bool spec `%s'\n", boolSpec );
}
//}}}

//{{{ void ftestPrintResult ( const char * resultName, const CanonicalForm & result )
//{{{ docu
//
// ftestPrintResult() - print a canonical form.
//
//}}}
void
ftestPrintResult ( const char * resultName, const CanonicalForm & result )
{
    if ( ftestPrintResultFlag ) {
	cout << "Result:\t\t" << resultName << ":" << endl;
	cout << result << endl;
    } else if ( ! ftestPrintFlag )
	cout << "(" << result << ")" << endl;
}
//}}}

//{{{ void ftestPrintResult ( const char * resultName, const CFFList & result )
//{{{ docu
//
// ftestPrintResult() - print a list of canonical form factors
//
//}}}
void
ftestPrintResult ( const char * resultName, const CFFList & result )
{
    CFFListIterator I;

    if ( ftestPrintResultFlag ) {
	cout << "Result:\t\t" << resultName << ":" << endl;
	for ( I = result; I.hasItem(); I++ )
	    cout << I.getItem() << endl;
    } else if ( ! ftestPrintFlag )
	for ( I = result; I.hasItem(); I++ )
	    cout << "(" << I.getItem() << ")" << endl;
}
//}}}

//{{{ void ftestPrintResult ( const char * resultName, const int result )
//{{{ docu
//
// ftestPrintResult() - print an integer.
//
//}}}
void
ftestPrintResult ( const char * resultName, const int result )
{
    if ( ftestPrintResultFlag )
	cout << "Result:\t\t" << resultName << ": " << result << endl;
    else if ( ! ftestPrintFlag )
	cout << result << endl;
}
//}}}

//{{{ void ftestPrintResult ( const char * resultName, const bool result )
//{{{ docu
//
// ftestPrintResult() - print a boolean.
//
//}}}
void
ftestPrintResult ( const char * resultName, const bool result )
{
    const char * stringResult = result ? "true" : "false";

    if ( ftestPrintResultFlag )
	cout << "Result:\t\t" << resultName << ": " << stringResult << endl;
    else if ( ! ftestPrintFlag )
	cout << stringResult << endl;
}
//}}}
