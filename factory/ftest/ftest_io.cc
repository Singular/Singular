/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_io.cc,v 1.1 1997-09-24 07:29:17 schmidt Exp $ */

//{{{ docu
//
// ftest_io.cc - io utilities for the factory test environment.
//
//}}}

#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <factory.h>

#include "ftest_util.h"

//
// - external functions.
//

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

    // read vf
    istrstream( terminatedStringF ) >> vf;

    delete [] terminatedStringF;

    // return main variable
    return vf.mvar();
}
//}}}

//{{{ void ftestPrintCanonicalForm ( const char * resultName, const CanonicalForm & result )
//{{{ docu
//
// ftestPrintCanonicalForm() - print a canonical form.
//
//}}}
void
ftestPrintCanonicalForm ( const char * resultName, const CanonicalForm & result )
{
    if ( ftestPrintResultFlag ) {
	ftestPrint( "result: %s\n", 0, resultName );
	cout << result << endl;
    }
}
//}}}

//{{{ void ftestPrintInt ( const char * resultName, const int result )
//{{{ docu
//
// ftestPrintInt() - print an integer.
//
//}}}
void
ftestPrintInt ( const char * resultName, const int result )
{
    if ( ftestPrintResultFlag ) {
	ftestPrint( "result: %s\n", 0, resultName );
	cout << result << endl;
    }
}
//}}}
