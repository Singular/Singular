/* emacs edit mode for this file is -*- C -*- */
/* $Id$ */

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
 * ---RS---
 * This is the specialized version of config.h for compiling
 * Factory on Windows NT 4.0 with MS Visula C++ 5.x
 * ---RS---
 */
/* }}} */

/************** START OF CONFIGURABLE SECTION **************/

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* factory version */
#define FACTORYVERSION "1.3b"

/* factory configuration */
#define FACTORYCONFIGURATION "WINNT"

/* where the gftables live */
#define GFTABLEDIR "."

/* define if your compiler does arithmetic shift */
#define HAS_ARITHMETIC_SHIFT 1

/* define to build factory without stream IO */
#undef NOSTREAMIO

/* define if linked to Singular */
#undef SINGULAR

/* define if linked with factory memory manager */
#define USE_MEMUTIL 1

/* define if linked with old factory memory manager */
#define USE_OLD_MEMMAN 1

/* define if linked with new factory manager, debugging version */
#undef MDEBUG

/* define if you do not want to activate assertions */
#undef NOASSERT

/* define if you want to activate the timing stuff */
#undef TIMING

/* define if you want to have debugging output */
#undef DEBUGOUTPUT

/* define type of your compilers 64 bit integer type */
#define INT64 __int64

/************** END OF CONFIGURABLE SECTION **************/

#endif /* ! INCL_CONFIG_H */
