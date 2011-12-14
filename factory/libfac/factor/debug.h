////////////////////////////////////////////////////////////
// $Id$
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
void deb_inc_level(); // use this helper routines from factory
void deb_dec_level(); // use this helper routines from factory
extern char * deb_level_msg; // from factory
#define DEBINCLEVEL(stream,msg) \
(stream << "\n" << deb_level_msg << "entering << " << msg << " >>" << "\n", deb_inc_level())
#define DEBDECLEVEL(stream,msg) \
(deb_dec_level(), stream << deb_level_msg << "leaving << " << msg << " >>" << "\n")
#define DEBOUTSL(stream) \
(stream << deb_level_msg, stream.flush())
#define DEBOUT(stream,msg,object) \
(stream << deb_level_msg << msg << object, stream.flush())
#define DEBOUTLN(stream,msg,object) \
(stream << deb_level_msg << msg << object << "\n")
#define DEBOUTMSG(stream,msg) \
(stream << deb_level_msg << msg << "\n")
#define DEBOUTNL(stream) \
(stream << "\n")
#else
#define DEBINCLEVEL(stream,msg)
#define DEBDECLEVEL(stream,msg)
#define DEBOUTSL(stream)
#define DEBOUT(stream,msg,object)
#define DEBOUTLN(stream,msg,object)
#define DEBOUTMSG(stream,msg)
#define DEBOUTNL(stream)
#endif

