#ifndef IDREC_H
#define IDREC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

#include "structs.h"

class idrec
{
  public:
  /* !! do not change the first 6 entries !! (see subexpr.h: sleftv) */
  idhdl      next;
  const char *id;
  utypes     data;
  attr       attribute;
  BITSET     flag;
  int        typ;

  short      lev;
  short      ref;
  int        id_i;

  idrec() { memset(this,0,sizeof(*this)); }
  idhdl get(const char * s, int lev);
  idhdl set(const char * s, int lev, int t/*typ*/, BOOLEAN init=TRUE);
  char * String();
//  ~idrec();
};

#endif

