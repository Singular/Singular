/************************************************************************
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
 *   IMPLEMENTATION FILE:  MP_Util.c 
 *                                                                
 *        Useful utility routines used throughout.
 *                                                                
 * 
 *  Change Log: 
 *      11/25/95 sgray - commented out call to MP_ClearError() from 
 *                       MP_PrintError().  I think the error value should
 *                         remain until the next call has cleared it.
 *       4/25/96 sgray - Added logging support routines log_fixnum(), 
 *                       log_op(), and log_fixreal().
 *       5/7/96  sgray - Added MP_ApIntType to IMP_TypeToString().
 *       8/30/96 sgray - Added test for NULL link and environment in
 *                       MP_LogEvent().
 *
 **********************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_Util.c,v 1.6 2008-07-09 07:42:24 Singular Exp $";
#endif /* lint */

#include "MP.h"

#include <string.h>

#ifndef __WIN32__
#include <unistd.h>
#endif

/* these are used in a bunch of places */
char fix_log_msg[log_msg_len];
char AnnotFlagsStr[32];

#ifdef __STDC__
char* IMP_StrDup(char* s)
#else
void IMP_StrDup(s)
  char* s;
#endif
{
  char* d = NULL;
  
  if (s != NULL)
  {
    d = (char*) IMP_RawMemAllocFnc(strlen(s) + 1);
    strcpy(d, s);
  }
  return d;
}

#ifdef __STDC__
void MP_LogEvent(MP_Link_pt link,
                 char      *event,
                 char      *msg)
#else
void MP_LogEvent(link, event, msg)
    MP_Link_pt link;
    char      *event;
    char      *msg;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr, "MP_LogEvent: entering - event = %s\n", event);
    fflush(stderr);
#endif

    if (link == NULL || link->env == NULL){
      fprintf(stderr, "MP_LogEvent: link or environment pointer NULL!!!\n");
      return;
      }

    /*
     * Be kind and make sure there is a logfile first
     */
    if (link->env->logfilename != NULL) {
        fprintf(link->env->logfd, "L%d: %s %s\n", link->link_id, event, msg);
        fflush(link->env->logfd);
    }

#ifdef MP_DEBUG
    fprintf(stderr, "MP_LogEvent: exiting \n"); fflush(stderr);
#endif
}



#ifdef __STDC__
MP_Status_t MP_SetError(MP_Link_pt link, MP_Status_t the_err)
#else
MP_Status_t MP_SetError(link, the_err)
    MP_Link_pt  link;
    MP_Status_t the_err;
#endif
{
    link->MP_errno = the_err;
    if (the_err != MP_Failure && the_err >= 0 && the_err < MP_MaxError)
        MP_LogEvent(link, MP_ERROR_EVENT, MP_errlist[the_err]);
    else {
        sprintf(fix_log_msg, "Unknown error number %d", link->MP_errno);
        MP_LogEvent(link, MP_ERROR_EVENT, fix_log_msg);
    }

    return MP_Failure;
}


/*
#ifdef __STDC__
MP_Status_t MP_ClearError(MP_Link_pt link)
#else
MP_Status_t MP_ClearError(link)
    MP_Link_pt link;
#endif
{
   return (link->MP_errno = MP_Success);
}
*/


#ifdef __STDC__
void MP_PrintError(MP_Link_pt link)
#else
void MP_PrintError(link)
  MP_Link_pt link;
#endif
{
    if (link->MP_errno >= 0 && link->MP_errno < MP_MaxError) {
        fprintf(stderr, "\nMP ERROR: %s\n", MP_errlist[link->MP_errno]);
        fflush(stderr);
    } else
        fprintf(stderr, "MP: Unknown error number %d\n", link->MP_errno);
}


#ifdef __STDC__
char* MP_ErrorStr(MP_Link_pt link)
#else
char* MP_ErrorStr(link)
  MP_Link_pt link;
#endif
{
  if (link->MP_errno >= 0 && link->MP_errno < MP_MaxError)
    return MP_errlist[link->MP_errno];
  else
    return "MP: Unknown error number";

}

#ifdef __STDC__
char* MP_StatusErrorStr(MP_Link_pt link, MP_Status_t status)
#else
char* MP_ErrorStr(link, status)
  MP_Link_pt link;
  MP_Status_t status;
#endif
{
  if (link->MP_errno != MP_Success && link->MP_errno != MP_Failure 
      && link->MP_errno < MP_MaxError && link->MP_errno >= 0)
    return MP_ErrorStr(link);
  if (status != MP_Success && status != MP_Failure && 
      status < MP_MaxError && status >= 0)
    return MP_errlist[status];
  
  if (status == MP_Failure || link->MP_errno == MP_Failure)
    return MP_errlist[MP_Failure];

  if (status == MP_Success && link->MP_errno == MP_Success)
    return MP_errlist[MP_Success];
  
  return "MP: Unknown Error number";
}




/***********************************************************************
 * FUNCTION:  IMP_GetCmdlineArg
 * INPUT:     argc - number of arguments in argv
 *            argv - arguments as strings
 *            cmd  - the cmdline option for which we seek an argument
 * OUTPUT:    Success: pointer to the option's argument
 *            Failure: NULL
 * OPERATION: Just iterate through argv until we run out of things to
 *            look at or find what we want.  We assume that there are,
 *            in fact, argc things in argv.
 ***********************************************************************/
#ifdef __STDC__
char* IMP_GetCmdlineArg(int argc,
                        char **argv,
                        char *cmd)
#else
char* IMP_GetCmdlineArg(argc, argv, cmd)
    int argc;
    char **argv;
    char *cmd;
#endif
{
    int i;
    
#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetCmdlineArg: entering\n");
    fflush(stderr);
#endif
    while (*cmd == '-') cmd++;

    for (i = 0; i < argc; i++)
        if (strstr(argv[i], cmd) != NULL  && (*(argv[i]) == '-'))
            if (i+1 == argc)
              return NULL;
            else
              return argv[i+1];

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetCmdlineArg: exiting\n");fflush(stderr);
#endif

    return NULL;
}



#ifdef __STDC__
char * IMP_TypeToString(MP_NodeType_t t)
#else
char * IMP_TypeToString(t)
    MP_NodeType_t t;
#endif
{
    switch (t) {
    case MP_Sint32Type:
        return "MP_Sint32";
    case MP_Uint32Type:
        return "MP_Uint32";
    case MP_Uint8Type:
        return "MP_Uint8";
    case MP_Sint8Type:
        return "MP_Sint8";
    case MP_ApIntType:
        return "MP_ApInt";
    case MP_ApRealType:
        return "MP_ApReal";
    case MP_BooleanType:
        return "MP_Boolean";
    case MP_Real32Type:
        return "MP_Real32";
    case MP_Real64Type:
        return "MP_Real64";
    case MP_IdentifierType:
        return "MP_Identifier";
    case MP_CommonGreekIdentifierType:
        return "MP_CommonGreekId";
    case MP_CommonLatinIdentifierType:
        return "MP_CommonLatinId";
    case MP_ConstantType:
        return "MP_Constant";
    case MP_CommonConstantType:
        return "MP_CommonConstant";
    case MP_StringType:
        return "MP_String";
    case MP_RawType:
        return "MP_Raw";
    case MP_MetaType:
        return "MP_MetaType";
    case MP_CommonMetaType:
        return "MP_CommonMeta";
    case MP_OperatorType:
        return "MP_Operator";
    case MP_CommonOperatorType:
        return "MP_CommonOp";
    case MP_MetaOperatorType:
        return "MP_MetaOperator";
    case MP_CommonMetaOperatorType:
        return "MP_CommonMetaOp";
    default:
        return "Unknown type";
    }
}



#ifdef __STDC__
void log_dicttype(MP_Link_pt     link,
                  char          *event,
                  char          *type_str,
                  MP_NodeType_t  type,
                  MP_NumAnnot_t  na,
                  MP_DictTag_t   dtag,
                  void          *data,
                  MP_NumChild_t  nc)
#else
void log_dicttype(link, event, type_str, type, na, dtag, data,nc)
    MP_Link_pt     link;
    char          *event;
    char          *type_str;
    MP_NodeType_t  type;
    MP_NumAnnot_t  na;
    MP_DictTag_t   dtag;
    void          *data;
    MP_NumChild_t  nc;
#endif
{
    char *msg;
    int   len = 3;

    if (type != MP_CommonOperatorType) {
        len = strlen((char *)data);
        if (len < 24) len = 24;
    }

    msg = (char *)IMP_MemAllocFnc(len + 62);

    switch (type) {
    case MP_OperatorType:
    case MP_MetaOperatorType:
      
        sprintf(msg, "%-12s  annots: %lu   dict: %lu   op: %s   args: %lu",
                type_str, na, dtag, (char *)data, nc);
        break;

    case MP_CommonOperatorType:
    case MP_CommonMetaOperatorType:
        sprintf(msg, "%-12s  annots: %lu   dict: %lu   op-tag: %u   args: %lu",
                type_str, na, dtag, (unsigned int)*((MP_Common_t *)data), nc);
        break;

    case MP_CommonConstantType:
    case MP_CommonMetaType:
        sprintf(msg, "%-12s  annots: %lu   dict: %lu   id-tag: %u",
                type_str, na, dtag, *((MP_Common_t *)data));
        break;

    default: ;
    }

    MP_LogEvent(link, event, msg);
    IMP_MemFreeFnc(msg, len+62);
}



#ifdef __STDC__
void log_fixnum(MP_Link_pt     link,
                char          *event,
                char          *type_str,
                int            type,
                MP_NumAnnot_t  na,
                void          *val)
#else
void log_fixnum(link, event, type_str, type, na, val)
    MP_Link_pt     link;
    char          *event;
    char          *type_str;
    int            type;
    MP_NumAnnot_t  na;
    void          *val;
#endif
{
    MP_Uint8_t t;

    switch (type) {
        case MP_BooleanType:
        case MP_Uint8Type:
        t = (MP_Uint8_t)*((MP_Uint8_t *)val);
        sprintf(fix_log_msg, "%-12s  annots: %lu   value: %u", type_str, na,t);
        break;

    case MP_Uint32Type:
        sprintf(fix_log_msg, "%-12s  value: %lu\n", type_str,
                (MP_Uint32_t)*((MP_Uint32_t *)val));
        break;

    case MP_Sint8Type:
        sprintf(fix_log_msg, "%-12s  annots: %lu   value: %d", type_str, na,
                *((MP_Sint8_t *)val));
        break;

    case MP_Sint32Type:
        sprintf(fix_log_msg, "%-12s  value: %ld", type_str,
                (MP_Sint32_t)*((MP_Sint32_t *)val));
        break;

    default: ;
    }

    MP_LogEvent(link, event, fix_log_msg);
}



#ifdef __STDC__
void log_fixreal(MP_Link_pt     link,
                 char          *event,
                 char          *type_str,
                 int            type,
                 MP_NumAnnot_t  na,
                 void          *val)
#else
void log_fixreal(link, event, type_str, type, na, val)
    MP_Link_pt     link;
    char          *event;
    char          *type_str;
    int            type;
    MP_NumAnnot_t  na;
    void          *val;
#endif
{
    switch (type) {
    case MP_Real32Type:
        sprintf(fix_log_msg, "%-12s  value: %-20.10G", type_str,
                *((MP_Real32_t *)val));
        break;

    case MP_Real64Type:
        sprintf(fix_log_msg, "%-12s  value: %-20.15G", type_str,
                *((MP_Real64_t *)val));
        break;

    }

    MP_LogEvent(link, event, fix_log_msg);
}



#ifdef __STDC__
void annot_flags_to_str(MP_AnnotFlags_t flags)
#else
void annot_flags_to_str(flags)
    MP_AnnotFlags_t flags;
#endif
{
    char *req, *scope, *valuated;

    valuated = (flags & MP_AnnotValuated)  ? "Arg"  : "NoArg";
    req      = (flags & MP_AnnotRequired)  ? "Req"  : "Sup";
    scope    = (flags & MP_AnnotTreeScope) ? "Node" : "Tree";

    sprintf(AnnotFlagsStr, "%s %s %s", req, valuated, scope);
}


