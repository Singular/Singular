/*************************************************************************
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
 * 
 *   IMPLEMENTATION FILE:  MP_Env.c
 *                                
 *      Routines to initialize and destroy the MP environment.  The
 *      environment includes the host name of the machine, the log
 *      file name, the buffer pool, etc.  The call to MP_InitializeEnv()
 *      must occur before any attempt is made to open a link.
 *      MP_ReleaseEnv() is one of the last things that should be done
 *      before exiting the program.
 *
 *  Change Log:
 *       September 11, 1995 SG - Reorganization of files.  Cleaned up
 *                               header files and relocated source to
 *                               its own file.
 *       September 18, 1995 SG - Added MP_AllocateEnv().
 *                               Changed MP_InitializeEnv() to take env
 *                               as an argument.  If env is NULL, we start
 *                               from scratch, else we assume MP_AllocateEnv()
 *                               was called previously.
 *       10/16/95   OB - open always a new log file, i.e. do not
 *                       reuse an existing one
 *       11/25/96   SG - Added defines/declarations/functions to 
 *                                support negotiation of word ordering.
 *       1/29/96 sgray - added MP_GetEnvOption().  Note that since we return
 *                     MP_Failure for an illegal option, we must never
 *                     allow a legal option to be the value MP_Failure (0).
 *       3/3/96 SG - Added routines MP_AddEnvTranspDevice(), 
 *                   mp_free_env_transp_list(), and IMP_GetTranspByName(),
 *                   to support adding devices to an environment dynamically.
 *                   Also made necessary changes to the other environment fncs.
 *       4/23/96 SG - Added code to support different arbitrary precision
 *                    packages.  Modified MP_AllocateEnv() and added 
 *                    MP_SetEnvBigIntFormat().
 ***************************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_Env.c,v 1.8 2005-01-25 10:48:21 Singular Exp $";
#endif /*lint */

#include <values.h>  /* to figure out if we use IEEE or not */
#include "MP.h"

#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifndef __WIN32__
#include <unistd.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C" 
{
#endif  
#include <sys/stat.h>
#ifdef __cplusplus
}
#endif
#endif  /* __WIN32__ */

#ifdef __WIN32__
#define MP_LOGFILE          "mplog.100"
#define MP_LOGFILE_NAME     "mplog."
#else   /* not  __WIN32__ */
#define MP_LOGFILE          "/tmp/mplog.100"
#define MP_LOGFILE_NAME     "/tmp/mplog."
#endif  /* not  __WIN32__ */

/* These are the "builtin" devices we support */
EXTERN MP_TranspOps_t tcp_ops;
EXTERN MP_TranspOps_t file_ops;

/* This is the default Big Integer package - GNU Multiple Precision */
EXTERN MP_BigIntOps_t  gmp_bigint_ops;
EXTERN MP_BigRealOps_t gmp_bigreal_ops;

static char log_msg[128];
/* Note: this is dependent on the order of supported formats defined */
/*       in MP_BigNum.h.                                             */
static char *bignum_format2string[4] = {"Dummy", "GMP", "PARI", "SAC"};


/* At compile time we figure out what the native byte ordering is    */
/* and set it in the environment.  Links will inherit this attribute */
/* from the environment as the preferred word ordering and then may  */
/* negotiate it with their partner.                                  */

#ifdef WORDS_BIGENDIAN
static MP_WordOrder_t MP_NATIVE_WORD_ORDER = MP_BigEndian;
#else
static MP_WordOrder_t MP_NATIVE_WORD_ORDER = MP_LittleEndian;
#endif

#ifdef _IEEE
static MP_FpFormat_t MP_NATIVE_FP_FORMAT = MP_IEEE_fp;
#elif vax
static MP_FpFormat_t MP_NATIVE_FP_FORMAT = MP_Vax_fp;
#elif _AIX
static MP_FpFormat_t MP_NATIVE_FP_FORMAT = MP_IEEE_fp;
#else
static MP_FpFormat_t MP_NATIVE_FP_FORMAT = MP_IEEE_fp;
#endif


/* utility routines used private to this file */
#ifdef __STDC__
static void mp_free_env_transp_list(MP_Env_pt env)
#else
static void mp_free_env_transp_list(env)
    MP_Env_pt env;
#endif
{
    MP_TranspList_pt tp, tpn;

    if (env == NULL)
        return;

    tp  = env->transp_dev_list;
    tpn = (MP_TranspList_pt)tp->next;
    IMP_MemFreeFnc(tp, sizeof(struct transp_list_elem));

    while (tpn != NULL) {
        tp  = tpn;
        tpn = (MP_TranspList_pt)tp->next;
        IMP_MemFreeFnc(tp, sizeof(struct transp_list_elem));
    }
}


#ifdef __STDC__
MP_Status_t open_logfile(MP_ENV env)
#else
MP_Status_t open_logfile(env)
    MP_ENV env;
#endif
{
    char  fname[128] = MP_LOGFILE;
    char  logext[8];
    int   ext = MP_INIT_LOGFILE_EXT;

    /* Open a log file with a unique extension starting with mplog.100 */
#ifdef __WIN32__    
    while ((env->logfd = fopen(fname, "r")) != NULL && ext < 10000)
    {
      fclose(env->logfd);
#else
      struct stat buf;
      
      while ((stat(fname, &buf) == 0) && (ext < 100000))
      {
#endif        
        sprintf(logext, "%d", ++ext);
        fname[0]='\0';
        strcpy(fname, MP_LOGFILE_NAME);
        strcat(fname, logext);
      }

      /* Found a log file name which didn't exist previously, so open it now. */
      env->logfd = fopen(fname, "w");
      if (env->logfd == NULL) {
        fprintf(stderr, "MP_InitializeEnv: can't open log file!!\n");
        fflush(stderr);
        return MP_Failure;
      }

#ifndef __WIN32__
      /* chmod to a+w so that others can wipe out those files */
      chmod(fname,
             S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH | S_IWOTH);
#endif     

      /* Store the log file name. */
      env->logfilename = IMP_RawMemAllocFnc(strlen(fname) + 1);
      if (env->logfilename == NULL) {
        fprintf(stderr, "MP_InitializeEnv: can't allocate memory!!\n");
        fflush(stderr);
        IMP_MemFreeFnc(env, sizeof(MP_Env_t));
        return MP_Failure;
      }
      strcpy(env->logfilename, fname);

      return MP_Success;
    }


/******************  Public Interface starts here **************************/

/***************************************************************************
 * FUNCTION:  MP_AllocateEnv()
 * ARGUMENT:  none
 * RETURN:    env - pointer to a environment structure
 * PURPOSE:   Simply allocate memory for an environment stucture and
 *            initialize the variables the may be altered using
 *            MP_SetEnvOption().  The rest will be handled by a subsequent
 *            call to MP_InitializeEnv().
 **************************************************************************/
#ifdef __STDC__
MP_ENV MP_AllocateEnv(void)
#else
MP_ENV MP_AllocateEnv()
#endif
{
    MP_Env_pt env = ((MP_Env_pt)IMP_MemAllocFnc(sizeof(MP_Env_t)));

    if (env == NULL) { /* make sure malloc() worked */
        fprintf(stderr, "MP_AllocateEnv: can't allocate memory!!\n");
        fflush(stderr);
        return NULL;
    }

    env->transp_dev_list   = NULL;
    env->initialized       = MP_FALSE;
    env->buffer_size       = MP_DEFAULT_BUFF_SIZE;
    env->max_free_buffers  = MP_DEFAULT_MAX_FREE_BUFF;
    env->init_free_buffers = MP_DEFAULT_INIT_FREE_BUFF;
    env->num_o_buff        = 1;
    env->native_word_order = MP_NATIVE_WORD_ORDER;
    env->native_fp_format  = MP_NATIVE_FP_FORMAT;

    env->bignum.native_bigint_format  = imp_default_bigint_format;
    env->bignum.bigint_ops            = &imp_default_bigint_ops;
    env->bignum.native_bigreal_format = imp_default_bigreal_format;
    env->bignum.bigreal_ops           = &imp_default_bigreal_ops;

#ifndef NO_LOGGING
    if (open_logfile(env) != MP_Success)
        return NULL;
#else
    env->logfd       = 0;
    env->logfilename = NULL;
#endif

    return env;
}


/***************************************************************************
 * FUNCTION:  MP_InitializeEnv()
 * ARGUMENT:  env - pointer to a environment structure (possibly NULL)
 * RETURN:    env - pointer to a environment structure
 * PURPOSE:   If env is NULL, then we start from scratch to create and
 *            completely initialize an environment structure.  If env is
 *            not NULL, we assume it was created previously with a call
 *            to MP_AllocateEnv() and initialize everything except those
 *            fields that are settable as options using MP_SetEnvOption().
 **************************************************************************/
#ifdef __STDC__
MP_ENV MP_InitializeEnv(MP_Env_pt env)
#else
MP_ENV MP_InitializeEnv(env)
    MP_Env_pt env;
#endif
{
    char   *c;
    time_t  now;

    if (env == NULL)
        if ((env = MP_AllocateEnv()) == NULL)
            return NULL;

    if (gethostname(env->thishost, MP_HOST_NAME_LEN) == -1)
        if ((c = (char *)getenv("HOST")) != NULL)
            strcpy(env->thishost, c);
        else
            strcpy(env->thishost, "can't_get_my_hostname");

    env->num_links   = 0;
    env->initialized = MP_TRUE;

    if (m_create_pool(env, env->buffer_size, env->max_free_buffers,
                      env->init_free_buffers) != MP_Success) {
        fprintf(stderr, "MP_InitializeEnv: can't  allocate buffers!!\n");
        return NULL;
    }

#ifndef NO_LOGGING
    now = time(NULL);
    fprintf(env->logfd, "MP environment: Multi Protocol (MP) version %s %s",
            MP_VERSION, ctime(&now));
    fprintf(env->logfd, "\thost:    %s\n", env->thishost);
    fprintf(env->logfd, "\tlogfile: %s\n", env->logfilename);
    fprintf(env->logfd, "\tbuffer size: %d bytes\n", env->buffer_size);
    fprintf(env->logfd, "\tmax free buffers: %d\n", env->max_free_buffers);
    fprintf(env->logfd, "\tinit free buffers: %d\n", env->init_free_buffers);
    fprintf(env->logfd, "\tnumber output buffers: %d\n", env->num_o_buff);
    fprintf(env->logfd, "\tnative word ordering: ");

    if (MP_NATIVE_WORD_ORDER == MP_BigEndian)
        fprintf(env->logfd, "big endian\n");
    else
        fprintf(env->logfd, "little endian\n");

    /*
     * the following is good for now, but will have to be developed when we
     * add more floating point formats
     */

    fprintf(env->logfd, "\tnative floating point format: ");
    if (MP_NATIVE_FP_FORMAT == MP_IEEE_fp)
        fprintf(env->logfd, "IEEE\n");
    else
        fprintf(env->logfd, "VAX\n");
    fprintf(env->logfd, "\tnative multiple precision integer format: %s\n",
            bignum_format2string[env->bignum.native_bigint_format]);
    fprintf(env->logfd, "\n");
    fflush(env->logfd);
#endif /* NO_LOGGING */

    MP_AddEnvTranspDevice(env, MP_TcpTransportDev, &tcp_ops);
    MP_AddEnvTranspDevice(env, MP_FileTransportDev, &file_ops);

    return env;
}


/***************************************************************************
 * FUNCTION:  MP_ReleaseEnv()
 * ARGUMENT:  env - pointer to a environment structure (possibly NULL)
 * RETURN:    none
 * PURPOSE:   Release all the resources associated with this environment,
 *            then release the environment structure.
  **************************************************************************/
#ifdef __STDC__
void MP_ReleaseEnv(MP_Env_pt env)
#else
void MP_ReleaseEnv(env)
    MP_Env_pt env;
#endif
{
    if (env == NULL)
        return;

#ifndef NO_LOGGING
    fprintf(env->logfd, "\nReleasing MP environment resources. \n");
    fclose(env->logfd);
    IMP_RawMemFreeFnc(env->logfilename);
#endif

    mp_free_env_transp_list(env);
    m_free_pool(env->buff_pool);

    IMP_MemFreeFnc(env, sizeof(MP_Env_t));
}

/********************************************************************
 * FUNCTION:  MP_SetEnvOption
 * ARGUMENT:  env    - pointer to a env structure
 *            option - the option to set
 *            value  - value to set the option to
 * RETURN:    success - old value of the option
 *            failure - MP_Failure
 *
 * PURPOSE:   Make sure that the option and value are valid.
 ********************************************************************/
#ifdef __STDC__
int MP_SetEnvOption(MP_Env_pt env,
                    int option,
                    int value)
#else
int MP_SetEnvOption(env, option, value)
    MP_Env_pt env;
    int       option;
    int       value;
#endif
{
    int  oldval;
    char logmsg[128];

    if (env == NULL) {
        fprintf(stderr, "MP_SetEnvOption: null environment!!\n");
        fflush(stderr);
        return MP_Failure;
    }

    switch (option) {
    case MP_BUFFER_SIZE_OPT:
        oldval = env->buffer_size;
        sprintf(logmsg,
                "Init Event  %s %d", "MP_SetEnvOption: set buffer size to",
                value);
        env->buffer_size = value;
        break;

    case MP_MAX_FREE_BUFFERS_OPT:
        oldval = env->max_free_buffers;
        sprintf(logmsg, "Init Event  %s %d",
                "MP_SetEnvOption: set maximum free buffers to ", value);
        env->max_free_buffers = value;
        break;

    case MP_INIT_FREE_BUFFERS_OPT:
        oldval = env->init_free_buffers;
        if (env->init_free_buffers > env->max_free_buffers)
            env->init_free_buffers = env->max_free_buffers;
        else
            env->init_free_buffers = value;
            sprintf(logmsg, "Init Event  %s %d",
                    "MP_SetEnvOption: set initial number of free buffers to ",
                    env->init_free_buffers);
        break;

    default:
        sprintf(logmsg, "%s: MP_SetEnvOption: illegal option %d",
                MP_ERROR_EVENT, option);
        oldval = MP_Failure;
    }

#ifndef NO_LOGGING
    fprintf(env->logfd, "%s\n", logmsg);
#endif

    return oldval;
}



/********************************************************************
 * FUNCTION:  MP_GetEnvOption
 * ARGUMENT:  env    - pointer to a env structure
 *            option - the option to return
 * RETURN:    success - value of the option
 *            failure - MP_Failure
 *
 * PURPOSE:   Make sure that the option is valid.
 ********************************************************************/
#ifdef __STDC__
int MP_GetEnvOption(MP_Env_pt env,
                    int option)
#else
int MP_GetEnvOption(env, option)
  MP_Env_pt env;
  int       option;
#endif
{
    if (env == NULL) {
        fprintf(stderr, "MP_GetEnvOption: null environment!!\n");
        fflush(stderr);
        return MP_Failure;
    }

    switch (option) {
    case MP_BUFFER_SIZE_OPT:
        return env->buffer_size;

    case MP_INIT_FREE_BUFFERS_OPT:
        return env->init_free_buffers;

    default: ;
    }

    return MP_Failure;
}



/***************************************************************************
 * FUNCTION:  MP_AddEnvTranspDevice()
 * ARGUMENT:  env - the environment to which we will add the device
 *            name - the name by which the device will be known. This
 *                   is the value to be used with -MPtransp (changed to int)
 *            ops - the MP_TranspOps_t structure containing pointers to the
 *                  functions for this device.
 * RETURN:    MP_Status - MP_Success if all goes well
 *                      - MP_Failure only if env is null or we run out of mem.
 * PURPOSE:   Create a new transport list element structure, fill in the
 *            fields with the function's arguments and place the new list
 *            element at the head of the environment's device list.  Note
 *            that we don't check for duplicates.  As my brother and I used
 *            to say, if the user does this, "that his dumbness".
 **************************************************************************/
#ifdef __STDC__
MP_Status_t MP_AddEnvTranspDevice(MP_Env_pt       env,
                                  int             transp_dev,
                                  MP_TranspOps_t *ops)
#else
MP_Status_t MP_AddEnvTranspDevice(env, transp_dev, ops)
    MP_Env_pt      env;
    int            transp_dev;
    MP_TranspOps_t *ops;
#endif
{
    MP_TranspList_pt tp;

#ifdef MP_DEBUG
    fprintf(stderr,"MP_AddEnvTranspDevice: entering to add device %d\n", transp_dev);
    fflush(stderr);
#endif

    if (env == NULL)
        return MP_Failure; /* may have to do more */

    if ((tp = IMP_MemAllocFnc(sizeof(struct transp_list_elem))) == NULL)
        return MP_Failure;

    tp->transp_dev = transp_dev;

    memcpy((char *)&(tp->transp_ops), (char *)ops, sizeof(MP_TranspOps_t));
    tp->next = (MP_TranspList_pt *)env->transp_dev_list;
    env->transp_dev_list = tp;

#ifndef NO_LOGGING
    fprintf(env->logfd, "\tAdded transport device %d to environment\n",
       transp_dev );
    fflush(env->logfd);
#endif

#ifdef MP_DEBUG
    fprintf(stderr,"MP_AddEnvTranspDevice: exiting\n");
    fflush(stderr);
#endif

    return MP_Success;
}



/***************************************************************************
 * FUNCTION:  MP_SetEnvBigIntFormat()
 * ARGUMENT:  env - the environment for which we  change the big num package
 *            format - the name of the BigInt package to use.  The list of
 *                   supported packages can also be found in MP_Types.h.
 *            ops - the MP_BigIntOps_t structure containing pointers to the
 *                  functions for this BigInt package.
 * RETURN:    MP_Status - MP_Success if all goes well
 *                      - MP_Failure only if env is null or we run out of mem.
 * PURPOSE:   Change the big integer package to be used for sending and
 *            receiving arbitrary precision integers.  The default is the
 *            GNU Multiple Precision package.  But if communication is b/t
 *            homogeneous systems, we want to avoid the conversion to GMP.
 *            Also, this approach make any _necessary_ conversions transparent.
 *            The sender at least has the illusion of sending in its "native"
 *            format (assuming it is one of the supported packages).
 *            NOTE: if the ops structure is not one already supported by MP,
 *            we cannot verify that it has the correct functionality.  We
 *            trust the programmer on this one.
 **************************************************************************/
#ifdef __STDC__
MP_Status_t MP_SetEnvBigIntFormat(MP_Env_t          *env,
                                  MP_BigIntOps_t    *ops,
                                  MP_BigNumFormat_t  format)
#else
MP_Status_t MP_SetEnvBigIntFormat(env, ops, format)
    MP_Env_t          *env;
    MP_BigIntOps_t    *ops;
    MP_BigNumFormat_t  format;
#endif
{
    if (env == NULL)
        return MP_NullLink;

    switch (format) {
    case MP_GMP:
        sprintf(log_msg, "%s: environment BigInt format set to %s\n",
                MP_INIT_EVENT, bignum_format2string[format]);
        break;
    case MP_SAC:
        sprintf(log_msg, "%s: environment BigInt format set to %s\n",
                MP_INIT_EVENT, bignum_format2string[format]);
        break;
    case MP_PARI:
        sprintf(log_msg, "%s: environment BigInt format set to %s\n",
                MP_INIT_EVENT, bignum_format2string[format]);
        break;

    default:
	sprintf(log_msg, 
	  "%s: environment BigInt format set to user-supplied package: %d\n",
          MP_INIT_EVENT, format);
    }
    env->bignum.native_bigint_format = format;
    env->bignum.bigint_ops           = ops;

#ifndef NO_LOGGING
    fprintf(env->logfd, log_msg);
#endif

    return MP_Success;
}

/***************************************************************************
 * FUNCTION:  MP_SetEnvBigRealFormat()
 * ARGUMENT:  env - the environment for which we  change the big num package
 *            format - the name of the BigReal package to use.  The list of
 *                   supported packages can also be found in MP_Types.h.
 *            ops - the MP_BigRealOps_t structure containing pointers to the
 *                  functions for this BigReal package.
 * RETURN:    MP_Status - MP_Success if all goes well
 *                      - MP_Failure only if env is null or we run out of mem.
 * PURPOSE:   Change the big real package to be used for sending and
 *            receiving arbitrary precision reals.  The default is the
 *            GNU Multiple Precision package.  But if communication is b/t
 *            homogeneous systems, we want to avoid the conversion to GMP.
 *            Also, this approach make any _necessary_ conversions transparent.
 *            The sender at least has the illusion of sending in its "native"
 *            format (assuming it is one of the supported packages).
 *            NOTE: if the ops structure is not one already supported by MP,
 *            we cannot verify that it has the correct functionality.  We
 *            trust the programmer on this one.
 **************************************************************************/
#ifdef __STDC__
MP_Status_t MP_SetEnvBigRealFormat(MP_Env_t         *env,
                                  MP_BigRealOps_t   *ops,
                                  MP_BigNumFormat_t  format)
#else
MP_Status_t MP_SetEnvBigRealFormat(env, ops, format)
    MP_Env_t          *env;
    MP_BigRealOps_t   *ops;
    MP_BigNumFormat_t  format;
#endif
{
    MP_Status_t status = MP_Success;

    if (env == NULL)
        return MP_NullLink;

    switch (format) {
    case MP_GMP:
        sprintf(log_msg, "%s: environment BigReal format set to %s\n",
                MP_INIT_EVENT, bignum_format2string[format]);
        break;
    case MP_SAC:
        sprintf(log_msg, "%s: %s does not support a BigReal format\n",
                MP_ERROR_EVENT, bignum_format2string[format]);
        status = MP_Failure;
        break;
    case MP_PARI:
        sprintf(log_msg, "%s: environment BigReal format set to %s\n",
                MP_INIT_EVENT, bignum_format2string[format]);
        break;

    default:
	sprintf(log_msg, 
	   "%s: environment BigReal format set to user-supplied package: %d\n",
           MP_INIT_EVENT, format);
    }
    if (status == MP_Success) {
	env->bignum.native_bigreal_format = format;
	env->bignum.bigreal_ops           = ops;
    }

#ifndef NO_LOGGING
    fprintf(env->logfd, log_msg);
#endif

    return status;
}


/***************************************************************************
 * FUNCTION:  IMP_GetTranspByName
 * ARGUMENT:  env - the environment to which we will add the device
 *            name - the name of the device we are looking for.  This is the
 *                   name used with -MPtransp.
 * RETURN:    NULL on failure to find the device or env is NULL.
 *            pointer to a transport list element if name is found.
 * PURPOSE:   Search the environment's device list for a device with the
 *            given name.  Return the _first_ occurrence found.
 **************************************************************************/
#ifdef __STDC__
MP_TranspList_pt IMP_GetTranspByName(MP_Env_pt  env,
                                     int        transp_dev)
#else
MP_TranspList_pt IMP_GetTranspByName(env, transp_dev)
    MP_Env_pt  env;
    int        transp_dev;
#endif
{
    MP_TranspList_pt tp;

    if (env == NULL)
        return NULL;

    tp = env->transp_dev_list;
    while (tp != NULL && (transp_dev != tp->transp_dev))
        tp = (MP_TranspList_pt)tp->next;

    return tp;
}


