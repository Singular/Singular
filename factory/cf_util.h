/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_CF_UTIL_H
#define INCL_CF_UTIL_H

//{{{ docu
//
// cf_util.h - header to cf_util.cc.
//
//}}}

#include <config.h>

int ipower ( int b, int n );
int ilog2 (int a);

/*BEGINPUBLIC*/
void factoryError_intern(const char *s);
extern void (*factoryError)(const char *s);
/*ENDPUBLIC*/


#endif /* ! INCL_CF_UTIL_H */
