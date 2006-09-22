/***************************************************************************
 *
 *   HEADER FILE:  MP_Types.h
 *
 *        Most of the typedefs needed by MP are here, including the definition
 *      of node and annotation packets.
 *
 *   Change Log:
 *     May 30, 1995 SG - added defines for new types for MP 1.0
 *     December 12, 1995 SG - added ifdef for AIX per Hannes' instructions
 *     December 20, 1995 SG - added enum for fp format
 *     4/23/96 sgray - added defines to support different arbitrary precision
 *                     integer formats.
 *     Jan 6, 1997  OB - rearranged integer assignments for MP types
 *
 ***************************************************************************/

#ifndef _MP_Types_h
#define _MP_Types_h

#define MP_TRUE  0x01
#define MP_FALSE 0x00

#ifdef __hpux
#   define SELECT(maxfdp1, readfs, writefs, exceptfds, tvptr) \
        select((int)maxfdp1, (int*)readfs, (int*)writefs,     \
               (int*)exceptfds, tvptr)
#else /*  not __hpux */
#   define SELECT  select
#endif /* not __hpux */


/* to indicate the direction of traffic */
#define MP_SEND     0
#define MP_RECEIVE  1

/* For checking the status of a link*/
enum link_status_option {MP_UnknownStatus, MP_LinkReadyReading,
                         MP_LinkReadyWriting,
                         MP_LinkIsParent, MP_LinkIsChild};
typedef enum link_status_option MP_LinkStatus_t;

/* To determine which byte ordering is native and which is actually used. */
/* The default is BigEndian. */
enum word_order {MP_BigEndian, MP_LittleEndian};
typedef enum word_order MP_WordOrder_t;

/* To determine which floating format is native and which is actually used. */
/* The default is IEEE format. */
enum fp_format {MP_IEEE_fp, MP_Vax_fp};
typedef enum fp_format MP_FpFormat_t;



/*
*  Important size used throughout
*/
#define  MP_BytesPerMPUnit        4

/* New defines for MP 1.0 */

/* shift values for the node packet */
#define  MP_NodeOpFieldShift           8
#define  MP_NodeNumChildFieldShift     8
#define  MP_NodeAnnotFieldShift       12
#define  MP_NodeDictTagFieldShift     16
#define  MP_NodeCommonValueFieldShift 24

/* used for the #children and #annots field in the node packet header
   to indicate that there are more than 14 children or annotations and
   that an extra field(s) (a long) must be retrieved from the stream */
#define  MP_ExtraFieldIndicator             0xf

/* used for the dictionary tag field to indicate that an extra four
   bytes must be read to get the dictionary tag - don't expect to
   actually have to use this for a long time                         */
#define  MP_ExtraMPDictTagFieldIndicator    0x7f
#define  MP_ExtraUserDictTagFieldIndicator  0xff

/* masks for the node packet fields */
#define  MP_NodeTypeMask         0x000000ff
#define  MP_NodeNumChildMask     0x00000f00
#define  MP_NodeNumAnnotMask     0x0000f000
#define  MP_NodeDictTagMask      0x00ff0000
#define  MP_NodeCommonValueMask  0xff000000


#define MP_ExtraAnnotField   MP_ExtraFieldIndicator << MP_NodeAnnotFieldShift
#define MP_ExtraChildField   MP_ExtraFieldIndicator << MP_NodeOpFieldShift
#define MP_ExtraDictTagField MP_ExtraFieldIndicator << MP_NodeDictTagFieldShift


/*
*  The format of a node packet header is:
*
*  |    3           2            1              0    |
*  +-----------+-----------|-------------+-----------+
*  |  common   |dictionary |  ^   |  ^   |    Node   |
*  |  value    |   tag     |  |   |  |   |    type   |
*                             |      |- #children
*                             |- #annots
*/

typedef unsigned char  MP_NodeType_t;
typedef unsigned char  MP_CommonValue_t;
typedef unsigned long  MP_NodeHeader_t;
typedef unsigned long  MP_NumChild_t;
typedef unsigned long  MP_NumAnnot_t;
typedef unsigned long  MP_DictTag_t;      /* common to annotation packet header */

/* macros for setting and getting the fields of a node header */

#define MP_SetNodeTypeField(hdr, type)              \
     (hdr = (hdr & ~MP_NodeTypeMask) | (long) type)

#define MP_SetNodeNumChildField(hdr, nchild)                 \
     (hdr =  (hdr & ~MP_NodeNumChildMask)                    \
             | ((long) nchild << MP_NodeNumChildFieldShift))

#define MP_SetNodeNumAnnotsField(hdr, nannots)            \
     (hdr = (hdr & ~MP_NodeNumAnnotMask)                  \
            | ((long) nannots << MP_NodeAnnotFieldShift))

#define MP_SetNodeDictTagField(hdr, dtag)         \
     (hdr = (hdr & ~MP_NodeDictTagMask)           \
            | (dtag << MP_NodeDictTagFieldShift))

#define MP_SetNodeExtraChildField(hdr)  \
     (hdr |= (long) MP_ExtraChildField)

#define MP_SetNodeExtraAnnotsField(hdr)  \
     (hdr |= (long) MP_ExtraAnnotField)

#define MP_SetNodeCommonValueField(hdr, value)                 \
     (hdr = (hdr & ~MP_NodeCommonValueMask)                    \
            | ((long) value <<  MP_NodeCommonValueFieldShift))

#define MP_GetNodeFieldType(hdr) \
     (hdr & MP_NodeTypeMask)

#define MP_GetNodeNumAnnotsField(hdr) \
     ((hdr & MP_NodeNumAnnotMask) >> MP_NodeAnnotFieldShift)

#define MP_GetNodeNumChildField(hdr) \
     ((hdr & MP_NodeNumChildMask) >> MP_NodeOpFieldShift)

#define MP_GetNodeDictTagField(hdr) \
     ((hdr & MP_NodeDictTagMask) >> MP_NodeDictTagFieldShift)

#define MP_GetNodeCommonValueField(hdr) \
    ((hdr & MP_NodeCommonValueMask) >> MP_NodeCommonValueFieldShift)

/* see MP_Annotations.h for the defines for annotations */

/* Note:  plain cc (old style?) doesn't like signed char!! */
typedef signed char      MP_Sint8_t;
typedef unsigned char    MP_Uint8_t;
typedef unsigned short   MP_Uint16_t;   /* aren't real MP types yet */
typedef signed short     MP_Sint16_t;
typedef long             MP_Sint32_t;
typedef unsigned long    MP_Uint32_t;
typedef float            MP_Real32_t;
typedef double           MP_Real64_t;
typedef unsigned char    MP_Boolean_t;
typedef unsigned char    MP_Common_t;  /* for common types rep as a char */
typedef void *           MP_ApInt_t;
typedef MP_ApInt_t *   MP_ApInt_pt;
typedef void *          MP_ApReal_t;
typedef MP_ApReal_t *   MP_ApReal_pt;

typedef struct MP_Link_t MP_Link_t;
typedef MP_Link_t * MP_Link_pt;

/*
   encodings for the Multi dictionaries
*/
#define MP_BasicDict              0
#define MP_NoSemanticsDict      128
#define MP_ReceiverDict         129
#define MP_UnavailSemanticsDict 130
#define MP_LocalDict            155


/*
*  encodings for the Multi types plus some useful macros
*/
#define MP_Sint8Type                        65
#define MP_Uint8Type                        67
#define MP_BooleanType                      68
#define MP_CommonLatinIdentifierType        72
#define MP_CommonGreekIdentifierType        76
#define MP_CommonConstantType               80
#define MP_CommonMetaType                   84
#define MP_CommonMetaOperatorType           88

#define MP_StringType                       36
#define MP_ConstantType                     40
#define MP_IdentifierType                   44
#define MP_MetaType                         48
#define MP_MetaOperatorType                 52

#define MP_Sint32Type                       17
#define MP_Uint32Type                       19
#define MP_Real32Type                       18

#define MP_ApIntType                        9
#define MP_ApRealType                       10

#define  MP_Real64Type                      6

#define MP_OperatorType                     128
#define MP_CommonOperatorType               129

#define MP_RawType                          134

/* MP_ANY_TYPE is used by the internal get node packet routine */
#define MP_ANY_TYPE                     255


/* And here are some Handy Macros */
/* Basic types, i.e. those which appear as a leaf of a data tree
   have numerical values between 0 and 127 ,i.e. bit 8 is zero */
#define MP_IsBasicType(type)    (!((type) & 128))

/* native 8-bit types have numeric values between 64 and 127
   i.e. bit 8 is zero and bit 7 is one */
#define MP_Is8BitNumericType(type)    (!((type) & 128) && ((type) & 64))

/* native String-based types have numeric values between 32 an 63,
   i.e. bit 8,7 are zero, bit 6 is one */
#define MP_IsStringBasedType(type)  (!((type) & 192) && ((type) & 32))

/* native 32 bit types have numeric values beween 16 and 31, i.e. bits
   8,7,6, are zero, bit 5 is one. Furthermore, integers have bit 1 set
   to 1 and Reals have bit 1 set to 0 */
#define MP_Is32BitNumericType(type)    (!((type) & 224) && ((type) & 16))


/* Ap Numbers have bit 5-8 set to zero, bit 4 set to one */
#define MP_IsApNumberType(type) (!((type) & 240) && ((type) & 8))

/* Native 64-bit numbers have bit 4-8 set to zero, bit 3 set to one */
#define MP_Is64BitNumericType(type)    (type == MP_Real64Type)

/* Predicates for real and integers
   Integers are odd numbers, reals are even numbers, not divisble by 4*/
#define MP_IsRealType(type)   (MP_IsBasicType(type) &&    \
                                 !((type) & 1) &&           \
                                 ((type) >> 1) & 1)
#define MP_IsIntegerType(type)(MP_IsBasicType(type) && ((type) & 1))

#define MP_IsFixedIntegerType(type)                     \
(MP_IsIntegerType(type) && ! (type == MP_ApIntType))

#define MP_IsIdType(type)                       \
(type  == MP_IdentifierType ||                  \
 type == MP_CommonLatinIdentifierType ||        \
 type == MP_CommonGreekIdentifierType)

#define MP_IsCommonType(type) ((type >= 72 && type <= 88) || type == 129)
#endif /* _MP_Types_h */
