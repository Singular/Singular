/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

/* It should be possible to include this file multiple times for different */
/* settings of DEBUGOUTPUT */

#undef DEBINCLEVEL
#undef DEBDECLEVEL
#undef DEBOUTSL
#undef DEBOUT
#undef DEBOUTENDL
#undef DEBOUTLN

#ifdef DEBUGOUTPUT
#ifdef HAVE_IOSTREAM
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#endif
void deb_inc_level();
void deb_dec_level();
extern char * deb_level_msg;
#define DEBINCLEVEL(stream, msg) \
(stream << deb_level_msg << "entering << " << msg << " >>" << endl, deb_inc_level())
#define DEBDECLEVEL(stream, msg) \
(deb_dec_level(), stream << deb_level_msg << "leaving << " << msg << " >>" << endl)
#define DEBOUTSL(stream) \
(stream << deb_level_msg, stream.flush())
#define DEBOUT(stream, objects) \
(stream << objects, stream.flush())
#define DEBOUTENDL(stream) \
(stream << endl)
#define DEBOUTLN(stream, objects) \
(stream << deb_level_msg << objects << endl)
#else /* DEBUGOUTPUT */
#define DEBINCLEVEL(stream, msg) ((void) 0)
#define DEBDECLEVEL(stream, msg) ((void) 0)
#define DEBOUTSL(stream) ((void) 0)
#define DEBOUT(stream, objects) ((void) 0)
#define DEBOUTENDL(stream) ((void) 0)
#define DEBOUTLN(stream, objects) ((void) 0)
#endif /* DEBUGOUTPUT */
