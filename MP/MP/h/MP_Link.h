/****************************************************************
 *
 *   HEADER FILE:  MP_Link.h
 *
 *   struct link is adapted from the Sun XDR distribution.
 *   Copyright (C) 1984, Sun Microsystems, Inc.
 *
 ***************************************************************/

#ifndef _MP_Link_h
#define _MP_Link_h

/* Sending modes */
#define MP_SEND_FRAG_MSG      1
#define MP_SEND_WHOLE_MSG     2
#define MP_DEFAULT_SEND_MODE  MP_SEND_FRAG_MSG

/* Receiving modes */
#define MP_RECV_FRAG_MSG_RECVER   1
#define MP_RECV_FRAG_MSG_SENDER   2
#define MP_RECV_WHOLE_MSG         3
#define MP_DEFAULT_RECV_MODE      MP_RECV_FRAG_MSG_RECVER

/* Options that can be set for a link */
#define MP_LINK_SEND_MODE_OPT 1
#define MP_LINK_RECV_MODE_OPT 2
#define MP_LINK_LOG_MASK_OPT  3

#define  eor_mask ((unsigned long)(1 << 31))


struct MP_Link_t
{
    int               link_id;
    int               MP_errno;
    MP_Env_pt         env;
    MP_Transport_t    transp;
    MP_BigNum_t       bignum;       /* bignum type & fncs struct  */
    int               logmask;
    buffer_handle_pt  i_buff;       /* ptr to list of input buff  */
    buffer_handle_pt  o_buff;       /* ptr to list of output buff */
    buffer_handle_pt  curr_o_buff;  /* current output buffer      */
    buffer_handle_pt  curr_i_buff;  /* current input buffer       */
    int               num_o_buff;   /* number of o_buff to keep with stream */

    int snd_mode; /* Two possible values:
                   * MP_SEND_FRAG_MSG - send a fragment at a time
                   * MP_SEND_WHOLE_MSG - send a tree at a time
                   */

    int rcv_mode; /* Three possible value:
                   * MP_RECV_FRAG_MSG_RECVER - read one sender's
                   * fragment at a time,
                   * MP_RECV_FRAG_MSG_SENDER - read one reciever's
                   * fragment at a time,
                   * MP_RECV_WHOLE_MSG - read a tree at a time.
                   */
    /*
     * out-going bits
     */
    char   *out_base;         /* output buffer (points to frag header) */
    char   *out_finger;       /* next output position */
    char   *out_boundry;      /* data can go up to this address */
    unsigned long  *o_frag_header;    /* beginning of current fragment */

    /*
     * in-coming bits
     */
    char   *in_base;          /* input buffer (points to frag header) */
    char   *in_finger;        /* location of next byte to be had */
    char   *in_boundry;       /* can read up to this location */
    unsigned long  *i_frag_header;    /* points to fragment header */

    long         fbtbc;        /* fragment bytes to be consumed */
    long         sfblr;        /* sender's fragment bytes left to be read */
    MP_Boolean_t last_frag;    /* tell if curr. input buff. is last of tree */
    MP_Boolean_t s_last_frag;  /* tell if current input buffer is part of
                                  sender's last frag. of tree */

    /* the next three are inherited from the link's  environment, but
     * are resettable and, in some cases, negotiated with the partner
     */
    MP_WordOrder_t    link_word_order;
    MP_FpFormat_t     link_fp_format;
    MP_BigNumFormat_t link_bigint_format;   /* format of SENDER! */
    MP_BigNumFormat_t link_bigreal_format;
};

#define TEST_LINK_NULL(link) \
   if (link == NULL) return (MP_NullLink)

EXTERN MP_Link_pt MP_OpenLink _ANSI_ARGS_((MP_Env_pt env, int argc,
                                             char **argv));

EXTERN void MP_CloseLink _ANSI_ARGS_((MP_Link_pt link));

EXTERN void MP_KillLink _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_PeekHeader _ANSI_ARGS_((MP_Link_pt link,
                                              MP_NodeType_t *ntype,
                                              MP_DictTag_t *dtag,
                                              MP_Common_t *cval));

EXTERN MP_Status_t MP_InitMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_SkipMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_EndMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_EndMsgReset _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Boolean_t MP_TestEofMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN int MP_SetLinkOption _ANSI_ARGS_((MP_Link_pt link, int option,
                                         int value));

EXTERN int MP_GetLinkOption _ANSI_ARGS_((MP_Link_pt link, int option));

EXTERN MP_Boolean_t MP_GetLinkStatus _ANSI_ARGS_((MP_Link_pt link,
                                            MP_LinkStatus_t status_to_check));

EXTERN MP_Status_t IMP_GetLong _ANSI_ARGS_((MP_Link_pt link, long *lp));

EXTERN MP_Status_t IMP_PutLong _ANSI_ARGS_((MP_Link_pt link, long *lp));

EXTERN MP_Status_t IMP_GetBytes _ANSI_ARGS_((MP_Link_pt link, char *addr,
                                             unsigned long len));

EXTERN MP_Status_t IMP_PutBytes _ANSI_ARGS_((MP_Link_pt link, char *addr,
                                             unsigned long len));

EXTERN MP_Status_t IMP_Restore _ANSI_ARGS_((MP_Link_pt link, unsigned long n));
#endif  /* _MP_Link_h */
