/****************************************************************
 *
 *   HEADER FILE:  MP_Put.h
 *
 ***************************************************************/

#ifndef _MP_Put_h
#define _MP_Put_h

#include "MP.h"

EXTERN MP_Status_t  MP_PutAnnotationPacket _ANSI_ARGS_((MP_Link_pt link,
                                                        MP_DictTag_t dtag,
                                                        MP_AnnotType_t atype,
                                                        MP_AnnotFlags_t flags));

EXTERN MP_Status_t  MP_PutApIntPacket _ANSI_ARGS_((MP_Link_pt link,
                                                   MP_ApInt_t apint,
                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutApRealPacket _ANSI_ARGS_((MP_Link_pt link,
                                                    MP_ApReal_t apreal,
                                                    MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutSint32Packet _ANSI_ARGS_((MP_Link_pt link,
                                                    MP_Sint32_t  n,
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutUint32Packet _ANSI_ARGS_((MP_Link_pt link,
                                                    MP_Uint32_t  n,
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutSint8Packet _ANSI_ARGS_((MP_Link_pt link,
                                                   MP_Sint8_t n,
                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutUint8Packet _ANSI_ARGS_((MP_Link_pt link,
                                                   MP_Uint8_t n,
                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutBooleanPacket _ANSI_ARGS_((MP_Link_pt link,
                                                     MP_Boolean_t n,
                                                     MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutReal32Packet _ANSI_ARGS_((MP_Link_pt link,
                                                    MP_Real32_t n,
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutReal64Packet _ANSI_ARGS_((MP_Link_pt link,
                                                    MP_Real64_t n,
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                                                      MP_DictTag_t dtag,
                                                      char *op,
                                                      MP_NumAnnot_t num_annots,
                                                      MP_NumChild_t num_child));
EXTERN MP_Status_t  MP_PutMetaOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
						      MP_DictTag_t dtag,
						      char *op,
						      MP_NumAnnot_t num_annots,
						      MP_NumChild_t num_child));

EXTERN MP_Status_t  MP_PutCommonOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                                                            MP_DictTag_t dtag,
                                                            MP_Common_t op,
                                                            MP_NumAnnot_t num_annots,
                                                            MP_NumChild_t num_child));

EXTERN MP_Status_t  MP_PutCommonMetaOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
							    MP_DictTag_t dtag,
							    MP_Common_t op,
							    MP_NumAnnot_t num_annots,
							    MP_NumChild_t num_child));
EXTERN MP_Status_t  MP_PutCommonMetaTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                                        MP_DictTag_t dtag,
                                                        MP_Common_t cm,
                                                        MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutRawPacket _ANSI_ARGS_((MP_Link_pt link,
                                                 char *data,
                                                 MP_Uint32_t len,
                                                 MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutConstantPacket _ANSI_ARGS_((MP_Link_pt link,
                                                      MP_DictTag_t dtag,
                                                      char *cnst,
                                                      MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutCommonConstantPacket _ANSI_ARGS_((MP_Link_pt link,
                                                            MP_DictTag_t dtag,
                                                            MP_Common_t cnst,
                                                            MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                                        MP_DictTag_t dtag,
                                                        char *id,
                                                        MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutCommonLatinIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                                                   MP_DictTag_t dtag,
                                                                   MP_Common_t id,
                                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutCommonGreekIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                                                   MP_DictTag_t dtag,
                                                                   MP_Common_t id,
                                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutStringPacket _ANSI_ARGS_((MP_Link_pt link,
                                                    char *s,
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutMetaTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                                  MP_DictTag_t dtag, char *s,
                                                  MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  IMP_PutString _ANSI_ARGS_((MP_Link_pt link,
                                               char *s));

EXTERN MP_Status_t  IMP_PutOperator _ANSI_ARGS_((MP_Link_pt link,
                                                 char *op));

EXTERN MP_Status_t  IMP_PutMetaOperator _ANSI_ARGS_((MP_Link_pt link,
                                                     char *op));

EXTERN MP_Status_t  IMP_PutApInt _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApInt_t apint));

EXTERN MP_Status_t  IMP_PutApReal _ANSI_ARGS_((MP_Link_pt link,
                                               MP_ApReal_t apreal));

EXTERN MP_Status_t  IMP_PutBoolean _ANSI_ARGS_((MP_Link_pt link,
                                                MP_Boolean_t n));

EXTERN MP_Status_t  IMP_PutSint8 _ANSI_ARGS_((MP_Link_pt link,
                                              MP_Sint8_t  n));

EXTERN MP_Status_t  IMP_PutUint8 _ANSI_ARGS_((MP_Link_pt link,
                                              MP_Uint8_t  n));

EXTERN MP_Status_t  IMP_PutSint32 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Sint32_t n));

EXTERN MP_Status_t  IMP_PutUint32 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Uint32_t n));

EXTERN MP_Status_t  IMP_PutReal32 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Real32_t n));

EXTERN MP_Status_t  IMP_PutReal64 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Real64_t n));

EXTERN MP_Status_t  IMP_PutRaw _ANSI_ARGS_((MP_Link_pt link,
                                            char *rawdata,
                                            MP_Uint32_t len));

EXTERN MP_Status_t  IMP_PutNodeHeader _ANSI_ARGS_((MP_Link_pt link,
                                                   MP_NodeType_t ntype,
                                                   MP_DictTag_t dtag,
                                                   MP_Common_t cval,
                                                   MP_NumAnnot_t num_annots,
                                                   MP_NumChild_t num_child));

EXTERN MP_Status_t  IMP_PutStringBasedTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                                              char *s,
                                                              MP_NumAnnot_t num_annots,
                                                              MP_NodeType_t ntype));

/*
*  defines for putting a string-based type to the data stream.
*  The prototype
*        IMP_PutStringBasedTypePacket  (MP_Link_pt link, char *s,
*                MP_NumAnnot_t  num_annots, MP_NodeType _t ntype)
*  clarifies the type of the arguments.
*/

#define IMP_PutIdentifier(link, id) \
   (IMP_PutString(link, id))

#define IMP_PutMetaType(link, meta) \
   (IMP_PutString(link, meta))

#endif  /* _MP_put_h */
