// emacs edit mode for this file is -*- C++ -*-
// $Id: gf_tabutil.h,v 1.1 1997-04-08 16:06:20 schmidt Exp $

#ifndef INCL_GF_TABUTIL_H
#define INCL_GF_TABUTIL_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:40  stobbe
Initial revision

*/

#include <config.h>

int gf_tab_numdigits62 ( int q );

char conv62 ( int i );

void convert62 ( int i, int n, char * p );

int convback62 ( char c );

int convertback62 ( char * p, int n );

#endif /* INCL_GF_TABUTIL_H */
