/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: gf_tabutil.h,v 1.2 1997-06-19 12:23:01 schmidt Exp $ */

#ifndef INCL_GF_TABUTIL_H
#define INCL_GF_TABUTIL_H

#include <config.h>

int gf_tab_numdigits62 ( int q );

char conv62 ( int i );

void convert62 ( int i, int n, char * p );

int convback62 ( char c );

int convertback62 ( char * p, int n );

#endif /* ! INCL_GF_TABUTIL_H */
