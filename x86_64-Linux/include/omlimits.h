/* omlimits.h.  Generated automatically by configure.  */
/* -*-c++-*- */
/*******************************************************************
 *  File:    mylimits.h
 *  Purpose: limits.h configuration for omalloc
 *  Author:  hannes (Hans Schoenemann)
 *  Created: 03/01
 *  Version: $Id$
 *******************************************************************/
#ifndef MYLIMITS_H
#define MYLIMITS_H
/*******************************************************************
 * Defines which are set by configure
 ******************************************************************/
/* Define sizeof(long) */
#define SIZEOF_LONG 8
/* Define sizeof(int) */
#define SIZEOF_INT 4
/* Define sizeof(double) */
#define SIZEOF_DOUBLE 8
/*
 *    ISO C Standard: 4.14/2.2.4.2 Limits of integral types        <limits.h>
 */

/* These assume 8-bit `char's, 16-bit `short int's,
   and 32-bit `int's and and 32/64-bit `long int's.  */

/* maximum value a `signed short int' can hold.  */
#define SHRT_MAX        32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#define USHRT_MAX        65535

/* Maximum/minimum value an `signed int' can hold. */
#define INT_MAX        2147483647
#define INT_MIN (-INT_MAX-1)

/* Minimum and maximum values a `signed long int' can hold.  */
/* Maximum value an `unsigned long int' can hold.  (Minimum is 0.)  */
#if SIZEOF_LONG == 8
# define LONG_MAX        9223372036854775807L
# define ULONG_MAX      18446744073709551615UL
#else
# define LONG_MAX        2147483647L
# ifdef __STDC__
#  define ULONG_MAX      4294967295UL
# else
#  define ULONG_MAX      4294967295L
# endif
#endif
#define LONG_MIN        (-LONG_MAX - 1L)

#endif
