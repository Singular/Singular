#ifndef  SING_MP_H
#define SING_MP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: sing_mp.h,v 1.4 1997-04-08 08:43:31 obachman Exp $ */


#ifdef HAVE_MPSR
#include "silink.h"
si_link_extension slInitMPFileExtension(si_link_extension s);
si_link_extension slInitMPTcpExtension(si_link_extension s);
int Batch_do(int argc, char **argv);
#endif // HAVE_MPSR

#endif // SING_MP_H
