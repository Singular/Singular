/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_io.cc,v 1.4 1997-10-01 14:30:36 schmidt Exp $ */

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

//{{{ CanonicalForm ftestGetCanonicalForm ( const char * canFormSpec )
//{{{ docu
//
// ftestGetCanonicalForm() - read a canonical form from canFormSpec,
//   return it.
//
//}}}
CanonicalForm
ftestGetCanonicalForm ( const char * canFormSpec )
{
    // get string to read canonical form from
    const char * stringF = canFormSpec;
    stringF = ftestSkipBlancs( stringF );
    if ( *stringF == '$' ) {
	const char * tokenCursor = ftestSkipBlancs( stringF+1 );
	// read canonical form from environment
	stringF = getenv( tokenCursor );
	if ( ! stringF )
	    ftestError( CanFormSpecError,
			"no environment variable `$%s'\n",
			tokenCursor );
    }

    // create terminated CanonicalForm
    int i = strlen( stringF );
    char * terminatedStringF = new char[i+2];
    char * stringCursor = terminatedStringF;
    while ( *stringF ) {
	switch ( *stringF ) {
	case '.': *stringCursor = '*'; break;
	case '{': *stringCursor = '('; break;
	case '}': *stringCursor = ')'; break;
	default: *stringCursor = *stringF; break;
	}
	stringF++; stringCursor++;
    }
    *stringCursor++ = ';';
    *stringCursor = '\0';

    // read f
    CanonicalForm f;
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
    Variable v;
    stringVariable = ftestSkipBlancs( stringVariable );

    if ( isalpha( *stringVariable ) )
	v = Variable( *stringVariable );
    else if ( isdigit( *stringVariable ) )
	v = Variable();
    else
	ftestError( CommandlineError,
		    "not a variable: `%s'\n", stringVariable );

    stringVariable = ftestSkipBlancs( stringVariable+1 );
    if ( *stringVariable )
	ftestError( CommandlineError,
		    "extra characters after var spec: `%s'\n", stringVariable );

    return v;
}
//}}}

//{{{ int ftestGetint ( const char * stringInt )
//{{{ docu
//
// ftestGetint() - read an integer from stringInt,
//   return it.
//
//}}}
int
ftestGetint ( const char * stringInt )
{
    const char * tokenCursor;

    int i = (int)strtol( stringInt, (char**)&tokenCursor, 0 );

    // do error checks
    if ( stringInt == tokenCursor )
	ftestError( CommandlineError,
		    "integer expected at `%s'\n", stringInt );

    stringInt = ftestSkipBlancs( tokenCursor );
    if ( *stringInt )
	ftestError( CommandlineError,
		    "extra characters after int spec `%s'\n", stringInt );

    return i;
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
	ftestPrint( "result: %s\n", "@@%s\n", resultName );
	cout << result << endl;
    } else if ( ! ftestPrintFlag )
	cout << result << endl;
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
    if ( ftestPrintResultFlag ) {
	ftestPrint( "result: %s\n", "@@%s\n", resultName );
	cout << result << endl;
    } else if ( ! ftestPrintFlag )
	cout << result << endl;
}
//}}}
