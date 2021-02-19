/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_GF_TABUTIL_H
#define INCL_GF_TABUTIL_H

/**
 * @file gf_tabutil.h
 *
 * utility functions to access GF Tables
**/

// #include "config.h"

int FACTORY_PUBLIC gf_tab_numdigits62 ( int q );

char conv62 ( int i );

void convert62 ( int i, int n, char * p );

int convback62 ( char c );

int FACTORY_PUBLIC convertback62 ( char * p, int n );

#endif /* ! INCL_GF_TABUTIL_H */
