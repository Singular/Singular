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
 *   IMPLEMENTATION FILE:  MP_TbTransp.c
 *
 *      Public interface routines to the "tb" transport tbice.
 *      These include read, write, open, close, and init routines.
 *      There are some support routines as well.
 *
 *  Change Log:
 *       1/25/96   sgray - formal creation of TB device
 *       2/12/96   sgray - added MP_TbPutMsg() and MP_TbGetMsg() routines.
 *       3/3/96    sgray - moved call to mp_tb_init_transport() inside
 *                         mp_tb_open_transport() and removed the init
 *                         routine from the tb_ops struct.
 *
 *  Note to me:  consider modifying the mp_tb_write() routine to increase 
 *               the size of the MP send buffer using realloc().  I believe
 *               the toolbus buffer continues to increase in size as needed.
 *               Also, the documentation on this interface could/should
 *               suggest that the initial buffer size be set to that of
 *               ToolBus's initial buffer size.  ALSO, _ONLY_ MP_SEND_FRAG_MSG
 *               and MP_RECV_FRAG_SENDER should be used.
 *
 ***************************************************************************/
#ifndef lint
static char vcid[] = "@(#) $Id: MP_TbTransp.c,v 1.3 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"

#ifdef MP_HAVE_TB


extern char fix_log_msg[]; 

/* the device interface routines */

long mp_tb_write();
long mp_tb_read();
MP_Boolean_t mp_tb_get_status();
MP_Status_t mp_tb_flush();
MP_Status_t mp_tb_init_transport();
MP_Status_t mp_tb_open_connection();
MP_Status_t mp_tb_close_connection();

/*
 * the device-specific transport operations structure that gets 
 * loaded into the transp_ops field to the transport structure 
 */

static MP_TranspOps_t tb_ops = {
  mp_tb_write,
  mp_tb_read,
  mp_tb_flush,
  mp_tb_get_status,
  mp_tb_open_connection,
  mp_tb_close_connection,
  mp_tb_close_connection
};


/*  Okay, the trouble here is that the routines that make calls to this read
 * routine asking for len bytes really expect to get JUST len bytes copied
 * to the location given in where.  When initially filling the buffer, len
 * is just 4 bytes, but this code goes ahead and copies ALL the data to the
 * address specified by where (which is just the address of a single long!).
 * I suppose I need to add another level of buffering here.  Read in a single
 * binary term, place it in the auxiliary buffer and return the number of
 * bytes requested from this buffer.  Subsequent requests would come from
 * the auxiliary buffer until it was tried out, after which a call must be
 * made to strip off another binary term from read_term and store its data
 * in the auxiliary buffer.  This is a lot of work!!
 */
static MP_Status_t get_tb_term(MP_Link_pt link)
{
    MP_TB_t  *tb_rec = (MP_TB_t *)(link->transp.private1);
    term     *tmp = NULL;
    int i;


#ifdef MP_DEBUG
    fprintf(stderr, "get_tb_term: entering, buf_len = %d, tbtbc = %d\n",
            tb_rec->buf_len, tb_rec->tbtbc); 
    fflush(stderr);
#endif
    if (tb_rec->read_term == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "get_tb_term: link's read term is null");
        return MP_SetError(link, MP_Failure);
    }
    IMP_MemFreeFnc(tb_rec->term_buf, tb_rec->buf_len);
    if (!TBmatch(tb_rec->read_term, "f(%t,%b)", &tmp, &(tb_rec->term_buf),
                 &(tb_rec->buf_len))) {
        if (!TBmatch(tb_rec->read_term, "%b", &(tb_rec->term_buf), 
                     &(tb_rec->buf_len))) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "mp_tb_read: matches failed!");
            return MP_SetError(link, MP_Failure);
        }
    }

    /* printf("here is the buffer just extracted from a term: \n");
       for (i = 0; i < (tb_rec->buf_len); i++) {
       printf("%X ", tb_rec->term_buf[i]);
       if (i % 10 == 0) printf("\n");
       }
       */
    tb_rec->read_term = tmp;
    tb_rec->next_byte = tb_rec->term_buf;
    tb_rec->tbtbc = tb_rec->buf_len;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, 
                "tb_get_term: extracted %d bytes from link's read "
                "ToolBus term",
                tb_rec->buf_len);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif
#ifdef MP_DEBUG
    fprintf(stderr, "get_tb_term: exiting,"
            " extracted %d bytes\n", tb_rec->buf_len); 
    fflush(stderr);
#endif
 
    return(MP_Success);
}



/* ROUTINES FOR THE Abstract Device Interface begin HERE */

/*********************************************************
 * mp_tb_write(link, where, len)
 * return: 0 if success, -1 if failure.
 * arguments: link - can be anything, even NULL.
 *            where - pointer to data to be packed.
 *            len - number of bytes to be packed.
 *********************************************************/
#ifdef __STDC__
long mp_tb_write(MP_Link_pt link, char *where, unsigned long len)
#else
long mp_tb_write(link, where, len)
    MP_Link_pt link;
    char *where;
    unsigned long len;
#endif
{
    term *tmp = NULL;
    MP_TB_t  *tb_rec = (MP_TB_t *)(link->transp.private1);
    int i;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_write: entering, writing %d bytes\n", len); 
    fprintf(stderr, "mp_tb_write: on entry write_term is 0x%X\n", 
            tb_rec->write_term);  fflush(stderr);
#endif
            /*  printf("here is the buffer we are writing to a term: \n");
                for (i = 0; i < len; i++) {
                printf("%X ", where[i]);
                if (i % 10 == 0) printf("\n");
                }
                */
            if (tb_rec->write_term == NULL) {
                tb_rec->write_term = TBmake("%b", link->out_base, len);
            } else {
                tmp = TBmake("f(%t,%b)", 
                             tb_rec->write_term, link->out_base, len);
                tb_rec->write_term = tmp;
                tmp = NULL; 
                /* so garbage collection doesn't get to write_term */
            }

#ifndef NO_LOGGING
            if (link->logmask & MP_LOG_WRITE_EVENTS) {
                sprintf(fix_log_msg, 
                        "mp_tb_write: wrote %d bytes to link's "
                        "write term", len);
                MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
            }
#endif
#ifdef MP_DEBUG
            fprintf(stderr, "mp_tb_write: exiting \n"); 
            fflush(stderr);
#endif
            return len;
}



/**************************************************************************
 * mp_tb_read(link, where, len) 
 * return: number of bytes actually unpacked. -1 if failure.
 * arguments: link - can be anything, even NULL.
 *            where - pointer to memory location to store unpacked data.
 *            len - number of bytes to be packed.
 **************************************************************************/
#ifdef __STDC__
long mp_tb_read(MP_Link_pt link, char *where, unsigned long len)
#else
long mp_tb_read(link, where, len)
    MP_Link_pt link;
    char *where;
    unsigned long len;
#endif
{
    MP_TB_t  *tb_rec = (MP_TB_t *)(link->transp.private1);
    unsigned long  nbytes = len, total = 0, current = 0;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_read: entering - want to read %d bytes\n", len); 
    fflush(stderr);
#endif

    while (nbytes > 0) {
        if (tb_rec->tbtbc == 0) 
            if (!get_tb_term(link)) 
                return 0;  /* indicates failure to caller */
        current = (nbytes < tb_rec->tbtbc) ? nbytes : tb_rec->tbtbc;
        memcpy(where, tb_rec->next_byte, current);
        where += current;
        tb_rec->next_byte += current;
        tb_rec->tbtbc -= current;
        nbytes -= current;
        total += current;
    }
#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, 
                "mp_tb_read: read %d bytes from link's term buffer", total);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif
#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_read: exiting - read %d bytes\n", total); 
    fflush(stderr);
#endif

    return total;
}

  

/***********************************************************************
 * FUNCTION:  mp_tb_init_transport
 * INPUT:     link - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success and link structure initialized for the
 *                      tb connection.  
 *            Failure:  MP_Failure
 * OPERATION: If there are no problems, allocate a tb structure and 
 *            attach it to the private pointer inside link.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t mp_tb_init_transport(MP_Link_pt link)
#else
MP_Status_t mp_tb_init_transport(link)
  MP_Link_pt link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_init_transport: entering\n");
    fflush(stderr);
#endif

    /* this routine is almost complete for you */

    TEST_LINK_NULL(link);
    link->transp.transp_dev = MP_TbTransportDev;
    link->transp.transp_ops = &tb_ops;
    link->transp.private1 = (char *)IMP_MemAllocFnc(sizeof(MP_TB_t));
    TEST_MEM_ALLOC_ERROR(link, link->transp.private1);

#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_init_transport: exiting\n"); fflush(stderr);
#endif
    RETURN_OK(link);
}



/***********************************************************************
 * FUNCTION:  mp_tb_close_connection
 * INPUT:     link  - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success
 *                      Release the tb structure pointed to by private1.
 *            Failure:  MP_Failure
 * OPERATION: 
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t mp_tb_close_connection(MP_Link_pt link)
#else
MP_Status_t mp_tb_close_connection(link)
    MP_Link_pt link;
#endif
{
    MP_TB_t  *tb_rec = (MP_TB_t *)(link->transp.private1);
#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_close_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    if (link->transp.private1 == NULL) 
        return(MP_SetError(link, MP_NullTransport));
    IMP_MemFreeFnc((*(MP_TB_t *)(link->transp.private1)).myhost, 
                   MP_HOST_NAME_LEN);

    TBunprotect(&(tb_rec->write_term));
    TBunprotect(&(tb_rec->read_term));

    IMP_MemFreeFnc((MP_TB_t *)(link->transp.private1), sizeof(MP_TB_t));
    link->transp.private1 = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_close_connection: exiting\n");
    fflush(stderr);
#endif
    RETURN_OK(link);
}



/***********************************************************************
 * FUNCTION:  mp_tb_open_connection
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      link established as a tb connection
 *            Failure:  MP_Failure
 * OPERATION: Determine the tb mode we are using and attempt to
 *            establish a connection accordingly.  We could fail
 *            for any of a number of reasons.  Bad hostname, bad
 *            port number, missing arguments....
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t mp_tb_open_connection(MP_Link_pt link,
                                  int        argc,
                                  char     **argv)
#else
MP_Status_t mp_tb_open_connection(link, argc, argv)
    MP_Link_pt link;
    int        argc;
    char     **argv;
#endif
{
    MP_TB_t  *tb_rec = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_open_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    ERR_CHK(mp_tb_init_transport(link));
    tb_rec = (MP_TB_t *)(link->transp.private1);
    tb_rec->myhost = (char *) IMP_MemAllocFnc(MP_HOST_NAME_LEN);
    TEST_MEM_ALLOC_ERROR(link, tb_rec->myhost);
    if (gethostname(tb_rec->myhost, MP_HOST_NAME_LEN) == -1) {
        IMP_MemFreeFnc(tb_rec->myhost, MP_HOST_NAME_LEN);
        tb_rec->myhost = NULL;
    }
    tb_rec->write_term = NULL;
    tb_rec->read_term = NULL;
    TBprotect(&(tb_rec->write_term));
    TBprotect(&(tb_rec->read_term));
    tb_rec->term_buf = tb_rec->next_byte = NULL;
    tb_rec->buf_len = tb_rec->tbtbc = 0;

#ifdef MP_DEBUG
    fprintf(stderr, "mp_tb_open_connection: exiting\n"); fflush(stderr);
#endif
    RETURN_OK(link);
}

MP_Status_t
#ifdef __STDC__
mp_tb_flush(MP_Link_pt link)
#else
mp_tb_flush(link)
    MP_Link_pt link;
#endif
{   
    TEST_LINK_NULL(link);
#ifdef MP_DEBUG
    printf("mp_tb_flush: entering and exiting for link %d\n", link->link_id);
#endif
    return MP_Success;
}


#ifdef __STDC__
MP_Boolean_t mp_tb_get_status(MP_Link_pt link, MP_LinkStatus_t status_to_check)
#else
MP_Boolean_t mp_tb_get_status(link, status_to_check)
    MP_Link_pt link;
    MP_LinkStatus_t status_to_check;
#endif
{   

    TEST_LINK_NULL(link);

#ifdef MP_DEBUG
    printf("mp_tb_get_status: entering for link %d\n", link->link_id);
#endif

    switch (status_to_check) {
    case MP_LinkReadyReading:
         
        /* your code goes here */
        break;
    case MP_LinkReadyWriting:

        /* your code goes here */
        break;

    default:
        sprintf(fix_log_msg,"mp_tb_get_status: illegal option %d", 
                status_to_check);
        MP_LogEvent(link, MP_ERROR_EVENT, fix_log_msg);
        return MP_FALSE;
    }
#ifdef MP_DEBUG
    printf("mp_tb_get_status: exiting\n");
#endif
}

/* Additional utility routines */


/***********************************************************************
 * FUNCTION:  MP_TbGetMsg
 * INPUT:     link    - pointer to the link structure for this connection
 *            tb_data - pointer to the data returned by ToolBus
 *            len     - the number of bytes returned by ToolBus and 
 *                      pointed to by tb_data.
 * OUTPUT:    Success:  MP_Success
 *            Failure:  MP_Failure - only if the link is NULL or if
 *                      MP_SkipMsg() fails (which isn't likely!).
 * OPERATION: Simply copy the data from tb_data to the beginning of the
 *            MP receive buffer and set all the link fields appropriately.
 *            Note that we MUST call MP_SkipMsg() to set all the 
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t MP_TbGetMsg (MP_Link_pt link, char *tb_data, unsigned long len)
#else
MP_Status_t MP_TbGetMsg(link, tb_data, len)
    MP_Link_pt link;
    char *tb_data;
    unsigned long len;
#endif
{
    TEST_LINK_NULL(link);
    if (MP_SkipMsg(link) != MP_Success) {
        fprintf(stderr,"MP_TbGetMsg: failed MP_SkipMsg()\n");
        return MP_Failure;
    }
    memcpy(link->in_finger, tb_data, len);
    link->last_frag = MP_TRUE;
    *(link->in_base) = len;
    link->fbtbc = len; 
    link->sfblr = 0;
    /*   (unsigned long)link->in_finger = (unsigned long) link->in_base + 4;*/
    (unsigned long)link->in_boundry = (unsigned long) link->in_base + sizeof(long) + len;
  
    return MP_Success;
}



/***********************************************************************
 * FUNCTION:  MP_TbPutMsg
 * INPUT:     link    - pointer to the link structure for this connection
 * OUTPUT:    Success:  return MP_Success as value of the function
 *               tb_data - address of the pointer to the data to be sent by 
 *                         ToolBus - make this the beginning of the MP send
 *                         buffer.
 *               len     - the number of bytes to be sent by ToolBus and 
 *                         pointed to by *tb_data.
 *            Failure:  MP_Failure - only if the link is NULL
 * OPERATION: Simply set the data pointer to point to the beginning of 
 *            the MP send buffer and set all the link fields appropriately.
 * NOTE:      We also RESET the out_finger pointer in preparation for 
 *            the writing the NEXT MP tree to the MP send buffer.  In 
 *            respects this is equivalent to using MP_EndMsg().  In the
 *            future, I might want to make MP_EndMsg() link-specific AND
 *            add an MP_ResetMsg() routine to be called by the sender
 *            before each write of a NEW message to the link.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t MP_TbPutMsg(MP_Link_pt link, char **tb_data, unsigned long *len)
#else
MP_TbPutMsg(link, tb_data, len)
    MP_Status_t MP_Link_pt link;
    char **tb_data;
    unsigned long *len;
#endif
{
    TEST_LINK_NULL(link);
    *tb_data = (char *)(link->out_base + 4); 
    *len = (((unsigned long)link->out_finger) - ((unsigned long)link->out_base) - 4);
    (unsigned long)link->out_finger = (unsigned long)link->out_base + sizeof(unsigned long);
  
    return MP_Success;
}




#ifdef __STDC__
MP_Status_t MP_SetTbTerm(MP_Link_pt link, term *t)
#else
MP_Status_t MP_SetTbTerm(link, t)
    MP_Link_pt link;
    term *t;
#endif
{   
    term *tmp1, *tmp2 = NULL;
    char *tb_data = NULL;
    long  len = 0;
    MP_TB_t  *tb_rec = (MP_TB_t *)(link->transp.private1);
#ifdef MP_DEBUG
    fprintf(stderr,"MP_SetTbTerm: entering, t = 0x%X, read_term = 0x%X\n", t,
            tb_rec->read_term); 
    fflush(stderr);
#endif 

    while (TBmatch(t, "f(%t,%b)", &tmp1, &tb_data, &len)) {
        if (tb_rec->read_term == NULL) 
            tb_rec->read_term = TBmake("%b", tb_data, len);
        else {
            tmp2 = TBmake("f(%t,%b)", tb_rec->read_term, tb_data, len);
            tb_rec->read_term = tmp2;
        }
        t = tmp1;
    }

    if (tb_rec->read_term == NULL) {
        tb_rec->read_term = t;
    } else {
        if (!TBmatch(t, "%b", &tb_data, &len)) {
            MP_LogEvent(link, MP_ERROR_EVENT, "MP_TbSetMsg: match failed");
            return (MP_SetError(link, MP_Failure));
        }
        tmp2 = TBmake("f(%t,%b)", tb_rec->read_term, tb_data, len);
        tb_rec->read_term = tmp2;
    }
#ifdef MP_DEBUG
    fprintf(stderr,"MP_SetTbTerm: done reversal of list, exiting\n");
    fflush(stderr);
#endif 

    return MP_Success;
}
  
#endif /* MP_HAVE_TB */
