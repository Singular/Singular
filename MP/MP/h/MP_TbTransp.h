/***************************************************************************
 *
 *   HEADER FILE:  MP_TbTransp.h
 *
 *   PURPOSE: ToolBus stuff - not much to do.
 *
 *   Change Log:
 *      2/6/96 sgray - creation of TB transport device.
 *      6/9/96 sgray - updated tb struct to support buffering of terms
 *                     consistent with what the buffering layer expects.
 *
 ***************************************************************************/
#ifndef __TbTransp__h
#define __TbTransp__h

#include "MP_Config.h"

#ifdef MP_HAVE_TB
#include "TB.h"


typedef struct{
  term    *write_term,
          *read_term;
  char    *term_buf;     /* where next binary TB term will be unloaded    */
  char    *next_byte;    /* next byte to be consumed in term_buf          */
  unsigned long buf_len; /* number of bytes in term_buf                   */
  unsigned long tbtbc;   /* Term Bytes To Be Consumed in term_buf         */
  char    *myhost;       /* host on which we are running - must have this */
} MP_TB_t;

EXTERN MP_Status_t MP_TbPutMsg _ANSI_ARGS_((MP_Link_pt link, char **tb_data,
                                            unsigned long *len));
EXTERN MP_Status_t MP_TbGetMsg _ANSI_ARGS_((MP_Link_pt link, char *tb_data,
                                            unsigned long len));
EXTERN MP_Status_t MP_TbSetTerm _ANSI_ARGS_((MP_Link_pt link, term *t));

/*
 * if your device may not be available everywhere you want to use MP,
 * don't try to compile the source into the library as the link will
 * fail.  The alternative is to include the transport device interface
 * source here and have the user include this header file in their
 * application code.  Then the corresponding .c file will be compiled
 * with the application
 */
/* #include "../MP/MP_TbTransp.c" */

#endif /* MP_HAVE_TB */

#endif
