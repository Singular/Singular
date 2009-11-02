/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_FTEST_UTIL_H
#define INCL_FTEST_UTIL_H

//{{{ docu
//
// ftest_util.h - header to ftest_util.cc.
//
//}}}

enum ftestStatusT {
    Passed = 0, Failed, UndefinedResult
};

//{{{ enum ftestErrorT
//{{{ docu
//
// enum ftestErrorT - FTE error types.
//
// noError: no error (rarely used :-)
// CommandlineError: commandline syntax error
// EnvSyntaxError: error in environment specification
// FileError: error reading some file
// CanFormSpecError: error in canonical form specification
// CheckError: error in result checks
// SignalError: process received a signal
// TimeoutError: and this signal was due to our own call to `alarm()'
//
//}}}
enum ftestErrorT
{
    noError, CommandlineError, EnvSyntaxError, FileError,
    CanFormSpecError, CheckError,
    SignalError, TimeoutError = SignalError + 14
};
//}}}

extern int ftestCircle;
extern int ftestAlarm;

extern int ftestCheckFlag;
extern int ftestPrintFlag;
extern int ftestPrintResultFlag;

const char * ftestSkipBlancs ( const char * string );
char * ftestCutBlancs ( char * string );
const char * ftestSubStr ( const char * subString, const char * string );
void ftestError ( ftestErrorT errno, const char * format ... );
void ftestUsagePrint ( const char * additionalUsage = 0 );

void ftestSignalCatch ( bool block = false );

void ftestSetName ( const char * execName, const char * algorithmName, const char * usage );

void ftestGetOpts ( int argc, char ** argv, int & optind );
void ftestGetEnv ( int, char ** argv, int & optind );
bool ftestSearchTaggedArg ( int, char ** argv, int & optind, const char * optionTag );
void ftestWriteSeed ();

void ftestPrintTimer ( long timer );
void ftestPrintCheck ( ftestStatusT check );
void ftestPrintEnv ();

#endif /* ! INCL_FTEST_UTIL_H */
