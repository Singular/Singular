/****************************************************************
 *
 *   HEADER FILE:  MP_Annotations.h
 *
 *        Define the MP-defined annotations and establish
 *        macros to set and clear the annotation packet
 *        flag fields.
 *
 *   Change Log:
 *     5/9/96  sgray - Fixed macros to set/clear the annotation
 *                     packet header flags field.
 *                   - Added predicate macros for the flags field
 *                     per suggestion from obachman (good friend
 *                     and drinking buddy).
 ****************************************************************/

#ifndef _MP_Annotations_h
#define _MP_Annotations_h

/* global variables - maps annotation numbers to string
 * identifying the annotation - used for testing.  See
 * the corresponding dictionary for the defines.
 */
EXTERN char *MpAnnotations[];         /* MP_MpDict.h    */
EXTERN char *ProtoAnnotations[];      /* MP_ProtoDict.h */

/*
 * Annotation structure
 *  The format of a annotation packet  is:
 *
 *  |     3           2            1            0     |
 *  +-----------+-----------|-------------+-----------+
 *  |   flags   |dictionary |        Annot type       |
 *  |           |   tag     |                         |
 *
 *  sizes, typedefs, and masks for the fields of an
 *  annotation packet
 */

typedef  unsigned char  MP_AnnotFlags_t;
typedef  unsigned short MP_AnnotType_t;
typedef  unsigned long  MP_AnnotHeader_t;

#define  MP_AnnotTypeMask    0x0000ffff
#define  MP_AnnotDictTagMask 0x00ff0000
#define  MP_AnnotFlagsMask   0xff000000

/*  Format of an annotation packet by field */

#define  MP_AnnotDictTagFieldShift     16
#define  MP_AnnotFlagsFieldShift       24


/*
 * Annotation flags
 *                                         0             1
 */
#define MP_AnnotRequired    0x0001  /* supplemental   required   */
#define MP_AnnotValuated    0x0002  /* simple         valuated   */
#define MP_AnnotTreeScope   0x0004  /* node           subtree    */


/*
 * macros to set and unset flag fields
 */
#define MP_SetAnnotRequired(aflags)    (aflags |= MP_AnnotRequired)
#define MP_SetAnnotValuated(aflags)    (aflags |= MP_AnnotValuated)
#define MP_SetAnnotTreeScope(aflags)   (aflags |= MP_AnnotTreeScope)
#define MP_ClearAnnotRequired(aflags)  (aflags &= ~MP_AnnotRequired)
#define MP_ClearAnnotValuated(aflags)  (aflags &= ~MP_AnnotValuated)
#define MP_ClearAnnotTreeScope(aflags) (aflags &= ~MP_AnnotTreeScope)
#define MP_ClearAnnotFlags(aflags)     (aflags &= 0x0000)

/*
 * macros to detrmine if an annotation flag is set
 */
#define MP_IsAnnotRequired(aflags)     (aflags & MP_AnnotRequired)
#define MP_IsAnnotValuated(aflags)     (aflags & MP_AnnotValuated)
#define MP_IsAnnotTreeScope(aflags)    (aflags & MP_AnnotTreeScope)

/*
 * macros to set the fields of an annotation packet header
 */
#define MP_SetAnnotTypeField(hdr, type) \
     (hdr = (hdr & ~MP_AnnotTypeMask) | (long) type)

#define MP_SetAnnotDictTagField(hdr, dtag) \
     (hdr = (hdr & ~MP_AnnotDictTagMask) | (dtag << MP_AnnotDictTagFieldShift))

#define MP_SetAnnotFlagsField(hdr, aflag)          \
     (hdr = (hdr & ~MP_AnnotFlagsMask)             \
     | ((long) aflag <<  MP_AnnotFlagsFieldShift))

#define MP_GetAnnotFieldType(hdr) \
     (hdr & MP_AnnotTypeMask)

#define MP_GetAnnotDictTagField(hdr) \
     ((hdr & MP_AnnotDictTagMask) >> MP_AnnotDictTagFieldShift)

#define MP_GetAnnotFlagsField(hdr) \
    ((hdr & MP_AnnotFlagsMask) >> MP_AnnotFlagsFieldShift)


#endif /* _MP_Annotations_h */
