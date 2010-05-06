/* -*-c++-*- */

/*******************************************************************
 *  File:    aux.h
 *  Purpose: 
 *  Author:  
 *  Created: 
 *  Version: $Id$
 *******************************************************************/

// BOOLEAN
// max, min?


#ifndef _MISC_AUX_H
#define _MISC_AUX_H

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL        (0)
#endif

// #ifdef _TRY
#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif
// #endif

static const int MAX_INT_LEN= 11;
typedef void* ADDRESS;

#if (SIZEOF_LONG == 8)
typedef int BOOLEAN;
/* testet on x86_64, gcc 3.4.6: 2 % */
/* testet on IA64, gcc 3.4.6: 1 % */
#else
/* testet on athlon, gcc 2.95.4: 1 % */
typedef short BOOLEAN;
#endif
#define loop for(;;)

#if defined(SI_CPU_I386) || defined(SI_CPU_X86_64)
  // the following settings seems to be better on i386 and x86_64 processors
  // define if a*b is with mod instead of tables
#define HAVE_MULT_MOD
  // #define HAVE_GENERIC_ADD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#elif defined(SI_CPU_IA64)
  // the following settings seems to be better on itanium processors
  // #define HAVE_MULT_MOD
#define HAVE_GENERIC_ADD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#elif defined(SI_CPU_SPARC)
  // #define HAVE_GENERIC_ADD
#define HAVE_MULT_MOD
#ifdef HAVE_MULT_MOD
#define HAVE_DIV_MOD
#endif
#elif defined(SI_CPU_PPC)
  // the following settings seems to be better on ppc processors
  // testet on: ppc_Linux, 740/750 PowerMac G3, 512k L2 cache
#define HAVE_MULT_MOD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#endif


#endif /* _MISC_AUX_H */

