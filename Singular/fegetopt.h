/* Declarations for getopt.
   Copyright (C) 1989, 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
/*
   obachman 9/99: adapted to Singular by
    * adding prefix fe_ to global variables
    * extended fe_option structure
*/

#ifndef FEGETOPT_H
#define FEGETOPT_H 1

#ifdef        __cplusplus
extern "C" {
#endif

/* For communication from `getopt' to the caller.
   When `getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when `ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

EXTERN_VAR char *fe_optarg;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `getopt'.

   On entry to `getopt', zero means this is the first call; initialize.

   When `getopt' returns EOF, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

EXTERN_VAR int fe_optind;

/* Callers store zero here to inhibit the error message `getopt' prints
   for unrecognized options.  */

EXTERN_VAR int fe_opterr;

/* Set to an option character which was unrecognized.  */

EXTERN_VAR int fe_optopt;

/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument                (or 0) if the option does not take an argument,
   required_argument        (or 1) if the option requires an argument,
   optional_argument         (or 2) if the option takes an optional argument.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `optarg', set the
   `val' field to a nonzero value (the equivalent single-letter option
   character, if there is one).
   For long options `getopt' returns the contents of the `val' field.  */

typedef enum {feOptUntyped, feOptBool, feOptInt, feOptString} feOptType;
struct fe_option
{
#if        __STDC__
  const char *name;
#else
  char *name;
#endif
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int val;
  /* Stuff added for Singular  */
  const char*   arg_name;/* name of argument, for display in help */
  const char*   help;    /* (short) help string */
  feOptType     type;    /* type of argument, if has_arg > 0 */
  void*         value;   /* (default) value of option */
  int           set;     /* only relevant for strings: 0 if not set, 1 if set */
};

/* Names for the values of the `has_arg' field of `struct option'.  */

#define        no_argument       0
#define required_argument        1
#define optional_argument        2

#if defined(__STDC__) || defined(PROTO) || defined(__cplusplus)
extern int fe_getopt (int argc, char *const *argv, const char *shortopts);
extern int fe_getopt_long (int argc, char *const *argv, const char *shortopts,
                        const struct fe_option *longopts, int *longind);
extern int fe_getopt_long_only (int argc, char *const *argv,
                             const char *shortopts,
                             const struct fe_option *longopts, int *longind);

/* Internal only.  Users should not call this directly.  */
extern int _fe_getopt_internal (int argc, char *const *argv,
                             const char *shortopts,
                             const struct fe_option *longopts, int *longind,
                             int long_only);
#else /* not __STDC__ */
extern int fe_getopt ();
extern int fe_getopt_long ();
extern int fe_getopt_long_only ();

extern int _fe_getopt_internal ();
#endif /* not __STDC__ */

#ifdef        __cplusplus
}
#endif

#endif /* _GETOPT_H */
