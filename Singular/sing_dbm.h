#ifndef SING_DBM_H
#define SING_DBM_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sing_dbm.h,v 1.5 2001-02-27 15:50:04 levandov Exp $ */
/*
* ABSTRACT: interface to DBM links
*/

#include "structs.h"

#ifdef ix86_Win
#ifdef HAVE_DBM
#  define GDBM_STATIC
#  define USE_GDBM
#  define BLOCKSIZE 1024
#endif
#endif

si_link_extension slInitDBMExtension(si_link_extension s);
#endif
