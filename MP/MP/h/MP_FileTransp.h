/***************************************************************************
 *
 *   HEADER FILE:  MP_FileTransp.h
 *
 *        Declarations for the "file" transport device.
 *
 *  Change Log:
 *       September 10, 1995  SG - Updated implementation.  Much cleaning
 *                                to make it presentable.
 *       August 21, 1996     SG - Changed FILE_MODE from 0x666 to 0666.
 *                                Now it behaves properly
 *
 ***************************************************************************/
#ifndef _MP_FileTransp_h
#define _MP_FileTransp_h

#define MP_NO_SUCH_FILE_MODE    0
#define MP_READ_MODE            1
#define MP_WRITE_MODE           2
#define MP_APPEND_MODE          3

typedef struct {
    FILE    *fptr;
    int      access_mode; /* read, write, or append as defined above */
    char    *fname;
} MP_FILE_t;

MP_Status_t file_flush _ANSI_ARGS_((MP_Link_pt link));
long file_write _ANSI_ARGS_((MP_Link_pt link, char * buf, long len));
long file_read _ANSI_ARGS_((MP_Link_pt link,
                            char *buf,
                            long len));
MP_Boolean_t file_get_status _ANSI_ARGS_((MP_Link_pt      link,
                                          MP_LinkStatus_t status_to_check));
MP_Status_t file_open_connection _ANSI_ARGS_((MP_Link_pt link,
                                              int        argc,
                                              char       **argv));
MP_Status_t file_close_connection _ANSI_ARGS_((MP_Link_pt link));
MP_Status_t file_init_transport _ANSI_ARGS_((MP_Link_pt link));
int get_file_mode _ANSI_ARGS_((int  argc,
                               char **argv));
#endif /* _MP_FileTransp_h */
