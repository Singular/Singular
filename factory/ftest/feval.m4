/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: feval.m4,v 1.2 1997-10-15 09:19:09 schmidt Exp $ */

ftestSetNameOfGame( feval,
        `"Usage: feval [<options>] [<envSpec>] <f>\n"
        "  evaluates canonical form <f>.\n"
    	"  In contrast to the other programs of the factory test\n"
	"  environment, <f> may span more than one argument.\n"
	"  More precisely, all arguments are pasted together and\n"
	"  then evaluated as one canonical form.\n"
	"  If there are not any arguments, neither time nor status\n"
	"  nor result nor data information is printed.\n"' )

//{{{ docu
//
// ftestAlgorithm.m4 - ftestAlgorithm test program.
//
// To create ftestAlgorithm.cc, run m4 using the ftest_util.m4 library in
// the following way:
//
// m4 ftest_util.m4 ftestAlgorithm.m4 > ftestAlgorithm.cc
//
//}}}

#include <string.h>
ftestPreprocInit();

ftestGlobalInit();

//
// - main program.
//
int
main ( int argc, char ** argv )
{
    // initialization
    ftestMainInit();
    check = Passed;

    // declare input and output variables
    ftestOutVar( CanonicalForm, f );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();

    if ( argv[optind] ) {
	int i = optind;
	int len = 0;

	// get length of arguments
	while ( argv[i] ) {
	    len += strlen( argv[i] );
	    i++;
	}

	// paste arguments together
	char * argString = new char[len+1];
	argString[0] = '\0';
	while ( optind <= i ) {
	    strcat( argString, argv[optind] );
	    optind++;
	}
	
	f = ftestGetCanonicalForm( argString );
	delete [] argString;
	ftestOutput( "f", f );
    }

    // clean up
    ftestMainExit();
}
