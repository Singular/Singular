/****************************************************************
 *
 *   HEADER FILE:  MP_Get.h
 *
 ***************************************************************/

#ifndef _MP_Get_h
#define _MP_Get_h

EXTERN MP_Status_t  MP_GetAnnotationPacket _ANSI_ARGS_((MP_Link_pt link,
                                MP_DictTag_t *dtag, MP_AnnotType_t *atype,
                                MP_AnnotFlags_t *flags));

EXTERN MP_Status_t  MP_GetApIntPacket _ANSI_ARGS_((MP_Link_pt link,
                        MP_ApInt_pt apint, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetApRealPacket _ANSI_ARGS_((MP_Link_pt link,
                        MP_ApReal_pt apreal, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetSint32Packet _ANSI_ARGS_((MP_Link_pt link,
                                    MP_Sint32_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetUint32Packet _ANSI_ARGS_((MP_Link_pt link,
                                    MP_Uint32_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetSint8Packet _ANSI_ARGS_((MP_Link_pt link,
                                    MP_Sint8_t *n,  MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetUint8Packet _ANSI_ARGS_((MP_Link_pt link,
                                     MP_Uint8_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetBooleanPacket _ANSI_ARGS_((MP_Link_pt link,
                                   MP_Boolean_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetReal32Packet _ANSI_ARGS_((MP_Link_pt link,
                                    MP_Real32_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetReal64Packet _ANSI_ARGS_((MP_Link_pt link,
                                    MP_Real64_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                           MP_DictTag_t *dtag, char **op,
                          MP_NumAnnot_t *num_annots, MP_NumChild_t *num_child));

EXTERN MP_Status_t MP_GetCommonOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                         MP_DictTag_t *dtag, MP_Common_t *op,
                         MP_NumAnnot_t *num_annots,  MP_NumChild_t *num_child));

EXTERN MP_Status_t MP_GetCommonMetaTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                        MP_DictTag_t *dtag, MP_Common_t *cmt,
                                                  MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                              MP_DictTag_t *dtag, char **id,
                                              MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetCommonLatinIdentifierPacket _ANSI_ARGS_((MP_Link_pt
                                 link,  MP_DictTag_t *dtag, MP_Common_t *id,
                                 MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetCommonGreekIdentifierPacket _ANSI_ARGS_((MP_Link_pt
                             link, MP_DictTag_t *dtag, MP_Common_t *id,
                             MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetStringPacket _ANSI_ARGS_((MP_Link_pt link, char **s,
                                                          MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetConstantPacket _ANSI_ARGS_((MP_Link_pt link,
                                 MP_DictTag_t *dtag, char **cnst,
                                 MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetCommonConstantPacket _ANSI_ARGS_((MP_Link_pt link,
                            MP_DictTag_t *dtag, MP_Common_t *cnst,
                            MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetMetaTypePacket _ANSI_ARGS_((MP_Link_pt link,
                        MP_DictTag_t *dtag, char **m,
                        MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetRawPacket _ANSI_ARGS_((MP_Link_pt link, char **rawdata,
                        MP_Uint32_t *len,  MP_NumAnnot_t *num_annots));
EXTERN MP_Status_t IMP_GetUint8 _ANSI_ARGS_((MP_Link_pt link, MP_Uint8_t *n));
EXTERN MP_Status_t IMP_GetSint8 _ANSI_ARGS_((MP_Link_pt link, MP_Sint8_t *n));
EXTERN MP_Status_t IMP_GetBoolean _ANSI_ARGS_((MP_Link_pt link, MP_Boolean_t *n));
EXTERN MP_Status_t IMP_GetApInt _ANSI_ARGS_((MP_Link_pt link, MP_ApInt_pt apint));
EXTERN MP_Status_t IMP_GetApReal _ANSI_ARGS_((MP_Link_pt link, MP_ApReal_pt apreal));
EXTERN MP_Status_t IMP_GetReal32 _ANSI_ARGS_((MP_Link_pt link, MP_Real32_t *n));
EXTERN MP_Status_t IMP_GetReal64 _ANSI_ARGS_((MP_Link_pt link, MP_Real64_t *n));
EXTERN MP_Status_t IMP_GetSint32 _ANSI_ARGS_((MP_Link_pt link, MP_Sint32_t *n));
EXTERN MP_Status_t IMP_GetUint32 _ANSI_ARGS_((MP_Link_pt link, MP_Uint32_t *n));

EXTERN MP_Status_t IMP_GetNodeHeader _ANSI_ARGS_((MP_Link_pt link,
                          MP_NodeType_t *ntype, MP_DictTag_t *dtag,
                          MP_Common_t *cv, MP_NumAnnot_t *num_annots,
                          MP_NumChild_t *num_child));

EXTERN MP_Status_t IMP_GetStringBasedType _ANSI_ARGS_((MP_Link_pt link,
                                                        char **s));

EXTERN MP_Status_t IMP_GetRaw _ANSI_ARGS_((MP_Link_pt link, char **rawdata,
                                           MP_Uint32_t *len));

EXTERN MP_Status_t IMP_GetOperator _ANSI_ARGS_((MP_Link_pt link, char **op));

/*
 *  defines for getting a string-based type from the data stream.
 */

#define IMP_GetIdentifier(link, id) \
   (IMP_GetStringBasedType(link, id))

#define IMP_GetString(link, str) \
   (IMP_GetStringBasedType(link, str))

#define IMP_GetMetaType(link, meta) \
   (IMP_GetStringBasedType(link, meta))

#define MP_GetCommonMetaOperatorPacket(link, dtag, op, num_annots, num_child) \
   (MP_GetCommonOperatorPacket(link, dtag, op, num_annots, num_child))

#define MP_GetMetaOperatorPacket(link, dtag, op, num_annots, num_child) \
   (MP_GetOperatorPacket(link, dtag, op, num_annots, num_child))

#define IMP_GetMetaOperator(link, op) \
   (IMP_GetStringBasedType(link, op))

#endif /* _MP_Get_h */
