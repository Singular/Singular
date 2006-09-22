/****************************************************************
 *
 *   HEADER FILE:  MP_Vector.h
 *
 ***************************************************************/

#ifndef _MP_Vector_h
#define _MP_Vector_h

#include "MP.h"

/* Putting a Vector */
EXTERN MP_Status_t IMP_PutBasicVectorHeader
_ANSI_ARGS_((MP_Link_pt link, MP_Common_t elem_type,
              MP_NumAnnot_t na, MP_NumChild_t num_elems));

EXTERN MP_Status_t MP_PutBasicVector
_ANSI_ARGS_((MP_Link_pt link, void *elems,
             MP_Common_t elem_type, MP_NumChild_t num_elems));

EXTERN MP_Status_t IMP_PutBasicVector
_ANSI_ARGS_((MP_Link_pt link, void *elems,
             MP_Common_t elem_type, MP_NumChild_t num_elems));

EXTERN MP_Status_t  IMP_PutReal64Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Real64_t *dvec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_PutUint32Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint32_t *si32vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_PutUint16Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint16_t *si16vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_PutUint8Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint8_t *si8vec, MP_NumChild_t len));

#define IMP_PutReal32Vector(link, vec, len)         \
IMP_PutUint32Vector(link, (MP_Uint32_t *) vec, len)

#define IMP_PutSint32Vector(link, vec, len)         \
IMP_PutUint32Vector(link, (MP_Uint32_t *) vec, len)

#define IMP_PutSint16Vector(link, vec, len)         \
IMP_PutUint16Vector(link, (MP_Uint16_t *) vec, len)

#define IMP_PutSint8Vector(link, vec, len)         \
IMP_PutUint8Vector(link, (MP_Uint8_t *) vec, len)

/* Getting a vector */
EXTERN MP_Status_t MP_GetBasicVector
_ANSI_ARGS_((MP_Link_pt link, void **elems,
             MP_Common_t elem_type, MP_NumChild_t num_elems));

EXTERN MP_Status_t
IMP_GetReal64Vector _ANSI_ARGS_((MP_Link_pt link,
                                 MP_Real64_t **dvec, MP_NumChild_t len));

EXTERN MP_Status_t
IMP_GetUint32Vector _ANSI_ARGS_((MP_Link_pt link,
                                 MP_Uint32_t **si32vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_GetUint16Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint16_t **si16vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_GetUint8Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint8_t **si8vec, MP_NumChild_t len));

#define IMP_GetReal32Vector(link, vec, len)         \
IMP_GetUint32Vector(link, (MP_Uint32_t **) vec, len)

#define IMP_GetSint32Vector(link, vec, len)         \
IMP_GetUint32Vector(link, (MP_Uint32_t **) vec, len)

#define IMP_GetSint16Vector(link, vec, len)         \
IMP_GetUint16Vector(link, (MP_Uint16_t **) vec, len)

#define IMP_GetSint8Vector(link, vec, len)         \
IMP_GetUint8Vector(link, (MP_Uint8_t **) vec, len)

#endif /* _MP_Vector_h */
