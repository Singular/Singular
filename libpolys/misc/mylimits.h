/* -*-c++-*- */
/*******************************************************************
 *  File:    mylimits.h
 *  Purpose: limits.h configuration for omalloc
 *  Author:  hannes (Hans Schoenemann)
 *  Created: 03/01
 *******************************************************************/
#ifndef _MYLIMITS_H
#define _MYLIMITS_H

/* Maximum/minimum value an `signed int' can hold. */
// #define MAX_INT_VAL        2147483647
#ifndef MAX_INT_VAL
#define MAX_INT_VAL     (2147483647)
#endif

#ifndef MIN_INT_VAL
#define MIN_INT_VAL     (-MAX_INT_VAL-1)
#endif


// #define ULONG_MAX (~0UL) // ??
#define ULONG_MAX (~0L)

// TODO: fixing the following BUG (!?) leads to wrong ring::bitmask
// #if ~0UL != 4294967295UL
#if ~0UL == 4294967295UL
#define LONG_MAX 9223372036854775807L
#else
#define LONG_MAX 2147483647L
#endif

#endif /* _MYLIMITS_H */
