#ifndef  SING_MP_H
#define SING_MP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sing_mp.h,v 1.6 1999-11-15 17:20:49 obachman Exp $ */
/*
* ABSTRACT: interface to MP links
*/


#ifdef HAVE_MPSR
#include "silink.h"
si_link_extension slInitMPFileExtension(si_link_extension s);
si_link_extension slInitMPTcpExtension(si_link_extension s);
int Batch_do(int argc, char **argv);
#endif // HAVE_MPSR

#endif // SING_MP_H
