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
 *   IMPLEMENTATION FILE:  MP_Vector.c
 *                                        
 *        The routines here put and get vectors of type Real64, Real32
 *      and Sint32.  As an optimization we use put/getBytes when
 *      the native format of the machine corresponds to the MP format.
 *
 *      Note: The MP_GetVector() routines assume that the data was put  
 *      with the corresponding MP_PutVector() routine.
 *
 *      2/1/97 obachman  - updated put routines to reflect ProtoDict defs
 *                       - cleaned up various things
 *                       - deleted MP_Get routines, since they are obsolete
 *                         (use those of the MPT lib, instead)
 *      5/24/96 sgray - updated routines to use MP_PeekHeader() and to
 *                      use the meta types in the prototypes.
 *
 ************************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_Vector.c,v 1.2 2008-07-09 07:42:24 Singular Exp $";
#endif /* lint */

#include "MP.h"
#include <unistd.h>

/*#define MP_DEBUG*/

/***************************************************************************
 *
 * The tmp vector buisness for 16 bit quantities
 *
 ***************************************************************************/

static MP_Uint16_t *TmpVec = NULL;
static MP_NumChild_t TmpVecLen = 0;

static void UpdateTmpVec(MP_NumChild_t len)
{
  IMP_MemFreeFnc(TmpVec, TmpVecLen*sizeof(MP_Uint16_t));
  TmpVec = IMP_MemAllocFnc(len*sizeof(MP_Uint16_t));
  TmpVecLen = len;
}

/***************************************************************************
 *
 * Putting vectors 
 *
 ***************************************************************************/

/* This puts the vector header together with the protoannot
 * na should NOT include the protoannot
 */
#ifdef __STDC__
MP_Status_t IMP_PutBasicVectorHeader(MP_Link_pt    link,
                                     MP_Common_t   elem_type,
                                     MP_NumAnnot_t na,
                                     MP_NumChild_t num_elems)
#else
MP_Status_t IMP_PutBasicVectorHeader(link, elem_type, na, num_elems)
    MP_Link_pt    link;
    MP_Common_t   elem_type;
    MP_NumAnnot_t na;
    MP_NumChild_t num_elems;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr,"IMP_PutBasicVectorHeader: entering\n");
    fflush(stderr);
#endif

    if (MP_PutCommonOperatorPacket(link,
                                   MP_ProtoDict,
                                   MP_CopProtoArray,
                                   na + 1,
                                   num_elems)
        != MP_Success)
      return MP_SetError(link, MP_CantPutNodePacket);

    if (MP_PutAnnotationPacket(link,
                               MP_ProtoDict,
                               MP_AnnotProtoPrototype,
                               MP_AnnotRequired | MP_AnnotValuated)
        != MP_Success)
      return MP_SetError(link, MP_CantPutAnnotationPacket);

    if (MP_PutCommonMetaTypePacket(link, MP_ProtoDict, elem_type, 0)
        != MP_Success)
      return MP_SetError(link, MP_CantPutNodePacket);

#ifdef MP_DEBUG
    fprintf(stderr,"IMP_PutBasicVectorHeader: exiting successfully\n");
    fflush(stderr);
#endif

    return MP_Success;
}

/* A general routine fro putting vectors -- can only be used if NO extra
 * annots should be put. Otherwise do: IMP_PutBasicVectorHeader, put extra
 * annots, IMP_PutBasicVector
 */

#ifdef __STDC__
MP_Status_t MP_PutBasicVector(MP_Link_pt     link,
                              void          *elems,
                              MP_Common_t    elem_type,
                              MP_NumChild_t  num_elems)
#else
MP_Status_t MP_PutBasicVector(link, elems, elem_type, num_elems)
    MP_Link_pt    link;
    void         *elems;
    MP_Common_t   elem_type;
    MP_NumChild_t num_elems;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr,"MP_PutBasicVector: entering");
    fprintf(stderr, "\t elem_type = %d, num_elems = %u\n",
            elem_type, num_elems);
    fflush(stderr);
#endif

    if (IMP_PutBasicVectorHeader(link, elem_type, 0, num_elems) != MP_Success)
      return MP_Failure;

    return IMP_PutBasicVector(link, elems, elem_type, num_elems);
}

#ifdef __STDC__
MP_Status_t IMP_PutBasicVector(MP_Link_pt     link,
                               void          *elems,
                               MP_Common_t    elem_type,
                               MP_NumChild_t  num_elems)
#else
MP_Status_t IMP_PutBasicVector(link, elems, elem_type, num_elems)
    MP_Link_pt    link;
    void         *elems;
    MP_Common_t   elem_type;
    MP_NumChild_t num_elems;
#endif
{
#ifdef MP_DEBUG
    fprintf(stderr,"IMP_PutBasicVector: entering\n");
    fprintf(stderr, "\t elem_type = %d, num_elems = %u\n",
            elem_type, num_elems);
    fflush(stderr);
#endif
    
  if (IMP_Is64BitNumericMetaType(elem_type))
    return IMP_PutReal64Vector(link, (MP_Real64_t *) elems, num_elems);
  else if (IMP_Is32BitNumericMetaType(elem_type)) 
    return IMP_PutUint32Vector(link, (MP_Uint32_t *)elems, num_elems);
  else if (IMP_Is16BitNumericMetaType(elem_type))
    return IMP_PutUint16Vector(link, (MP_Uint16_t *)elems, num_elems);
  else if (IMP_Is8BitNumericMetaType(elem_type))
    return IMP_PutUint8Vector(link, (MP_Uint8_t *) elems, num_elems);
  else
  {
#ifdef MP_DEBUG
    fprintf(stderr,
          "IMP_PutBasicVector: exiting with error - unknown elem type\n");
    fflush(stderr);
#endif
    MP_SetError(link, MP_WrongBasicVectorType);
    return MP_Failure;
  }
}

#ifdef __STDC__
MP_Status_t IMP_PutReal64Vector(MP_Link_pt     link,
                                MP_Real64_t   *dvec,
                                MP_NumChild_t  len)
#else
MP_Status_t IMP_PutReal64Vector(link, dvec, len)
    MP_Link_pt     link;
    MP_Real64_t   *dvec;
    MP_NumChild_t  len;
#endif
{
    MP_Real64_t   *dp;
    MP_NumChild_t  i;

    if (link->link_word_order != link->env->native_word_order) {
        for (i = 0, dp = dvec; i < len; i++, dp++)
            if (IMP_PutReal64(link, *dp) != MP_Success)
                return MP_SetError(link, MP_CantPutNodePacket);
    } else
        return IMP_PutBytes(link, (char *)dvec, len * sizeof(MP_Real64_t));

    return MP_ClearError(link);
}

#ifdef __STDC__
MP_Status_t IMP_PutUint32Vector(MP_Link_pt     link,
                                MP_Uint32_t   *si32vec,
                                MP_NumChild_t  len)
#else
MP_Status_t IMP_PutUint32Vector(link, si32vec, len)
    MP_Link_pt     link;
    MP_Uint32_t   *si32vec;
    MP_NumChild_t  len;
#endif
{
    MP_Uint32_t   *s32p;
    MP_NumChild_t  i;

    if (link->link_word_order != link->env->native_word_order) {
        for (i = 0, s32p = si32vec; i < len; i++, s32p++)
            if (IMP_PutLong(link, s32p) != MP_Success)
                return MP_SetError(link, MP_CantPutNodePacket);
    } else
        if (IMP_PutBytes(link, (char *)si32vec, len * sizeof(MP_Uint32_t))
            != MP_Success)
            return MP_SetError(link, MP_Failure);

    return MP_ClearError(link);
}

static char filler[4]={0, 0, 0, 0};

MP_Status_t IMP_PutUint16Vector(MP_Link_pt      link,
                                MP_Uint16_t     *u16vec,
                                MP_NumChild_t   len)
{
  unsigned long n = len*sizeof(MP_Uint16_t);
  short extra = n % MP_BytesPerMPUnit;
  
  
  if (link->link_word_order != link->env->native_word_order)
  {
    if (TmpVecLen < len) UpdateTmpVec(len);
    swab(u16vec, TmpVec, n);
    u16vec = TmpVec;
  }

  if (! IMP_PutBytes(link, (char *) u16vec, n))
    return MP_SetError(link, MP_CantPutDataPacket);

  if (extra && !IMP_PutBytes(link, filler, MP_BytesPerMPUnit - extra))
    return MP_SetError(link, MP_CantPutDataPacket);

  return MP_ClearError(link);
}

MP_Status_t IMP_PutUint8Vector(MP_Link_pt       link,
                               MP_Uint8_t       *u8vec,
                               MP_NumChild_t    len)
{
  short extra = len % MP_BytesPerMPUnit;

  if (! IMP_PutBytes(link, (char *) u8vec, len))
    return MP_SetError(link, MP_CantPutDataPacket);

  if (extra && !IMP_PutBytes(link, filler, MP_BytesPerMPUnit - extra))
    return MP_SetError(link, MP_CantPutDataPacket);

  return MP_ClearError(link);
}


/***************************************************************************
 *
 * Getting Vectors
 *
 * We only provide the IMP_Get*Vector routines -- MP_Get*Vector
 * routines are obsolete by the MPT library
 *
 ***************************************************************************/
#ifdef __STDC__

MP_Status_t IMP_GetBasicVector(MP_Link_pt       link,
                               void             **elems,
                               MP_Common_t      elem_type,
                               MP_NumChild_t    num_elems)
{
  if (IMP_Is64BitNumericMetaType(elem_type))
    return IMP_GetReal64Vector(link, (MP_Real64_t **) elems, num_elems);
  else if (IMP_Is32BitNumericMetaType(elem_type)) 
    return IMP_GetUint32Vector(link, (MP_Uint32_t **)elems, num_elems);
  else if (IMP_Is16BitNumericMetaType(elem_type))
    return IMP_GetUint16Vector(link, (MP_Uint16_t **)elems, num_elems);
  else if (IMP_Is8BitNumericMetaType(elem_type))
    return IMP_GetUint8Vector(link, (MP_Uint8_t **) elems, num_elems);
  else
  {
    MP_SetError(link, MP_WrongBasicVectorType);
    return MP_Failure;
  }
}
  
                          
MP_Status_t IMP_GetReal64Vector(MP_Link_pt      link,
                                MP_Real64_t   **dvec,
                                MP_NumChild_t   len)
#else
MP_Status_t IMP_GetReal64Vector(link, dvec, len)
    MP_Link_pt      link;
    MP_Real64_t   **dvec;
    MP_NumChild_t   len;
#endif
{
    MP_NumChild_t  i;
    MP_Real64_t   *dp;

    CHK_ALLOC_MEM(link, *dvec, len, MP_Real64_t);

    if (link->link_word_order != link->env->native_word_order) {
        for (i = 0, dp = *dvec; i < len; i++, dp++)
            if (IMP_GetReal64(link, dp) != MP_Success)
                return MP_SetError(link, MP_CantGetNodePacket);
    } else
        return IMP_GetBytes(link, (char *)*dvec, len * sizeof(MP_Real64_t));

    return MP_ClearError(link);
}


#ifdef __STDC__
MP_Status_t IMP_GetUint32Vector(MP_Link_pt      link,
                                MP_Uint32_t   **si32vec,
                                MP_NumChild_t   len)
#else
MP_Status_t IMP_GetUint32Vector(link, si32vec, len)
    MP_Link_pt      link;
    MP_Uint32_t   **si32vec;
    MP_NumChild_t   len;
#endif
{
    MP_NumChild_t  i;
    MP_Uint32_t   *s32p;

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetUint32Vector: entering.  len = %d\n", len);
    fflush(stderr); 
#endif
    CHK_ALLOC_MEM(link, *si32vec, len, MP_Sint32_t);

    if (link->link_word_order != link->env->native_word_order) {
        for (i = 0, s32p = *si32vec; i < len; i++, s32p++)
            if (IMP_GetLong(link, s32p) != MP_Success)
                return MP_SetError(link, MP_CantGetNodePacket);
    } else
        if (IMP_GetBytes(link, (char *) *si32vec, len * sizeof(MP_Sint32_t))
            != MP_Success)
            return MP_SetError(link, MP_Failure);

#ifdef MP_DEBUG
    fprintf(stderr, "IMP_GetUint32Vector: exiting with success!\n");
    fflush(stderr); 
#endif
    return MP_ClearError(link);
}


static char junk[4];

MP_Status_t IMP_GetUint16Vector(MP_Link_pt link,
                                MP_Uint16_t **u16vec,
                                MP_NumChild_t len)
{
  MP_Uint16_t *v;
  unsigned long n = len*sizeof(MP_Uint16_t);
  short extra = n % MP_BytesPerMPUnit;

  CHK_ALLOC_MEM(link, *u16vec, len, MP_Uint16_t);
  if (link->link_word_order != link->env->native_word_order)
  {
    if (TmpVecLen < len) UpdateTmpVec(len);
    v = TmpVec;
  }
  else
    v = *u16vec;

  if (! IMP_GetBytes(link, (char *) v, n))
    return MP_SetError(link, MP_CantGetNodePacket);

  if (v != *u16vec) swab(v, *u16vec, n);

  if (extra && !(IMP_GetBytes(link, junk, MP_BytesPerMPUnit - extra)))
    return MP_SetError(link, MP_CantGetNodePacket);

  return MP_ClearError(link);
}


MP_Status_t IMP_GetUint8Vector(MP_Link_pt link,
                               MP_Uint8_t **u8vec,
                               MP_NumChild_t len)
{
  short extra = len % MP_BytesPerMPUnit;

  CHK_ALLOC_MEM(link, *u8vec, len, MP_Uint8_t);
  
  if (! IMP_GetBytes(link, *u8vec, len))
    return MP_SetError(link, MP_CantGetNodePacket);

  if (extra && !(IMP_GetBytes(link, junk, MP_BytesPerMPUnit - extra)))
    return MP_SetError(link, MP_CantGetNodePacket);

  return MP_ClearError(link);
}


