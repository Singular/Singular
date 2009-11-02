/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

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
#include <fstream.h>

// we include timing.h for calculation of HZ only
#define TIMING
#include <timing.h>

#include <factory.h>

// we include `cf_switches.h' for the maximum number of switches
// and hope that we will not get any conflicts
#include <cf_switches.h>

#include "ftest_util.h"
#include "ftest_io.h"

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
// ftestAlarm: set by ftestGetOpts() from commandline, read by
//   main().
// ftestCheckFlag: set by ftestGetOpts() from commandline, read by
//   main().  False iff checks should not be executed.
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

int ftestCheckFlag = 1;

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
// ftestUsage: set by ftestSetName(), read by ftestUsagePrint().
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
	    ifstream seedStream( ftestSeedFile );
	    if ( ! seedStream ) {
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
	    } else {
		// if file exists, read seed
		seedStream >> seed;
		if ( ! seedStream )
		    ftestError( FileError,
				"garbled seed in seed file `%s'\n", ftestSeedFile );

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
	    ftestReadString( mipoString, mipo );
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

//{{{ char * ftestCutBlancs ( char * string )
//{{{ docu
//
// ftestCutBlancs() - cut off all trailing blancs in `string'.
//
// Returns `string' unchanged.
//
//}}}
char *
ftestCutBlancs ( char * string )
{
    char * stringCursor = string + strlen( string );

    while ( (string != stringCursor)
	    && isspace( *(stringCursor-1) ) )
	stringCursor--;

    *stringCursor = '\0';
    return string;
}
//}}}

//{{{ const char * ftestSubStr ( const char * subString, const char * string )
//{{{ docu
//
// ftestSubStr() - check whether subString is a substring of string.
//
// If so, return index behind subString in string, otherwise
// string.
//
//}}}
const char *
ftestSubStr ( const char * subString, const char * string )
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

//{{{ void ftestError ( ftestErrorT errno, const char * format ... )
//{{{ docu
//
// ftestError() - main error handler.
//
// Prints error message consisting of formatString and following
// arguments, some additional information, and exits with errno.
// In case of an `CheckError', does not exit but returns to the
// caller.
//
//}}}
void
ftestError ( ftestErrorT errno, const char * format ... )
{
    // print error message
    if ( format ) {
	if ( errno != CheckError )
	    fprintf( stderr, "%s: ", ftestExecName );
	else
	    fprintf( stderr, "%s(CheckError): ", ftestExecName );
	va_list ap;
	va_start( ap, format );
	vfprintf( stderr, format, ap );
	va_end( ap );
    }

    switch ( errno ) {
    case noError:
	return;
    case CommandlineError:
    case EnvSyntaxError: 
    case CanFormSpecError:
	ftestUsagePrint();
	break;
    case FileError:
	break;
    case CheckError:
	return;
    case TimeoutError:
	if ( ftestPrintTimingFlag )
	    cout << "Time:\t\t> " << (float)ftestAlarm << endl;
	if ( ftestPrintCheckFlag )
	    cout << "Check:\t\tTime Out" << endl;
	break;
    default:
	if ( ftestPrintCheckFlag )
	    cout << "Check:\t\tReceived Signal " << (int)errno-(int)SignalError << endl;
    }
    exit( errno );
}
//}}}

//{{{ void ftestUsagePrint ( const char * additionalUsage )
//{{{ docu
//
// ftestUsagePrint() - print usage message to stderr.
//
// We use the static variable `ftestUsage' to do so.  If
// `additionalUsage' is non-zero, we print it after
// `ftestUsage', otherwise some short reference where to find
// more information.
//
//}}}
void
ftestUsagePrint ( const char * additionalUsage )
{
    cerr << ftestUsage << flush;
    if ( ! additionalUsage )
	cerr << "
For a descriptions of the features common to all programs of the
Factory Test Environment, call `feval -?' (exactly in this way).
" << endl;
    else
	cerr << additionalUsage << endl;
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

//{{{ void ftestGetOpts ( int argc, char ** argv, int & optind )
//{{{ docu
//
// ftestGetOpts() - read options from commandline.
//
// Returns new index into commandline in `optind'.
//
//}}}
void
ftestGetOpts ( int argc, char ** argv, int & optind )
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
    while ( (optionChar = getopt( argc, argv, "a:o:c:t" )) != -1 ) {
	switch ( optionChar ) {
	case 'a': ftestAlarm = (int)strtol( optarg, 0, 0 ); break;
	case 'c': ftestCircle = (int)strtol( optarg, 0, 0 ); break;
	case 'o': outputType = optarg; break;
	case 't': ftestCheckFlag = false; break;
	default: ftestError( CommandlineError, 0 );
	}
    }
    optind = ::optind;

    if ( outputType )
	ftestParseOutputType( outputType );
}
//}}}

//{{{ void ftestGetEnv ( int, char ** argv, int & optind )
//{{{ docu
//
// ftestGetEnv() - read factory environment specs, set environment.
//
// The new index into the commandline is returned in optind.
// The results are stored in global variable `ftestEnv'.
//
//}}}
void
ftestGetEnv ( int, char ** argv, int & optind )
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

//{{{ bool ftestSearchTaggedArg ( int, char ** argv, int & optind, const char * optionTag )
//{{{ docu
//
// ftestSearchTaggedArg() - search for tagged argument.
//
// If the current argument (`argv[ optind ]') is of the form
// `<tag>=<value>', where `<tag>' is a substring of
// `<optionTag>', the current argument is modified to point
// directly after the `=' sign and `true' is returned.
//
// Otherwise, `false' is returned.
//
// Note: There may be arbitrary white-space between the several
// tokens of the tagged argument, but the tokens must not span
// more than one argument.
//
//}}}
bool
ftestSearchTaggedArg ( int, char ** argv, int & optind, const char * optionTag )
{
    const char * tokenCursor;
    const char * tokenString = argv[ optind ];
    int tokenLen;

    // check for end of argument list
    if ( ! tokenString )
	return false;

    // skip leading blancs
    tokenString = ftestSkipBlancs( tokenString );

    // check whether it is a tagged argument
    tokenCursor = strchr( tokenString, '=' );
    if ( ! tokenCursor )
	return false;
    tokenLen = tokenCursor - tokenString;

    // isolate possible keyword
    char * keyword = new char[ tokenLen+1 ];
    strncpy( keyword, tokenString, tokenLen );
    keyword[ tokenLen ] = '\0';
    ftestCutBlancs( keyword );

    // advance tokenString after `=' sign
    tokenString = tokenCursor+1;

    // check whether keyword is a substring of `optionTag'
    tokenCursor = ftestSubStr( keyword, optionTag );
    if ( tokenCursor == optionTag ) {
	// optionTag not found
	delete [] keyword;
	return false;
    } else {
	argv[ optind ] = (char *)tokenString;
	return true;
    }
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
	ofstream seedStream( ftestSeedFile );
	if ( ! seedStream )
	    ftestError( FileError,
			"failed to open `%s' for writing: %s\n", ftestSeedFile,
			strerror( errno ) );
	seedStream << factoryrandom( 0 ) << endl;
    }
}
//}}}

//{{{ void ftestPrintTimer ( long timer )
//{{{ docu
//
// ftestPrintTimer() - print value of timer.
//
//}}}
void
ftestPrintTimer ( long timer )
{
    if ( ftestPrintTimingFlag )
	cout << "Time:\t\t" << (float)timer / HZ << endl;
}
//}}}

//{{{ void ftestPrintCheck ( ftestSatusT check )
//{{{ docu
//
// ftestPrintCheck() - print status of checks.
//
//}}}
void
ftestPrintCheck ( ftestStatusT check )
{
    if ( ftestPrintCheckFlag ) {
	char * checkStr = 0;

	switch (check) {
	case Passed: checkStr = "Passed"; break;
	case Failed: checkStr = "Failed"; break;
	case UndefinedResult: checkStr = "Undefined"; break;
	default: checkStr = "Error!";
	}

	cout << "Check:\t\t" << checkStr << endl;
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
	    cout << "Characteristic:\t" << ftestEnv.characteristic
		 << '^' << ftestEnv.extDegree
		 << '(' << ftestEnv.extGen << ')' << endl;
	else
	    cout << "Characteristic:\t" << ftestEnv.characteristic << endl;

	// switches
	bool switchSet = false;
	cout << "Switches:\t";
	for ( int i = 0; i < CFSwitchesMax; i++ )
	    if ( ftestEnv.switches[i] ) {
		switchSet = true;
		cout << ftestSwitchNames[i] << ' ';
	    }
	if ( ! switchSet )
	    cout << "(not set)" << endl;
	else
	    cout << endl;

	// variables
	varSpecT * varSpec = ftestEnv.varSpec;
	cout << "Variables:\t";
	if ( ! varSpec )
	    cout << "(not specified)" << endl;
	else {
	    while ( varSpec ) {
		if ( varSpec->mipo.isZero() )
		    cout << varSpec->variable
			 << "(" << Variable( varSpec->variable ).level() << ") ";
		else {
		    cout << varSpec->variable
			 << "(" << Variable( varSpec->variable ).level()
			 << ", " << varSpec->mipo << ") ";
		}
		varSpec = varSpec->next;
	    }
	    cout << endl;
	}

	// number of repetitions
	cout << "Circles:\t" << ftestCircle << endl;

	// random generator seed
	if ( ftestEnv.seedSet )
	    cout << "Seed:\t\t" << ftestEnv.seed << endl;
	else
	    cout << "Seed:\t\t(not set)" << endl;

	// factory version
	const char * version;
	version = strchr( factoryVersion, '=' )+2;
	cout << "Version:\t" << version << endl;
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
