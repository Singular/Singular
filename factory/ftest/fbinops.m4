/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( fbinops, `"
Usage: fbinops [<options>] [<envSpec>] <f> <operator> <g>
  executes operator an canonical forms f, g.

  The following operators (with aliases) are recognized:
  `+', `-', `*' (= `mul'), `/', `div', `%', `mod': return a canonicalform;
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
ftestCheckImplementation ( const CanonicalForm & f, const CanonicalForm & g )
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
    if ( ! ( f == f ) ) {
	ftestError( CheckError, "reflexivity check (f) failed\n" );
	return Failed;
    }
    // check reflexivity
    if ( ! ( g == g ) ) {
	ftestError( CheckError, "reflexivity check (g) failed\n" );
	return Failed;
    }
    // check symmetry
    if ( ! ftestBoolEquiv( f == g, g == f ) ) {
	ftestError( CheckError, "symmetry check failed\n" );
	return Failed;
    }
    // check implementation of operators
    if ( ! ftestCheckImplementation( f, g ) ) {
	ftestError( CheckError, "implementation check (f, g) failed\n" );
	return Failed;
    }
    // check implementation of operators
    if ( ! ftestCheckImplementation( g, f ) ) {
	ftestError( CheckError, "implementation check (g, f) failed\n" );
	return Failed;
    }
    // check trichotomy
    if ( ! ftestCheckTrichotomy( f, g ) ) {
	ftestError( CheckError, "trichotomy check (f, g) failed\n" );
	return Failed;
    }
    // check trichotomy
    if ( ! ftestCheckTrichotomy( g, f ) ) {
	ftestError( CheckError, "trichotomy check (g, f) failed\n" );
	return Failed;
    }

    return Passed;
}
//}}}

//{{{ arithmetic functions, tests
static ftestStatusT
ftestArithTest( const CanonicalForm &, const CanonicalForm &, const CanonicalForm & )
{
    return UndefinedResult;
}

static ftestStatusT
ftestDivideTest( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & quot )
{
    CanonicalForm rem = f % g;
    if ( ! ((quot*g)+rem-f).isZero() ) {
	ftestError( CheckError, "f != (f/g)*g+(f%%g)\n" );
	return Failed;
    } else if ( f.inBaseDomain() && g.inBaseDomain() && getCharacteristic() == 0
		&& (rem < 0 || rem > abs( g )) ) {
	// check euclidean division in Z
	ftestError( CheckError, "!(0 <= f%g < abs(g))\n" );
	return Failed;
    } else if ( f.inPolyDomain() || g.inPolyDomain() ) {
	// check euclidean division in R[x]
	Variable x = (mvar( f ) >= mvar( g )) ? mvar( f ) : mvar( g );
	if ( rem.isZero() || degree( rem, x ) < degree( g, x ) )
	    return Passed;
	else {
	    ftestError( CheckError, "degree(rem) >= degree(g)\n" );
	    return Failed;
	}
    } else
	return Passed;
}

static ftestStatusT
ftestDivTest( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & quot )
{
    if ( ! (f%g).isZero() ) {
	ftestError( CheckError, "g does not divide f\n" );
	return Failed;
    } else if ( f != (quot*g) ) {
	ftestError( CheckError, "f != (div(f, g)*g)\n" );
	return Failed;
    } else
	return Passed;
}

static ftestStatusT
ftestModuloTest( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & rem )
{
    if ( ! (((f/g)*g)+(rem)-f).isZero() ) {
	ftestError( CheckError, "f != (f/g)*g+(f%%g)\n" );
	return Failed;
    } else if ( f.inBaseDomain() && g.inBaseDomain() && getCharacteristic() == 0
		&& (rem < 0 || rem > abs( g )) ) {
	// check euclidean division in Z
	ftestError( CheckError, "!(0 <= f%g < abs(g))\n" );
	return Failed;
    } else if ( f.inPolyDomain() || g.inPolyDomain() ) {
	// check euclidean division in R[x]
	Variable x = (mvar( f ) >= mvar( g )) ? mvar( f ) : mvar( g );
	if ( rem.isZero() || degree( rem, x ) < degree( g, x ) )
	    return Passed;
	else {
	    ftestError( CheckError, "degree(rem) >= degree(g)\n" );
	    return Failed;
	}
    } else
	return Passed;
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
    { &operator/, ftestDivideTest, "/", "f/g" },
    { &div, ftestDivTest, "div", "div(f,g)" },
    { &operator%, ftestModuloTest, "%", "f%g" },
    { &mod, ftestModuloTest, "mod", "mod(f,g)" },
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
    const char * operatorName = 0;
    const char * operatorTag = 0;
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
