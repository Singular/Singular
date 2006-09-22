/***************************************************************************
 *
 *   HEADER FILE:  MP_PvmTransp.h
 *
 *
 *  Change Log:
 *      1/25/96 sgray - formal creation of PVM transport device.
 *
 ***************************************************************************/
#ifndef __PvmTransp__h
#define __PvmTransp__h

#include "MP_Config.h"

#ifdef MP_HAVE_PVM
#include "pvm3.h"

#define MP_NO_SUCH_PVM_MODE    0

typedef struct{
  int     nhosts;
  int     ntids;  /* the number of tids in next field */
  int     *tids;
  int     tag;
  int     send_mode;
  char    *myhost;
} MP_PVM_t;

/*#include "/tmp_mnt/home/Owl/sgray/MP/MP/MP_PvmTransp.c" */

#endif /*  MP_HAVE_PVM */

#endif
