/***************************************************************************
 *
 *   HEADER FILE:  MP_ProtoDict.h
 *
 *      The prototype dictionary (definitions and declarations)
 *
 *
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_ProtoDict_h
#define _MP_ProtoDict_h

#define MP_ProtoDict 6

/***************************************************************************
 *
 * Meta type definitions
 *
 ***************************************************************************/
/* We use the same numerical values as we use for the MP types */
#define MP_CmtProto_2_MPType(cmt)   cmt

#define MP_CmtProtoIMP_Sint8                        65
#define MP_CmtProtoIMP_Uint8                        67
#define MP_CmtProtoIMP_Boolean                      68
#define MP_CmtProtoIMP_LatinIdentifier              72
#define MP_CmtProtoIMP_GreekIdentifier              76
#define MP_CmtProtoIMP_CommonConstant               80

#define MP_CmtProtoIMP_String                       36
#define MP_CmtProtoIMP_Constant                     40
#define MP_CmtProtoIMP_Identifier                   44

#define MP_CmtProtoIMP_Sint32                       17
#define MP_CmtProtoIMP_Uint32                       19
#define MP_CmtProtoIMP_Real32                       18

#define MP_CmtProtoIMP_ApInt                        9
#define MP_CmtProtoIMP_ApReal                       10

#define MP_CmtProtoIMP_Real64                       6

#define MP_CmtProtoRecStruct   128
#define MP_CmtProtoRecUnion    129

/* Basic types, i.e. those which appear as a leaf of a data tree
   have numerical values between 0 and 127 ,i.e. bit 8 is zero */
#define IMP_IsBasicMetaType(type)    (!((type) & 128))

/* native 8-bit types have numeric values between 64 and 127
   i.e. bit 8 is zero and bit 7 is one */
#define IMP_Is8BitNumericMetaType(type)    (!((type) & 128) && ((type) & 64))

/* native String-based types have numeric values between 32 an 63,
   i.e. bit 8,7 are zero, bit 6 is one */
#define IMP_IsStringBasedMetaType(type)  (!((type) & 192) && ((type) & 32))

/* native 32 bit types have numeric values beween 16 and 31, i.e. bits
   8,7,6, are zero, bit 5 is one. Furthermore, integers have bit 1 set
   to 1 and Reals have bit 1 set to 0 */
#define IMP_Is32BitNumericMetaType(type)    (!((type) & 224) && ((type) & 16))
#define IMP_Is16BitNumericMetaType(type) 0
/* Ap Numbers have bit 5-8 set to zero, bit 4 set to one */
#define IMP_IsApNumberMetaType(type) (!((type) & 240) && ((type) & 8))

/* Native 64-bit numbers have bit 4-8 set to zero, bit 3 set to one */
#define IMP_Is64BitNumericMetaType(type)    (type == MP_CmtProtoIMP_Real64)

/* Predicates for real and integers
   Integers are odd numbers, reals are even numbers, not divisble by 4*/
#define IMP_IsRealMetaType(type)   (IMP_IsBasicMetaType(type) &&    \
                                 !((type) & 1) &&           \
                                 ((type) >> 1) & 1)
#define IMP_IsIntegerMetaType(type)(IMP_IsBasicMetaType(type) && ((type) & 1))

/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
#define MP_CopProtoStruct                1
#define MP_CopProtoUnion                 2
#define MP_CopProtoArray                 3
#define MP_CopProtoPointer               4
#define MP_CopProtoRecStruct             5
#define MP_CopProtoRecUnion              6

/***************************************************************************
 *
 * Definition of Annots
 *
 ***************************************************************************/
#define MP_AnnotProtoPrototype           1

#endif /* _MP_ProtoDict_h */
