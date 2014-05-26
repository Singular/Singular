/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_CF_UTIL_H
#define INCL_CF_UTIL_H

//{{{ docu
//
// cf_util.h - header to cf_util.cc.
//
//}}}

// #include "config.h"

int ilog2 (int a);

/*BEGINPUBLIC*/
int igcd (int a, int b);
int ipower ( int b, int n );
void factoryError_intern(const char *s);
extern void (*factoryError)(const char *s);
/*ENDPUBLIC*/


#endif /* ! INCL_CF_UTIL_H */
