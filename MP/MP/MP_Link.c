/*******************************************************************
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
 *   IMPLEMENTATION FILE:  MP_Link.c
 *
 *      General routines associated with a link.
 *      This code provides the buffering layer above tcp/ip for Multi.
 *      The code is largely the RPC buffering code with a few .
 *      modifications
 *
 *  Change Log:
 *                Jan 30  Added m_restore(link) SG
 *                        Added m_eot(link) SG
 *                        changed RECSTREAM * to MP_Link_pt SG
 *
 *                May 25, 1995  baw
 *                  Added:
 *                    structures:
 *                        buffer_handle
 *                        buffer_pool
 *                    routines:
 *                        pool_init(buffer_size, max_free_buffer, 
 *                                  init_free_buffer) 
 *                        pool_destroy()
 *                        create_buffer()
 *                        destroy_buffer(buffer_handle)
 *                        get_buffer()
 *                        free_buffer(buffer_handle)
 *                        get_o_buff(link)
 *                        get_i_buff(link)
 *                        free_o(link)
 *                        free_i(link)
 *                        --- 09-June-1995 ---
 *                        reset_i_buff(link)
 *                        fill_a_buff(link)
 *                  Modified:
 *                    structures:
 *                        MP_Link_t
 *                    routines:
 *                        m_create_link(sock, num_o_buff, (*Readit)(),
 *                                 (*writeit)(), snd_mode, rcv_mode)
 *                        m_destroy(link)
 *                        IMP_PutLong(link, *lp)
 *                        IMP_PutBytes(link, addr, len)
 *                        m_endofmessage(link)
 *                        flush_out(buff)
 *                        --- 09-June-1995 ---
 *                        fill_input_buf(link)
 *                
 *                August 9, 1995 SG
 *                    renamed free_i() to m_free_in_bufs()
 *                            free_o() to m_free_out_bufs()
 *                            pool_destroy() to m_free_pool()
 *                            pool_init() to m_create_pool()
 *                            m_destroy() to m_destroy_link()
 *                            m_create() to m_create_link()
 * 
 *              September 13, 1995 SG
 *                  moved all the buffer code to MP_Buffer.c
 *
 *      11/18/95 obachman added ntohl(hdr) in fill_a_buf
 *      11/25/95 sgray added defines to support negotiating
 *               endian word order.
 *       1/25/96 sgray formally added PVM device to MP_OpenLink().
 *       1/29/96 sgray added MP_GetLinkOption() to return the current
 *                     setting for an option.  Note that since we return
 *                     MP_Failure for an illegal option, we must never
 *                     allow a legal option to be the value MP_Failure (0).
 *               sgray Improved the logging for MP_SetLinkOption().
 *       3/3/96  sgray Changed the MP_OpenLink() routine to look for the
 *                     requested device in the environment device list and 
 *                     call the device open routine from there.  We no 
 *                     longer call the device init routine, whose
 *                     responsibilities have been shoved into the open fnc.
 *       4/19/96 sgray Added support for alternative BigInt formats. This
 *                     means that OpenLink() now loads GMP as the default
 *                     BigInt package.
 *       4/23/96 sgray Completed support for SAC big integers.
 *       5/16/96 sgray Changed MP_PeekLink() to MP_PeekHeader().
 *                     Made MP_EndMsgReset() a function instead of a macro.
 *       8/21/96 sgray Restored recv_mode2string string array.
 *                     Fixed error in MP_SetLinkOption(): send_mode2string 
 *                     was accessed instead of recv_mode2string
 *       8/30/96 sgray Added test for NULL environment in MP_CloseLink()
 *                     and MP_SkipMsg().
 *       3/11/97 sgray Fixed bug in fill_input_buf() that made it appear
 *                     that data had been read in when the read operation
 *                     had returned failure.
 *       3/11/97 sgray Added code to fill_a_buf() so that repeated
 *                     attempts to read from an empty data stream (e.g., 
 *                     an empty file) would fail.  Before this, a first
 *                     attempt to read would fail but subsequent attempts 
 *                     would think there was data in the link because 
 *                     m_free_in_bufs(), invoked in fill_input_buf(), resets 
 *                     these two pointers to be the beginning (in_finger) 
 *                     and end (in_boundry) of an input buffer.  Other 
 *                     routines rely on this to know how much space there 
 *                     is to put data. 
 *       3/11/97 sgray Changed some "int"s to "unsigned long"s in fill_a_buf()
 *                     and fill_input_buf().
 *
 ************************************************************************/

/* @(#)xdr_rec.c        1.1 87/11/04 3.9 RPCSRC
 * MP_Link.c, Implements TCP/IP based Multi links with a "message marking"
 * layer above tcp (for rpc's use).
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * These routines interface XDRSTREAMS to a tcp/ip connection.
 * There is a message marking layer between the xdr link
 * and the tcp transport level.  A message is composed on one or more
 * message fragments.  A message fragment is a thirty-two bit header followed
 * by n bytes of data, where n is contained in the header.  The header
 * is represented as a htonl(unsigned long).  The order bit encodes
 * whether or not the fragment is the last fragment of the message
 * (1 => fragment is last, 0 => more fragments to follow.
 * The other 31 bits encode the byte length of the fragment.
 */

#ifndef lint
static char vcid[] = "@(#) $Id: MP_Link.c,v 1.6 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"

#define log_msg_len 128
static char log_msg[log_msg_len];  /* for event logging */
static char *msg = NULL;           /* for variable length logging messages */
static char *send_mode2string[2] = {"MP_SEND_FRAG_MSG", "MP_SEND_WHOLE_MSG"};
static char *recv_mode2string[3] = {"MP_RECV_FRAG_MSG_RECVER",
                                    "MP_RECV_FRAG_MSG_SENDER",
                                    "MP_RECV_WHOLE_MSG"};
static char *transp_dev2string[5] = {"TCP", "FILE", "PVM", "TB", NULL};

static MP_Status_t fill_a_buff _ANSI_ARGS_((MP_Link_pt));
static MP_Status_t flush_out _ANSI_ARGS_((MP_Link_pt, buffer_handle_pt));
static MP_Status_t get_input_bytes _ANSI_ARGS_((MP_Link_pt, char *, int));
static MP_Status_t set_input_fragment _ANSI_ARGS_((MP_Link_pt));
static MP_Status_t skip_input_bytes _ANSI_ARGS_((MP_Link_pt, long));
static int         transp_to_num _ANSI_ARGS_((char *transp));

/*
  Backup the necessary fields of the link message by n bytes.
  The idea is to restore some field of a packet to the buffer
  so it can be reread.  This is intended to be used by a tool
  when it expects one type, but does a read and discovers another
  type.  There may be a better way to solve this problem.
  It is assumed that all reads will occur in multiple of four bytes,
  so a four byte read (or restore) should never cross a buffer fragment
  boundary. If it does, MP_Failure is returned. Else return MP_Success.
*/


#ifdef __STDC__
MP_Status_t IMP_Restore(MP_Link_pt link, unsigned long n)
#else
MP_Status_t IMP_Restore(link, n)
    MP_Link_pt     link;
    unsigned long  n;
#endif
{
   if (n > ((unsigned long)link->in_finger - (unsigned long)link->in_base - 
	    sizeof(long))) {
    return MP_Failure;
   }
   link->fbtbc     += n;
   link->in_finger -= n;

   return MP_Success;
}


#ifdef __STDC__
MP_Link_pt MP_OpenLink(MP_Env_pt env, int argc, char **argv)
#else
MP_Link_pt MP_OpenLink(env, argc, argv)
    MP_Env_pt   env;
    int         argc;
    char      **argv;
#endif
{
    buffer_handle_pt  cur_buff, old_buff;
    MP_Link_pt        link;
    MP_TranspList_pt  tp_ops;
    int               counter;
    char             *transp = NULL;
    int               transp_dev = 0;

#ifdef MP_DEBUG
    fprintf(stderr, "MP_OpenLink: entering, argc = %d\n", argc);
#endif

    link = (MP_Link_pt)IMP_MemAllocFnc(sizeof(MP_Link_t));

    if (link == NULL) {
        fprintf(stderr,
                "MP_OpenLink: memory allocation error - can't continue\n");
        fflush(stderr);
        return NULL;
    }

    if (env == NULL || !env->initialized) {
        fprintf(stderr,
                "MP_OpenLink: MP environment not initialized"
                " - can't continue\n");
        fflush(stderr);
        return NULL;
    }

    link->env        = env;
    link->link_id    = ++env->num_links;
    link->snd_mode   = MP_DEFAULT_SEND_MODE;
    link->rcv_mode   = MP_DEFAULT_RECV_MODE;
    link->num_o_buff = 1;

    link->i_buff      = NULL;
    link->curr_i_buff = NULL;

    link->i_buff      = get_i_buff(link);
    link->curr_i_buff = link->i_buff;

    if (link->i_buff == NULL)
        return NULL;

    link->o_buff      = NULL;
    link->curr_o_buff = NULL;

    link->o_buff      = get_o_buff(link);
    link->curr_o_buff = link->o_buff;

    if (link->o_buff == NULL)
        return NULL;

    old_buff = link->o_buff;
    for (counter = 1; counter < env->num_o_buff; counter++) {
        cur_buff = get_buffer(env->buff_pool);
        if (cur_buff != NULL)
            old_buff = old_buff->next = cur_buff;
        else
            return NULL;
    }

    link->MP_errno    = MP_Success;
    link->logmask     = MP_LOG_INIT_EVENTS;
    link->num_o_buff  = counter;
    link->fbtbc       = 0;
    link->last_frag   = MP_TRUE;
    link->s_last_frag = MP_FALSE;
    link->sfblr       = -1;
    link->in_boundry  = link->in_finger;

    /* The default to start with, but is negotiable between partners: */
    link->link_word_order = MP_BigEndian;

    /* Default floating point format is IEEE - this is negotiable: */
    link->link_fp_format = MP_IEEE_fp;

    link->bignum.bigint_ops   = env->bignum.bigint_ops;
    link->link_bigint_format  = env->bignum.native_bigint_format;
    link->bignum.bigreal_ops  = env->bignum.bigreal_ops;
    link->link_bigreal_format = env->bignum.native_bigreal_format;


    if ((transp = IMP_GetCmdlineArg(argc, argv, "-MPtransp")) == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: Missing -MPtransp option");
        return NULL;
    }
    transp_dev = transp_to_num(transp);

    if ((tp_ops = IMP_GetTranspByName(env, transp_dev)) == NULL) {
	msg = (char*) IMP_RawMemAllocFnc(strlen(transp) + 28);
        sprintf(msg, "Unknown transport device %s", transp);
        MP_LogEvent(link, MP_ERROR_EVENT, msg);
	IMP_RawMemFreeFnc(msg);
        return NULL;
    }

    if (tp_ops->transp_ops.open_transp(link, argc, argv) != MP_Success) {
	msg = (char*) IMP_RawMemAllocFnc(strlen(transp) + 46);
        sprintf(msg, "MP_OpenLink: Couldn't open %s transport device",
                transp);
        MP_LogEvent(link, MP_ERROR_EVENT, msg);
	IMP_RawMemFreeFnc(msg);
        return NULL;
    }

#ifndef NO_LOGGING
    fprintf(env->logfd, "L%d: Application: %s\n", link->link_id, argv[0]);
    msg = (char*) IMP_RawMemAllocFnc(strlen(transp) + 46);
    sprintf(msg, "MP_OpenLink: opened link for transport %s",
            transp);
    MP_LogEvent(link, MP_INIT_EVENT, msg);
    IMP_RawMemFreeFnc(msg);
#endif

#ifdef MP_DEBUG
    fprintf(stderr, "MP_OpenLink: exiting\n");fflush(stderr);
#endif

    return link;
}


/********************************************************************
 * FUNCTION:  MP_CloseLink
 * ARGUMENT:  link - pointer to a link structure
 * RETURN:    none
 * PURPOSE:   Release any buffers associated with the link by
 *            returning them to the buffer pool and call the
 *            transport-specific close routine.
 ********************************************************************/
#ifdef __STDC__
void MP_CloseLink(MP_Link_pt link)
#else
void MP_CloseLink(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt cur_buff, old_buff;

#ifdef MP_DEBUG
    fprintf(stderr, "MP_CloseLink: entering for link %d\n", link->link_id);
#endif

    if (link != NULL) {
        if ((cur_buff = link->o_buff) == NULL)
            return;

      /* Check just in case the environment was released first. */
      /* It shouldn't have been, but you never know.            */
      if (link->env == NULL) {
        fprintf(stderr, "MP_CloseLink: NULL environment pointer!!\n");
        return;
      }


        while (cur_buff != NULL) {
            old_buff = cur_buff;
            cur_buff = old_buff->next;
            free_buffer(link->env->buff_pool, old_buff);
        }

        if ((cur_buff = link->i_buff) == NULL)
            return;

        while (cur_buff != NULL) {
            old_buff = cur_buff;
            cur_buff = old_buff->next;
            free_buffer(link->env->buff_pool, old_buff);
        }
        link->transp.transp_ops->close_transp(link);

#ifndef NO_LOGGING
        if (link->logmask & MP_LOG_INIT_EVENTS)
            MP_LogEvent(link, MP_INIT_EVENT, "MP_CloseLink: closed link\n");
#endif

        IMP_MemFreeFnc(link, sizeof(MP_Link_t));
    }

#ifdef MP_DEBUG
    fprintf(stderr, "MP_CloseLink: exiting\n");
#endif

    return;
}

#ifdef __STDC__
void MP_KillLink(MP_Link_pt link)
#else
void MP_KillLink(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt cur_buff, old_buff;

#ifdef MP_DEBUG
    fprintf(stderr, "MP_KillLink: entering for link %d\n", link->link_id);
#endif

    if (link != NULL) {
        if ((cur_buff = link->o_buff) == NULL)
            return;

      /* Check just in case the environment was released first. */
      /* It shouldn't have been, but you never know.            */
      if (link->env == NULL) {
        fprintf(stderr, "MP_KillLink: NULL environment pointer!!\n");
        return;
      }


        while (cur_buff != NULL) {
            old_buff = cur_buff;
            cur_buff = old_buff->next;
            free_buffer(link->env->buff_pool, old_buff);
        }

        if ((cur_buff = link->i_buff) == NULL)
            return;

        while (cur_buff != NULL) {
            old_buff = cur_buff;
            cur_buff = old_buff->next;
            free_buffer(link->env->buff_pool, old_buff);
        }
        link->transp.transp_ops->kill_transp(link);

#ifndef NO_LOGGING
        if (link->logmask & MP_LOG_INIT_EVENTS)
            MP_LogEvent(link, MP_INIT_EVENT, "MP_KillLink: closed link\n");
#endif

        IMP_MemFreeFnc(link, sizeof(MP_Link_t));
    }

#ifdef MP_DEBUG
    fprintf(stderr, "MP_KillLink: exiting\n");
#endif

    return;
}



/********************************************************************
 * FUNCTION:  MP_GetLinkStatus
 * ARGUMENT:  link - pointer to a link structure
 * RETURN:    true - the status request checked is valid
 *            false - status request checked is not valid
 *
 * PURPOSE:   Determine if the requested status check is valid or not
 *            for the given link.
 * NOTES:     For some devices this will be a meaningless test?
 *            They should return MP_False and print an error message
 *            to the logfile.
 ********************************************************************/
#ifdef __STDC__
MP_Boolean_t MP_GetLinkStatus(MP_Link_pt      link,
                              MP_LinkStatus_t status_to_check)
#else
MP_Boolean_t MP_GetLinkStatus(link, status_to_check)
    MP_Link_pt      link;
    MP_LinkStatus_t status_to_check;
#endif
{
    return link->transp.transp_ops->get_status(link, status_to_check);
}

/********************************************************************
 * FUNCTION:  MP_EndMsgReset
 * ARGUMENTS: link - (in) pointer to a link structure
 * RETURN:    success - MP_Success
 *            failure - MP_Failure
 * PURPOSE:   Combine the MP_EndMsg() and MP_ResetLink() operations.
 *            This was formerly done using a macro, but we ran into
 *            trouble when people wanted to do
 *            return(MP_EndMsgReset(link)), which only executed the
 *            first of the two operations.
 ********************************************************************/
#ifdef __STDC__
MP_Status_t MP_EndMsgReset(MP_Link_pt link)
#else
MP_Status_t MP_EndMsgReset(link)
    MP_Link_pt link;
#endif
{
    ERR_CHK(MP_EndMsg(link));
    ERR_CHK(MP_ResetLink(link));

    return MP_ClearError(link);
}

/********************************************************************
 * FUNCTION:  MP_PeekHeader
 * ARGUMENTS: link - (in) pointer to a link structure
 *            ntype - (out) value from type field of node pkt hdr
 *            cval  - (out) value from common value field of node pkt hdr
 *            dtag  - (out) value from dictionary tag field of node pkt hdr
 * RETURN:    success - MP_Success
 *            failure - MP_Failure
 *
 * PURPOSE:   Read in the next node packet header and extract the
 *            type, common value and dictionary tag fields.  Then
 *            back up in the read buffer to restore the node pkt hdr
 *            so the application can reread it.
 * NOTES:     The 4 bytes read aren't guaranteed to be meaningful.
 *            It is assumed that the next 4 bytes really are a node
 *            packet header. One aspect of dealing with a binary
 *            encoding is that we can't be certain what the bits really
 *            are.
 ********************************************************************/

#ifdef __STDC__
MP_Status_t MP_PeekHeader(MP_Link_pt     link,
                          MP_NodeType_t *ntype,
                          MP_DictTag_t  *dtag,
                          MP_Common_t   *cval)
#else
MP_Status_t MP_PeekHeader(link, ntype, dtag, cval)
    MP_Link_pt     link;
    MP_NodeType_t *ntype;
    MP_DictTag_t  *dtag;
    MP_Common_t   *cval;
#endif
{
    MP_NodeHeader_t hdr;

    ERR_CHK(IMP_GetLong(link, (long*)&hdr));
    ERR_CHK(IMP_Restore(link, sizeof(MP_NodeHeader_t)));

    *ntype = MP_GetNodeFieldType(hdr);
    *cval  = MP_GetNodeCommonValueField(hdr);
    *dtag  = MP_GetNodeDictTagField(hdr);

    return MP_Success;
}


/********************************************************************
 * FUNCTION:  MP_SetLinkOption
 * ARGUMENT:  link   - (in) pointer to a link structure
 *            option - (in) the option to set
 *            value  - (in) value to set the option to
 * RETURN:    success - old value of the option
 *            failure - MP_Failure
 *
 * PURPOSE:   Make sure that the option and value are valid.  For
 *            the log mask option, we don't do this and trust the
 *            user to not do something stupid.
 ********************************************************************/
#ifdef __STDC__
int MP_SetLinkOption(MP_Link_pt link,
                     int        option,
                     int        value)
#else
int MP_SetLinkOption(link, option, value)
    MP_Link_pt link;
    int        option;
    int        value;
#endif
{
    int oldval = 0;

    if (link == NULL) {
        fprintf(stderr, "MP_SetLinkOption: null link!!\n");
        fflush(stderr);
        return MP_Failure;
    }

    switch (option) {
    case MP_LINK_SEND_MODE_OPT:
        oldval = link->snd_mode;
        if (value != MP_SEND_WHOLE_MSG && value != MP_SEND_FRAG_MSG) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_SetLinkOption: "
                        "illegal option value for link send mode");
            return MP_Failure;
        }
        sprintf(log_msg, "%s %s", "MP_SetLinkOption: set Send Mode to",
                send_mode2string[value - 1]);
        link->snd_mode = value;
        break;

    case MP_LINK_RECV_MODE_OPT:
        oldval = link->rcv_mode;
        if (value != MP_RECV_WHOLE_MSG && value != MP_RECV_FRAG_MSG_SENDER
            && value != MP_RECV_FRAG_MSG_RECVER) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_SetLinkOption: "
                        "illegal option value for link receive mode");
            return MP_Failure;
        }
        sprintf(log_msg, "%s %s", "MP_SetLinkOption: set Receive Mode to",
                recv_mode2string[value - 1]);
        link->rcv_mode = value;
        break;

    case MP_LINK_LOG_MASK_OPT:
        oldval = link->logmask;
        switch (value) {
        case MP_LOG_READ_EVENTS:
        case MP_LOG_WRITE_EVENTS:
        case MP_LOG_INIT_EVENTS:
        case MP_LOG_CONTROL_EVENTS:
        case MP_LOG_URGENT_EVENTS:
        case MP_LOG_ALL_EVENTS:
            link->logmask |= value;
            break;

        case MP_LOG_NO_READ_EVENTS:
        case MP_LOG_NO_WRITE_EVENTS:
        case MP_LOG_NO_INIT_EVENTS:
        case MP_LOG_NO_CONTROL_EVENTS:
        case MP_LOG_NO_URGENT_EVENTS:
        case MP_LOG_NO_EVENTS:
            link->logmask &= value;
            break;

        default:
            sprintf(log_msg, "MP_SetLinkOption: bad logmask - 0x%X", value);
            MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
            return MP_Failure;
        }
        sprintf(log_msg, "MP_SetLinkOption: set logmask to 0x%X",
                link->logmask);
        break;

    default:
        sprintf(log_msg, "MP_SetLinkOption: illegal option - 0x%X", option);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
        return MP_Failure;
    }

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_INIT_EVENTS)
        MP_LogEvent(link, MP_INIT_EVENT, log_msg);
#endif

    return oldval;
}

/********************************************************************
 * FUNCTION:  MP_GetLinkOption
 * ARGUMENT:  link   - (in) pointer to a link structure
 *            option - (in) the option to return
 * RETURN:    success - value of the option
 *            failure - MP_Failure
 *
 * PURPOSE:   Make sure that the option is valid.
 * NOTE:      Since we return MP_Failure for an illegal option, we
 *            must make sure that there can never be a legal option
 *            value of 0 (MP_Failure).
 ********************************************************************/
#ifdef __STDC__
int MP_GetLinkOption(MP_Link_pt link, int option)
#else
int MP_GetLinkOption(link, option)
    MP_Link_pt link;
    int        option;
#endif
{
    if (link == NULL) {
        fprintf(stderr, "MP_GetLinkOption: null link!!\n");
        fflush(stderr);
        return MP_Failure;
    }

    switch (option) {
    case MP_LINK_SEND_MODE_OPT:
        return link->snd_mode;
    case MP_LINK_RECV_MODE_OPT:
        return link->rcv_mode;
    case MP_LINK_LOG_MASK_OPT:
        return link->logmask;
    default:
        MP_LogEvent(link, MP_ERROR_EVENT,  "MP_GetLinkOption: illegal option");
    }

    return MP_Failure;
}


/*
 * IMP_GetLong() fetches a long integer from a link and store it in lp.
 * returns either MP_Success or MP_Failure.
 * arguments:
 *     link: the link from which the long integer is to be fetched.
 *     lp: pointer to memory location where the long integer is to be stored.
 */
#ifdef __STDC__
MP_Status_t IMP_GetLong(MP_Link_pt  link,
                        long       *lp)
#else
MP_Status_t IMP_GetLong(link, lp)
    MP_Link_pt  link;
    long       *lp;
#endif
{
    long mylong, *buflp = (long *)link->in_finger;

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetLong: entering\n");
#endif

    /* first try the inline, fast case */
    if (link->fbtbc >= sizeof(long)
        && (int)link->in_boundry - (int)buflp >= sizeof(long)) {

#ifdef WORDS_BIGENDIAN
        *lp = *buflp;
#else /* not WORDS_BIGENDIAN */
        /*
         * Ooh-argh! The following may be problematic if we are doing the
         * conversion on the SOURCE (the buffer).  If the programmer backs
         * up in the buffer and then rereads this data, what is the result?
         * The bytes will have been moved around!
         */

        if (link->link_word_order == MP_BigEndian)
            *lp = (long)ntohl((unsigned long)(*buflp));
        else
            *lp = *buflp;
#endif /* not WORDS_BIGENDIAN */

        link->fbtbc     -= sizeof(long);
        link->in_finger += sizeof(long);
    } else {
        if (!IMP_GetBytes(link, (char *)&mylong, sizeof(long)))
            return MP_Failure;

#ifdef WORDS_BIGENDIAN
        *lp = mylong;
#else /* not WORDS_BIGENDIAN */
        if (link->link_word_order == MP_BigEndian)
            *lp = (long)ntohl((unsigned long)mylong);
        else
            *lp = mylong;
#endif /* not WORDS_BIGENDIAN */
    }

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetLong: exiting successfully, returning 0x%X\n", *lp);
#endif

    return MP_Success;
}


/*
 * IMP_PutLong() puts a long integer from lp to a link.
 * returns either MP_Success or MP_Failure.
 * arguments:
 *     link: the link to which the long integer is to be put.
 *     lp: pointer to memory location where the long integer is stored.
 */

#ifdef __STDC__
MP_Status_t IMP_PutLong(MP_Link_pt  link,
                        long       *lp)
#else
MP_Status_t IMP_PutLong(link, lp)
    MP_Link_pt  link;
    long       *lp;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "IMP_PutLong: entering - long is %d, 0x%X\n", *lp, *lp);
#endif

    if (link->out_finger + sizeof(long) > link->out_boundry) {
        *(link->o_frag_header) = (unsigned long)link->out_finger
                                 - (unsigned long)link->o_frag_header - sizeof(unsigned long);
        if (link->snd_mode) {
            if (!flush_out(link,link->o_buff))
                return MP_Failure;
            MP_ResetLink(link);
        }
        else {
            link->curr_o_buff = get_o_buff(link);
            if (link->curr_o_buff == NULL)
                return MP_Failure;
        }
    }

#ifdef WORDS_BIGENDIAN
    *(long *)link->out_finger = (long)*lp;
#else /* not WORDS_BIGENDIAN */
    /*
     * I am a little endian machine - see if I have to use big-endian
     * to satisfy my partner
     */
    if (link->link_word_order == MP_BigEndian)
        *(long *)link->out_finger = (long)htonl((unsigned long)(*lp));
    else
        *(long *)link->out_finger = *lp;
#endif /* not WORDS_BIGENDIAN */

    link->out_finger += sizeof(long);

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_PutLong: exiting successfully - put 0x%X\n", *lp);
#endif

    return MP_Success;

}


/*
 * IMP_GetBytes() fetches a given number of bytes from a link.
 * returns either MP_Success or MP_Failure.
 * arguments:
 *     link: the link from which the bytes are fetched.
 *     addr: pointer to memory location in which the bytes are to be stored.
 *     len: number of bytes to be fetched.
 * must manage buffers, fragments, and messages
 */

#ifdef __STDC__
MP_Status_t  IMP_GetBytes(MP_Link_pt    link,
                          char *       addr,
                          unsigned long len)
#else
MP_Status_t  IMP_GetBytes(link, addr, len)
    MP_Link_pt    link;
    char *       addr;
    unsigned long len;
#endif
{
    unsigned long current;

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetBytes: entering - len = %u, addr = 0x%X\n",
            len, addr);
    fprintf(stderr, "IMP_GetBytes: entering - last_frag = %d, fbtbc = %d\n",
            link->last_frag, link->fbtbc);
#endif

    while (len > 0) {
        current = link->fbtbc;
        if (current == 0) {
            if (link->last_frag || !set_input_fragment(link))
                return MP_Failure;
        } else {
            current = (len < current) ? len : current;
            if (!get_input_bytes(link, addr, current))
                return MP_Failure;

            addr        += current;
            link->fbtbc -= current;
            len         -= current;
        }
    }

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetBytes: exiting - addr = 0x%X\n", addr);
#endif

    return MP_Success;
}

/*
 * IMP_PutBytes() puts a given number of bytes into a link.
 * returns either MP_Success or MP_Failure.
 * arguments:
 *     link: the link into which the bytes are put.
 *     addr: pointer to memory location in which the bytes are stored.
 *     len: number of bytes to put.
 */

#ifdef __STDC__
MP_Status_t IMP_PutBytes(MP_Link_pt    link,
                         char *       addr,
                         unsigned long len)
#else
MP_Status_t IMP_PutBytes(link, addr, len)
    MP_Link_pt    link;
    char *       addr;
    unsigned long len;
#endif
{
    unsigned long current;

#ifdef MP_DEBUG
    printf("IMP_PutBytes: entering - len = %d, addr = %X\n", len, addr);
#endif

    while (len > 0) {
        current = (unsigned long)link->out_boundry - (unsigned long)link->out_finger;
        current = (len < current) ? len : current;
        memcpy(link->out_finger, addr, current);

        link->out_finger += current;
        addr             += current;
        len              -= current;

        if (link->out_finger == link->out_boundry) {
            *(link->o_frag_header) = (unsigned long)link->out_finger
                                     - (unsigned long)link->o_frag_header
                                     - sizeof(unsigned long);
            if (link->snd_mode) {
                if (!flush_out(link, link->o_buff))
                    return MP_Failure;
                MP_ResetLink(link);
            }
            else
                if (len <= 0)
                    return MP_Success;
                else {
                    link->curr_o_buff = get_o_buff(link);
                    if (link->curr_o_buff == NULL)
                        return MP_Failure;
                }

        }
    }

#ifdef MP_DEBUG
    printf("IMP_PutBytes: exiting\n");
#endif

    return MP_Success;
}


/*
 * MP_SkipMsg() skips all data in current message.
 * Before reading (deserializing from the link), one should always call
 * this procedure to guarantee proper message alignment.
 * MP_SkipMsg() does not cause the link to read in a buffer upon first
 * call.
 * returns either MP_Success or MP_Failure.
 * arguments:
 *     link: the link to be processed by MP_SkipMsg().
 */
#ifdef __STDC__
MP_Status_t MP_InitMsg(MP_Link_pt link)
#else
MP_Status_t MP_InitMsg(link)
    MP_Link_pt link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "MP_InitMsg: entering - fbtbc = %u, last_frag = %u\n",
            link->fbtbc, link->last_frag);
#endif

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(log_msg,
                "MP_InitMsg: %lu bytes left in previous fragment, "
                "last fragment bit is %s",
                link->fbtbc, (link->last_frag) ? "TRUE" : "FALSE");
        MP_LogEvent(link, MP_READ_EVENT, log_msg);
    }
#endif
    if (link->fbtbc > 0 || link->last_frag)
	ERR_CHK(MP_SkipMsg(link)); 
    if(!set_input_fragment(link))
      return MP_SetError(link, MP_CantInitMsg);

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_SkipMsg(MP_Link_pt link)
#else
MP_Status_t MP_SkipMsg(link)
    MP_Link_pt link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "MP_SkipMsg: entering - fbtbc = %u, last_frag = %u\n",
            link->fbtbc, link->last_frag);
#endif

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(log_msg,
                "MP_SkipMsg: %lu bytes left in previous fragment, "
                "last fragment bit is %s",
                link->fbtbc, (link->last_frag) ? "TRUE" : "FALSE");
        MP_LogEvent(link, MP_READ_EVENT, log_msg);
    }
#endif

    if (link == NULL || link->env == NULL) return MP_NullLink;
    while (link->fbtbc > 0 || !link->last_frag) {
        if (!skip_input_bytes(link, link->fbtbc))
            return MP_SetError(link, MP_CantSkipMsg);

        link->fbtbc = 0;
        if (!link->last_frag && !set_input_fragment(link))
            return MP_SetError(link, MP_CantSkipMsg);
    }

    link->last_frag  = MP_FALSE;
    m_free_in_bufs(link);
    link->in_boundry = link->in_finger;
    link->sfblr      = -1;

#ifdef MP_DEBUG
    fprintf(stderr, "MP_SkipMsg: exiting - fbtbc = %u, last_frag = %u\n",
            link->fbtbc, link->last_frag);
#endif

    return MP_ClearError(link);
}



/*
 * MP_EndMsg() signifies the end of an MP message. It sets the proper header
 * information in the buffer and initiates a flush to send the buffer(s).
 * The client must tell the package when an end-of-message has occurred.
 * returns either MP_Success or MP_Failure
 * argument:
 *     link: the link in which end of message occurrs.
 */

#ifdef __STDC__
MP_Status_t MP_EndMsg(MP_Link_pt link)
#else
MP_Status_t MP_EndMsg(link)
    MP_Link_pt link;
#endif
{
    buffer_handle_pt cur_buff;

    if (link == NULL)
        return MP_Failure;

    /*
     * the problem here is that o_frag_header appears not to have been
     * set to the beginning of the current buffer, pointed to by
     * cur_buff->buff.  So the eor bit does not get set properly.
     */

    *(long*)link->o_frag_header = ((unsigned long)link->out_finger
                                   - (unsigned long)link->o_frag_header
                                   - sizeof(long)) | eor_mask;

    cur_buff = link->o_buff;
    while ((*(long *)cur_buff->buff & eor_mask) == 0) {
        if (!flush_out(link, cur_buff))
            return MP_Failure;
        cur_buff = cur_buff->next;
    }

    if (!flush_out(link, cur_buff))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        MP_LogEvent(link, MP_WRITE_EVENT, "MP_EndMsg: end of message");
#endif

#ifdef MP_DEBUG
    fprintf(stderr,"MP_EndMsg: exiting\n");
#endif

    return MP_Success;
}


/*******************************************************************************
 *
 * Internal useful routines
 *
 ******************************************************************************/

#ifdef __STDC__
static MP_Status_t flush_out(MP_Link_pt       link,
                             buffer_handle_pt buff)
#else
static MP_Status_t flush_out(link, buff)
    MP_Link_pt       link;
    buffer_handle_pt buff;
#endif
{
    unsigned long len, hdr;

    /*
     * QUESTION: Has the end-of-message bit been set by the time we get here?
     *      Yes, in MP_EndMsg()
     */

    if (buff == NULL || buff->buff == NULL)
        return MP_Failure;

    len = (*(long *)buff->buff & ~eor_mask) + sizeof(long);
    hdr = *(unsigned long *)buff->buff;
    *(long *)buff->buff = htonl(hdr);

    if (link->transp.transp_ops->write_buf(link, buff->buff, len) != len)
        return MP_Failure;

    if (link->transp.transp_ops->flush_buf(link) != MP_Success) {
	fprintf(stderr,"flush_out: failed on flush_buf\n"); fflush(stderr);
        return MP_Failure;
	}

#ifdef MP_DEBUG
    fprintf(stderr, "flush_out: exiting, len = %d, hdr = 0x%X\n",
            len, htonl(hdr));
    fflush(stderr);
#endif

    return MP_Success;
}


#ifdef __STDC__
MP_Boolean_t MP_TestEofMsg(MP_Link_pt link)
#else
MP_Boolean_t MP_TestEofMsg(link)
    MP_Link_pt link;
#endif
{
    if (link == NULL) {
        fprintf(stderr, "MP_TestEofMsg: NULL link!\n");
        fflush(stderr);
        return MP_TRUE;
    }

    if (link->fbtbc == 0 && link->last_frag)
        return MP_TRUE;

    return MP_FALSE;
}


int transp_to_num(char *transp)
{
   int i = 0;
   while (transp_dev2string[i] != NULL &&
          strcmp(transp, transp_dev2string[i]) != 0) i++;
   return i;

}

/*
 * fill_a_buff() fills a reciever's buffer. It quits when a recievers' buffer
 * is full, or when it has reached the end of a message.
 * This function is called internally by fill_input_buf().
 */
#ifdef __STDC__
static MP_Status_t fill_a_buff(MP_Link_pt link)
#else
static MP_Status_t fill_a_buff(link)
  MP_Link_pt link;
#endif
{
    unsigned long bytes_to_read, bytes_read, bytes_left;
    unsigned long hdr;

#ifdef MP_DEBUG
    fprintf(stderr, "fill_a_buff: entering\n");
    fprintf(stderr, "\t\tin_boundry = 0x%X, in_finger = 0x%X\n", 
            link->in_boundry, link->in_finger); 
#endif

    for ( ; ; ) {
        bytes_read = link->transp.transp_ops->read_buf(link, &hdr,
                                                       sizeof(long));
        if (bytes_read != sizeof(long)) {
            /* March 11  sgray - added the following so that repeated
               attempts to read from an empty data stream (e.g., an 
               empty file) would fail.  Before this, a first attempt
               to read would fail but subsequent attempts would think
               there was data in the link because m_free_in_bufs(),
               invoked in fill_input_buf(), resets these two pointers
               to be the beginning (in_finger) and end (in_boundry) of 
               an input buffer.  Other routines rely on this to know
               how much space there is to put data. */
            link->in_boundry = link->in_finger;
            return MP_Failure;
            }

        hdr = ntohl(hdr);
        link->s_last_frag = (hdr & eor_mask) == 0 ? MP_FALSE : MP_TRUE;
        hdr = hdr & ~eor_mask;
        link->sfblr = hdr;

        /*  sometimes the link seems ready and the read routine returns, but  */
        /*  nothing was actually read and we return as if everything is okay! */
        /*  if (link->sfblr == 0) return(MP_Failure);                        */
        /*  I commented out the line above so that we could handle the      */
        /*  special case in which the amount of data sent is exactly the    */
        /*  buffer size. In this case, the sender calls MP_EndMsg() which   */
        /*  sends an _empty_ buffer (0 bytes) but with the eor bit set.     */
        /*  We want to read in that 4 byte message fragment header, and     */
        /*  handle it properly.  Returning MP_Failure, as the line above    */
        /*  did, is clearly wrong.  I do not remember clearly why I         */
        /*  put that line in.  We'll see what happens :-)                   */

        while (link->sfblr != 0) {
            if (link->in_finger == link->in_boundry) {
                link->in_finger = link->in_base;
                return MP_Success;
            }

            bytes_left    = (unsigned long)link->in_boundry - (unsigned long)link->in_finger;
            bytes_to_read = link->sfblr < bytes_left ? link->sfblr : bytes_left;
            bytes_read    = link->transp.transp_ops->read_buf(link,
                                                              link->in_finger,
                                                              bytes_to_read);

            link->in_finger      += bytes_read;
            *link->i_frag_header += bytes_read;
            link->sfblr          -= bytes_read;
        }

        if (link->s_last_frag) {
            *link->i_frag_header = *link->i_frag_header | eor_mask;
            link->in_finger      = link->in_base;
            return MP_Success;
        }
    }

#ifdef MP_DEBUG
    fprintf(stderr, "fill_a_buff: exiting\n");
    fflush(stderr);
#endif
}


/******************************************************************************
 *
 * fill_input_buf() does the following:
 * 1. Depending on rcv_mode, it reads in an appropriate amount of data
 *    into reciever's buffers.
 * 2. It prepares the pointers in the input links for other routines
 *    that read data from the input buffers.
 * This routine manages both message and fragment.
 * returns either MP_Success or MP_Failure.
 *
 ******************************************************************************/
#ifdef __STDC__
MP_Status_t fill_input_buf(MP_Link_pt link)
#else
MP_Status_t fill_input_buf(link)
    MP_Link_pt link;
#endif
{
    unsigned long  bytes_to_read, bytes_read, bytes_left, eor, x, xx;
    unsigned long  tmp_int, hdr;

#ifdef MP_DEBUG
    fprintf(stderr, "fill_input_buf: entering\n");
    fprintf(stderr, "\t\tin_boundry = 0x%X, in_finger = 0x%X\n", link->in_boundry, link->in_finger); 
#endif

    if (link == NULL || link->curr_i_buff == NULL)
        return MP_Failure;

    if (link->curr_i_buff->next == NULL) {

        /* This means we've exhausted all read data, have to read some more. */
        /* First thing to do is to get rid of all data read previously. */
        m_free_in_bufs(link);

        /* Then determine in what fashion we're going to read in new data. */
        switch (link->rcv_mode) {
        case MP_RECV_FRAG_MSG_RECVER:

#ifdef MP_DEBUG
            fprintf(stderr, "\tMSG_RECVER: sfblr = %d\n", link->sfblr);
#endif

            if (link->sfblr > 0 ) {
                bytes_to_read = link->sfblr < link->env->buff_pool->buff_size ?
                                link->sfblr : link->env->buff_pool->buff_size;

                bytes_read = link->transp.transp_ops->read_buf(link,
                                                               link->in_finger,
                                                               bytes_to_read);
                link->in_finger      += bytes_read;
                link->sfblr          -= bytes_read;
                *link->i_frag_header  = bytes_read;

                if (bytes_read != bytes_to_read) {
                    link->in_boundry  = link->in_finger;
                    link->in_finger  -= bytes_read;
                    return MP_Failure;
                }

                if (link->sfblr == 0)
                    if (link->s_last_frag)
                        *link->i_frag_header = *link->i_frag_header | eor_mask;
                    else
                        return fill_a_buff(link);

                link->in_finger = link->in_base;
                return MP_Success;
            }
            else
                return fill_a_buff(link);


        case MP_RECV_FRAG_MSG_SENDER:
        case MP_RECV_WHOLE_MSG:

#ifdef MP_DEBUG
            fprintf(stderr, "\tMSG_SENDER/WHOLE_MSG: sfblr = %d\n",
                    link->sfblr);
#endif

            if (link->sfblr < 0)
                link->in_boundry = link->in_finger
                                   + link->env->buff_pool->buff_size;
            for ( ; ; ) {
                bytes_read = link->transp.transp_ops->read_buf(link, &hdr,
                                                               sizeof(long));
                if (bytes_read != sizeof(long)) {
		  if (link->sfblr < 0) link->in_boundry = link->in_finger;
                    return MP_Failure;
	            }

                /*
                 * There was no conversion of the header from network to host
                 * long!!  August 4, 1995
                 */

                hdr = ntohl(hdr);
                eor = (hdr & eor_mask) == 0 ? MP_FALSE : MP_TRUE;
                hdr = hdr & ~eor_mask;
                x = hdr;

                while (x != 0) {
                    if (link->in_finger == link->in_boundry) {
                        link->curr_i_buff = get_i_buff(link);
                        if (link->curr_i_buff == NULL)
                            return MP_Failure;
                    }

                    bytes_left = (unsigned int)link->in_boundry
                                 - (unsigned int)link->in_finger;
                    xx = x < bytes_left ? x : bytes_left;
                    bytes_read = link->transp.transp_ops->read_buf(link,
                                                           link->in_finger, xx);
                    link->in_finger      += bytes_read;
                    *link->i_frag_header += bytes_read;
                    x -= bytes_read;
                }

                if (eor) {
                    *link->i_frag_header = *link->i_frag_header | eor_mask;
                    break;
                }

                if (link->rcv_mode == MP_RECV_FRAG_MSG_SENDER)
                    break;
            }

            link->curr_i_buff->next = NULL; /* supposedly guaranteed */
            reset_i_buff(link);

#ifdef MP_DEBUG
            fprintf(stderr, "fill_input_buf: exiting "
                    "- MP_RECV_FRAG_MSG_SENDER/MP_RECV_WHOLE_MSG\n");
#endif
            return MP_Success;

        default:
            return MP_Failure;
        }
    }
    else {
        link->curr_i_buff   = link->curr_i_buff->next;
        link->in_base       = link->curr_i_buff->buff;
        link->in_finger     = link->in_base;
        link->i_frag_header = (unsigned long *)link->in_base;
        tmp_int             = *link->i_frag_header;
        link->in_boundry    = link->in_finger + (tmp_int & ~eor_mask)
                              + sizeof(long);
    }

    return MP_Success;
}



/*******************************************************************************
 *
 * knows nothing about messages! Only about input buffers
 *
 *******************************************************************************/
#ifdef __STDC__
static MP_Status_t  get_input_bytes(MP_Link_pt link,
                                    char *    addr,
                                    int        len)
#else
static MP_Status_t  get_input_bytes(link, addr, len)
    MP_Link_pt link;
    char *    addr;
    int        len;
#endif
{
    int current;

#ifdef MP_DEBUG
    fprintf(stderr, "get_input_bytes: entering - len = %u, addr = 0x%X\n",
            len, addr);
#endif

    while (len > 0) {
        current = (int)link->in_boundry - (int)link->in_finger;
        if (current == 0) {
            if (!fill_input_buf(link))
                return MP_Failure;
        } else {
            current = len < current ? len : current;
            memcpy(addr, link->in_finger, current);
            link->in_finger += current;
            addr            += current;
            len             -= current;
        }
    }

#ifdef MP_DEBUG
    fprintf(stderr, "get_input_bytes: exiting - addr = 0x%X\n", addr);
#endif

    return MP_Success;
}


/*******************************************************************************
 *
 * next two bytes of the input link are treated as a header
 *
 *******************************************************************************/
#ifdef __STDC__
static MP_Status_t set_input_fragment(MP_Link_pt link)
#else
static MP_Status_t set_input_fragment(link)
    MP_Link_pt link;
#endif
{
    unsigned long header;

#ifdef MP_DEBUG
    fprintf(stderr, "set_input_fragment: entering\n");
#endif

    if (!get_input_bytes(link, (char *)&header, sizeof(unsigned long)))
        return MP_Failure;

    /*
     * Unfortunately we also have to use ntohl in fill_input_buf, so we
     * can't do it here also.  It must be done in one place only!!
     */

    /* header = (long)ntohl(header); */
    link->last_frag = (header & eor_mask) == 0 ? MP_FALSE : MP_TRUE;
    link->fbtbc = header & ~eor_mask;

#ifdef MP_DEBUG
    fprintf(stderr, "set_input_fragment: exiting, last_frag = %d, fbtbc = %d\n",
            link->last_frag, link->fbtbc);
#endif

    return MP_Success;
}

/*******************************************************************************
 *
 * consumes input bytes; knows nothing about messages!
 *
 *******************************************************************************/
#ifdef __STDC__
static MP_Status_t skip_input_bytes(MP_Link_pt link,
                                    long       cnt)
#else
static MP_Status_t skip_input_bytes(link, cnt)
    MP_Link_pt link;
    long       cnt;
#endif
{
    int current;

    while (cnt > 0) {
        current = (int)link->in_boundry - (int)link->in_finger;
        if (current == 0) {
            if (!fill_input_buf(link))
                return MP_Failure;
        } else {
            current = cnt < current ? cnt : current;
            link->in_finger += current;
            cnt -= current;
        }
    }

    return MP_Success;
}


