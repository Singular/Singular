/**************************************************************************
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
 *   IMPLEMENTATION FILE:  MP_Put.c 
 * 
 *        All the put routines.
 *                                                                
 *   Change Log: 
 *     May 30, 1995  SG - added MP_PutSint8Packet()
 *     June 5, 1995  SG - added MP_PutUint8Packet()
 *                      - changed IMP_PutOperator() to only output
 *                        the operator, as a string.  That is, it no
 *                        longer also sends a num_child field, since that 
 *                        job is now done by imp_put_node_header().
 *                        IMP_PutMpOperator() is handled in MP_Put.h
 *     July 6, 1995  SG - added MP_PutLatin/GreekCommonIdentifierPacket()
 *                      - added MP_PutCommonOperatorPacket()
 *                      - added MP_PutCommonConstantPacket()
 *     July 7, 1995  SG - added MP_PutBooleanPacket()
 *                      - changed MP_CantPut<type>Packet error values to
 *                        MP_CantPut<type>
 *                      - cleaned up some comments/unneeded code.
 *     10/18/95     OB  - changed PutApInt routines such that
 *                        only size field is communicated, not the alloc field
 *     12/1/95      SG  - fixed bug in routines which have to pad a value with
 *                        NULL bytes to get the right alignment
 *     2/28/96      SG  - Changed the NodeFlags fields to DictTag to reflect
 *                        the change in how we identify dictionaries and do
 *                        semantics.  Also changed IMP_PutNodeHeader() so
 *                        that it now takes arguments for all of the fields
 *                        in a packet header.  BUT - see comment with 
 *                        IMP_PutNodeHeader() regarding dictionary tags.
 *     4/29/96 sgray - Made the PutApInt() routines more generic.  They
 *                     now take a pointer to void.  So the sender can 
 *                     send in its "native" bignum format (provided we
 *                     have the supporting routines in place).  New formats 
 *                     can be added as needed without affecting the design.
 *     3/11/97 sgray - Fixed logging of ApInts as recommended by obachman.
 *                     Also cleaned up logging in the PutString and ApReal 
 *                     functions. 
 *                   - added Olaf's routines to put the MetaOperator and 
 *                     CommonMeta types.
 **************************************************************************
 *
 * xdr_float.c, Generic XDR routine implementation.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * These are the "floating point" xdr routines used to (de)serialize
 * most common data items.  See xdr.h for more info on the interface to
 * xdr.
 *
 *  The MP Library is distributed in the hope that it will be useful, 
 *  but without any warranty of any kind and without even the implied 
 *  warranty of merchantability or fitness for a particular purpose.
 *
 **************************************************************************/


#include "MP.h"


static char filler[4]={0, 0, 0, 0};

/*
 * these two defined in MP_Util.c
 */
EXTERN char fix_log_msg[];    
EXTERN char AnnotFlagsStr[];


#ifdef __STDC__
MP_Status_t MP_PutAnnotationPacket(MP_Link_pt      link,
                                   MP_DictTag_t    dtag,
                                   MP_AnnotType_t  atype,
                                   MP_AnnotFlags_t flags)
#else
MP_Status_t MP_PutAnnotationPacket(link, dtag, atype, flags)
    MP_Link_pt      link; 
    MP_DictTag_t    dtag;
    MP_AnnotType_t  atype;
    MP_AnnotFlags_t flags;
#endif
{
    MP_AnnotHeader_t annot = 0;
        
    MP_SetAnnotTypeField(annot, atype);
    MP_SetAnnotDictTagField(annot, dtag);
    MP_SetAnnotFlagsField(annot, flags);

    if (!IMP_PutLong(link, (long*)&annot))
        return MP_SetError(link, MP_CantPutAnnotationPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        annot_flags_to_str(flags);
	/* The following is acceptable only for now.  Eventually I
	   must clearly identify the MP dictionary range. */
        /* 3/28/97 - sgray - annotations are spread out through a variety
           of header files, so giving a string version of the annotation
           is not practical at this time.  Maybe have they are stable we
           can create function to do a (dtag, atype) mapping to a string. */
	/*       if (dtag == 0)
            sprintf(fix_log_msg, "%-12s  flags: %s  dict: %lu   ",
                    annotations[atype], AnnotFlagsStr, dtag);
        else
	*/
        sprintf(fix_log_msg, "AnnotPkt      dict: %lu   atype: %u   flags: %s",
                     dtag, atype, AnnotFlagsStr);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}

/* this is a private version of the IMP_PutNodeHeader() routine
 * IMPORTANT: At this time we have not determined how dictionary
 * tags beyond 255 should be allocated.  The code below make NO
 * check to see if dtag is 127 or 255 (the indicators telling us
 * the read in another 4 bytes with the true tag.  Once we know
 * how those tags will be disbursed, we can do the proper check
 * here.  To be clear.  The problem is that if the incoming dtag
 * is > 255, we can't yet identify it as MP-extended or user-extended,
 * so can't know whether to use 127 or 255.  It might be a good
 * idea to combine these into a single "extended tag" and just
 * divy up the 4 byte tags as well. This has to be fixed for
 * imp_get_node_header() as well.
*/
#ifdef __STDC__
static MP_Status_t imp_put_node_header(MP_Link_pt    link,
                                       MP_NodeType_t ntype,
                                       MP_DictTag_t  dtag,
                                       MP_Common_t   cval,
                                       MP_NumAnnot_t num_annots,
                                       MP_NumChild_t num_child)
#else
static MP_Status_t imp_put_node_header(link, ntype, dtag, cval, num_annots,
                                       num_child)
    MP_Link_pt    link;
    MP_NodeType_t ntype;
    MP_DictTag_t  dtag;
    MP_Common_t   cval;
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
#endif
{
    MP_NodeHeader_t hdr = 0;

#ifdef MP_DEBUG
    fprintf(stderr,
            "imp_put_node_header: entering - ntype = %d, num_child = %d\n",
            ntype, num_child);
#endif

    if (link == NULL)
        return MP_SetError(link, MP_NullLink);

    MP_SetNodeTypeField(hdr, ntype);
    MP_SetNodeCommonValueField(hdr, cval);
    MP_SetNodeDictTagField(hdr, dtag); /* see important note above */

    if (num_child != 0)
        if (num_child < MP_ExtraFieldIndicator)
            MP_SetNodeNumChildField(hdr, num_child);
        else
            MP_SetNodeExtraChildField(hdr);

    if (num_annots != 0)
        if (num_annots < MP_ExtraFieldIndicator)
            MP_SetNodeNumAnnotsField(hdr, num_annots);
        else
            MP_SetNodeExtraAnnotsField(hdr);

    if (!IMP_PutLong(link, (long *)&hdr))
        return MP_SetError(link, MP_CantPutNodeHeader);

    /* see if there is an extra number of children field to send */
    if (num_child >= MP_ExtraFieldIndicator
        && !IMP_PutLong(link, (long*)&num_child))
        return MP_SetError(link, MP_CantPutNodeHeader);

    /* see if there is an extra number of annotations field to send */
    if (num_annots >= MP_ExtraFieldIndicator
        && !IMP_PutLong(link, (long*)&num_annots))
        return MP_SetError(link, MP_CantPutNodeHeader);

#ifdef MP_DEBUG
    fprintf(stderr,
            "imp_put_node_header: exiting - hdr = 0x%X, \ttype = %d\tnum_child"
            " = %d\tnum_annot = %d\n",
            hdr, ntype, num_child, num_annots);
#endif

    return MP_ClearError(link);
}



/*
 * This is what the programmer sees through the API.  But we just call the
 * private version above, then do the logging (if enabled). 
 */
#ifdef __STDC__
MP_Status_t IMP_PutNodeHeader(MP_Link_pt    link,
                              MP_NodeType_t ntype,
                              MP_DictTag_t  dtag,
                              MP_Common_t   cval,
                              MP_NumAnnot_t num_annots,
                              MP_NumChild_t num_child)
#else
MP_Status_t IMP_PutNodeHeader(link, ntype, dtag, cval, num_annots, num_child)
    MP_Link_pt    link;
    MP_NodeType_t ntype;
    MP_DictTag_t  dtag;
    MP_Common_t   cval;
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
#endif
{
    imp_put_node_header(link, ntype, dtag, cval, num_annots, num_child);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg,
                "%-12s  %-12s  annots: %lu   args: %lu   dict: %lu   cval: %u",
                "NodePktHdr", IMP_TypeToString(ntype), num_annots,
                num_child, dtag, cval);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t IMP_PutSint32(MP_Link_pt  link,
                          MP_Sint32_t n)
#else
MP_Status_t IMP_PutSint32(link, n)
    MP_Link_pt  link;
    MP_Sint32_t n;
#endif
{
    if (!IMP_PutLong(link, &n))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Sint32_DP", MP_Sint32Type, 0, &n);
#endif

    return MP_ClearError(link);

}



#ifdef __STDC__
MP_Status_t IMP_PutUint32(MP_Link_pt  link,
                          MP_Uint32_t n)
#else
MP_Status_t IMP_PutUint32(link, n)
    MP_Link_pt  link;
    MP_Uint32_t n;
#endif
{
    if (!IMP_PutLong(link, (long*)&n))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Uint32_DP", MP_Uint32Type, 0, &n);
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t IMP_PutRaw(MP_Link_pt   link,
                       char        *rawdata,
                       MP_Uint32_t  len)
#else
MP_Status_t IMP_PutRaw(link, rawdata, len)
    MP_Link_pt   link;
    char        *rawdata;
    MP_Uint32_t  len;
#endif
{
    unsigned long extra;

    extra = len % MP_BytesPerMPUnit;
    extra = MP_BytesPerMPUnit - extra;

    if (!(IMP_PutLong(link, (long*)&len) && IMP_PutBytes(link, rawdata, len)))
        return MP_SetError(link, MP_CantPutDataPacket);

    if (extra != MP_BytesPerMPUnit     /* have to pad with extra null bytes */
        && !IMP_PutBytes(link, filler, extra))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg, "%-12s  #bytes: %lu  (data not shown)",
                "Raw_DP", len);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t IMP_PutString(MP_Link_pt  link,
                          char       *s)
#else
MP_Status_t IMP_PutString(link, s)
    MP_Link_pt  link;
    char       *s;
#endif
{
    short   extra;
    unsigned long  n;

    n = strlen(s);
    extra = n % MP_BytesPerMPUnit;
    extra = MP_BytesPerMPUnit - extra;

    if (! (IMP_PutLong(link, (long*)&n) && IMP_PutBytes(link, s, n)))
        return MP_SetError(link, MP_CantPutDataPacket);

    if (extra != MP_BytesPerMPUnit     /* have to pad with extra null byres */
        && !IMP_PutBytes(link, filler, extra))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        int len = strlen(s) + 36;
        char *tmp_msg = IMP_MemAllocFnc(len);
        sprintf(tmp_msg, "%-12s  value: %s", "String_DP", s);
        MP_LogEvent(link, MP_WRITE_EVENT, tmp_msg);
        IMP_MemFreeFnc(tmp_msg, len);
    }
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t common_put_string(MP_Link_pt link, char *s)
#else
MP_Status_t common_put_string(link, s)
    MP_Link_pt  link;
    char       *s;
#endif
{
    short   extra;
    unsigned long  n;

    n = strlen(s);
    extra = n % MP_BytesPerMPUnit;
    extra = MP_BytesPerMPUnit - extra;

    if (! (IMP_PutLong(link, (long*)&n) && IMP_PutBytes(link, s, n)))
        return MP_SetError(link, MP_CantPutDataPacket);

    if (extra != MP_BytesPerMPUnit     /* have to pad with extra null byres */
        && !IMP_PutBytes(link, filler, extra))
        return MP_SetError(link, MP_CantPutDataPacket);

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_PutConstantPacket(MP_Link_pt     link,
                                 MP_DictTag_t   dtag,
                                 char          *cnst,
                                 MP_NumAnnot_t  num_annots)
#else
MP_Status_t MP_PutConstantPacket(link, dtag, cnst, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t   dtag;
    char          *cnst;
    MP_NumAnnot_t  num_annots;
#endif
{
    if (! (imp_put_node_header(link, MP_ConstantType, dtag, 0, num_annots, 0)
           && common_put_string(link, cnst)))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg, "%-12s  annots: %lu   dict: %lu   value: %s",
                "Constant_NP", num_annots, dtag, cnst);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutCommonLatinIdentifierPacket(MP_Link_pt    link,
                                              MP_DictTag_t  dtag,
                                              MP_Common_t   id,
                                              MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutCommonLatinIdentifierPacket(link,  dtag, id, num_annots)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    MP_Common_t   id;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_CommonLatinIdentifierType, dtag, id, 
                             num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_dicttype(link, MP_WRITE_EVENT, "Latin Id_NP",
                     MP_CommonLatinIdentifierType, num_annots, dtag, &id, 0);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_PutCommonGreekIdentifierPacket(MP_Link_pt    link,
                                              MP_DictTag_t  dtag,
                                              MP_Common_t   id,
                                              MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutCommonGreekIdentifierPacket(link, dtag, id, num_annots)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    MP_Common_t   id;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_CommonGreekIdentifierType, dtag, id, 
                             num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_dicttype(link, MP_WRITE_EVENT, "Greek Id_NP",
                     MP_CommonGreekIdentifierType, num_annots, dtag, &id, 0);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutIdentifierPacket(MP_Link_pt     link,
                                   MP_DictTag_t   dtag,
                                   const char     *id,
                                   MP_NumAnnot_t  num_annots)
#else
MP_Status_t MP_PutIdentifierPacket(link, dtag, id, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t   dtag;
    char          *id;
    MP_NumAnnot_t  num_annots;
#endif
{
    if (!(imp_put_node_header(link, MP_IdentifierType, dtag, *id, num_annots, 0)
          && common_put_string(link, id)))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg, "%-12s  annots: %lu   dict: %lu   value: %s",
                "Ident_NP", num_annots, dtag, id);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_PutStringBasedTypePacket(MP_Link_pt     link,
                                         char          *s,
                                         MP_NumAnnot_t  num_annots,
                                         MP_NodeType_t  ntype)
#else
MP_Status_t IMP_PutStringBasedTypePacket(link, s, num_annots, ntype)
    MP_Link_pt     link;
    char          *s;
    MP_NumAnnot_t  num_annots;
    MP_NodeType_t  ntype;
#endif
{
    /*
     * generic put routine for all string-based types
     */

#ifndef NO_LOGGING
    char *thetype = NULL;  /* for logging only */

    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        if (ntype == MP_StringType)
	        thetype = "String_NP";
        else if (ntype == MP_MetaType)
		    thetype = "Meta_NP";

        sprintf(fix_log_msg, "%-12s  %4lu                   %-24s", thetype,
                num_annots, s);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    if (!imp_put_node_header(link, ntype, 0, 0, num_annots, 0))
        return MP_Failure;

    if (!common_put_string(link, s))
        return MP_SetError(link, MP_CantPutNodePacket);

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t MP_PutStringPacket(MP_Link_pt     link,
                               char          *s,
                               MP_NumAnnot_t  num_annots)
#else
MP_Status_t MP_PutStringPacket(link, s, num_annots)
    MP_Link_pt     link;
    char          *s;
    MP_NumAnnot_t  num_annots;
#endif
{
    if (!(imp_put_node_header(link, MP_StringType, 0, 0, num_annots, 0)
          && common_put_string(link, s)))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        int len = strlen(s) + 36;
        char *tmp_msg = IMP_MemAllocFnc(len);
        sprintf(tmp_msg, "%-12s  annots: %lu   value: %s", "String_NP",
                    num_annots, s);
        MP_LogEvent(link, MP_WRITE_EVENT, tmp_msg);
        IMP_MemFreeFnc(tmp_msg, len);
    }
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t MP_PutMetaTypePacket(MP_Link_pt     link,
                                 MP_DictTag_t  dtag,
                                 char          *s,
                                 MP_NumAnnot_t  num_annots)
#else
MP_Status_t MP_PutMetaTypePacket(link, dtag, s, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t   dtag;
    char          *s;
    MP_NumAnnot_t  num_annots;
#endif
{
    if (!(imp_put_node_header(link, MP_MetaType, dtag, 0, num_annots, 0)
          && common_put_string(link, s)))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg, "%-12s  annots: %lu   dict: %lu   value: %s",
                "MetaType_NP", num_annots, dtag, s);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t MP_PutCommonOperatorPacket(MP_Link_pt    link,
                                       MP_DictTag_t  dtag,
                                       MP_Common_t   op,
                                       MP_NumAnnot_t num_annots,
                                       MP_NumChild_t num_child)
#else
MP_Status_t MP_PutCommonOperatorPacket(link, dtag, op, num_annots, num_child)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    MP_Common_t   op;
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
#endif
{
    if (!imp_put_node_header(link, MP_CommonOperatorType, dtag, op, num_annots,
                             num_child))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        log_dicttype(link, MP_WRITE_EVENT, "Common Op_NP",
                     MP_CommonOperatorType,num_annots, dtag, &op, num_child);
    }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutCommonMetaOperatorPacket(MP_Link_pt    link,
                                           MP_DictTag_t  dtag,
                                           MP_Common_t   op,
                                           MP_NumAnnot_t num_annots,
                                           MP_NumChild_t num_child)
#else
MP_Status_t MP_PutCommonMetaOperatorPacket(link, dtag, op, num_annots,
                                           num_child)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    MP_Common_t   op;
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
#endif
{
    if (!imp_put_node_header(link, MP_CommonMetaOperatorType, dtag, op, 
                             num_annots, num_child))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        log_dicttype(link, MP_WRITE_EVENT, "Common MetaOp_NP",
                     MP_CommonMetaOperatorType,num_annots, dtag, &op, num_child);
    }
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t MP_PutCommonMetaTypePacket(MP_Link_pt    link,
                                   MP_DictTag_t  dtag,
                                   MP_Common_t   cm,
                                   MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutCommonMetaTypePacket(link, dtag, cm, num_annots)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    MP_Common_t   cm;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_CommonMetaType,  dtag, cm,num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_dicttype(link, MP_WRITE_EVENT, "Common Meta_NP", MP_CommonMetaType,
                     num_annots, dtag, &cm, 0);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_PutCommonConstantPacket(MP_Link_pt    link,
                                       MP_DictTag_t  dtag,
                                       MP_Common_t   cnst,
                                       MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutCommonConstantPacket(link, dtag, cnst, num_annots)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    MP_Common_t   cnst;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_CommonConstantType, dtag, cnst, 
                             num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_dicttype(link, MP_WRITE_EVENT, "Common Cnst_NP",
                     MP_CommonConstantType, num_annots, dtag, &cnst, 0);
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t MP_PutOperatorPacket(MP_Link_pt     link,
                                 MP_DictTag_t   dtag,
                                 char          *op,
                                 MP_NumAnnot_t  num_annots,
                                 MP_NumChild_t  num_child)
#else
MP_Status_t MP_PutOperatorPacket(link, dtag, op, num_annots, num_child)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    char         *op;
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
#endif
{
    if (!(imp_put_node_header(link, MP_OperatorType, dtag, 0, num_annots,
                              num_child) && common_put_string(link, op)))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_dicttype(link, MP_WRITE_EVENT, "Operator_NP", MP_OperatorType,
                     num_annots, dtag, op, num_child);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutMetaOperatorPacket(MP_Link_pt     link,
                                     MP_DictTag_t   dtag,
                                     char          *op,
                                     MP_NumAnnot_t  num_annots,
                                     MP_NumChild_t  num_child)
#else
MP_Status_t MP_PutMetaOperatorPacket(link, dtag, op, num_annots, num_child)
    MP_Link_pt    link;
    MP_DictTag_t  dtag;
    char         *op;
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
#endif
{
    if (!(imp_put_node_header(link, MP_MetaOperatorType, dtag, 0, num_annots,
                              num_child) && common_put_string(link, op)))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_dicttype(link, MP_WRITE_EVENT, "MetaOperator_NP", MP_MetaOperatorType,
                     num_annots, dtag, op, num_child);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_PutOperator(MP_Link_pt link, char *op)
#else
MP_Status_t IMP_PutOperator(link, op)
    MP_Link_pt  link;
    char       *op;
#endif
{
    if (!common_put_string(link, op))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg, "%-12s  op: %s", "Operator_DP", op);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_PutMetaOperator(MP_Link_pt link, char *op)
#else
MP_Status_t IMP_PutMetaOperator(link, op)
    MP_Link_pt  link;
    char       *op;
#endif
{
    if (!common_put_string(link, op))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg, "%-12s  op: %s", "MetaOperator_DP", op);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_PutApInt(MP_Link_pt link, MP_ApInt_t apint)
#else
MP_Status_t IMP_PutApInt(link, apint)
    MP_Link_pt  link;
    MP_ApInt_t apint;
#endif
{
#ifndef NO_LOGGING
  if (link->logmask & MP_LOG_WRITE_EVENTS)
  {
    int len;
    char *msg = NULL;
    len = link->bignum.bigint_ops->bigint_ascii_size(apint) + 25;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  value: ", "ApInt_DP");
    link->bignum.bigint_ops->bigint_to_str(apint, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_WRITE_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
  }
#endif

  if (link->bignum.bigint_ops->put_bigint(link, apint) != MP_Success)
    return MP_SetError(link, MP_CantPutDataPacket);

  return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutApIntPacket(MP_Link_pt     link,
                              MP_ApInt_t     apint,
                              MP_NumAnnot_t  num_annots)
#else
MP_Status_t MP_PutApIntPacket(link, apint, num_annots)
    MP_Link_pt     link;
    MP_ApInt_t     apint;
    MP_NumAnnot_t  num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_ApIntType, 0, 0, num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

    if (link->bignum.bigint_ops->put_bigint(link, apint) != MP_Success)
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
  if (link->logmask & MP_LOG_WRITE_EVENTS)
  {
    int len;
    char *msg = NULL;
    len = link->bignum.bigint_ops->bigint_ascii_size(apint) + 42;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  annots: %lu   value: ", "ApInt_NP", num_annots);
    link->bignum.bigint_ops->bigint_to_str(apint, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_WRITE_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
  }
#endif
    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_PutApReal(MP_Link_pt link, MP_ApReal_t apreal)
#else
MP_Status_t IMP_PutApReal(link, apreal)
  MP_Link_pt   link;
  MP_ApReal_t apreal;
#endif
{
#ifndef NO_LOGGING
  if (link->logmask & MP_LOG_WRITE_EVENTS)
  {
    int len;
    char *msg = NULL;
    len = link->bignum.bigreal_ops->bigreal_ascii_size(apreal) + 25;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  value: ", "ApReal_DP");
    link->bignum.bigreal_ops->bigreal_to_str(apreal, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_WRITE_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
  }
#endif

    if (link->bignum.bigreal_ops->put_bigreal(link, apreal) != MP_Success)
        return MP_SetError(link, MP_CantPutDataPacket);

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_PutApRealPacket(MP_Link_pt    link,
                               MP_ApReal_t  apreal,
                               MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutApRealPacket(link, apreal, num_annots)
    MP_Link_pt    link;
    MP_ApReal_t  apreal;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_ApRealType, 0, 0, num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

    if (link->bignum.bigreal_ops->put_bigreal(link, apreal) != MP_Success)
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
  if (link->logmask & MP_LOG_WRITE_EVENTS)
  {
    int len;
    char *msg = NULL;
    len = link->bignum.bigreal_ops->bigreal_ascii_size(apreal) + 45;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  annots: %lu   value: ", "ApReal_NP", num_annots);
    link->bignum.bigreal_ops->bigreal_to_str(apreal, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_WRITE_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
  }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutBooleanPacket(MP_Link_pt    link,
                                MP_Boolean_t  mbool,
                                MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutBooleanPacket(link, mbool, num_annots)
    MP_Link_pt    link;
    MP_Boolean_t  mbool;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_BooleanType, 0, mbool, num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Boolean_NP", MP_BooleanType,
                   num_annots, &mbool);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_PutBoolean(MP_Link_pt link, MP_Boolean_t mbool)
#else
MP_Status_t IMP_PutBoolean(link, mbool)
    MP_Link_pt   link;
    MP_Boolean_t mbool;
#endif
{
    if (!imp_put_node_header(link, MP_BooleanType, 0, mbool, 0, 0))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Boolean_DP", MP_BooleanType, 0,
                   &mbool);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutSint8Packet(MP_Link_pt    link,
                              MP_Sint8_t    n,
                              MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutSint8Packet(link, n, num_annots)
    MP_Link_pt    link;
    MP_Sint8_t    n;
    MP_NumAnnot_t num_annots;
#endif
{
    if (!imp_put_node_header(link, MP_Sint8Type, 0, n, num_annots, 0))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Sint8_NP", MP_Sint8Type,
                   num_annots, &n);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutUint8Packet(MP_Link_pt    link,
                              MP_Uint8_t    n,
                              MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutUint8Packet(link, n, num_annots)
    MP_Link_pt    link;
    MP_Uint8_t    n;
    MP_NumAnnot_t num_annots;
#endif
{
  /* need the following to make the HPs happy, otherwise it treats the
     incoming value as an unsigned integer (2 - 4 bytes) 
  */
    unsigned char    x = (unsigned char) n;

    if (!imp_put_node_header(link, MP_Uint8Type, 0, x, num_annots, 0))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Uint8_NP", MP_Uint8Type,
                   num_annots, &x);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_PutSint8(MP_Link_pt link, MP_Sint8_t n)
#else
MP_Status_t IMP_PutSint8(link, n)
    MP_Link_pt link;
    MP_Sint8_t n;
#endif
{
    if (!imp_put_node_header(link, MP_Sint8Type, 0, n, 0, 0))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Sint8_DP", MP_Sint8Type, 0, &n);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_PutUint8(MP_Link_pt link,
                         MP_Uint8_t n)
#else
MP_Status_t IMP_PutUint8(link, n)
    MP_Link_pt link;
    MP_Uint8_t n;
#endif
{
  /* need the following to make the HPs happy, otherwise it treats the
     incoming value as an unsigned integer (2 - 4 bytes) 
  */
    unsigned char    x = (unsigned char) n;
    if (!imp_put_node_header(link, MP_Uint8Type, 0, x, 0, 0))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT, "Uint8_DP", MP_Uint8Type, 0, &x);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutSint32Packet(MP_Link_pt    link,
                               MP_Sint32_t   n,
                               MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutSint32Packet(link, n, num_annots)
  MP_Link_pt    link;
  MP_Sint32_t   n;
  MP_NumAnnot_t num_annots;
#endif
{
    if (! (imp_put_node_header(link, MP_Sint32Type, 0, 0, num_annots, 0)
           && IMP_PutLong(link, &n)))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT,"Sint32_NP",MP_Sint32Type,
                   num_annots, &n);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutUint32Packet(MP_Link_pt    link,
                               MP_Uint32_t   n,
                               MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutUint32Packet(link, n, num_annots)
  MP_Link_pt    link;
  MP_Uint32_t   n;
  MP_NumAnnot_t num_annots;
#endif
{
    if (! (imp_put_node_header(link, MP_Uint32Type, 0, 0, num_annots, 0)
           && IMP_PutLong(link, (long*)&n)))
        return MP_SetError(link, MP_CantPutDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixnum(link, MP_WRITE_EVENT,"Uint32_NP",MP_Uint32Type,
                   num_annots, &n);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_PutRawPacket(MP_Link_pt    link,
                            char         *data,
                            MP_Uint32_t   len,
                            MP_NumAnnot_t num_annots)
#else
MP_Status_t MP_PutRawPacket(link, data, len, num_annots)
    MP_Link_pt    link;
    char         *data;
    MP_Uint32_t   len;
    MP_NumAnnot_t num_annots;
#endif
{
    short extra;

    if (!imp_put_node_header(link, MP_RawType, 0, 0, num_annots, 0))
        return MP_Failure;

    extra = len % MP_BytesPerMPUnit;
    extra = MP_BytesPerMPUnit - extra;

    if (! (IMP_PutLong(link, (long*)&len) && IMP_PutBytes(link, data, len)))
        return MP_SetError(link, MP_CantPutNodePacket);

    if (extra != MP_BytesPerMPUnit     /* have to pad with extra null bytes */
        && !IMP_PutBytes(link, filler, extra))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS) {
        sprintf(fix_log_msg,
                "%-12s  annots: %lu   #bytes: %lu  (data not shown)",
                "Raw_NP", num_annots, len);
        MP_LogEvent(link, MP_WRITE_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


/*
 * NB: Not portable.
 * This routine works on Suns (Sky / 68000's) and Vaxen.
 */
#ifdef vax
/* What IEEE single precision floating point looks like on a Vax */
struct  ieee_single {
        unsigned int        mantissa : 23;
        unsigned int        exp      :  8;
        unsigned int        sign     :  1;
};

/* Vax single precision floating point */
struct  vax_single {
        unsigned int        mantissa1 :  7;
        unsigned int        exp       :  8;
        unsigned int        sign      :  1;
        unsigned int        mantissa2 : 16;

};

#define VAX_SNG_BIAS    0x81
#define IEEE_SNG_BIAS   0x7f

static struct sgl_limits {
        struct vax_single  s;
        struct ieee_single ieee;
} sgl_limits[2] = {
        {{ 0x3f, 0xff, 0x0, 0xffff },         /* Max Vax */
        {  0x0,  0xff, 0x0 }},                /* Max IEEE */
        {{ 0x0,   0x0, 0x0, 0x0 },            /* Min Vax */
        {  0x0,   0x0, 0x0 }}                 /* Min IEEE */
};

#endif



#ifdef __STDC__
MP_Status_t IMP_PutReal32(MP_Link_pt link, MP_Real32_t n)
#else
MP_Status_t IMP_PutReal32(link, n)
    MP_Link_pt  link;
    MP_Real32_t n;
#endif
{
    float  x  = n;
    float *fp = &x;
#ifdef vax
    struct ieee_single  is;
    struct vax_single   vs, *vsp;
    struct sgl_limits  *lim;
#endif

#ifdef MP_DEBUGGING
    fprintf(stderr, "IMP_PutReal32: entering, fp = %-20.10G\n", *fp);
#endif

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixreal(link, MP_WRITE_EVENT, "Real32_DP", MP_Real32Type, 0, fp);
#endif

#ifndef vax
    return IMP_PutLong(link, (long *)fp);
#else /* not vax */
    vs = *((struct vax_single *)fp);
    for (i = 0, lim = sgl_limits;
         i < sizeof(sgl_limits)/sizeof(struct sgl_limits);  i++, lim++)
        if (vs.mantissa2 == lim->s.mantissa2
            && vs.exp == lim->s.exp
            && vs.mantissa1 == lim->s.mantissa1) {
            is = lim->ieee;
            goto shipit;
        }

    is.exp = vs.exp - VAX_SNG_BIAS + IEEE_SNG_BIAS;
    is.mantissa = (vs.mantissa1 << 16) | vs.mantissa2;

shipit:
    is.sign = vs.sign;
    return IMP_PutLong(link, (long *)&is);
#endif /* not vax */
}



/*
 * This routine works on Suns (Sky / 68000's) and Vaxen.
 */

#ifdef vax

/* What IEEE double precision floating point looks like on a Vax */
struct  ieee_double {
        unsigned int        mantissa1 : 20;
        unsigned int        exp       : 11;
        unsigned int        sign      :  1;
        unsigned int        mantissa2 : 32;
};

/* Vax double precision floating point */
struct  vax_double {
        unsigned int        mantissa1 :  7;
        unsigned int        exp       :  8;
        unsigned int        sign      :  1;
        unsigned int        mantissa2 : 16;
        unsigned int        mantissa3 : 16;
        unsigned int        mantissa4 : 16;
};

#define VAX_DBL_BIAS    0x81
#define IEEE_DBL_BIAS   0x3ff
#define MASK(nbits)     ((1 << nbits) - 1)

static struct dbl_limits {
        struct  vax_double  d;
        struct  ieee_double ieee;
} dbl_limits[2] = {
        {{ 0x7f, 0xff, 0x0, 0xffff, 0xffff, 0xffff },    /* Max Vax */
         { 0x0, 0x7ff, 0x0, 0x0 }},                      /* Max IEEE */
        {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},                /* Min Vax */
         { 0x0, 0x0, 0x0, 0x0 }}                         /* Min IEEE */
};

#endif  /* vax */


#ifdef __STDC__
MP_Status_t IMP_PutReal64(MP_Link_pt link, MP_Real64_t n)
#else
MP_Status_t IMP_PutReal64(link, n)
    register MP_Link_pt link;
    MP_Real64_t         n;
#endif
{
    register long   *lp;
    register double *dp = &n;

#ifdef vax
    struct   ieee_double id;
    struct   vax_double  vd;
    register struct dbl_limits *lim;
#endif

#ifdef MP_DEBUGGING
    fprintf(stderr, "IMP_PutReal64: entering, dp = %-20.10G\n", *dp);
#endif

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixreal(link, MP_WRITE_EVENT, "Real64_DP", MP_Real64Type, 0, dp);
#endif

#ifndef vax
    lp = (long *)dp;
#else /* not vax */
    vd = *((struct  vax_double *)dp);
    for (i = 0, lim = dbl_limits;
         i < sizeof(dbl_limits)/sizeof(struct dbl_limits); i++, lim++)
        if (vd.mantissa4 == lim->d.mantissa4
            && vd.mantissa3 == lim->d.mantissa3
            && vd.mantissa2 == lim->d.mantissa2
            && vd.mantissa1 == lim->d.mantissa1
            && vd.exp == lim->d.exp) {
            id = lim->ieee;
            goto shipit;
        }

    id.exp = vd.exp - VAX_DBL_BIAS + IEEE_DBL_BIAS;
    id.mantissa1 = (vd.mantissa1 << 13) | (vd.mantissa2 >> 3);
    id.mantissa2 = ((vd.mantissa2 & MASK(3)) << 29)
                    | (vd.mantissa3 << 13) | ((vd.mantissa4 >> 3) & MASK(13));

shipit:
    id.sign = vd.sign;
    lp = (long *)&id;
#endif /* not vax */

#ifdef WORDS_BIGENDIAN
    return IMP_PutLong(link, lp++) && IMP_PutLong(link, lp);
#else /* not WORDS_BIGENDIAN */
    return IMP_PutLong(link, lp+1) && IMP_PutLong(link, lp);
#endif /* not WORDS_BIGENDIAN */
}



#ifdef __STDC__
MP_Status_t MP_PutReal32Packet(MP_Link_pt    link,
                               MP_Real32_t   n,
                               MP_NumAnnot_t num_annots
)
#else
MP_Status_t MP_PutReal32Packet(link, n,  num_annots)
  MP_Link_pt    link;
  MP_Real32_t   n;
  MP_NumAnnot_t num_annots;
#endif
{
#ifdef vax
  struct ieee_single  is;
  struct vax_single   vs, *vsp;
  struct sgl_limits  *lim;
#endif
  float  x  = n;
  float *fp = &x;

#ifndef NO_LOGGING
  if (link->logmask & MP_LOG_WRITE_EVENTS)
      log_fixreal(link, MP_WRITE_EVENT,
                  "Real32_NP", MP_Real32Type, num_annots, fp);
#endif

  if (!imp_put_node_header(link, MP_Real32Type, 0, 0, num_annots, 0))
      return MP_SetError(link, MP_CantPutNodePacket);

#ifndef vax
    return IMP_PutLong(link, (long *)fp);
#else /* not vax */
    vs = *((struct vax_single *)fp);
    for (i = 0, lim = sgl_limits;
         i < sizeof(sgl_limits)/sizeof(struct sgl_limits); i++, lim++)
        if (vs.mantissa2 == lim->s.mantissa2
            && vs.exp == lim->s.exp
            && vs.mantissa1 == lim->s.mantissa1) {
            is = lim->ieee;
            goto shipit;
        }

    is.exp = vs.exp - VAX_SNG_BIAS + IEEE_SNG_BIAS;
    is.mantissa = (vs.mantissa1 << 16) | vs.mantissa2;

shipit:
    is.sign = vs.sign;
    return IMP_PutLong(link, (long *)&is);
#endif /* not vax */
}


#ifdef __STDC__
MP_Status_t MP_PutReal64Packet(MP_Link_pt     link,
                               MP_Real64_t    n,
                               MP_NumAnnot_t  num_annots
)
#else
MP_Status_t MP_PutReal64Packet(link, n, num_annots)
  MP_Link_pt    link;
  MP_Real64_t   n;
  MP_NumAnnot_t num_annots;
#endif
{
    register long   *lp;
    register double *dp = &n;

#ifdef vax
    struct  ieee_double id;
    struct  vax_double  vd;
    register struct dbl_limits *lim;
#endif /* vax */

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_WRITE_EVENTS)
        log_fixreal(link, MP_WRITE_EVENT,
                    "Real64_NP", MP_Real64Type, num_annots, dp);
#endif

    if (!imp_put_node_header(link, MP_Real64Type, 0, 0, num_annots, 0))
        return MP_SetError(link, MP_CantPutNodePacket);

#ifndef vax
    lp = (long *)dp;
#else /* not vax */
    vd = *((struct  vax_double *)dp);
    for (i = 0, lim = dbl_limits;
         i < sizeof(dbl_limits)/sizeof(struct dbl_limits); i++, lim++)
        if (vd.mantissa4 == lim->d.mantissa4
            && vd.mantissa3 == lim->d.mantissa3
            && vd.mantissa2 == lim->d.mantissa2
            && vd.mantissa1 == lim->d.mantissa1
            && vd.exp == lim->d.exp) {
            id = lim->ieee;
            goto shipit;
        }

    id.exp = vd.exp - VAX_DBL_BIAS + IEEE_DBL_BIAS;
    id.mantissa1 = (vd.mantissa1 << 13) | (vd.mantissa2 >> 3);
    id.mantissa2 = ((vd.mantissa2 & MASK(3)) << 29)
                    | (vd.mantissa3 << 13) | ((vd.mantissa4 >> 3) & MASK(13));

shipit:
    id.sign = vd.sign;
    lp = (long *)&id;
#endif /* not vax */

#ifdef WORDS_BIGENDIAN
    return IMP_PutLong(link, lp++) && IMP_PutLong(link, lp);
#else /* WORDS_BIGENDIAN */
    return IMP_PutLong(link, lp+1) && IMP_PutLong(link, lp);
#endif /* WORDS_BIGENDIAN */
}


