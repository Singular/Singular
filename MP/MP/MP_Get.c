/**********************************************************************
 *                                                                  
 *                    MP version 1.1.2  Multi Protocol
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
 *   IMPLEMENTATION FILE:  MP_Get.c - All the get routines.
 *
 *  Change Log: 
 *     May 30, 1996  SG - added MP_GetSint8Packet().
 *     June 5, 1995  SG - added MP_GetUint8Packet()
 *                      - changed IMP_GetOperator() to only output
 *                        the operator, as a string.  That is, it no
 *                        longer also reads a num_child field, since that 
 *                        job is now done by imp_get_node_header().
 *                        IMP_GetMpOperator() is handled in MP_Get.h
 *     July 7, 1995  SG - added MP_GetBooleanPacket(),
 *                        MP_GetCommonConstantPacket(),
 *                        MP_GetCommonOperatorPacket(),
 *                        and updated the error constants to reflect
 *                        the changes to MP_Error .c and .h
 *     October 6, 1995 SG - changed imp_get_node_header() to return MP_Failure
 *                          if it cannot properly retrieve the 4 byte 
 *                          header.  MP_Failure is set to 0.  We must
 *                          never define an MP type with an integer 
 *                          representation of 0 or we will be unable do
 *                          distinguish between success and failure here.
 *     10/18/95     OB  - changed GetApInt routines such that
 *                        only size field is communicated, not the alloc field
 *     10/16/95     OB  - changed memory allocation function for ApInt's to
 *                        _mpz_realloc
 *     12/1/95      SG  - fixed bug in routines which have to handle values 
 *                        padded with NULL bytes to get the right alignment
 *     2/28/96      SG  - Changed the NodeFlags fields to DictTag to reflect
 *                        the change in how we identify dictionaries and do
 *                        semantics.Also changed IMP_GetNodeHeader() so
 *                        that it now takes arguments for all of the fields
 *                        in a packet header.  BUT - see comment with 
 *                        IMP_GetNodeHeader() regarding dictionary tags.
 *     4/29/96 sgray - Made the GetApInt() routines more generic.  They
 *                     now take a pointer to void.  So the sender can 
 *                     send in its "native" bignum format (provided we
 *                     have the supporting routines in place).  New formats 
 *                     can be added as needed without affecting the design.
 *     3/11/97 sgray - Fixed logging of ApInts as recommended by obachman.
 *                     Also cleaned up logging in the GetString and ApReal 
 *                     functions. 
 *
 **************************************************************************/

#include "MP.h"


/* 
 * these two defined in MP_Util.c 
 */
EXTERN char fix_log_msg[];
EXTERN char AnnotFlagsStr[];

static char junk[4];        /* for reading filler bytes */

/*
 * this is a private version of the IMP_GetNodeHeader() routine
 * IMPORTANT: At this time we have not determined how dictionary
 * tags beyond 255 should be allocated.  The code below make NO 
 * check to see if dtag is 127 or 255 (the indicators telling us
 * the read in another 4 bytes with the true tag.  Once we know
 * how those tags will be disbursed, we can do the proper check 
 * here.  To be clear.  The problem is that if the incoming dtag
 * is > 255, we can't yet identify it as MP-extended or user-extended,
 * so can't know whether to use 127 or 255.  It might be a good
 * idea to combine these into a single "extended tag" and just
 * divvy up the 4 byte tags as well.  This has to be fixed for 
 * imp_get_node_header() as well.
 */

#ifdef __STDC__
static MP_Status_t imp_get_node_header(MP_Link_pt     link,
                                       MP_NodeType_t *ntype,
                                       MP_DictTag_t  *dtag,
                                       MP_Common_t   *cv,
                                       MP_NumAnnot_t *num_annots,
                                       MP_NumChild_t *num_child)
#else
static MP_Status_t imp_get_node_header(link, ntype, dtag, cv, 
                                       num_annots, num_child)
  MP_Link_pt     link;
  MP_NodeType_t *ntype;
  MP_DictTag_t  *dtag;
  MP_Common_t   *cv;
  MP_NumAnnot_t *num_annots;
  MP_NumChild_t *num_child;
#endif
{
    MP_NodeHeader_t hdr;
    MP_NodeType_t   ntype_requested = *ntype;

#ifdef MP_DEBUG
    fprintf(stderr, "imp_get_node_header: entering for link %d\n",
            link->link_id);
#endif

    if(!IMP_GetLong(link, (long*)&hdr)) {
        MP_SetError(link, MP_CantGetNodeHeader);
        return MP_Failure;
    }

    *ntype = MP_GetNodeFieldType(hdr);
    if (ntype_requested != *ntype && ntype_requested != MP_ANY_TYPE) {
        IMP_Restore(link, sizeof(hdr));
        sprintf(fix_log_msg,"Wrong packet type. requested %s, received %s",
                IMP_TypeToString(ntype_requested), IMP_TypeToString(*ntype));
        MP_LogEvent(link, MP_ERROR_EVENT, fix_log_msg);
        return MP_SetError(link, MP_CantGetNodePacket);
    }

    *num_annots = MP_GetNodeNumAnnotsField(hdr);
    *num_child  = MP_GetNodeNumChildField(hdr);
    *cv         = MP_GetNodeCommonValueField(hdr);
    *dtag       = MP_GetNodeDictTagField(hdr);

    if (*num_annots == MP_ExtraFieldIndicator
        && !IMP_GetLong(link, (long*)num_annots))
        return MP_SetError(link, MP_CantGetNodeHeader);

    if (*num_child == MP_ExtraFieldIndicator
        && !IMP_GetLong(link, (long*)num_child))
        return MP_SetError(link, MP_CantGetNodeHeader);

#ifdef MP_DEBUG
    fprintf(stderr,
            "imp_get_node_header: exiting - hdr = 0x%X, "
            "\ttype = %d\tnum_child = %d\tnum_annot = %d\n",
            hdr, *ntype, *num_child, *num_annots);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetNodeHeader(MP_Link_pt     link,
                              MP_NodeType_t *ntype,
                              MP_DictTag_t  *dtag,
                              MP_Common_t   *cv,
                              MP_NumAnnot_t *num_annots,
                              MP_NumChild_t *num_child)
#else
MP_Status_t IMP_GetNodeHeader(link, ntype, dtag, cv, num_annots, num_child)
    MP_Link_pt     link;
    MP_NodeType_t *ntype;
    MP_DictTag_t  *dtag;
    MP_Common_t   *cv;
    MP_NumAnnot_t *num_annots;
    MP_NumChild_t *num_child;
#endif
{
    unsigned char t;
    MP_Status_t status;
#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetNodeHeader: entering for link %d\n", link->link_id);
#endif
    /*
     * For now we don't let the programmer in on this little secret (unless,
     * of course, you are looking at the code :-0).  Make sure that the
     * internal get node header routine doesn't gag.
     */

    *ntype = MP_ANY_TYPE;
    status = imp_get_node_header(link, ntype, dtag, cv, num_annots, num_child);
    
#ifdef MP_DEBUG
    fprintf(stderr,
            "IMP_GetNodeHeader: exiting\ttype = %d"
            "\tnum_child = %d\tnum_annot = %d\n",
            *ntype, *num_child, *num_annots);
#endif

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
	/* the following is a pain, but we have to do it for HPs so the output
	   is displayed correctly *//*
	if (*ntype == MP_Uint8Type){
	  t = (MP_Uint8_t)*cv;
	  sprintf(fix_log_msg,
	        "%-12s  %-11s  annots: %lu   args: %lu   dict: %lu   cval: %u",
	        "NodePktHdr", IMP_TypeToString(*ntype),
	        *num_annots, *num_child, *dtag, t);
	   }
	else */
	if (*ntype == MP_Sint8Type){
	  sprintf(fix_log_msg,
	        "%-12s  %-11s  annots: %lu   args: %lu   dict: %lu   cval: %d",
	        "NodePktHdr", IMP_TypeToString(*ntype),
	        *num_annots, *num_child, *dtag, t);
	   }
        else {
	  t = (MP_Uint8_t)*cv;
          sprintf(fix_log_msg,
	         "%-12s  %-11s  annots: %lu   args: %lu   dict: %lu   cval: %u",
		 "NodePktHdr", IMP_TypeToString(*ntype),
		 *num_annots, *num_child, *dtag, *cv);
	}
     MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
     }
#endif
    if (status == MP_Success)
      return MP_ClearError(link);
    else return MP_Failure;
}



#ifdef __STDC__
MP_Status_t IMP_GetSint32(MP_Link_pt link, MP_Sint32_t *n)
#else
MP_Status_t IMP_GetSint32(link, n)
    MP_Link_pt   link;
    MP_Sint32_t *n;
#endif
{
    if (!IMP_GetLong(link, n))
        return MP_SetError(link, MP_CantGetDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Sint32_DP", MP_Sint32Type, 0, n);
#endif
      return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetUint32(MP_Link_pt link, MP_Uint32_t *n)
#else
IMP_GetUint32(link, n)
    MP_Link_pt   link;
    MP_Uint32_t *n;
#endif
{
    if (!IMP_GetLong(link, (long*)n))
        return MP_SetError(link, MP_CantGetDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Uint32_DP", MP_Uint32Type, 0, n);
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t IMP_GetOperator(MP_Link_pt link, char **op)
#else
MP_Status_t IMP_GetOperator(link, op)
    MP_Link_pt   link;
    char       **op;
#endif
{
    if (!IMP_GetStringBasedType(link, op))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, "%-12s  op: %s", "Operator_DP", *op);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetRaw(MP_Link_pt    link,
                       char        **rawdata,
                       MP_Uint32_t  *len)
#else
MP_Status_t IMP_GetRaw(link, rawdata, len)
    MP_Link_pt    link;
    char        **rawdata;
    MP_Uint32_t  *len;
#endif
{
    int extra;


    if (!IMP_GetLong(link, (long*)len))
        return MP_SetError(link, MP_CantGetDataPacket);

    if (*rawdata == NULL)
        if ((*rawdata = (char*)IMP_RawMemAllocFnc(*len)) == NULL)
            return MP_SetError(link, MP_MemAlloc);

    if (!IMP_GetBytes(link, *rawdata, *len))
        return MP_SetError(link, MP_CantGetDataPacket);
 
    extra = MP_BytesPerMPUnit - (*len % MP_BytesPerMPUnit);
    if (extra != 0 && extra != MP_BytesPerMPUnit)
        if (!IMP_GetBytes(link, junk, extra))
            return MP_SetError(link, MP_CantGetDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, "%-12s  #bytes: %lu  (data not shown)",
                "Raw_DP", *len);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetAnnotationPacket(MP_Link_pt       link,
                                   MP_DictTag_t    *dtag,
                                   MP_AnnotType_t  *atype,
                                   MP_AnnotFlags_t *flags)
#else
MP_Status_t MP_GetAnnotationPacket(link, dtag, atype, flags)
    MP_Link_pt       link;
    MP_DictTag_t    *dtag;
    MP_AnnotType_t  *atype;
    MP_AnnotFlags_t *flags;
#endif
{
    MP_AnnotHeader_t annot;

    if (!IMP_GetLong(link, (long*)&annot))
        return MP_SetError(link, MP_CantGetAnnotationPacket);

    *atype = MP_GetAnnotFieldType(annot);
    *dtag  = MP_GetAnnotDictTagField(annot);
    *flags = MP_GetAnnotFlagsField(annot);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        annot_flags_to_str(*flags);
        /* 3/28/97 - sgray - annotations are spread out through a variety
           of header files, so giving a string version of the annotation
           is not practical at this time.  Maybe have they are stable we
           can create function to do a (dtag, atype) mapping to a string. */
/*        if (*dtag == 0)
            sprintf(fix_log_msg, "%-12s  flags: %s  dict: %lu   ",
                    annotations[*atype], AnnotFlagsStr, *dtag);
        else
            sprintf(fix_log_msg, "%-12s  flags: %s  dict: %lu   atype: %u",
                    "User_AP", AnnotFlagsStr, *dtag, *atype);*/
        sprintf(fix_log_msg, "AnnotPkt      dict: %lu   atype: %u   flags: %s",
                     *dtag, *atype, AnnotFlagsStr);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_GetStringBasedType(MP_Link_pt link, char **s)
#else
MP_Status_t IMP_GetStringBasedType(link, s)
  MP_Link_pt   link;
  char       **s;
#endif
{
    unsigned long  extra, len;
    char   *cp;

    if (!IMP_GetLong(link, (long*)&len))
        return MP_SetError(link, MP_CantGetNodePacket);

    if (*s == NULL)
        if ((*s = (char*)IMP_RawMemAllocFnc(len+1)) == NULL)
            return MP_SetError(link, MP_MemAlloc);

    if (!IMP_GetBytes(link, *s, len))
        return MP_SetError(link, MP_CantGetNodePacket);

    cp = *s + len;
    *cp = '\0';

    extra = MP_BytesPerMPUnit - (len % MP_BytesPerMPUnit);
    if (extra != 0 && extra != MP_BytesPerMPUnit)
        if (!IMP_GetBytes(link, junk, extra))
            return MP_SetError(link, MP_CantGetNodePacket);

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetRawPacket(MP_Link_pt      link,
                            char          **s,
                            MP_Uint32_t    *len,
                            MP_NumAnnot_t  *num_annots)
#else
MP_Status_t MP_GetRawPacket(link, s, len, num_annots)
    MP_Link_pt      link;
    char          **s;
    MP_Uint32_t    *len;
    MP_NumAnnot_t  *num_annots;
#endif
{
    MP_NumChild_t  num_child;
    MP_NodeType_t  ntype = MP_RawType;
    MP_DictTag_t   dtag;
    MP_Common_t    cv;
    int            extra;


    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, &cv, 
                                num_annots, &num_child));

    if (!IMP_GetLong(link, (long*)len))
        return MP_SetError(link, MP_CantGetNodePacket);

    if (*s == NULL)
        if ((*s = (char*)IMP_RawMemAllocFnc(*len)) == NULL)
            return MP_SetError(link, MP_MemAlloc);

    if (!IMP_GetBytes(link, *s, *len))
        return MP_SetError(link, MP_CantGetNodePacket);

    extra = MP_BytesPerMPUnit - (*len % MP_BytesPerMPUnit);
    if (extra != 0 && extra != MP_BytesPerMPUnit)
        if (!IMP_GetBytes(link, junk, extra))
            return MP_SetError(link, MP_CantGetNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg,
                "%-12s  annots: %lu   #bytes: %lu  (data not shown)",
                "Raw_NP", *num_annots, *len);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


/*
 * Change Log:  May 30, 1995 SG
 *              - added this routine.  In MP_Types.h I made MP_NodeType_t
 *               and MP_AnnotType_t both u_char.  This really bothers the
 *               IMP_GetLong() and IMP_PutLong() routines.  My fix is to
 *               declare a local variable of type long, do a get on it
 *               and then assign it to the parameter using a typecast.
 */
#ifdef __STDC__
MP_Status_t MP_GetBooleanPacket(MP_Link_pt     link,
                                MP_Boolean_t  *n,
                                MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetBooleanPacket(link, n, num_annots)
    MP_Link_pt     link;
    MP_Boolean_t  *n;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t  num_child;
    MP_DictTag_t   dtag;
    MP_NodeType_t  ntype = MP_BooleanType;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, n, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        log_fixnum(link, MP_READ_EVENT, "Boolean_NP", MP_BooleanType,
                   *num_annots, n);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetSint8Packet(MP_Link_pt     link,
                              MP_Sint8_t    *n,
                              MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetSint8Packet(link, n, num_annots)
  MP_Link_pt     link;
  MP_Sint8_t    *n;
  MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t  num_child;
    MP_NodeType_t  ntype = MP_Sint8Type;
    MP_DictTag_t   dtag;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, (MP_Common_t*)n, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Sint8_NP", MP_Sint8Type,
                   *num_annots, n);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetUint8Packet(MP_Link_pt     link,
                              MP_Uint8_t    *n,
                              MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetUint8Packet(link, n, num_annots)
    MP_Link_pt     link;
    MP_Uint8_t    *n;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_Uint8Type;
    MP_DictTag_t  dtag;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, n, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Uint8_NP", MP_Uint8Type,
                   *num_annots, n);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetBoolean(MP_Link_pt link, MP_Boolean_t *n)
#else
MP_Status_t IMP_GetBoolean(link, n)
    MP_Link_pt    link;
    MP_Boolean_t *n;
#endif
{
    MP_NumAnnot_t num_annots;
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_BooleanType;
    MP_DictTag_t  dtag;

    ERR_CHK(imp_get_node_header(link, &ntype,&dtag, n,&num_annots,&num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Boolean_DP", MP_BooleanType, 0, n);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_GetSint8(MP_Link_pt link, MP_Sint8_t *n)
#else
MP_Status_t IMP_GetSint8(link, n)
    MP_Link_pt  link;
    MP_Sint8_t *n;
#endif
{
    MP_NumChild_t num_child;
    MP_NumAnnot_t num_annots = MP_Sint8Type;
    MP_NodeType_t ntype;
    MP_DictTag_t  dtag;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, (MP_Common_t*)n, 
                                &num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Sint8_DP", MP_Sint8Type, 0, n);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetUint8(MP_Link_pt link, MP_Uint8_t *n)
#else
MP_Status_t IMP_GetUint8(link, n)
    MP_Link_pt  link;
    MP_Uint8_t *n;
#endif
{
    MP_NumChild_t num_child;
    MP_NumAnnot_t num_annots;
    MP_NodeType_t ntype = MP_Uint8Type;
    MP_DictTag_t  dtag;

    ERR_CHK(imp_get_node_header(link,&ntype, &dtag,n,&num_annots,&num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Uint8_DP", MP_Uint8Type, 0, n);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__ 
MP_Status_t MP_GetSint32Packet(MP_Link_pt     link,
                               MP_Sint32_t   *n,
                               MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetSint32Packet(link, n, num_annots)
    MP_Link_pt     link;
    MP_Sint32_t   *n;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_Sint32Type;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link,&ntype, &dtag, &cv, 
                                num_annots, &num_child));

    if (!IMP_GetLong(link, n))
        return MP_SetError(link, MP_CantGetNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT,"Sint32_NP", MP_Sint32Type,
                   *num_annots, n);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetUint32Packet(MP_Link_pt     link,
                               MP_Uint32_t   *n,
                               MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetUint32Packet(link, n, num_annots)
  MP_Link_pt     link;
  MP_Uint32_t   *n;
  MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_Uint32Type;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, &cv, 
                                num_annots, &num_child));

    if (!IMP_GetLong(link, (long*)n))
        return MP_SetError(link, MP_CantGetNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixnum(link, MP_READ_EVENT, "Uint32_NP", MP_Uint32Type,
                   *num_annots, n);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_GetApInt(MP_Link_pt link, MP_ApInt_pt apint)
#else
MP_Status_t IMP_GetApInt(link, apint)
    MP_Link_pt  link;
    MP_ApInt_pt apint;
#endif
{
    if (link->bignum.bigint_ops->get_bigint(link, apint) != MP_Success)
        return MP_SetError(link, MP_CantGetDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
    int len;
    char *msg = NULL;
    len = link->bignum.bigint_ops->bigint_ascii_size(*apint) + 23;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  value: ", "ApInt_DP");
    link->bignum.bigint_ops->bigint_to_str(*apint, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_READ_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetApIntPacket(MP_Link_pt     link,
                              MP_ApInt_pt    apint,
                              MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetApIntPacket(link, apint, num_annots)
    MP_Link_pt     link;
    MP_ApInt_pt    apint;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_ApIntType;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link,&ntype, &dtag, &cv, 
                                num_annots, &num_child));

    if (link->bignum.bigint_ops->get_bigint(link, apint) != MP_Success)
        return MP_SetError(link, MP_CantGetNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
    int len;
    char *msg = NULL;
    len = link->bignum.bigint_ops->bigint_ascii_size(*apint) + 45;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  annots: %lu   value: ", "ApInt_NP", *num_annots);
    link->bignum.bigint_ops->bigint_to_str(*apint, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_READ_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t IMP_GetApReal(MP_Link_pt link, MP_ApReal_pt apreal)
#else
MP_Status_t IMP_GetApReal(link, apreal)
    MP_Link_pt   link;
    MP_ApReal_pt apreal;
#endif
{
    if (link->bignum.bigreal_ops->get_bigreal(link, apreal) != MP_Success)
        return MP_SetError(link, MP_CantGetDataPacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
    int len;
    char *msg = NULL;
    len = link->bignum.bigreal_ops->bigreal_ascii_size(*apreal) + 25;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  value: ", "ApReal_DP");
    link->bignum.bigreal_ops->bigreal_to_str(*apreal, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_READ_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetApRealPacket(MP_Link_pt     link,
                               MP_ApReal_pt   apreal,
                               MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetApRealPacket(link, apreal, num_annots)
    MP_Link_pt     link;
    MP_ApReal_pt   apreal;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_ApRealType;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, &cv, 
                                num_annots, &num_child));

    if (link->bignum.bigreal_ops->get_bigreal(link, apreal) != MP_Success)
        return MP_SetError(link, MP_CantGetNodePacket);

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
    int len;
    char *msg = NULL;
    len = link->bignum.bigreal_ops->bigreal_ascii_size(*apreal) + 45;
    msg = IMP_MemAllocFnc(len);
    sprintf(msg, "%-12s  annots: %lu   value: ", "ApReal_NP", *num_annots);
    link->bignum.bigreal_ops->bigreal_to_str(*apreal, &(msg[strlen(msg)]));
    MP_LogEvent(link, MP_READ_EVENT, msg);
    IMP_MemFreeFnc(msg, len);
     }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_GetStringPacket(MP_Link_pt      link,
                               char          **s,
                               MP_NumAnnot_t  *num_annots)
#else
MP_Status_t MP_GetStringPacket(link, s, num_annots)
    MP_Link_pt      link;
    char          **s;
    MP_NumAnnot_t  *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_StringType;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, &cv, 
                                num_annots, &num_child));

    if (!IMP_GetStringBasedType(link, s))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        int len = strlen(*s) + 36;
        char *tmp_msg = IMP_MemAllocFnc(len);
        sprintf(tmp_msg, "%-12s  annots: %lu   value: %s", "String_NP",
                    *num_annots, *s);
        MP_LogEvent(link, MP_READ_EVENT, tmp_msg);
        IMP_MemFreeFnc(tmp_msg, len);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetIdentifierPacket(MP_Link_pt      link,
                                   MP_DictTag_t   *dtag,
                                   char          **id,
                                   MP_NumAnnot_t  *num_annots)
#else
MP_Status_t MP_GetIdentifierPacket(link, dtag, id, num_annots)
    MP_Link_pt      link;
    MP_DictTag_t   *dtag;
    char          **id;
    MP_NumAnnot_t  *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_IdentifierType;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, &cv, 
                                num_annots, &num_child));

    if (!IMP_GetStringBasedType(link, id))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, "%-12s  annots: %lu   dict: %lu   value: %s",
                "Ident_NP", *num_annots, *dtag, *id);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetCommonLatinIdentifierPacket(MP_Link_pt     link,
                                              MP_DictTag_t  *dtag,
                                              MP_Common_t   *id,
                                              MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetCommonLatinIdentifierPacket(link, dtag, id, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t  *dtag;
    MP_Common_t   *id;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_CommonLatinIdentifierType;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, id, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_dicttype(link, MP_READ_EVENT, "Latin Id_NP",
                     MP_CommonLatinIdentifierType, *num_annots, *dtag, id, 0);
#endif

    return MP_ClearError(link);
}




#ifdef __STDC__
MP_Status_t MP_GetCommonGreekIdentifierPacket(MP_Link_pt     link,
                                              MP_DictTag_t  *dtag,
                                              MP_Common_t   *id,
                                              MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetCommonGreekIdentifierPacket(link, dtag, id, num_annots)
  MP_Link_pt     link;
  MP_DictTag_t  *dtag;
  MP_Common_t   *id;
  MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_CommonGreekIdentifierType;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, id, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_dicttype(link, MP_READ_EVENT, "Greek Id_NP",
                     MP_CommonGreekIdentifierType, *num_annots, *dtag, id, 0);
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t MP_GetConstantPacket(MP_Link_pt      link,
                                 MP_DictTag_t   *dtag,
                                 char          **cnst,
                                 MP_NumAnnot_t  *num_annots)
#else
MP_Status_t MP_GetConstantPacket(link, dtag, cnst, num_annots)
  MP_Link_pt      link;
  MP_DictTag_t   *dtag;
  char          **cnst;
  MP_NumAnnot_t  *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_ConstantType;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, &cv, 
                                num_annots, &num_child));

    if (!IMP_GetStringBasedType(link, cnst))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, "%-12s  annots: %lu   dict: %lu   value: %s",
                "Constant_NP", *num_annots, *dtag, *cnst);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif
    return MP_ClearError(link);
}



/*
 * July 7, 1995 - I am not sure how much sense it makes to have Get
 * routines for the common constant and operator types.  The receiver
 * generally won't know if the type is common or not and once the
 * header has been retrieved, there is nothing to do but call a utility
 * routine to do the index lookup in the appropriate dictionary
 */
#ifdef __STDC__
MP_Status_t MP_GetCommonConstantPacket(MP_Link_pt     link,
                                       MP_DictTag_t  *dtag,
                                       MP_Common_t   *cnst,
                                       MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetCommonConstantPacket(link, dtag, cnst, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t  *dtag;
    MP_Common_t   *cnst;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_CommonConstantType;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, cnst, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_dicttype(link, MP_READ_EVENT, "Common Cnst_NP",
                     MP_CommonConstantType, *num_annots, *dtag, cnst, 0);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetCommonMetaTypePacket(MP_Link_pt     link,
                                   MP_DictTag_t  *dtag,
                                   MP_Common_t   *cm,
                                   MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetCommonMetaTypePacket(link, dtag, cm, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t  *dtag;
    MP_Common_t   *cm;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NodeType_t ntype = MP_CommonMetaType;
    MP_NumChild_t num_child;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, cm, 
                                num_annots, &num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_dicttype(link, MP_READ_EVENT, "Common Meta_NP", MP_CommonMetaType,
                     *num_annots, *dtag, cm, 0);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetCommonOperatorPacket(MP_Link_pt     link,
                                       MP_DictTag_t  *dtag,
                                       MP_Common_t   *op,
                                       MP_NumAnnot_t *num_annots,
                                       MP_NumChild_t *num_child)
#else
MP_Status_t MP_GetCommonOperatorPacket(link, dtag, op, num_annots, num_child)
  MP_Link_pt     link;
  MP_DictTag_t  *dtag;
  MP_Common_t   *op;
  MP_NumAnnot_t *num_annots;
  MP_NumChild_t *num_child;
#endif
{
    MP_NodeType_t ntype = MP_CommonOperatorType;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, op, num_annots, num_child));

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_dicttype(link, MP_READ_EVENT, "Common Op_NP", MP_CommonOperatorType,
                     *num_annots, *dtag, op, *num_child);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetOperatorPacket(MP_Link_pt      link,
                                 MP_DictTag_t   *dtag,
                                 char          **op,
                                 MP_NumAnnot_t  *num_annots,
                                 MP_NumChild_t  *num_child)
#else
MP_Status_t MP_GetOperatorPacket(link, dtag, op, num_annots, num_child)
    MP_Link_pt      link;
    MP_DictTag_t   *dtag;
    char          **op;
    MP_NumAnnot_t  *num_annots;
    MP_NumChild_t  *num_child;
#endif
{
    MP_NodeType_t ntype = MP_OperatorType;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, &cv, 
                                num_annots, num_child));

    if (!IMP_GetStringBasedType(link, op))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_dicttype(link, MP_READ_EVENT, "Operator_NP", MP_OperatorType,
                     *num_annots, *dtag, *op, *num_child);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetMetaTypePacket(MP_Link_pt      link,
                                 MP_DictTag_t  *dtag,
                                 char          **s,
                                 MP_NumAnnot_t  *num_annots)
#else
MP_Status_t MP_GetMetaTypePacket(link, dtag, s, num_annots)
    MP_Link_pt     link;
    MP_DictTag_t  *dtag,
    char          **s;
    MP_NumAnnot_t  *num_annots;
#endif
{
    MP_NumChild_t num_child;
    MP_NodeType_t ntype = MP_MetaType;
    MP_Common_t   cv;

    ERR_CHK(imp_get_node_header(link, &ntype, dtag, &cv,
                                num_annots, &num_child));

    if (!IMP_GetStringBasedType(link, s))
        return MP_Failure;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(fix_log_msg, "%-12s  annots: %lu   dict: %lu   value: %s",
                "MetaType_NP", *num_annots, *dtag, *s);
        MP_LogEvent(link, MP_READ_EVENT, fix_log_msg);
    }
#endif

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_GetReal32(MP_Link_pt link, MP_Real32_t *fp)
#else
MP_Status_t IMP_GetReal32(link, fp)
    MP_Link_pt   link;
    MP_Real32_t *fp;
#endif
{
#ifdef vax
    struct ieee_single  is;
    struct vax_single   vs, *vsp;
    struct sgl_limits  *lim;
#endif /* vax */

    MP_Status_t status;

#ifndef vax
    status = IMP_GetLong(link, (long *)fp);
#else /* not vax */
    vsp = (struct vax_single *)fp;
    if (!IMP_GetLong(link, (long *)&is))
        return MP_SetError(link, MP_CantGetDataPacket);

    for (i = 0, lim = sgl_limits;
         i < sizeof(sgl_limits)/sizeof(struct sgl_limits); i++, lim++)
        if (is.exp == lim->ieee.exp
            && is.mantissa = lim->ieee.mantissa) {
            *vsp = lim->s;
            goto doneit;
        }

    vsp->exp = is.exp - IEEE_SNG_BIAS + VAX_SNG_BIAS;
    vsp->mantissa2 = is.mantissa;
    vsp->mantissa1 = (is.mantissa >> 16);

doneit:
    vsp->sign = is.sign;
    status = MP_Success;
#endif /* not vax */

    /*
     * I _think_ this logging will work for vax floats.  April 25, 1996
     */

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixreal(link, MP_READ_EVENT, "Real32_DP", MP_Real32Type, 0, fp);
#endif

    return status;
}



#ifdef __STDC__
MP_Status_t IMP_GetReal64(MP_Link_pt link, MP_Real64_t *dp)
#else
MP_Status_t IMP_GetReal64(link, dp)
    MP_Link_pt   link;
    MP_Real64_t *dp;
#endif
{
    long *lp;

#ifdef vax
    struct ieee_double  id;
    struct vax_double   vd;
    struct dbl_limits  *lim;
#endif /* vax */

#ifndef vax
    lp = (long *)dp;
#  ifdef WORDS_BIGENDIAN

    if (!IMP_GetLong(link, lp++) || !IMP_GetLong(link, lp))
        return MP_SetError(link, MP_CantGetDataPacket);

#  else  /* WORDS_LITTLEENDIAN */

    if (!IMP_GetLong(link, lp+1) || !IMP_GetLong(link, lp))
        return MP_SetError(link, MP_CantGetDataPacket);

#  endif /* WORDS_BIGENDIAN */

#else /* vax */
    lp = (long *)&id;
    if (!IMP_GetLong(link, lp++) || !IMP_GetLong(link, lp))
        return MP_Failure;

    for (i = 0, lim = dbl_limits;
         i < sizeof(dbl_limits)/sizeof(struct dbl_limits); i++, lim++)
        if (id.mantissa2 == lim->ieee.mantissa2
            && id.mantissa1 == lim->ieee.mantissa1
            && id.exp == lim->ieee.exp) {
            vd = lim->d;
            goto doneit;
        }

    vd.exp = id.exp - IEEE_DBL_BIAS + VAX_DBL_BIAS;
    vd.mantissa1 = (id.mantissa1 >> 13);
    vd.mantissa2 = ((id.mantissa1 & MASK(13)) << 3)
                   | (id.mantissa2 >> 29);
    vd.mantissa3 = (id.mantissa2 >> 13);
    vd.mantissa4 = (id.mantissa2 << 3);

doneit:
    vd.sign = id.sign;
    *dp = *((double *)&vd);
#endif /* vax */

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixreal(link, MP_READ_EVENT, "Real64_DP", MP_Real64Type, 0, dp);
#endif

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetReal32Packet(MP_Link_pt     link,
                               MP_Real32_t   *fp,
                               MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetReal32Packet(link, fp, num_annots)
    MP_Link_pt     link;
    MP_Real32_t   *fp;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NodeType_t ntype = MP_Real32Type;
    MP_NumChild_t num_child;
    MP_DictTag_t  dtag;
    MP_Common_t   cv;

#ifdef vax
    struct ieee_single  is;
    struct vax_single   vs, *vsp;
    struct sgl_limits  *lim;
#endif /* vax */

    ERR_CHK(imp_get_node_header(link, &ntype, &dtag, &cv,
                                num_annots, &num_child));

#ifndef vax
    if (!IMP_GetLong(link, (long *)fp))
        return MP_SetError(link, MP_CantGetNodePacket);
#else /* vax */
    vsp = (struct vax_single *)fp;
    if (!IMP_GetLong(link, (long *)&is))
        return MP_SetError(link, MP_CantGetNodePacket);

    for (i = 0, lim = sgl_limits;
         i < sizeof(sgl_limits)/sizeof(struct sgl_limits); i++, lim++)
        if (is.exp == lim->ieee.exp
            && is.mantissa = lim->ieee.mantissa) {
            *vsp = lim->s;
            goto doneit;
        }

    vsp->exp = is.exp - IEEE_SNG_BIAS + VAX_SNG_BIAS;
    vsp->mantissa2 = is.mantissa;
    vsp->mantissa1 = (is.mantissa >> 16);

doneit:
    vsp->sign = is.sign;
#endif /* vax */

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixreal(link, MP_READ_EVENT, "Real32_DP",
                    MP_Real32Type, *num_annots,fp);
#endif /* NO_LOGGING */

    return MP_ClearError(link);
}



#ifdef __STDC__
MP_Status_t MP_GetReal64Packet(MP_Link_pt     link,
                               MP_Real64_t   *dp,
                               MP_NumAnnot_t *num_annots)
#else
MP_Status_t MP_GetReal64Packet(link, dp, num_annots)
    MP_Link_pt     link;
    MP_Real64_t   *dp;
    MP_NumAnnot_t *num_annots;
#endif
{
    MP_NodeType_t  ntype = MP_Real64Type;
    MP_NumChild_t  num_child;
    MP_DictTag_t   dtag;
    MP_Common_t    cv;
    register long *lp;
#ifdef vax
    struct   ieee_double id;
    struct   vax_double  vd;
    register struct dbl_limits *lim;
#endif /* vax */

    ERR_CHK(imp_get_node_header(link,&ntype, &dtag, &cv,
                                num_annots, &num_child));

#ifndef vax
    lp = (long *)dp;

#ifdef WORDS_BIGENDIAN
    if (!IMP_GetLong(link, lp++) || !IMP_GetLong(link, lp))
        return MP_SetError(link, MP_CantGetNodePacket);
#else  /* not WORDS_BIGENDIAN */
    if (!IMP_GetLong(link, lp+1) || !IMP_GetLong(link, lp))
        return MP_SetError(link, MP_CantGetNodePacket);
#endif /* not WORDS_BIGENDIAN */

#else /* vax */
    lp = (long *)&id;
    if (!IMP_GetLong(link, lp++) || !IMP_GetLong(link, lp))
        return MP_SetError(link, MP_CantGetNodePacket);

    for (i = 0, lim = dbl_limits;
         i < sizeof(dbl_limits)/sizeof(struct dbl_limits); i++, lim++)
        if (id.mantissa2 == lim->ieee.mantissa2
            && id.mantissa1 == lim->ieee.mantissa1
            && id.exp == lim->ieee.exp) {
            vd = lim->d;
            goto doneit;
        }

    vd.exp = id.exp - IEEE_DBL_BIAS + VAX_DBL_BIAS;
    vd.mantissa1 = (id.mantissa1 >> 13);
    vd.mantissa2 = ((id.mantissa1 & MASK(13)) << 3)
                   | (id.mantissa2 >> 29);
    vd.mantissa3 = (id.mantissa2 >> 13);
    vd.mantissa4 = (id.mantissa2 << 3);

doneit:
    vd.sign = id.sign;
    *dp = *((double *)&vd);
#endif /* vax */

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS)
        log_fixreal(link, MP_READ_EVENT,
                    "Real64_NP", MP_Real64Type, *num_annots,dp);
#endif /* NO_LOGGING */

    return MP_ClearError(link);
}


