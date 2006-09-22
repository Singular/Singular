/***************************************************************************
 *
 *   HEADER FILE:  MP_Env.h
 *
 *        Declarations to maintain the MP environment.  The environment
 *        includes the name of the host machine, the log file name and
 *      file descriptor and the buffer pool containing all the buffers
 *      to be shared among the links.  The initialization should be
 *      done once before any MP-related activities occur.
 *
 *  Change Log:
 *       September 10, 1995  SG - Updated implementation.  Much cleaning
 *                                to make it presentable.
 *       November 25, 1995   SG - Added field in env struct to support
 *                                negotiation of endian order.
 *       April 32, 1996      SG - Added support to handle different
 *                                Big Integer packages.  See note in
 *                                MP_SacBigInt.c for some details.
 ***************************************************************************/
#ifndef _MP_Env_h
#define _MP_Env_h



#include "MP.h"

#define MP_HOST_NAME_LEN    64
#define MP_INIT_LOGFILE_EXT 100

/* These are the transport types currently supported   */
/* These are the values stored in transp_dev.         */
/* If you add to this list, you must also add a string */
/* equivalent to transp_dev2string[] in MP_Link.c.    */

#define MP_TcpTransportDev   0
#define MP_FileTransportDev  1
#define MP_PvmTransportDev   2
#define MP_TbTransportDev    3


/*
 * Options that can be set for an environment
 */
#define MP_BUFFER_SIZE_OPT       1
#define MP_MAX_FREE_BUFFERS_OPT  2
#define MP_INIT_FREE_BUFFERS_OPT 3

typedef struct  {
    long         (*write_buf)VARARGS;       /* write buffer to transport   */
    long         (*read_buf)VARARGS;        /* read transport to buffer    */
    MP_Status_t  (*flush_buf)VARARGS;       /* flush buffer to transport   */
    MP_Boolean_t (*get_status)VARARGS;      /* check status of the link    */
    MP_Status_t  (*open_transp)VARARGS;     /* open the transport device   */
    MP_Status_t  (*close_transp)VARARGS;    /* close the transport device  */
    MP_Status_t  (*kill_transp)VARARGS;    /* kill the transport device  */
} MP_TranspOps_t;


typedef struct {
    int            transp_dev;
    MP_TranspOps_t *transp_ops;
    char           *private1;    /* for opaque transport-specific structure */
} MP_Transport_t;

typedef struct transp_list_elem *MP_TranspList_pt;

struct transp_list_elem {
    int               transp_dev;
    MP_TranspOps_t    transp_ops;
    MP_TranspList_pt *next;
};


typedef struct mp_environment {
    MP_TranspList_pt transp_dev_list; /* list of supported transport devices */
    buffer_pool_pt   buff_pool;       /* buffer pool shared by  links in this
                                       *  env
                                       */
    int   num_o_buff;
    int   buffer_size;                /* size of each buffer in the pool     */
    int   max_free_buffers;           /* max number of free buffers in the
                                       * pool
                                       */

    int   init_free_buffers;          /* initial number of free buffers in the
                                       * pool
                                       */

    int   num_links;                  /* to handout unique link ids for this
                                       * env
                                       */

    FILE  *logfd;                     /* log file for all logging events     */
    char  *logfilename;
    char  thishost[MP_HOST_NAME_LEN];
    MP_Boolean_t initialized;         /* can't set options after env has been
                                       * init
                                       */

    MP_WordOrder_t native_word_order; /* what this machine uses internally   */
    MP_FpFormat_t  native_fp_format;  /* floating pnt format used internally */
    MP_BigNum_t    bignum;            /* info on bignum package to be used   */
} MP_Env_t;

typedef MP_Env_t *MP_Env_pt;
typedef MP_Env_pt MP_ENV;

EXTERN MP_ENV MP_AllocateEnv _ANSI_ARGS_((void));
EXTERN MP_ENV MP_InitializeEnv _ANSI_ARGS_((MP_Env_pt env));
EXTERN void MP_ReleaseEnv _ANSI_ARGS_((MP_Env_pt env));
EXTERN int MP_SetEnvOption _ANSI_ARGS_((MP_Env_pt env, int option, int value));
EXTERN int    MP_GetEnvOption _ANSI_ARGS_((MP_Env_pt env, int option));
EXTERN MP_TranspList_pt IMP_GetTranspByName _ANSI_ARGS_((MP_Env_pt env,
                                                         int transp_dev));
EXTERN MP_Status_t MP_AddEnvTranspDevice _ANSI_ARGS_((MP_Env_pt env,
                                        int transp_dev, MP_TranspOps_t *ops));
EXTERN MP_Status_t MP_SetEnvBigIntFormat _ANSI_ARGS_((MP_Env_t *env,
                             MP_BigIntOps_t *ops, MP_BigNumFormat_t format));
EXTERN MP_Status_t MP_SetEnvBigRealFormat _ANSI_ARGS_((MP_Env_t *env,
                             MP_BigRealOps_t *ops, MP_BigNumFormat_t format));
EXTERN MP_Status_t open_logfile _ANSI_ARGS_((MP_ENV env));
#endif /* _MP_Env_h */
