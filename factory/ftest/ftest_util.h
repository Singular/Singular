/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftest_util.h,v 1.9 1997-11-21 10:40:48 schmidt Exp $ */

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
// Ok: no error (rarely used :-)
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
    Ok, CommandlineError, EnvSyntaxError, FileError,
    CanFormSpecError, CheckError,
    SignalError, TimeoutError = SignalError + 14
};
//}}}

extern int ftestCircle;
extern int ftestAlarm;

extern int ftestPrintFlag;
extern int ftestPrintResultFlag;

const char * ftestSkipBlancs ( const char * string );
void ftestError ( const ftestErrorT errno, const char * format ... );
void ftestUsagePrint ( const char * additionalUsage = 0 );
void ftestPrint ( const char * longFormat, const char * shortFormat ... );

void ftestSignalCatch ( bool block = false );

void ftestSetName ( const char * execName, const char * algorithmName, const char * usage );

void ftestGetOpts ( const int argc, char ** argv, int & optind );
void ftestGetEnv ( const int, char ** argv, int & optind );
void ftestWriteSeed ();

void ftestPrintTimer ( const long timer );
void ftestPrintCheck ( const ftestStatusT check );
void ftestPrintEnv ();

#endif /* ! INCL_FTEST_UTIL_H */
