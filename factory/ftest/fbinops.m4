/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fbinops.m4,v 1.1 1997-11-21 10:00:32 schmidt Exp $ */

ftestSetNameOfGame( fbinops, `"
Usage: fbinops [<options>] [<envSpec>] <f> <operator> <g>
  executes operator an canonical forms f, g.

  The following operators (with aliases) are recognized:
  `+', `-', `*' (= `mul'), `/', `%': return a canonicalform;
  `==', `!=', `<' (= `lt'), `>' (= `gt'): return an integer (i.e. boolean)
"'`' )

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

ftestPreprocInit();

ftestGlobalInit();

//{{{ typedef binOpCFT, binOpBoolT, binOpCFTestT
//{{{ docu
//
// typedef binOpCFT, binOpBoolT, binOpCFTestT -
//   pointer to functions types.
//
// binOpCFT, binOpBoolT: pointers to binary operators getting two
//   CanonicalForms and returning a CanonicalForm or a bool, resp.
// binOpCFTestT: pointers to test functions getting three
//   CanonicalForms and returning a ftestStatusT.
//
//}}}
typedef CanonicalForm (* binOpCFT)( const CanonicalForm &, const CanonicalForm & );
typedef bool (* binOpBoolT)( const CanonicalForm &, const CanonicalForm & );
typedef ftestStatusT (* binOpCFTestT)( const CanonicalForm &, const CanonicalForm &, const CanonicalForm & );
//}}}

//{{{ struct binOpCFSpecT, binOpBoolSpecT
//{{{ docu
//
// struct binOpCFSpecT, struct binOpBoolSpecT - types describing a binary
//   operator.
//
// binOpCFSpecT describes binary operators returning a CanonicalForm.
// binOpBoolSpecT describes binary operators returning a bool.
//
// op: pointer to operator
// test: test function
// operatorName: symbolic name, used as a key
// operatorTag: tag to print result
//
//}}}
struct binOpCFSpecT
{
    binOpCFT op;
    binOpCFTestT test;
    const char * operatorName;
    const char * operatorTag;
};

struct binOpBoolSpecT
{
    binOpBoolT op;
    const char * operatorName;
    const char * operatorTag;
};
//}}}

//{{{ comparison functions, tests
static inline bool
ftestBoolEquiv ( bool a, bool b )
{
    return ( ( !a || b ) && ( !b || a ) );
}

static inline bool
ftestCheckRelations ( const CanonicalForm & f, const CanonicalForm & g )
{
    return ( ftestBoolEquiv( f == g, !(f != g) )
	     && ftestBoolEquiv( f < g, g > f )
	     && ftestBoolEquiv( f != g, (f < g) || (f > g) ) );
}

static inline bool
ftestCheckTrichotomy ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f == g ) {
	if ( f < g ) return false;
	if ( g < f ) return false;
	return true;
    } else if ( f < g ) {
	if ( f == g ) return false;
	if ( g < f ) return false;
	return true;
    } else if ( g < f ) {
	if ( f == g ) return false;
	if ( f < g ) return false;
	return true;
    } else
	return false;
}

static ftestStatusT
ftestRelOpTest ( const CanonicalForm & f, const CanonicalForm & g )
{
    // check reflexivity
    if ( ! ( f == f ) )
	return Failed;
    // check reflexivity
    if ( ! ( g == g ) )
	return Failed;
    // check symmetry
    if ( ! ftestBoolEquiv( f == g, g == f ) )
	return Failed;
    // check relations between operators
    if ( ! ftestCheckRelations( f, g ) )
	return Failed;
    // check relations between operators
    if ( ! ftestCheckRelations( g, f ) )
	return Failed;
    // check trichotomy
    if ( ! ftestCheckTrichotomy( f, g ) )
	return Failed;
    // check trichotomy
    if ( ! ftestCheckTrichotomy( g, f ) )
	return Failed;

    return Passed;
}
//}}}

//{{{ arithmetic functions, tests
static ftestStatusT
ftestArithTest( const CanonicalForm &, const CanonicalForm &, const CanonicalForm & )
{
    return UndefinedResult;
}
//}}}

//{{{ binOpCFSpecArray, binOpBoolSpecArray
//{{{ docu
//
// binOpCFSpec, binOpBoolSpec - arrays of operator descriptions.
//
//}}}
binOpCFSpecT binOpCFSpecArray[] =
{
    { &operator+, ftestArithTest, "+", "f+g" },
    { &operator-, ftestArithTest, "-", "f-g" },
    { &operator*, ftestArithTest, "*", "f*g" },
    { &operator*, ftestArithTest, "mul", "f*g" },
    { &operator/, ftestArithTest, "/", "f/g" },
    { &operator%, ftestArithTest, "%", "f%g" },
    { 0, 0, 0, 0 }
};

binOpBoolSpecT binOpBoolSpecArray[] =
{
    { &operator==, "==", "f==g" },
    { &operator!=, "!=", "f!=g" },
    { &operator>, ">", "f>g" },
    { &operator>, "gt", "f>g" },
    { &operator<, "<", "f<g" },
    { &operator<, "lt", "f<g" },
    { 0, 0, 0 }
};
//}}}

//
// - main program.
//
int
main ( int argc, char ** argv )
{
    // initialization
    ftestMainInit();

    // declare input and output variables
    ftestOutVar( CanonicalForm, resultCF );
    ftestOutVar( bool, resultBool );
    ftestInVar( CanonicalForm, f );
    ftestInVar( CanonicalForm, g );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();

    // read first operand
    ftestGetInVar( f );

    // declarations to search operator
    const char * operatorName;
    const char * operatorTag;
    binOpCFT binOpCF = 0;
    binOpBoolT binOpBool = 0;
    binOpCFTestT binOpCFTest = 0;

    // get and search operator
    if ( argv[ optind ] ) {
        operatorName = ftestSkipBlancs( argv[ optind++ ] );
    } else
	ftestError( CommandlineError,
                    "expected operator specification at position %d in commandline\n",
                    optind );

    // search through binOpCFSpecArray
    int i = 0;
    while ( binOpCFSpecArray[i].operatorName ) {
	if ( strcmp( binOpCFSpecArray[i].operatorName, operatorName ) == 0 ) {
	    binOpCF = binOpCFSpecArray[i].op;
	    binOpCFTest = binOpCFSpecArray[i].test;
	    operatorTag = binOpCFSpecArray[i].operatorTag;
	    break;
	}
	i++;
    }

    // search through binOpBoolSpecArray
    i = 0;
    if ( ! binOpCF )
	while ( binOpBoolSpecArray[i].operatorName ) {
	    if ( strcmp( binOpBoolSpecArray[i].operatorName, operatorName ) == 0 ) {
		binOpBool = binOpBoolSpecArray[i].op;
		operatorTag = binOpBoolSpecArray[i].operatorTag;
		break;
	    }
	    i++;
	}

    // check whether operator has been found
    if ( binOpCF == 0 && binOpBool == 0 )
	ftestError( CommandlineError,
		    "unknown operator `%s'\n", operatorName );

    // read second operand
    ftestGetInVar( g );

    // do the test!
    if ( binOpCF ) {
	ftestRun(
	    resultCF = binOpCF( f, g ); );
	ftestCheck(
	    binOpCFTest( f, g, resultCF ) );
    } else {
	ftestRun(
	    resultBool = binOpBool( f, g ); );
	ftestCheck(
	    ftestRelOpTest( f, g ) );
    }

    // print results
    if ( binOpCF ) {
	ftestOutput( operatorTag, resultCF );
    } else {
	ftestOutput( operatorTag, resultBool );
    }

    // clean up
    ftestMainExit();
}
