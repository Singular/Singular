#ifndef FE_OPT_TAB_H
#define FE_OPT_TAB_H

#define LONG_OPTION_RETURN 13

// Define here which cmd-line options are recognized
#ifndef FE_OPT_STRUCTURE
EXTERN_VAR struct fe_option feOptSpec[];
#else
VAR struct fe_option feOptSpec[] =
{
//
//  Has to be of the form
//    {name, has_arg, val,
//     arg_name, help, type, value, set}
//  where:
//
//        name   is the name of the long option.
//
//        has_arg
//               is:  no_argument (or 0) if the option does not take
//               an argument, required_argument (or 1) if the option
//               requires  an  argument, or optional_argument (or 2)
//               if the option takes an optional argument.
//
//        val    is  the  value  to  return,  or  to  load  into the
//               variable pointed to by flag.
//               NEEDS TO BE LONG_OPTION_RETURN, for long option
//                           short option char,  for short option
//
//        arg_name if set, uses this value as name for argument in
//                display of help
//
//        help  one-line description of option
//
//        type  one of feOptUntyped (value is never set),
//                     feOptBool, feOptInt, feOptString
//
//        value (default) value of option
//
//       set   only relevant for feOptString:
//             1: if value different from default value
//             0: otherwise
//
// The order in which options are specified is the order in which
// their help is printed on -h
//
// Options whose hel starts with an "//" are considered undocumented,
// i.e., their help is not printed on -h
//
#if defined(ESINGULAR) || defined(TSINGULAR)
#ifdef ESINGULAR
// options only relevant for ESINGULAR
  {"emacs",         required_argument,      LONG_OPTION_RETURN,
   "EMACS",     "Use EMACS as emacs program to run Singular",          feOptString, 0,   0},

  {"emacs-dir",         required_argument,  LONG_OPTION_RETURN,
   "DIR",       "Use DIR as directory to look for emacs lisp files",   feOptString, 0,   0},

  {"emacs-load",        required_argument,  LONG_OPTION_RETURN,
   "FILE",      "Load FILE on emacs start-up, instead of default",     feOptString, 0,   0},
#else
  {"xterm",         required_argument,      LONG_OPTION_RETURN,
   "XTERM",     "Use XTERM as terminal program to run Singular",       feOptString, 0,   0},
#endif

  {"singular",          required_argument,  LONG_OPTION_RETURN,
   "PROG",      "Start PROG as Singular program within emacs",         feOptString, 0,   0},

  {"no-call",     no_argument,        LONG_OPTION_RETURN,
   0,          "Do not start program. Print call to stdout",           feOptBool,   0,   0},
#endif

  {"batch",             no_argument,        'b',
   0,          "Run in batch mode (requires --link/--MPhost/--MPport)",                                    feOptBool,    0,     0},

  {"execute",           required_argument,  'c',
   "STRING",   "Execute STRING on start-up",                           feOptString, 0,   0},

  {"sdb",               no_argument,        'd',
   0,          "Enable source code debugger (experimental)",           feOptBool,    0,      0},

  {"echo",              optional_argument,  'e',
   "VAL",       "Set value of variable `echo' to (integer) VAL",       feOptInt,    0,      0},

  {"help",              no_argument,        'h',
   0,          "Print help message and exit",                          feOptUntyped,    0,      0},

  {"profile",           no_argument,        'p',
   0,          "Collect profiling data in smon.out",                   feOptBool,    0,      0},
  {"quiet",             no_argument,        'q',
   0,          "Do not print start-up banner and lib load messages",   feOptBool,    0,      0},
  {"sort",             no_argument,        's',
   0,          "// Sort NTL results",                                  feOptBool,    0,      0},

  {"random",            required_argument,  'r',
   "SEED",     "Seed random generator with (integer) SEED",            feOptInt,    0,      0},

  {"no-tty",            no_argument,        't',
   0,          "Do not redefine the terminal characteristics",         feOptBool,    0,      0},

  {"user-option",       required_argument,  'u',
   "STRING",   "Return STRING on `system(\"--user-option\")'",         feOptString, 0,   0},

  {"version",           no_argument,        'v',
   0,          "Print extended version and configuration info",        feOptUntyped,    0,      0},


  {"allow-net",         no_argument,        LONG_OPTION_RETURN,
   0,          "Allow one to fetch (html) help pages from the net",    feOptBool,    0,      0},

  {"browser",           required_argument,  LONG_OPTION_RETURN,
   "BROWSER",  "Display help in BROWSER (see help.cnf)",       feOptString, 0,   0},

  {"cntrlc",           optional_argument,   LONG_OPTION_RETURN,
   "CHAR",     "Automatic answer for CTRL-C prompt",                   feOptString, 0,   0},
#ifndef ESINGULAR
  {"emacs",             no_argument,        LONG_OPTION_RETURN,
   0,          "Set defaults for running within emacs",                feOptBool,    0,      0},
#endif

  {"no-stdlib",         no_argument,        LONG_OPTION_RETURN,
   0,          "Do not load `standard.lib' on start-up",               feOptBool,    0,      0},

  {"no-rc",             no_argument,        LONG_OPTION_RETURN,
   0,          "Do not execute `.singularrc' file(s) on start-up",     feOptBool,    0,      0},

  {"no-warn",           no_argument,        LONG_OPTION_RETURN,
   0,          "Do not display warning messages",                      feOptBool,    0,      0},

  {"no-out",            no_argument,        LONG_OPTION_RETURN,
   0,          "Suppress all output",                                  feOptBool,    0,      0},

  {"no-shell",          no_argument,        LONG_OPTION_RETURN,
   0,          "Restricted mode: prohibit shell escape commands and links", feOptBool,  0,      0},

  {"min-time",          required_argument,  LONG_OPTION_RETURN,
  "SECS",     "Do not display times smaller than SECS (in seconds)",   feOptString, (void*) "0.5",  0},

  {"cpus",            required_argument,    LONG_OPTION_RETURN,
   "#CPUs",   "maximal number of CPUs to use",                         feOptInt,    (void*)2,      0},

  {"threads",         required_argument,    LONG_OPTION_RETURN,
   "#threads", "maximal number of CPUs to use for threads",            feOptInt,    (void*)2,      0},


  {"MPport",           required_argument,   LONG_OPTION_RETURN,
   "PORT",     "Use PORT number for connections",                       feOptString,    0,      0},

  {"MPhost",           required_argument,   LONG_OPTION_RETURN,
   "HOST",     "Use HOST for connections",                             feOptString,    0,   0},

  {"link",           required_argument,   LONG_OPTION_RETURN,
   "LINK",     "Use LINK for connections",                             feOptString,    0,   0},

  {"ticks-per-sec",     required_argument,  LONG_OPTION_RETURN,
   "TICKS",    "Sets unit of timer to TICKS per second",               feOptInt,    (void*)1,      0},

  {"dump-versiontuple",       no_argument,  LONG_OPTION_RETURN,
   0,          "//Display the version-tuple and exit",  feOptUntyped,    0,      0},

// terminator -- do NOT remove
  { 0, 0, 0, 0, 0, feOptInt, 0, 0}
};

#endif
#endif
