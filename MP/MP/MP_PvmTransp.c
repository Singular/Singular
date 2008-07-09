/****************************************************************
 *                                                                  
 *                                                                  
 *                    MP version 1.1.2:  Multi Protocol
 *                    Kent State University, Kent, OH
 *                 Authors:  S. Gray, N. Kajler, P. Wang 
 *          (C) 1993, 1994, 1995, 1996, 1997 All Rights Reserved
 * 
 *                                 NOTICE
 * 
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation for non-commercial purposes and without fee is 
 *  hereby granted provided that the above copyright notice appear in all
 *  copies and that both the copyright notice and this permission notice 
 *  appear in supporting documentation.
 *  
 *  Neither Kent State University nor the Authors make any representations 
 *  about the suitability of this software for any purpose.  The MP Library
 *  is distributed in the hope that it will be useful, but is provided  "as
 *  is" and without any warranty of any kind and without even the implied  
 *  warranty of merchantability or fitness for a particular purpose. 
 *
 *    IMPORTANT ADDITION: as of September 2006, MP is also licenced under GPL
 *
 *   IMPLEMENTATION FILE:  MP_PvmTransp.c
 *
 *      Public interface routines to the "pvm" transport device.
 *      These include read, write, open, close, and get_status routines.
 *      There are some support routines as well.

 * Note:
 *       mp_pvm_read() anticipates PvmNoData error from pvm_recv().
 *       PVM defaults to printing an error message when encounter such
 *       error. To disable printing of error message, use the following:
 *
 *              pvm_setopt(PvmAutoErr, 0);
 *
 *       See "PVM 3 User'sGuide and Reference Manual" for detail.
 *
 *  Change Log:
 *       1/25/96   sgray - formal creation of PVM device
 *       1/30/96   sgray - added (status == PvmNoBuf) test in mp_pvm_read().
 *                         The original code worked in the past because all
 *                         the test programs sent data from the master to 
 *                         the slave using pvm_send/recv().  This "primed
 *                         the pump" I suppose.  Without the test above and
 *                         sending only MP data, I kept getting a PvmNoBuf
 *                         error message from the initial attempt to unpack 
 *                         the data.  
 *       3/3/96    sgray - moved call to pvm_init_transport() inside
 *                         pvm_open_transport() and removed the init
 *                         routine from the pvm_ops struct.
 *
 ****************************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_PvmTransp.c,v 1.3 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"
#ifdef MP_HAVE_PVM

/*
 * We don't make this file with the library.  Instead, users must include
 * MP_PvmTransp.h, which includes this file and they get compiled and linked
 * with the MP library together.  This is to avoid having to add in the PVM
 * library even when it isn't being used. 
 */

#define log_msg_len 128
static char log_msg[log_msg_len];  /* for event logging */


long mp_pvm_write();
long mp_pvm_read();
MP_Boolean_t mp_pvm_get_status();
MP_Status_t mp_pvm_flush();
MP_Status_t mp_pvm_init_transport();
MP_Status_t mp_pvm_open_connection();
MP_Status_t mp_pvm_close_connection();


MP_TranspOps_t pvm_ops = {
  mp_pvm_write,
  mp_pvm_read,
  mp_pvm_flush,
  mp_pvm_get_status,
  mp_pvm_open_connection,
  mp_pvm_close_connection,
  mp_pvm_close_connection
};

MP_Status_t 
#ifdef __STDC__
mp_pvm_flush(MP_Link_pt link)
#else
mp_pvm_flush(link)
    MP_Link_pt link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_flush: entering\n");
    fflush(stderr);
#endif
#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_flush: exiting \n");
    fflush(stderr);
#endif
  return MP_ClearError(link);
}


/*********************************************************
 * mp_pvm_write(link, where, len)
 * return: 0 if success, -1 if failure.
 * arguments: link - can be anything, even NULL.
 *            where - pointer to data to be packed.
 *            len - number of bytes to be packed.
 *********************************************************/
#ifdef __STDC__
long mp_pvm_write(MP_Link_pt link, char *where, unsigned long len)
#else
long mp_pvm_write(link, where, len)
    MP_Link_pt link;
    char      *where;
    unsigned long len;
#endif
{
    int status;
    MP_PVM_t  *pvm_rec = (MP_PVM_t *)(link->transp.private1);

#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_write: entering, writing %d bytes\n", len); 
    fflush(stderr);
#endif
    /* 
     * Actually I think the test here should be for the link_send_mode.
     * We really only want to do all three steps (init, pack, send) here 
     * if the user has requested that the link_send_mode is MP_Whole_Tree.
     * Also, we want to be sure that the user knows that if we are doing the 
     * send here, s/he shouldn't be doing it in the program.  A cleaner 
     * approach is to always do the send here, but provides the programmer with
     * less flexibility.  BTW: the body of the if could probably be replaced by
     * a single call using pvm_psend().  But note also that life is much more
     * complicated if the user is using pvm_mcast() or pvm_bcast().
     */
    if (pvm_rec->send_mode == PvmDataInPlace) {
        pvm_initsend(PvmDataInPlace);
        status = pvm_pkbyte(where, len, 1);
        pvm_send(pvm_rec->tids[0], 0);
    }
    else
        status = pvm_pkbyte(where, len, 1);
#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_write: exiting, status = %d\n", status); 
    fflush(stderr);
#endif
    if (status < 0) 
        return(-1);
    else 
        return(len);
}



/**************************************************************************
 * mp_pvm_read(link, where, len) 
 * return: number of bytes actually unpacked. -1 if failure.
 * arguments: link - can be anything, even NULL.
 *            where - pointer to memory location to store unpacked data.
 *            len - number of bytes to be packed.
 **************************************************************************/
#ifdef __STDC__
long mp_pvm_read(MP_Link_pt link, char *where, unsigned long len)
#else
long mp_pvm_read(link, where, len)
    MP_Link_pt link;
    char *where;
    unsigned long len;
#endif
{
    unsigned long status, bufid, actual_len;
    int msgtag,  lenrecv, tid;
#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_read: entering - want to read %d bytes\n", len); 
    fflush(stderr);
#endif
    actual_len = len;

    status = pvm_upkbyte(where, actual_len, 1);

    if (status == PvmNoData || status == PvmNoBuf) {
        bufid = pvm_recv(-1, -1);
        if (bufid < 0) 
            return(-1);
        status = pvm_bufinfo(bufid, &lenrecv, &msgtag, &tid);
        actual_len = (len < lenrecv) ? len : lenrecv;
        status = pvm_upkbyte(where, actual_len, 1);
    }

#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_read: exiting, read %d bytes, status = %d\n", 
            actual_len, status); 
    fflush(stderr);
#endif
 
    if (status < 0) return(-1);
    return(actual_len);
}


  
/***********************************************************************
 * FUNCTION:  mp_pvm_init_transport
 * INPUT:     link - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success and link structure initialized for the
 *                      pvm connection.  
 *            Failure:  MP_Failure
 * OPERATION: If there are no problems, allocate a pvm structure and 
 *            attach it to the private pointer inside link.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t mp_pvm_init_transport(MP_Link_pt link)
#else
MP_Status_t mp_pvm_init_transport(link)
  MP_Link_pt link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_init_transport: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    link->transp.transp_dev = MP_PvmTransportDev;
    link->transp.transp_ops = &pvm_ops;
    link->transp.private1 = (char *)IMP_MemAllocFnc(sizeof(MP_PVM_t));
    TEST_MEM_ALLOC_ERROR(link, link->transp.private1);

#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_init_transport: exiting\n"); 
    fflush(stderr);
#endif
    RETURN_OK(link);
}



/***********************************************************************
 * FUNCTION:  mp_pvm_close_connection
 * INPUT:     link  - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success
 *                      Release the pvm structure pointed to by private1.
 *            Failure:  MP_Failure
 * OPERATION: 
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t mp_pvm_close_connection(MP_Link_pt link)
#else
MP_Status_t mp_pvm_close_connection(link)
  MP_Link_pt link;
#endif
{  
    MP_PVM_t  *pvm_rec;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_close_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    if (link->transp.private1 == NULL) 
        return(MP_SetError(link, MP_NullTransport));
    pvm_rec = (MP_PVM_t *)(link->transp.private1);
    IMP_MemFreeFnc(pvm_rec->myhost, MP_HOST_NAME_LEN);
    /* 
     * IMP_RawMemFreeFnc((*(MP_PVM_t *)(link->transp.private1)).tids);
     */
    IMP_MemFreeFnc(pvm_rec->tids, pvm_rec->ntids * sizeof(int));
    IMP_MemFreeFnc(pvm_rec, sizeof(MP_PVM_t));
    link->transp.private1 = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_close_connection: exiting\n");
    fflush(stderr);
#endif
    RETURN_OK(link);
}


/***********************************************************************
 * FUNCTION:  mp_pvm_open_connection
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      link established as a pvm connection
 *            Failure:  MP_Failure
 * OPERATION: Determine the pvm mode we are using and attempt to
 *            establish a connection accordingly.  We could fail
 *            for any of a number of reasons.  Bad hostname, bad
 *            port number, missing arguments....
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t mp_pvm_open_connection(MP_Link_pt link, int argc, char **argv)
#else
MP_Status_t mp_pvm_open_connection(link, argc, argv)
    MP_Link_pt link;
    int argc;
    char **argv;
#endif
{
    MP_PVM_t  *pvm_rec = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_open_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    if (mp_pvm_init_transport(link) != MP_Success) 
        return MP_Failure;
    pvm_rec = (MP_PVM_t *)(link->transp.private1);
    pvm_rec->myhost = (char *) IMP_MemAllocFnc(MP_HOST_NAME_LEN);
    TEST_MEM_ALLOC_ERROR(link, pvm_rec->myhost);
    if (gethostname(pvm_rec->myhost, MP_HOST_NAME_LEN) == -1) {
        IMP_MemFreeFnc(pvm_rec->myhost, MP_HOST_NAME_LEN);
        pvm_rec->myhost = NULL;
    }
    pvm_rec->nhosts = 0;
    pvm_rec->ntids = 0;
    pvm_rec->tids = NULL;
    pvm_rec->tag = -1;
    pvm_rec->send_mode = PvmDataRaw;
#ifdef MP_DEBUG
    fprintf(stderr, "mp_pvm_open_connection: exiting\n"); 
    fflush(stderr);
#endif
    RETURN_OK(link);
}


#ifdef __STDC__
MP_Boolean_t mp_pvm_get_status(MP_Link_pt link, MP_LinkStatus_t status_to_check)
#else
MP_Boolean_t mp_pvm_get_status(link, status_to_check)
  MP_Link_pt      link;
  MP_LinkStatus_t status_to_check;
#endif
{   

    TEST_LINK_NULL(link);

#ifdef MP_DEBUG
    printf("mp_pvm_get_status: entering for link %d\n", link->link_id);
#endif

    switch (status_to_check) {
    case MP_LinkReadyReading:
        return (MP_Boolean_t) (pvm_probe(-1, -1));
    case MP_LinkReadyWriting:
        return MP_TRUE;
    default:
        sprintf(log_msg,"mp_pvm_get_status: illegal option %d",
                status_to_check);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
        return MP_FALSE;
    }
#ifdef MP_DEBUG
    printf("mp_pvm_get_status: exiting\n");
#endif
}



#ifdef __STDC__
MP_Status_t MP_PvmSetTids(MP_Link_pt link,
                          int nhosts,
                          int *tids)
#else
MP_Status_t MP_PvmSetTids(link, nhosts, tids)
    MP_Link_pt link;
    int nhosts;
    int *tids;
#endif
{
    int i;
    MP_PVM_t  *pvm_rec = NULL;

    TEST_LINK_NULL(link);
    pvm_rec = (MP_PVM_t *)(link->transp.private1);

    if (nhosts < 0) 
        return MP_Failure;
    pvm_rec->nhosts = nhosts;
    pvm_rec->tids = (int *)IMP_MemAllocFnc(nhosts * sizeof(int));
    TEST_MEM_ALLOC_ERROR(link, pvm_rec->tids);
    for (i = 0; i < nhosts; i++)
        pvm_rec->tids[i] = tids[0];

    return(MP_Success);
}




#ifdef __STDC__
MP_Status_t MP_PvmSetSendMode( MP_Link_pt link, int mode)
#else
MP_Status_t MP_PvmSetSendMode(link, mode)
    MP_Link_pt link;
    int mode;
#endif
{
    MP_PVM_t  *pvm_rec = NULL;

    TEST_LINK_NULL(link);
    pvm_rec = (MP_PVM_t *)(link->transp.private1);
    if (mode != PvmDataRaw && mode != PvmDataInPlace && mode != PvmDataDefault)
        return(MP_Failure);

    pvm_rec->send_mode = mode;
    return(MP_Success);
}

#endif /* MP_HAVE_PVM */
