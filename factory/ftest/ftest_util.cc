/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_util.cc,v 1.11 1997-11-13 08:31:24 schmidt Exp $ */

//{{{ docu
//
// ftest_util.cc - some utilities for the factory test environment.
//
//}}}

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <iostream.h>

// we include timing.h for calculation of HZ only
#define TIMING
#include <timing.h>

#include <factory.h>

#include "ftest_util.h"
#include "ftest_io.h"

//{{{ const int CFSwitchesMax
//{{{ docu
//
// const CFSwitchesMax - maximum number of switches.
//
//}}}
const int CFSwitchesMax = 10;
//}}}

//{{{ struct varSpecT
struct varSpecT
{
    char variable;
    CanonicalForm mipo;
    varSpecT * next;
};
//}}}

//{{{ struct ftestEnvT
//{{{ docu
//
// struct ftestEnvT - factory environment.
//
//}}}
struct ftestEnvT
{
    int seed;
    bool seedSet;
    int characteristic;
    int extDegree;
    char extGen;
    varSpecT * varSpec;
    bool switches[CFSwitchesMax];
};
//}}}

//{{{ external variables
//{{{ docu
//
// - external variables.
//
// ftestCircle: set by ftestGetOpts() from commandline, read by
//   main().  Number of test circles.
// ftestAlarm: set by ftestGetOpts() from acommandline, read by
//   main().
// ftestPrintFlag: set by ftestParseOutputType() from
//   commandline, read by ftestPrintResult().  True iff there was
//   some output type specifier on commandline.
// ftestPrintResultFlag: set by ftestParseOutputType() from
//   commandline, read by output routines.  Whether to print result
//   or not.
//
//}}}
int ftestCircle = 1;
int ftestAlarm = 0;

int ftestPrintFlag = 0;
int ftestPrintResultFlag = 0;
//}}}

//{{{ static variables
//{{{ docu
//
// - static variables.
//
// ftestPrint*Flag: set by ftestParseOutputType() from
//   commandline, read by output routines.  Things to print/not to
//   print.  Note that we need `ftestPrintFlag' and
//   `ftestPrintResultFlag' to be external.
// ftestExecName, ftestAlgorithmName: set by ftestSetName(), read
//   by output routines.  Name of executable and algorithm.
// ftestUsage: set by ftestSetName(), read by ftestUsage().
//   Algorithm-specific usage information.
// ftestEnv: factory environment specification.  Set by
//   ftestGetEnv(), read by ftestPrintEnv() function.  This
//   variable is quite spurious, but I keep it for future use.
// ftestSwitchNames: symbolic names of switches
// ftestSeedFile: file where to read/to which to print the random
//   generator seed.  Set by ftestParseRandom(), read by
//   ftestWriteSeed().
//
//}}}
static int ftestPrintTimingFlag = 0;
static int ftestPrintCheckFlag = 0;
static int ftestPrintEnvFlag = 0;
static int ftestPrintShortFlag = 0;

const char * ftestExecName = 0;
const char * ftestAlgorithmName = 0;
const char * ftestUsage = 0;

static ftestEnvT ftestEnv;

const char * ftestSwitchNames[] =
{
    "RATIONAL",
    "QUOTIENT",
    "SYMMETRIC_FF",
    "BERLEKAMP",
    "FAC_USE_BIG_PRIMES",
    "FAC_QUADRATICLIFT",
    "USE_EZGCD",
    "USE_SPARSEMOD"
};

static char * ftestSeedFile = 0;
//}}}

//
// - static functions.
//

//{{{ static const char * ftestSubStr( const char * subString, const char * string )
//{{{ docu
//
// ftestSubStr() - check whether subString is a substring of string.
//
// If so, return index behind subString in string, otherwise
// string.
//
//}}}
static const char *
ftestSubStr( const char * subString, const char * string )
{
    const char * stringStart = string;

    while ( *subString && *subString == *string ) {
	subString++; string++;
    }

    if ( *subString )
	return stringStart;
    else
	return string;
}
//}}}

//{{{ static char * ftestConcatEnv ( char ** argv, int & optind )
//{{{ docu
//
// ftestConcatEnv() - concatenate all environment information
//   together and return the result.
//
// The new index into the commandline is returned in optind.
//
//}}}
static char *
ftestConcatEnv ( char ** argv, int & optind )
{
    // first get length
    int i = optind;
    int len = 0;
    const char * envString = getenv( "FTEST_ENV" );
    if ( envString )
	len = strlen( envString );

    while ( (argv[i] != 0) && (*ftestSkipBlancs( argv[i] ) == '/') ) {
	len += strlen( argv[i] );
	i++;
    }

    char * fEnvString = new char[len+1];

    // now build string
    fEnvString[0] = '\0';
    if ( envString )
	strcat( fEnvString, envString );

    while ( optind < i ) {
	strcat( fEnvString, argv[optind] );
	optind++;
    }

    return fEnvString;
}
//}}}

//{{{ static const char * ftestParseRandom ( const char * tokenString )
//{{{ docu
//
// ftestParseRandom() - parse random specification and set factory's
//   random generator seed.
//
// The results are also stored in global variable `ftestEnv'.
// Returns pointer behind last parsed character in tokenString.
//
//}}}
static const char *
ftestParseRandom ( const char * tokenString )
{
    int seed = 0;
    bool seedSet = false;

    do {
	const char * tokenCursor;

	// we skip either `@' or `,'
	tokenString = ftestSkipBlancs( tokenString+1 );
	if ( isdigit( *tokenString ) ) {
	    // read seed specification from command line
	    seed = (int)strtol( tokenString, (char **)&tokenCursor, 0 );
	    if ( tokenCursor == tokenString )
		ftestError( EnvSyntaxError,
			    "bad seed specification `%s'\n",
			    tokenString );
	    seedSet = true;
	} else if ( isalpha( *tokenString ) ) {
	    // read seed specification from file
	    tokenCursor = strpbrk( tokenString, ",/" );
	    if ( ! tokenCursor )
		tokenCursor = strchr( tokenString, '\0' );

	    // store file name
	    int nameLen = tokenCursor-tokenString;
	    ftestSeedFile = new char[nameLen+1];
	    strncpy( ftestSeedFile, tokenString, nameLen );
	    ftestSeedFile[nameLen] = '\0';

	    // open file, read seed, close file
	    FILE * seedFile = fopen( ftestSeedFile, "r" );
	    if ( seedFile ) {
		// if file exists, read seed
		if ( fscanf( seedFile, "%d", &seed ) != 1 )
		    ftestError( FileError,
				"garbled seed in seed file `%s'\n", ftestSeedFile );
		fclose( seedFile );

		seedSet = true;
	    } else
		if ( errno != ENOENT )
		    ftestError( FileError,
				"failed to open `%s' for reading: %s\n", ftestSeedFile,
				strerror( errno ) );
		else if ( ! (ftestEnv.seedSet || seedSet) ) {
		    // if file does not exist, set seed to some
		    // default value if it has not been set before
		    seed = 0;
		    seedSet = true;
		}
	} else
	    ftestError( EnvSyntaxError,
			"bad random specification `%s'\n", tokenString );

	tokenString = ftestSkipBlancs( tokenCursor );
    } while ( *tokenString == ',' );

    // set seed
    ftestEnv.seedSet = seedSet;
    if ( seedSet ) {
	ftestEnv.seed = seed;
	factoryseed( seed );
    }

    return tokenString;
}
//}}}

//{{{ static const char * ftestParseChar ( const char * tokenString )
//{{{ docu
//
// ftestParseChar() - parse characteristic specification and set factory's
//   characteristic.
//
// The results are also stored in global variable `ftestEnv'.
// Returns pointer behind last parsed character in tokenString.
//
//}}}
static const char *
ftestParseChar ( const char * tokenString )
{
    const char * tokenCursor;
    int characteristic = 0;
    int extDegree = 0;
    char extGen = 'Z';

    characteristic = (int)strtol( tokenString, (char **)&tokenCursor, 0 );
    tokenString = ftestSkipBlancs( tokenCursor );

    // look for exponent
    if ( *tokenString == '^' ) {
	tokenString++;
	extDegree = (int)strtol( tokenString, (char **)&tokenCursor, 0 );
	if ( tokenCursor == tokenString )
	    ftestError( EnvSyntaxError,
			"bad exponent in char specification `%s'\n",
			tokenString );
	tokenString = ftestSkipBlancs( tokenCursor );
	// look for generator
	if ( *tokenString == ',' ) {
	    tokenString = ftestSkipBlancs( tokenString+1 );
	    if ( isalpha( *tokenString ) ) {
		extGen = *tokenString;
		tokenString++;
	    } else
		ftestError( EnvSyntaxError,
			    "bad generator in char specification `%s'\n",
			    tokenString );
	}
    }

    // set characteristic
    ftestEnv.characteristic = characteristic;
    if ( extDegree ) {
	ftestEnv.extDegree = extDegree;
	ftestEnv.extGen = extGen;
	setCharacteristic( characteristic, extDegree, extGen );
    } else
	setCharacteristic( characteristic );

    return tokenString;
}
//}}}

//{{{ static const char * ftestParseSwitches ( const char * tokenString )
//{{{ docu
//
// ftestParseSwitches() - parse switch specification and set factory's
//   switches.
//
// The results are also stored in global variable `ftestEnv'.
// Returns pointer behind last parsed character in tokenString.
//
//}}}
static const char *
ftestParseSwitches ( const char * tokenString )
{
    unsigned int switchNumber;
    bool switchSetting;

    while ( *tokenString == '+' || *tokenString == '-' ) {
	const char * tokenCursor;
	if ( *tokenString == '+' )
	    switchSetting = true;
	else
	    switchSetting = false;
	tokenString = ftestSkipBlancs( tokenString+1 );

	// skip optional leading "SW_"
	tokenString = ftestSubStr( "SW_", tokenString );

	// now search name of switch
	for ( switchNumber = 0;
	      switchNumber < sizeof( ftestSwitchNames ) / sizeof( char* );
	      switchNumber++ )
	    if ( (tokenCursor = ftestSubStr( ftestSwitchNames[switchNumber], tokenString ))
		 != tokenString )
		break;

	if ( tokenCursor == tokenString )
	    ftestError( EnvSyntaxError,
			"unknown switch `%s'\n", tokenString );

	// set switch
	ftestEnv.switches[switchNumber] = switchSetting;
	if ( switchSetting )
	    On( switchNumber );
	else
	    Off( switchNumber );

	tokenString = ftestSkipBlancs( tokenCursor );
    }

    return tokenString;
}
//}}}

//{{{ static const char * ftestParseVars ( const char * tokenString )
//{{{ docu
//
// ftestParseVars() - parse variable specification and set factory's
//   variable.
//
// The results are also stored in global variable `ftestEnv'.
// Returns pointer behind last parsed character in tokenString.
//
//}}}
static const char *
ftestParseVars ( const char * tokenString )
{
    char variable;
    CanonicalForm mipo = 0;
    Variable * dummy;

    // we append at end of list
    static varSpecT * endOfVarSpec = 0;
    varSpecT * varSpec;

    while ( isalpha( *tokenString ) ) {
	variable = *tokenString;
	mipo = 0;

	tokenString = ftestSkipBlancs( tokenString+1 );

	// look for mipo
	if ( *tokenString == '=' ) {
	    const char * tokenCursor;
	    tokenString = ftestSkipBlancs( tokenString+1 );

	    // search for end of mipo
	    tokenCursor = strpbrk( tokenString, ",/" );
	    if ( ! tokenCursor )
		tokenCursor = strchr( tokenString, '\0' );

	    // copy mipo to new string and read it
	    int mipoLen = tokenCursor-tokenString;
	    char * mipoString = new char[mipoLen+1];
	    strncpy( mipoString, tokenString, mipoLen );
	    mipoString[mipoLen] = '\0';
	    mipo = ftestGetCanonicalForm( mipoString );
	    delete [] mipoString;

	    tokenString = ftestSkipBlancs( tokenCursor );
	}
	if ( *tokenString == ',' )
	    // skip optional separator
	    tokenString = ftestSkipBlancs( tokenString+1 );

	// store information in ftestEnv
	varSpec	= new varSpecT;
	varSpec->variable = variable;
	varSpec->mipo = mipo;
	varSpec->next = 0;
	if ( ftestEnv.varSpec )
	    endOfVarSpec->next = varSpec;
	else
	    ftestEnv.varSpec = varSpec;
	endOfVarSpec = varSpec;

	// set variable
	if ( mipo.isZero() )
	    // polynomial variable
	    dummy = new Variable( variable );
	else
	    dummy = new Variable( rootOf( mipo, variable ) );
	// we only need the global information
	delete dummy;
    }

    return tokenString;
}
//}}}

//{{{ static void ftestParseEnv ( const char * tokenString )
//{{{ docu
//
// ftestParseEnv() - parse environment specification and set factory's
//   environment.
//
// The results are also stored in global variable `ftestEnv'.
//
// The parser is quite simple.  As long as `tokenString' starts
// with a `/', he looks up the next character to determine which
// token follows and parses it.  The functions used to do this
// should leave `tokenString' behind the last character it
// succeeded to read.
//
//}}}
static void
ftestParseEnv ( const char * tokenString )
{
    // read list of environment specifications
    tokenString = ftestSkipBlancs( tokenString );
    while ( *tokenString == '/' ) {
	tokenString = ftestSkipBlancs( tokenString+1 );

	if ( *tokenString == '@' ) {
	    // random specification
	    tokenString = ftestParseRandom( tokenString );
	} else if ( isdigit( *tokenString ) ) {
	    // specification of characteristics
	    tokenString = ftestParseChar( tokenString );
	} else if ( *tokenString == '+' || *tokenString == '-' ) {
	    // specification of switches
	    tokenString = ftestParseSwitches( tokenString );
	} else if ( isalpha( *tokenString ) ) {
	    // specification of variables
	    tokenString = ftestParseVars( tokenString );
	}
	tokenString = ftestSkipBlancs( tokenString );
    }
    
    // check if we reached end
    if ( *tokenString )
	ftestError( EnvSyntaxError,
		    "extra characters in environment specification list `%s'\n",
		    tokenString );
}
//}}}

//{{{ static void ftestParseOutputType ( const char * outputType )
//{{{ docu
//
// ftestParseOutputType() - parse output type and set ftestPrint*Flags.
//
//}}}
static void
ftestParseOutputType ( const char * outputType )
{
    ftestPrintFlag = 1;
    while ( *outputType ) {
	switch ( *outputType ) {
	case 'r': ftestPrintResultFlag = 1; break;
	case 't': ftestPrintTimingFlag = 1; break;
	case 'c': ftestPrintCheckFlag = 1; break;
	case 'e': ftestPrintEnvFlag = 1; break;
	case 's': ftestPrintShortFlag = 1; break;
	case 'a':
	    ftestPrintResultFlag = 1;
	    ftestPrintCheckFlag = 1;
	    ftestPrintTimingFlag = 1;
	    ftestPrintEnvFlag = 1; break;
	default: ftestError( CommandlineError, "unknown output type specifier `%c'\n", *outputType );
	}
	outputType++;
    }
}
//}}}

//{{{ static void ftestSignalHandler ( int signal ), static void ftestAlarmlHandler ( int )
//{{{ docu
//
// ftestSignalHandler(), ftestAlarmHandler() - signal handlers.
//
// Blocks new signals, flushes `cout', and calls `ftestError()'.
//
//}}}
static void
ftestSignalHandler ( int signal )
{
    ftestSignalCatch( true );
    cout.flush();
    ftestError( (ftestErrorT)((int)SignalError + signal ),
		"received signal %d\n", signal );
}
static void
ftestAlarmHandler ( int )
{
    ftestSignalCatch( true );
    cout.flush();
    ftestError( TimeoutError, "timeout after %d secs\n", ftestAlarm );
}
//}}}

//
// - external functions.
//

//{{{ const char * ftestSkipBlancs ( const char * string )
//{{{ docu
//
// ftestSkipBlancs() - skip all leading blancs in string.
//
// Return new position.
//
//}}}
const char *
ftestSkipBlancs ( const char * string )
{
    while ( *string && isspace( *string ) )
	string++;
    return string;
}
//}}}

//{{{ void ftestError ( const ftestErrorT errno, const char * format ... )
//{{{ docu
//
// ftestError() - main error handler.
//
// Prints error message consisting of formatString and following
// arguments, some additional information, and exits with errno.
//
//}}}
void
ftestError ( const ftestErrorT errno, const char * format ... )
{
    // print error message
    if ( format ) {
	fprintf( stderr, "%s: ", ftestExecName );
	va_list ap;
	va_start( ap, format );
	vfprintf( stderr, format, ap );
	va_end( ap );
    }

    switch ( errno ) {
    case CommandlineError:
    case EnvSyntaxError: 
    case CanFormSpecError:
    case FileError:
	// ftestUsage();
	break;
    case TimeoutError:
	if ( ftestPrintTimingFlag )
	    ftestPrint( "time  : > %.2f\n", "> %.2f\n", (float)ftestAlarm );
	if ( ftestPrintCheckFlag )
	    ftestPrint( "check : TmeOut\n", "TmeOut\n" );
	break;
    default:
	if ( ftestPrintTimingFlag )
	    ftestPrint( "time  : -0.00\n", "-0.00\n" );
	if ( ftestPrintCheckFlag )
	    ftestPrint( "check : Sig.%0.2d\n", "Sig.%0.2d\n", (int)errno-(int)SignalError );
    }
    exit( errno );
}
//}}}

//{{{ void ftestPrint ( const char * longFormat, const char * shortFormat ... )
//{{{ docu
//
// ftestPrint() - print according to `ftestPrintShortFlag'.
//
// If variable ftestPrintShortFlag is set, use shortFormat as
// format, otherwise longFormat.
//
//}}}
void
ftestPrint ( const char * longFormat, const char * shortFormat ... )
{
    va_list ap;
    va_start( ap, shortFormat );

    if ( ! ftestPrintShortFlag && longFormat )
	vprintf( longFormat, ap );
    else if ( ftestPrintShortFlag && shortFormat )
	vprintf( shortFormat, ap );

    va_end( ap );
}
//}}}

//{{{ void ftestSignalCatch ( bool block )
//{{{ docu
//
// ftestSignalCatch() - set signal and alarm handlers.
//
// If block is true block the signals instead.
//
//}}}
void
ftestSignalCatch ( bool block )
{
    void (*signalHandler)( int );
    void (*alarmHandler)( int );

    if ( block ) {
	signalHandler = 0;
	alarmHandler = 0;
    } else {
	signalHandler = ftestSignalHandler;
	alarmHandler = ftestAlarmHandler;
    }

#ifdef SIGHUP
    signal( SIGHUP, signalHandler );
#endif
#ifdef SIGINT
    signal( SIGINT, signalHandler );
#endif
#ifdef SIGQUIT
    signal( SIGQUIT, signalHandler );
#endif
#ifdef SIGILL
    signal( SIGILL, signalHandler );
#endif
#ifdef SIGIOT
    signal( SIGIOT, signalHandler );
#endif
#ifdef SIGFPE
    signal( SIGFPE, signalHandler );
#endif
#ifdef SIGBUS
    signal( SIGBUS, signalHandler );
#endif
#ifdef SIGSEGV
    signal( SIGSEGV, signalHandler );
#endif
#ifdef SIGTERM
    signal( SIGTERM, signalHandler );
#endif

    // alarm handler
    signal( SIGALRM, alarmHandler );
}
//}}}

//{{{ void ftestSetName ( const char * execName, const char * algorithmName, const char * usage )
//{{{ docu
//
// ftestSetName() - set name of executable and algorithm.
//
//}}}
void
ftestSetName ( const char * execName, const char * algorithmName, const char * usage )
{
    ftestExecName = execName;
    ftestAlgorithmName = algorithmName;
    ftestUsage = usage;
}
//}}}

//{{{ void ftestGetOpts ( const int argc, char ** argv, int & optind )
//{{{ docu
//
// ftestGetOpts() - read options from commandline.
//
// Returns new index into commandline in `optind'.
//
//}}}
void
ftestGetOpts ( const int argc, char ** argv, int & optind )
{
    // parse command line
    int optionChar;
    const char * outputType = 0;
    const char * envString = 0;

    // read from environment first
    envString = getenv( "FTEST_ALARM" );
    if ( envString )
	ftestAlarm = (int)strtol( envString, 0, 0 );
    envString = getenv( "FTEST_CIRCLE" );
    if ( envString )
	ftestCircle = (int)strtol( envString, 0, 0 );

    // parse options
    while ( (optionChar = getopt( argc, argv, "a:o:c:" )) != -1 ) {
	switch ( optionChar ) {
	case 'a': ftestAlarm = (int)strtol( optarg, 0, 0 ); break;
	case 'c': ftestCircle = (int)strtol( optarg, 0, 0 ); break;
	case 'o': outputType = optarg; break;
	default: ftestError( CommandlineError, 0 );
	}
    }
    optind = ::optind;

    if ( outputType )
	ftestParseOutputType( outputType );
}
//}}}

//{{{ void ftestGetEnv ( const int, char ** argv, int & optind )
//{{{ docu
//
// ftestGetEnv() - read factory environment specs, set environment.
//
// The new index into the commandline is returned in optind.
// The results are stored in global variable `ftestEnv'.
//
//}}}
void
ftestGetEnv ( const int, char ** argv, int & optind )
{
    // initialize environment
    ftestEnv.seed = 0;
    ftestEnv.seedSet = false;
    ftestEnv.characteristic = 0;
    ftestEnv.extDegree = 0;
    ftestEnv.extGen = 'Z';
    ftestEnv.varSpec = 0;
    for ( int i = 0; i < CFSwitchesMax; i++ )
	ftestEnv.switches[i] = false;

    char * envString = ftestConcatEnv( argv, optind );
    ftestParseEnv( envString );
    delete [] envString;
}
//}}}

//{{{ void ftestWriteSeed ()
//{{{ docu
//
// ftestWriteSeed() - write current seed to seed file.
//
//}}}
void
ftestWriteSeed ()
{
    if ( ftestSeedFile ) {
	FILE * seedFile = fopen( ftestSeedFile, "w" );
	if ( ! seedFile )
	    ftestError( FileError,
			"failed to open `%s' for writing: %s\n", ftestSeedFile,
			strerror( errno ) );
	fprintf( seedFile, "%d\n", factoryrandom( 0 ) );
	fclose( seedFile );
    }
}
//}}}

//{{{ void ftestPrintTimer ( const long timer )
//{{{ docu
//
// ftestPrintTimer() - print value of timer.
//
//}}}
void
ftestPrintTimer ( const long timer )
{
    if ( ftestPrintTimingFlag )
	ftestPrint( "time  : %.2f\n", "%.2f\n", (float)timer / HZ );
}
//}}}

//{{{ void ftestPrintCheck ( const ftestSatusT check )
//{{{ docu
//
// ftestPrintCheck() - print status of checks.
//
//}}}
void
ftestPrintCheck ( const ftestStatusT check )
{
    if ( ftestPrintCheckFlag ) {
	char * checkStr = 0;

	switch (check) {
	case Passed: checkStr = "Passed"; break;
	case Failed: checkStr = "Failed"; break;
	case UndefinedResult: checkStr = "Undef."; break;
	default: checkStr = "Error!";
	}

	ftestPrint( "check : %s\n", "%s\n", checkStr );
    }
}
//}}}

//{{{ void ftestPrintEnv ()
//{{{ docu
//
// ftestPrintEnv() - print current factory environment.
//
// The environment ist read from `ftestEnv'.
//
//}}}
void
ftestPrintEnv ()
{
    if ( ftestPrintEnvFlag ) {
	// characteristic
	if ( ftestEnv.extDegree )
	    ftestPrint( "char  : %d^%d(%c)\n",
			"%d^%d(%c)\n",
			ftestEnv.characteristic, ftestEnv.extDegree, ftestEnv.extGen );
	else
	    ftestPrint( "char  : %d\n", "%d\n", ftestEnv.characteristic );

	// switches
	bool switchSet = false;
	ftestPrint( "switch: ", (char *)0 );
	for ( int i = 0; i < CFSwitchesMax; i++ )
	    if ( ftestEnv.switches[i] ) {
		switchSet = true;
		printf( "%s ", ftestSwitchNames[i] );
	    }
	if ( ! switchSet )
	    printf( "(not set)\n" );
	else
	    printf( "\n" );

	// variables
	varSpecT * varSpec = ftestEnv.varSpec;
	ftestPrint( "vars  : ", (char *)0 );
	if ( ! varSpec )
	    printf( "(not specified)" );
	else
	    while ( varSpec ) {
		if ( varSpec->mipo.isZero() )
		    printf( "%c(%d) ",
			    varSpec->variable,
			    Variable( varSpec->variable ).level() );
		else {
		    printf( "%c(%d)",
			    varSpec->variable,
			    Variable( varSpec->variable ).level() );
		    cout << "(" << varSpec->mipo << ") ";
		}
		varSpec = varSpec->next;
	    }
	printf( "\n" );

	// number of repetitions
	ftestPrint( "circle: %d\n", "%d\n", ftestCircle );

	// random generator seed
	if ( ftestEnv.seedSet )
	    ftestPrint( "seed  : %d\n", "%d\n", ftestEnv.seed );
	else
	    ftestPrint( "seed  : (not set)\n", "(not set)\n" );

	// factory version
	const char * version;
	version = strchr( factoryVersion, '=' )+2;
	ftestPrint( "vers  : %s\n", "%s\n", version );
    }
}
//}}}

//
// - garbage.
//

#if 0
//{{{ static size_t ftestStrLen( const char * string )
//{{{ docu
//
// ftestStrLen() - return length of string, 0 for zero pointer.
//
//}}}
static size_t
ftestStrLen( const char * string )
{
    return string ? strlen( string ) : 0;
}
//}}}

//{{{ static char * ftestStrCat (char * to, const char * from)
//{{{ docu
//
// ftestStrCat() - concatenate (maybe empty) from and to.
//
//}}}
static char *
ftestStrCat (char * to, const char * from)
{
    if ( from )
	strcat( to, from );
    return to;
}
//}}}

//{{{ static void ftestSetRandom ()
//{{{ docu
//
// ftestSetRandom() - set random generator seed from `ftestEnv'.
//
// This function is quite spurious, but I keep it for future
// use.
//
//}}}
static void
ftestSetRandom ()
{
    if ( ftestEnv.seedSet )
	factoryseed( ftestEnv.seed );
}
//}}}

//{{{ static void ftestSetChar ( bool reset = false )
//{{{ docu
//
// ftestSetChar() - set characteristic from `ftestEnv'.
//
// If reset is true, reset characteristic to default.
//
// This function is quite spurious, but I keep it for future
// use.
//
//}}}
static void
ftestSetChar ( bool reset = false )
{
    if ( reset )
	setCharacteristic( 0 );
    else
	if ( ftestEnv.extDegree )
	    setCharacteristic( ftestEnv.characteristic,
			       ftestEnv.extDegree,
			       ftestEnv.extGen );
	else
	    setCharacteristic( ftestEnv.characteristic );
}
//}}}

//{{{ static void ftestSetSwitches ( bool reset = false )
//{{{ docu
//
// ftestSetSwitches() - set switches from `ftestEnv'.
//
// If reset is true, reset switches to default.
//
// This function is quite spurious, but I keep it for future
// use.
//
//}}}
static void
ftestSetSwitches ( bool reset = false )
{
    int i;

    if ( reset )
	for ( i = 0; i < CFSwitchesMax; i ++ )
	    Off( i );
    else
	for ( i = 0; i < CFSwitchesMax; i ++ )
	    if ( ftestEnv.switches[i] )
		On( i );
}
//}}}

//{{{ static void ftestSetVars ()
//{{{ docu
//
// ftestSetVars() - set variables from `ftestEnv'.
//
// This function is quite spurious, but I keep it for future
// use.
//
//}}}
static void
ftestSetVars ()
{
    varSpecT * varSpec = ftestEnv.varSpec;
    Variable * dummy;

    while ( varSpec ) {
	if ( varSpec->mipo.isZero() )
	    // polynomial variable
	    dummy = new Variable( varSpec->variable );
	else
	    dummy = new Variable( rootOf( varSpec->mipo, varSpec->variable ) );

	varSpec = varSpec->next;
    }
}
//}}}

//{{{ void ftestSetEnv ()
//{{{ docu
//
// ftestSetEnv() - set environment from `ftestEnv'.
//
// This function is quite spurious, but I keep it for future
// use.
//
//}}}
void
ftestSetEnv ()
{
    ftestSetRandom();
    ftestSetSwitches();
    ftestSetChar();
    ftestSetVars();
}
//}}}
#endif
