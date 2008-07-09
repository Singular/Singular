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
 *   IMPLEMENTATION FILE:  MP_FileTransp.c
 *                                
 *      Public interface routines to the "file" transport device.
 *      These include read, write, open, close, and init routines.
 *      There are some support routines as well.
 *
 *  Change Log:
 *       November 21, 1994 SG - Added file_has_data() and put it in the
 *                              the transp_op structure
 *       September 11, 1995 SG - Reorganization of files.  Cleaned up
 *                               header files and relocated source to
 *                               its own file.
 *       3/3/96    sgray - moved call to file_init_transport() inside
 *                         file_open_transport() and removed the init
 *                         routine from the file_ops struct.
 *       9/22/96   sgray  - changed tv_sec to 0 in file_get_status()
 *                               so that select() does polling.
 *       3/11/97   sgray - Changed all memory management calls that allocate
 *                         or deallocate strings to use the "Raw" calls.
 *
 ***************************************************************************/


#ifndef lint
static char vcid[] = "@(#) $Id: MP_FileTransp.c,v 1.3 2008-07-09 07:42:23 Singular Exp $";
#endif /* lint */

#include "MP.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>


#ifdef __WIN32__
   #include <io.h>
#endif

#ifdef __WIN32__

#   include <sys/stat.h>
#   define  FILE_MODE   S_IREAD | S_IWRITE

#else /* not __WIN32__ */

#    define  FILE_MODE   0666
# 
#    ifndef  O_BINARY
#        define  O_BINARY    0
#    endif /* O_BINARY */

#endif /* not __WIN32__ */

/* #define MP_DEBUG */

EXTERN char *IMP_GetCmdlineArg _ANSI_ARGS_((int, char**, char*));

#define log_msg_len 128
static char log_msg[log_msg_len];


MP_TranspOps_t file_ops = {
    file_write,
    file_read,
    file_flush,
    file_get_status,
    file_open_connection,
    file_close_connection,
    file_close_connection
};


#ifdef __STDC__
int get_file_mode(int    argc,
                  char **argv)
#else
int get_file_mode(argc, argv)
    int    argc;
    char **argv;
#endif
{
    char *cmode;

    if ((cmode = IMP_GetCmdlineArg(argc, argv, "-MPmode")) != NULL) {
        if (strcmp(cmode, "read") == 0)
            return MP_READ_MODE;
        else if (strcmp(cmode, "write") == 0)
            return MP_WRITE_MODE;
        else if (strcmp(cmode, "append") == 0)
            return MP_APPEND_MODE;
    }

    return MP_NO_SUCH_FILE_MODE;
}


/****  FILE transport "device" "public" routines ******/
#ifdef __STDC__
MP_Boolean_t file_get_status(MP_Link_pt link, MP_LinkStatus_t status_to_check)
#else
MP_Boolean_t file_get_status(link, status_to_check)
    MP_Link_pt      link;
    MP_LinkStatus_t status_to_check;
#endif
{

#ifndef __WIN32__
    int    filepos, fd = fileno((*(MP_FILE_t *)(link->transp.private1)).fptr);
    struct stat statbuf;
#endif  /* __WIN32__ */

#ifdef MP_DEBUG
    printf("file_get_status: entering for link %d\n", link->link_id);
#endif

#ifdef __WIN32__
   switch (status_to_check)
   {
      case MP_LinkReadyReading:
         return MP_TRUE;

      case MP_LinkReadyWriting:
         return MP_TRUE;

      default:
         sprintf(
            log_msg, "file_get_status: illegal option %d",
            status_to_check
         );
         MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
         return MP_FALSE;
   }

#else /* not __WIN32__ */

    switch (status_to_check) {
    case MP_LinkReadyReading:
	if ((*(MP_FILE_t *)(link->transp.private1)).access_mode 
	     == MP_READ_MODE) {
          fstat(fd, &statbuf);
	  filepos = ftell((*(MP_FILE_t *)(link->transp.private1)).fptr);
	  return (!(filepos == statbuf.st_size));
	}
	else {
          MP_LogEvent(link, MP_ERROR_EVENT, 
          "file_get_status: can't check read status on a file opened for writing");
	  return MP_FALSE;
          }

    case MP_LinkReadyWriting:
	if ((*(MP_FILE_t *)(link->transp.private1)).access_mode 
	     == MP_WRITE_MODE) 
	  return MP_TRUE;
	else {
          MP_LogEvent(link, MP_ERROR_EVENT, 
          "file_get_status: can't check write status on a file opened for reading");
	  return MP_FALSE;
          }

    default:
        sprintf(log_msg,"file_get_status: illegal option %d",
                status_to_check);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
        return MP_FALSE;
    }
#endif /* not __WIN32__ */

}



#ifdef __STDC__
long file_read(MP_Link_pt link,
               char *    buf,
               long       len)
#else
long file_read(link, buf, len)
    MP_Link_pt link;
    char *    buf;
    long       len;
#endif
{
    int   fd   = fileno((*(MP_FILE_t *)(link->transp.private1)).fptr);
    FILE *fptr = (*(MP_FILE_t *)(link->transp.private1)).fptr;
#ifndef __WIN32__
    fd_set mask, readfds;
    /*     fd_set mask = 1 << fd, readfds; */
#endif

#ifdef MP_DEBUG
    printf("file_read: entering -len = %d\n", len);
    fflush(stdout);
#endif

#ifndef __WIN32__
    FD_ZERO(&mask);
    FD_SET(fd, &mask); 
    while (MP_TRUE) {
        readfds = mask;
        switch (SELECT(FD_SETSIZE, &readfds, NULL, NULL, NULL)) {
        case 0:
            MP_LogEvent(link, MP_ERROR_EVENT, "file_read: timed out");
            return -1;

        case -1:
            if (errno == EINTR)
                continue;
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "file_read: something bad happened with select()");
            return -1;
        }
        if (FD_ISSET(fd, &readfds))
            break;
    }
#endif /* __WIN32__ */

    if ((len = fread(buf, 1, len, fptr)) == -1) {
        MP_LogEvent(link, MP_ERROR_EVENT, "file_read: problem with the read");
        return MP_SetError(link, MP_CantReadLink);
    }

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(log_msg, "file_read: read %ld bytes", len);
        MP_LogEvent(link, MP_READ_EVENT, log_msg);
    }
#endif

#ifdef MP_DEBUG
    printf("file_read: exiting - read %d bytes \n", len);
    fflush(stdout);
#endif

    return len;
}




#ifdef __STDC__
long file_write(MP_Link_pt link, char * buf, long len)
#else
long file_write(link, buf, len)
    MP_Link_pt link;
    char *     buf;
    long       len;
#endif
{
  long   cnt, fd = fileno((*(MP_FILE_t *)(link->transp.private1)).fptr);
  FILE * fptr = (*(MP_FILE_t *)(link->transp.private1)).fptr;

#ifndef __WIN32__
  fd_set mask, writefs;
#endif

#ifdef MP_DEBUG
  fprintf(stderr, "file_write: entering -len = %d\n", len);
  fflush(stderr);
#endif

#ifdef __WIN32__

   /*
    * There's not much we can do to check if the file is ready for writing.
    * If it isn't we'll know it later.
    */

#else /* not __WIN32__ */

    FD_ZERO(&mask);
    FD_SET(fd, &mask);

    while (MP_TRUE) {
        writefs = mask;
        switch (SELECT(FD_SETSIZE, NULL, &writefs, NULL, NULL)) {
        case 0:
            MP_LogEvent(link, MP_ERROR_EVENT, "file_write: timed out");
            return -1;

        case -1:
            if (errno == EINTR)
                continue;
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "file_write: something bad happened with select()");
            return -1;
        }
        if (FD_ISSET(fd, &writefs))
            break;
    }
#endif /* __WIN32__ */

  if ((cnt = fwrite(buf, 1, len, fptr)) != len) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "file_write: can't do write:");
            return MP_SetError(link, MP_CantWriteLink);
        }

#ifndef NO_LOGGING
        if (link->logmask & MP_LOG_WRITE_EVENTS) {
            sprintf(log_msg, "file_write: wrote %ld bytes", cnt);
            MP_LogEvent(link, MP_WRITE_EVENT, log_msg);
        }
#endif

#ifdef MP_DEBUG
    fprintf(stderr, "file_write: exiting - wrote %d bytes \n", cnt); 
    fflush(stderr);
#endif

    return len;
}

MP_Status_t 
#ifdef __STDC__
file_flush(MP_Link_pt link)
#else
file_flush(link)
    MP_Link_pt  link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "file_flush: entering\n");
    fflush(stderr);
#endif

  if (fflush((*(MP_FILE_t *)(link->transp.private1)).fptr) != 0)
    return MP_Failure;

#ifdef MP_DEBUG
    fprintf(stderr, "file_flush: exiting  \n");
    fflush(stderr);
#endif
  return MP_ClearError(link);
}


/***********************************************************************
 * FUNCTION:  file_open_connection
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      link established as a file connection
 *            Failure:  MP_Failure
 * OPERATION: Determine the file mode we are using and attempt to
 *            establish a connection accordingly.  We could fail
 *            for any of a number of reasons.  Bad hostname, bad
 *            port number, missing arguments....
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t file_open_connection(MP_Link_pt   link,
                                 int          argc,
                                 char       **argv)
#else
MP_Status_t file_open_connection(link, argc, argv)
    MP_Link_pt   link;
    int          argc;
    char       **argv;
#endif
{
    char      *cmode, *fname, *mode_open_flag = NULL;
    MP_FILE_t *file;

#ifdef MP_DEBUG
    fprintf(stderr, "file_open_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    if (file_init_transport(link) != MP_Success)
        return MP_Failure;

    file = (MP_FILE_t *)(link->transp.private1);
    if ((fname = IMP_GetCmdlineArg(argc, argv, "-MPfile")) == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "file_open_connection: error opening file:"
                    " bad or missing -MPfile option");
        return MP_SetError(link, MP_Failure);
    }
    file->access_mode = get_file_mode(argc, argv);
    switch (file->access_mode) {
    case MP_NO_SUCH_FILE_MODE:
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "file_open_connection: error opening file:"
                    " bad or missing -MPmode option");
        return MP_SetError(link, MP_Failure);

    case MP_WRITE_MODE:
        mode_open_flag = "wb";
        break;

    case MP_READ_MODE:
        mode_open_flag = "rb";
        break;

    case MP_APPEND_MODE:
        mode_open_flag = "ab";
        break;
    }
    
    if ((file->fptr = fopen(fname, mode_open_flag)) == NULL) {
	char *errmsg = NULL;
        cmode = IMP_GetCmdlineArg(argc, argv, "-MPmode");
	errmsg = (char *) IMP_RawMemAllocFnc(strlen(fname) + 80);
        sprintf(errmsg, "file_open_connection: Cannot open file %s "
                "in %s mode", fname, cmode);
        MP_LogEvent(link, MP_ERROR_EVENT, errmsg);
	IMP_RawMemFreeFnc(errmsg);
        return MP_SetError(link, MP_CantOpenFile);
    }

    file->fname  = IMP_RawMemAllocFnc(strlen(fname) + 1);
    if (file->fname == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "file_open_connection: memory allocation error");
        return MP_SetError(link, MP_Failure);
    }
    strcpy(file->fname, fname);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_INIT_EVENTS) {
	char *msg = NULL;
        cmode = IMP_GetCmdlineArg(argc, argv, "-MPmode");
	msg = IMP_RawMemAllocFnc(strlen(file->fname) + strlen(cmode) + 48);
        sprintf(msg, "file_open_connection: opened file %s in %s mode",
                file->fname, cmode);
        MP_LogEvent(link, MP_INIT_EVENT, msg);
        IMP_RawMemFreeFnc(msg);
    }
#endif

#ifdef MP_DEBUG
    fprintf(stderr, "file_open_connection: exiting, filename is %s\n",
            file->fname);
    fflush(stderr);
#endif

    RETURN_OK(link);
}



/***********************************************************************
 * FUNCTION:  file_close_connection
 * INPUT:     link  - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success
 *                      Release the file structure pointed to by private1.
 *            Failure:  MP_Failure
 * OPERATION:
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t file_close_connection(MP_Link_pt link)
#else
MP_Status_t file_close_connection(link)
    MP_Link_pt link;
#endif
{
    MP_FILE_t *file;

#ifdef MP_DEBUG
    fprintf(stderr, "file_close_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    file = (MP_FILE_t *)(link->transp.private1);
    if (file == NULL)
        return MP_SetError(link, MP_NullTransport);

    fclose(file->fptr);
    IMP_RawMemFreeFnc(file->fname);

    IMP_MemFreeFnc(file, sizeof(MP_FILE_t));
    link->transp.private1 = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "file_close_connection: exiting\n");
    fflush(stderr);
#endif

    RETURN_OK(link);
}




/***********************************************************************
 * FUNCTION:  file_init_transport
 * INPUT:     link - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success and link structure initialized for the
 *                      file input or output.
 *            Failure:  MP_Failure
 * OPERATION: If there are no problems, allocate a file structure and
 *            attach it to the private pointer inside link.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t file_init_transport(MP_Link_pt link)
#else
MP_Status_t file_init_transport(link)
    MP_Link_pt link;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "file_init_transport: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    link->transp.transp_dev = MP_FileTransportDev;
    link->transp.transp_ops = &file_ops;
    link->transp.private1 = (char *)IMP_MemAllocFnc(sizeof(MP_FILE_t));
    TEST_MEM_ALLOC_ERROR(link, link->transp.private1);
/*     link->link_bigint_format = MP_GMP;
 *     link->link_bigreal_format = MP_GMP;
 */

#ifdef MP_DEBUG
    fprintf(stderr, "file_init_transport: exiting\n");
    fflush(stderr);
#endif

    RETURN_OK(link);
}


