/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: debug.h,v 1.1 1997-03-26 17:14:06 schmidt Exp $ */

/*
$Log: not supported by cvs2svn $
*/

/* It should be possible to include this file multiple times for different */
/* settings of DEBUGOUTPUT */

#undef DEBINCLEVEL
#undef DEBDECLEVEL
#undef DEBOUTSL
#undef DEBOUT
#undef DEBOUTENDL
#undef DEBOUTLN

#ifdef DEBUGOUTPUT
#include <iostream.h>
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
#define DEBOUTLN(stream, msg, object) \
(stream << deb_level_msg << msg << object << endl)
#else /* DEBUGOUTPUT */
#define DEBINCLEVEL(stream, msg)
#define DEBDECLEVEL(stream, msg)
#define DEBOUTSL(stream)
#define DEBOUT(stream, objects)
#define DEBOUTENDL(stream)
#define DEBOUTLN(stream, msg, object)
#endif /* DEBUGOUTPUT */
