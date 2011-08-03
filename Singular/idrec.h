#ifndef IDREC_H
#define IDREC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

#include <kernel/structs.h>

typedef union uutypes      utypes;
union uutypes
{
  int           i;
  ring          uring;
  poly          p;
  number        n;
  ideal         uideal;
  map           umap;
  matrix        umatrix;
  char *        ustring;
  intvec *      iv;
  lists         l;
  si_link       li;
  package       pack;
  procinfo *    pinf;
};

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

