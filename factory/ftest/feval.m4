/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: feval.m4,v 1.7 1998-07-03 10:20:46 schmidt Exp $ */

ftestSetNameOfGame( feval, `"
Usage: feval [<options>] [<envSpec>] [expand=<n>] <f>
  evaluates canonical form <f>.

  In contrast to the other programs of the Factory Test
  Environment, <f> may span more than one argument.  More
  precisely, all arguments are textually pasted together and are
  then evaluated as one canonical form.  If there are not any
  arguments (except options), neither time nor status nor result
  nor data information is printed.

  If the optional argument `expand' is given, <f> is printed
  in a format dependent on <n>:
  <n> = 0: print <f> in standard format (default)
  <n> = 1: print <f> in NTL format, i.e., as a dense vector of
           coefficients (for univariate polynomials only)
"'`' )
dnl // the trailing quotes at the end of the second argument
dnl // are for font-lock only (another font-lock-trick)

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

// necessary to paste the arguments together
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

    // print long usage if called with exactly one argument `-?'
    if ( argc == 2 && strcmp( "-?", argv[1] ) == 0 ) {
	ftestUsagePrint( "
Long usage not yet written, sorry.
" );
	exit( 0 );
    }

    // declare input and output variables
    ftestInVar( int, expand );
    ftestOutVar( CanonicalForm, f );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( expand, 0, "expand" );

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
	while ( optind < i ) {
	    strcat( argString, argv[optind] );
	    optind++;
	}
	
	ftestReadString( argString, f );
	delete [] argString;
	ftestOutput( "f", f );
    }

    // clean up
    ftestMainExit();
}
