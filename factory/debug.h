/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file debug.h
 *
 * functions to print debug output
 *
 * @note needs --enable-debugoutput --enable-streamio at configure
**/

/* It should be possible to include this file multiple times for different */
/* settings of DEBUGOUTPUT */

#undef DEBINCLEVEL
#undef DEBDECLEVEL
#undef DEBOUTSL
#undef DEBOUT
#undef DEBOUTENDL
#undef DEBOUTLN

#ifdef DEBUGOUTPUT
#include <iostream>
#ifdef HAVE_IOSTREAM
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#endif
void deb_inc_level();
void deb_dec_level();
EXTERN_VAR char * deb_level_msg;
#define DEBINCLEVEL(stream, msg) \
(std::stream << deb_level_msg << "entering << " << msg << " >>" << std::endl, deb_inc_level())
#define DEBDECLEVEL(stream, msg) \
(deb_dec_level(), std::stream << deb_level_msg << "leaving << " << msg << " >>" << std::endl)
#define DEBOUTSL(stream) \
(std::stream << deb_level_msg, std::stream.flush())
#define DEBOUT(stream, objects) \
(std::stream << objects, std::stream.flush())
#define DEBOUTENDL(stream) \
(std::stream << std::endl)
#define DEBOUTLN(stream, objects) \
(std::stream << deb_level_msg << objects << std::endl)
#else /* DEBUGOUTPUT */
#define DEBINCLEVEL(stream, msg)
#define DEBDECLEVEL(stream, msg)
#define DEBOUTSL(stream)
#define DEBOUT(stream, objects)
#define DEBOUTENDL(stream)
#define DEBOUTLN(stream, objects)
#endif /* DEBUGOUTPUT */
