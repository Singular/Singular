/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: assert.h,v 1.0 1996-05-17 10:59:37 stobbe Exp $ */

/*
$Log: not supported by cvs2svn $
*/

/* It should be possible to include this file multiple times for different */
/* settings of NDEBUG */

#undef ASSERT
#undef __ASSERT
#undef STICKYASSERT
#undef DEBOUT
#undef DEBOUTLN

#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
#define ASSERT(ignore1,ignore2)((void)0)
#else
#define ASSERT(expression,message) \
((void)((expression) ? 0 : __ASSERT(#expression, message, __FILE__, __LINE__)))
#endif

#define STICKYASSERT(expression,message) \
((void)((expression) ? 0 : __ASSERT(#expression, message, __FILE__, __LINE__)))

#define __ASSERT(expression, message, file, line)  \
(fprintf( stderr, "error: %s\n%s:%u: failed assertion `%s'\n", \
 message, file, line, expression ), abort(), 0 )

#ifdef DEBUGOUTPUT
#define DEBOUT(stream,msg,object) \
(stream << msg << object, stream.flush())
#define DEBOUTLN(stream,msg,object) \
(stream << msg << object << endl)
#else
#define DEBOUT(stream,msg,object)
#define DEBOUTLN(stream,msg,object)
#endif
