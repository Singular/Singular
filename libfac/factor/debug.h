/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// static char * rcsid = "$Id: debug.h,v 1.1.1.1 1997-05-02 17:00:46 Singular Exp $" ;
////////////////////////////////////////////////////////////
// It is possible to include this file multiple times for different 
// settings of FACDEBUG and/or DEBUGOUTPUT

#undef ASSERT
#undef __ASSERT
#undef STICKYASSERT
#undef DEBOUT
#undef DEBOUTLN
#undef DEBINCLEVEL
#undef DEBDECLEVEL
#undef DEBOUTSL
#undef DEBOUTMSG
#undef DEBOUTNL

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
void deb_inc_level();
void deb_dec_level();
extern char * deb_level_msg;
#define DEBINCLEVEL(stream,msg) \
(stream << "\n" << deb_level_msg << "entering << " << msg << " >>" << endl, deb_inc_level())
#define DEBDECLEVEL(stream,msg) \
(deb_dec_level(), stream << deb_level_msg << "leaving << " << msg << " >>" << endl)
#define DEBOUTSL(stream) \
(stream << deb_level_msg, stream.flush())
#define DEBOUT(stream,msg,object) \
(stream << msg << object, stream.flush())
#define DEBOUTLN(stream,msg,object) \
(stream << deb_level_msg << msg << object << endl)
#define DEBOUTMSG(stream,msg) \
(stream << deb_level_msg << msg << endl)
#define DEBOUTNL(stream) \
(stream << endl)
#else
#define DEBINCLEVEL(stream,msg)
#define DEBDECLEVEL(stream,msg)
#define DEBOUTSL(stream)
#define DEBOUT(stream,msg,object)
#define DEBOUTLN(stream,msg,object)
#define DEBOUTMSG(stream,msg)
#define DEBOUTNL(stream)
#endif

/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:15:33  michael
Version for libfac-0.2.1

Revision 1.1  1996/01/17 05:19:43  michael
Initial revision

*/
