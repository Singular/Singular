/****************************************************************
 *
 *   HEADER FILE:  MP_Util.h
 *
 ***************************************************************/

#ifndef _MP_Util_h
#define _MP_Util_h

/* defines for logging events */
#define MP_ERROR_EVENT   "ErrorE "
#define MP_WRITE_EVENT   "WriteE "
#define MP_READ_EVENT    "ReadE  "
#define MP_INIT_EVENT    "InitE  "
#define MP_URGENT_EVENT  "UrgentE"
#define MP_CONTROL_EVENT "CntrlE "

/*
 * Logging is done on a per link basis.  The user is allowed to
 * specify which events to log.  The default is to log all events.
 * Error events are always logged and cannot be disabled.
 */

#define MP_LOG_READ_EVENTS         1
#define MP_LOG_WRITE_EVENTS        2
#define MP_LOG_INIT_EVENTS         4
#define MP_LOG_CONTROL_EVENTS      8
#define MP_LOG_URGENT_EVENTS      16
#define MP_LOG_ALL_EVENTS         31
#define MP_LOG_NO_EVENTS           0
#define MP_LOG_NO_READ_EVENTS     ~MP_LOG_READ_EVENTS
#define MP_LOG_NO_WRITE_EVENTS    ~MP_LOG_WRITE_EVENTS
#define MP_LOG_NO_INIT_EVENTS     ~MP_LOG_INIT_EVENTS
#define MP_LOG_NO_CONTROL_EVENTS  ~MP_LOG_CONTROL_EVENTS
#define MP_LOG_NO_URGENT_EVENTS   ~MP_LOG_URGENT_EVENTS

#define log_msg_len 128

EXTERN char *IMP_StrDup  _ANSI_ARGS_((char* s));

EXTERN void MP_LogEvent _ANSI_ARGS_((MP_Link_pt link, char *event, char *msg));

EXTERN MP_Status_t MP_SetError _ANSI_ARGS_((MP_Link_pt link,
                                            MP_Status_t the_err));

#define MP_ClearError(link) ((MP_Status_t) ((link)->MP_errno = (int)MP_Success))

EXTERN void MP_PrintError _ANSI_ARGS_((MP_Link_pt link));

EXTERN char* MP_ErrorStr _ANSI_ARGS_((MP_Link_pt link));

EXTERN char* MP_StatusErrorStr _ANSI_ARGS_((MP_Link_pt link, MP_Status_t status));

EXTERN char *IMP_GetCmdlineArg _ANSI_ARGS_((int argc, char **argv, char *cmd));

EXTERN char *IMP_TypeToString  _ANSI_ARGS_((MP_NodeType_t t));

/* the following are used internally by MP for logging */

EXTERN void log_dicttype _ANSI_ARGS_((MP_Link_pt link, char *event,
                                      char *type_str, MP_NodeType_t type,
                                      MP_NumAnnot_t na, MP_DictTag_t dtag,
                                      void *data, MP_NumChild_t nc));

EXTERN void log_fixnum _ANSI_ARGS_((MP_Link_pt link, char *event,
                                    char *type_str, int type, MP_NumAnnot_t na,
                                    void *val));

EXTERN void log_fixreal _ANSI_ARGS_((MP_Link_pt link, char *event,
                                     char *type_str, int type, MP_NumAnnot_t na,
                                     void *val));

EXTERN void annot_flags_to_str _ANSI_ARGS_((MP_AnnotFlags_t flags));

#endif /* _MP_Util_h */
