dnl $Id: ftest_util.m4,v 1.1 1997-09-12 10:08:45 schmidt Exp $
dnl
dnl ftest_util.m4 - m4 macros used by the factory test environment.
dnl
dnl "External" macro start with prefix `ftest', "internal" macros
dnl with prefix `_'.
dnl
dnl Note: mind the ';'!
dnl
dnl do not output anything of this library
divert(-1)


#
# - internal macros.
#

#
# _stripTWS() - strip trailing white space from $1.
#
define(`_stripTWS', `dnl
patsubst(`$1', `[ 	]*$')')

#
# _qstripTWS() - strip trailing white space from $1, return
#   quoted result.
#
define(`_qstripTWS', `dnl
ifelse(
  translit(`$1', ` 	'), `', ,
  `patsubst(`$1', `\(.*[^ 	]\)[ 	]*$', ``\1'')')')


#
# - external macros.
#

#
# ftestPreprocInit() - initial preprocessor directives.
#
define(`ftestPreprocInit', `dnl
`#define TIMING
#include <timing.h>
'')

#
# ftestGlobalInit() - global initialization.
#
define(`ftestGlobalInit', `dnl
`TIMING_DEFINE_PRINT( ftestTimer )'')

#
# ftestMainInit() - initialization in main().
#
define(`ftestMainInit', `dnl
`int optind = 0;
    int check = UndefinedResult'')

#
# ftestMainExit() - clean up in main().
#
define(`ftestMainExit', `dnl
`ftestPrintExit();
    return check'')

#
# ftestOutVar() - declare output variable.
#
# $1: type of output variable
# $2: name of output variable
#
# Stores type of variable in macro _ftestOutType_<name>.
#
define(`ftestOutVar', `dnl
define(`_ftestOutType_'_qstripTWS(`$2'), `$1')dnl
`$1 '_qstripTWS(`$2')')

#
# ftestInVar() - declare input variable.
#
# $1: type of input variable
# $2: name of input variable
#
# Stores type of variable in macro _ftestInType_<name>.
#
define(`ftestInVar', `dnl
define(`_ftestInType_'_qstripTWS(`$2'), `$1')dnl
`$1 '_qstripTWS(`$2')')

#
# ftestGetOpts() - read options.
#
define(`ftestGetOpts', `dnl
`optind = ftestGetOpts( argc, argv )'')

#
# ftestGetEnv() - read environment.
#
define(`ftestGetEnv', `dnl
`optind = ftestGetEnv( argc, argv, optind )'')

#
# ftestGetInVar() - read varable from command line.
#
# $1: name of input variable.
#
define(`ftestGetInVar', `dnl
`$1= ftestGet'_stripTWS(`_ftestInType_$1')`( argv[ optind++ ] )'')

#
# ftestSetEnv() - set factory environment.
#
define(`ftestSetEnv', `')

#
# ftestRun() - run test.
#
define(`ftestRun', `dnl
`while ( ftestCircle > 0 ) {
	TIMING_START(ftestTimer);
	$1
	TIMING_END(ftestTimer);
	ftestCircle--;
    }'')

#
# ftestCheck() - run check.
#
define(`ftestCheck', `dnl
`check = '_qstripTWS(`$1')')

#
# ftestOuput() - print results.
#
# $1: name of algorithm
# $2: name of result (printed immediately before result
# $3: result variable to print
#
define(`ftestOutput', `dnl
`ftestPrintTimer( $1 );
    ftestPrintCheck( $1, check );
    ftestPrint'_stripTWS(`_ftestOutType_$3')`( $1, $2, $3)'')

dnl switch on output again
divert`'dnl
