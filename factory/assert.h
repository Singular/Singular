/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: assert.h,v 1.2 1997-03-26 16:20:48 schmidt Exp $ */

/*
$Log: not supported by cvs2svn $
Revision 1.1  1996/12/05 18:19:31  schmidt
``Unconditional'' check-in.
Now it is my turn to develop factory.

Revision 1.0  1996/05/17 10:59:37  stobbe
Initial revision

*/

/* This is for backward compatibility only */
#if defined (NDEBUG) && ! defined (NOASSERT)
#define NOASSERT
#endif

/* It should be possible to include this file multiple times for different */
/* settings of NOASSERT */

#undef __ASSERT
#undef __ASSERT1
#undef STICKYASSERT
#undef ASSERT
#undef ASSERT1

#undef __WARN
#undef STICKYWARN
#undef WARN

#undef PVIRT_VOID
#undef PVIRT_INTCF
#undef PVIRT_BOOL
#undef PVIRT_INT
#undef PVIRT_CHARCC

#include <stdio.h>
#include <stdlib.h>

#define __ASSERT(expression, message, file, line) \
(fprintf( stderr, "error: " message "\n%s:%u: failed assertion `%s'\n", \
 file, line, expression ), abort(), 0 )
#define __ASSERT1(expression, message, parameter1, file, line)  \
(fprintf( stderr, "error: " message "\n%s:%u: failed assertion `%s'\n", \
 parameter1, file, line, expression ), abort(), 0 )
#define STICKYASSERT(expression, message) \
((void)((expression) ? 0 : __ASSERT(#expression, message, __FILE__, __LINE__)))

#define __WARN(expression, message, file, line)  \
(fprintf( stderr, "warning: " message "\n%s:%u: failed assertion `%s'\n", \
 file, line, expression ), 0 )
#define STICKYWARN(expression, message) \
((void)((expression) ? 0 : __WARN(#expression, message, __FILE__, __LINE__)))

#ifndef NOASSERT
#define ASSERT(expression, message) \
((void)((expression) ? 0 : __ASSERT(#expression, message, __FILE__, __LINE__)))
#define ASSERT1(expression, message, parameter1) \
((void)((expression) ? 0 : __ASSERT1(#expression, message, parameter1, __FILE__, __LINE__)))

#define WARN(expression, message) \
((void)((expression) ? 0 : __WARN(#expression, message, __FILE__, __LINE__)))

#define PVIRT_VOID(msg) \
{ fprintf( stderr, "pure method( " msg " ) called\n" ); abort(); }
#define PVIRT_INTCF(msg) \
{ fprintf( stderr, "pure method( " msg " ) called\n" ); abort(); return 0; }
#define PVIRT_BOOL(msg) \
{ fprintf( stderr, "pure method( " msg " ) called\n" ); abort(); return false; }
#define PVIRT_INT(msg) \
{ fprintf( stderr, "pure method( " msg " ) called\n" ); abort(); return 0; }
#define PVIRT_CHARCC(msg) \
{ fprintf( stderr, "pure method( " msg " ) called\n" ); abort(); return 0; }
#else /* NOASSERT */
#define ASSERT(expression, message)
#define ASSERT1(expression, message, parameter1)

#define WARN(expression, message)

#define PVIRT_VOID(msg) = 0
#define PVIRT_INTCF(msg) = 0
#define PVIRT_BOOL(msg) = 0
#define PVIRT_INT(msg) = 0
#define PVIRT_CHARCC(msg) = 0
#endif /* NOASSERT */
