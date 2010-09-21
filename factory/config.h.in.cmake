/* emacs edit mode for this file is -*- C -*- */
/* $Id: config.h.in,v 1.14 2008/08/20 13:35:55 Singular Exp $ */

#ifndef INCL_CONFIG_H
#define INCL_CONFIG_H

/* {{{ docu
 *
 * config.h.in - used by `configure' to create `config.h'.
 *
 * This file is included at building time from almost all source
 * files belonging to Factory.  Furthermore, it is (textually)
 * included into `factoryconf.h' by `makeheader' so we have an
 * installed version of this file, too.  This way, the installed
 * source files will be compiled with the same settings as the
 * library itself.
 *
 * In general, you should let `configure' guess the correct
 * values for the `#define's below.  But if something seriously
 * goes wrong in configuring, please inform the authors and feel
 * free to edit the marked section.
 *
 */
/* }}} */

/************** START OF CONFIGURABLE SECTION **************/

/* Define to empty if the keyword does not work.  */
#cmakedefine const

/* Define as __inline if that's what the C compiler calls it.  */
#cmakedefine inline

/* factory version */
#cmakedefine FACTORYVERSION "@FACTORYVERSION@"

/* factory configuration */
#cmakedefine FACTORYCONFIGURATION "@FACTORYCONFIGURATION@"

/* where the gftables live */
#cmakedefine GFTABLEDIR "@GFTABLEDIR@"

/* define if your compiler does arithmetic shift */
#cmakedefine HAS_ARITHMETIC_SHIFT 1

/* define to use "configurable inline methods" (see `cf_inline.cc') */
#cmakedefine CF_USE_INLINE 1

/* define to build factory without stream IO */
#cmakedefine NOSTREAMIO 1

/* which C++ header variants to use */
#cmakedefine HAVE_IOSTREAM_H 1 
#cmakedefine HAVE_FSTREAM_H 1
#cmakedefine HAVE_STRSTREAM_H 1

#cmakedefine HAVE_IOSTREAM 1
#cmakedefine HAVE_FSTREAM 1
#cmakedefine HAVE_STRING 1

#cmakedefine HAVE_CSTDIO 1

/* define if linked to Singular */
#cmakedefine SINGULAR 1

/* define if build with OMALLOC */
#cmakedefine HAVE_OMALLOC 1

/* define if linked with factory memory manager */
#cmakedefine USE_MEMUTIL 1

/* define if linked with old factory memory manager */
#cmakedefine USE_OLD_MEMMAN 1

/* define if linked with new factory manager, debugging version */
#cmakedefine MDEBUG 1

/* define if you do not want to activate assertions */
#cmakedefine NOASSERT 1

/* define if you want to activate the timing stuff */
#cmakedefine TIMING 1

/* define if you want to have debugging output */
#cmakedefine DEBUGOUTPUT 1

/* define type of your compilers 64 bit integer type */
#define INT64 long long int

#cmakedefine HAVE_NTL 1

/************** END OF CONFIGURABLE SECTION **************/

#endif /* ! INCL_CONFIG_H */
