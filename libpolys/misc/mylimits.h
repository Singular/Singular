/* -*-c++-*- */
/*******************************************************************
 *  File:    mylimits.h
 *  Purpose: limits.h configuration for omalloc
 *  Author:  hannes (Hans Schoenemann)
 *  Created: 03/01
 *  Version: $Id$
 *******************************************************************/
#ifndef _MYLIMITS_H
#define _MYLIMITS_H

static const int MAX_INT_VAL = 2147483647;

#define ULONG_MAX (~0L)

#if ~0UL == 4294967295UL
#define LONG_MAX 9223372036854775807L
#else
#define LONG_MAX 2147483647L
#endif

#endif /* _MYLIMITS_H */
