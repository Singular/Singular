/******************************************************************
 *
 * File:    MPT.h
 * Purpose: Main Header file for MP Tree routines
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 11/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/
#ifndef _MPT_H_
#define _MPT_H_

#include "MP.h"

/*************************************************************** 
 * 
 * Declare all names of types that we use
 * 
 ***************************************************************/
struct MPT_Tree_t;
typedef MPT_Tree_t * MPT_Tree_pt;

struct MPT_Node_t;
typedef MPT_Node_t * MPT_Node_pt;

struct MPT_Annot_t;
typedef MPT_Annot_t * MPT_Annot_pt;

typedef void * MPT_Arg_t;
typedef MPT_Arg_t * MPT_Arg_pt;

struct MPT_Union_t;
typedef MPT_Union_t * MPT_Union_pt;

struct MPT_DynArgs_t;
typedef MPT_DynArgs_t * MPT_DynArgs_pt;

typedef void * MPT_ExternalData_t;

/*************************************************************** 
 * 
 * Tree declarations
 * 
 ***************************************************************/

/* A Tree consists of */
struct MPT_Tree_t
{
  MPT_Node_pt     node;  /* a node  */
  MPT_Arg_pt      args;  /* and possibly of arguments */
};

/*************************************************************** 
 * 
 * Node declaration
 * 
 ***************************************************************/
/* A node has a */
struct MPT_Node_t
{
  MP_NodeType_t     type; 
  MP_DictTag_t      dict; /* may have a defined semantics in a dictionary */
  MPT_Arg_t         nvalue; /* always has a (node) value */
  MP_NumChild_t     numchild; /* may specify that numchild args follow */
  MP_NumAnnot_t     numannot; /* always may have 0 or more annots */
  MPT_Annot_pt      *annots; /* which are stored here */
};

/*************************************************************** 
 * 
 * Annotation declaration 
 * 
 ***************************************************************/
/* An annot has */
struct MPT_Annot_t
{
  MP_AnnotFlags_t   flags; /* as defined in the MP spec */
  MP_AnnotType_t    type;  /* as defined in some dict*/
  MP_DictTag_t      dict;  /* always need to be defined in a dict */
  MPT_Tree_pt       value; /* and may have a tree as value */
};

/*************************************************************** 
 * 
 * Argument declaration
 *
 * The problem is that we need to compactly represent (arrays and
 * 8, 16, 32, and 64 bit (numeric) quantities. Hence, we can not
 * just do
 *
 * union MPT_Arg_t { ... all types ... };
 * typedef MPT_Arg_t * MPT_Args_pt;
 *
 * since in this case sizeof(MPT_Arg_t) == sizeof(type_with_max_size);
 *
 * Hence, we need to let MPT_Arg_t be (void *) and cast it to whatever
 * it is supposed to be (which is determined from the context)
 *
 * We provide Macros for doing that
 ***************************************************************/
/* First of all, an argument may have one of the basic types */
/* For the following, we assume that sizeof(void *) >= sizeof(type) */
#define MP_BOOLEAN_T(arg)      ((MP_Boolean_t) (MP_UINT32_T(arg)))
#define MP_ARG_BOOLEAN_T(arg)  ((MPT_Arg_t)((MP_Boolean_t) (MP_UINT32_T(arg))))
#define MP_COMMON_T(arg)       ((MP_Common_t)  (MP_UINT32_T(arg)))
#define MP_ARG_COMMON_T(arg)   ((MPT_Arg_t)((MP_Common_t)  (MP_UINT32_T(arg))))
#define MP_SINT8_T(arg)        ((MP_Sint8_t)  (MP_SINT32_T(arg)))
#define MP_ARG_SINT8_T(arg)    ((MPT_Arg_t)((MP_Sint8_t)  (MP_SINT32_T(arg))))
#define MP_UINT8_T(arg)        ((MP_Uint8_t) (MP_UINT32_T(arg)))
#define MP_ARG_UINT8_T(arg)    ((MPT_Arg_t)((MP_Uint8_t) (MP_UINT32_T(arg))))
#define MP_SINT32_T(arg)       ((MP_Sint32_t) (arg))
#define MP_ARG_SINT32_T(arg)   ((MPT_Arg_t)((MP_Sint32_t) (arg)))
#define MP_UINT32_T(arg)       ((MP_Uint32_t) (arg))
#define MP_ARG_UINT32_T(arg)   ((MPT_Arg_t)((MP_Uint32_t) (arg)))
#define MP_REAL32_T(arg)       ((MP_Real32_t)  (arg))
#define MP_ARG_REAL32_T(arg)   ((MPT_Arg_t)((MP_Real32_t)  (arg)))
#ifndef __64_BIT__
/* sizeof(void *) < sizeof(MP_Real64_t) */
#define MP_REAL64_T(arg)       (*((MP_Real64_t *) (arg)))
#else
/* sizeof(void *) >= sizeof(MP_Real64_t) */
#define MP_REAL64_T(arg)       ((MP_Real64_t)  (arg))
#endif
#define MP_ARG_REAL64_T(arg)   ((MPT_Arg_t)MP_REAL64_T(arg))
#define MP_APINT_T(arg)        ((MP_ApInt_t) (arg))
#define MP_ARG_APINT_T(arg)    ((MPT_Arg_t)((MP_ApInt_t) (arg)))
#define MP_APREAL_T(arg)       ((MP_ApReal_t) (arg))
#define MP_ARG_APREAL_T(arg)   ((MPT_Arg_t)((MP_ApReal_t) (arg)))
#define MP_STRING_T(arg)       ((char *)   (arg))
#define MP_ARG_STRING_T(arg)   ((MPT_Arg_t)((char *)   (arg)))
#define MPT_ARG_T(arg)         ((MPT_Arg_t) (arg))

/* Now we come to "structured" or composite types */
/* For ProtoD:Union */
#define MPT_UNION_PT(arg)      ((MPT_Union_pt) (arg))
/* for MetaOps with numchild == 0 */
#define MPT_DYNARGS_PT(arg)     ((MPT_DynArgs_pt) (arg))
/* For ProtoD:Struct, and ProtoD:Ptr, and MetaOps with numchild > 0 */
#define MPT_ARG_PT(arg)        ((MPT_Arg_pt) arg)
/* For user defined Meta types or unspecified types */
#define MPT_TREE_PT(arg)       ((MPT_Tree_pt) arg)

/* And here are Macros tofor dealing with MPT_Arg_pt's */

#define MP_BOOLEAN_PT(arg_pt)      ((MP_Boolean_t *) (arg_pt))
#define MP_COMMON_PT(arg_pt)       ((MP_Common_t *)  (arg_pt))
#define MP_SINT8_PT(arg_pt)        ((MP_Sint8_t *) (arg_pt))
#define MP_UINT8_PT(arg_pt)        ((MP_Uint8_t *) (arg_pt))
#define MP_SINT32_PT(arg_pt)       ((MP_Sint32_t *) (arg_pt))
#define MP_UINT32_PT(arg_pt)       ((MP_Uint32_t *) (arg_pt))
#define MP_REAL32_PT(arg_pt)       ((MP_Real32_t *)  (arg_pt))
#define MP_REAL64_PT(arg_pt)       ((MP_Real64_t *)  (arg_pt))


/* Argument of Dynamic length -- used for MetaOperators with numchil == 0 */
struct MPT_DynArgs_t
{
  MP_Uint32_t   length; /* Length of array pointed to by */
  MPT_Arg_pt    args;    
};

/* Union -- for ProtoD:Union data*/
struct MPT_Union_t
{
  MP_Uint32_t   tag; /* Identifying type of the arg */
  MPT_Arg_t     arg; /* different typespecs */
};


/* Furthermore, we need a numeric value which identifies external data in
   node->type */
#define MPT_ExternalDataType    255

/*************************************************************** 
 * 
 * From MPT_Error.c 
 * 
 ***************************************************************/

/* Enumeration of MPT specific errors */
typedef enum MPT_Errors
{
  MPT_Failure,
  MPT_Success,
  MPT_MP_Failure,
  MPT_UnknownType,
  MPT_OutOfMemory,
  MPT_WrongNodeType,
  MPT_WrongTreeSyntax,
  MPT_NoPointerTypespec,
  MPT_WrongUnionDiscriminator,
  MPT_ViolatedAssumption,
  MPT_NotExternalData,
  MPT_WrongApIntFormat,
  MPT_MaxError
} MPT_Errors;

typedef MPT_Errors MPT_Status_t;

/* global MPT error variable */
extern MPT_Status_t MPT_errno;

/* defined in MPT_Error.c */
extern char *MPT_errlist[];

extern MPT_Status_t MPT_CheckStatus();
extern char* MPT_ErrorStr(MPT_Status_t error);
extern char* MPT_LinkErrorStr(MP_Link_pt link, MPT_Status_t error);

/* #define MPT_DEBUG 1 */
#ifdef MPT_DEBUG
/* For Debugging, it is more convenient to have the following defined
   as functions. This way, we can easily set a breakpoints there. */
extern MPT_Status_t MPT_SetError(MPT_Status_t error);
extern MPT_Status_t MPT_ClearError();
extern void MPT_Assume(MP_Boolean_t value);
#else
#define MPT_SetError(error) MPT_errno = (error)
#define MPT_ClearError()    MPT_errno = MPT_Success
#define MPT_Assume(x) ((void)0)
#endif

/*************************************************************** 
 * 
 * From MPT_Tree.c Copying/Deleting a tree
 * 
 ***************************************************************/
extern void MPT_DeleteTree(MPT_Tree_pt tree);
extern void MPT_DeleteNode(MPT_Node_pt node);
extern void MPT_DeleteArgs(MPT_Arg_pt args, MP_NumChild_t nc,
                           MPT_Tree_pt typespec);
extern void MPT_DeleteTypespecedArg(MPT_Arg_t arg, MPT_Tree_pt typespec);

extern void MPT_CpyTree(MPT_Tree_pt *dest, MPT_Tree_pt src);
extern void MPT_CpyNode(MPT_Node_pt *odest, MPT_Node_pt src);
extern void MPT_CpyArgs(MPT_Arg_pt *oargs, MPT_Arg_pt args,
                        MP_NumChild_t nc, MPT_Tree_pt typespec);
extern void MPT_CpyTypespecedArg(MPT_Arg_t *oarg, MPT_Arg_t arg,
                                 MPT_Tree_pt typespec);

/*************************************************************** 
 * 
 * From MPT_Get.c
 * 
 ***************************************************************/

extern MPT_Status_t MPT_GetTree(MP_Link_pt link, MPT_Tree_pt *otree);
extern MPT_Status_t MPT_GetNode(MP_Link_pt link, MPT_Node_pt *onode);
extern MPT_Status_t MPT_GetAnnot(MP_Link_pt link, MPT_Annot_pt *oanot);
extern MPT_Status_t MPT_GetArgs(MP_Link_pt link, MPT_Arg_pt *oargs,
                                MP_NumChild_t nc, MPT_Tree_pt typespec);
extern MPT_Status_t MPT_GetTypespecedArg(MP_Link_pt link, MPT_Arg_t  *oarg,
                                         MPT_Tree_pt typespec);
extern MPT_Status_t MPT_SkipTree(MP_Link_pt link);
extern MPT_Status_t MPT_SkipArgs(MP_Link_pt link, MPT_Node_pt node);
extern MPT_Status_t MPT_SkipAnnot(MP_Link_pt link, MP_Boolean_t *req);
extern MPT_Status_t MPT_SkipAnnots(MP_Link_pt link, MP_NumAnnot_t na,
                                   MP_Boolean_t *req);

/*************************************************************** 
 * 
 * From MPT_Put.c
 * 
 ***************************************************************/
extern MPT_Status_t MPT_PutTree(MP_Link_pt link, MPT_Tree_pt tree);
extern MPT_Status_t MPT_PutNode(MP_Link_pt link, MPT_Node_pt node);
extern MPT_Status_t MPT_PutArgs(MP_Link_pt link, MPT_Arg_pt args,
                                MP_NumChild_t nc,
                                MPT_Tree_pt typespec);
extern MPT_Status_t MPT_PutTypespecedArg(MP_Link_pt link, MPT_Arg_t arg,
                                         MPT_Tree_pt typespec);

/*************************************************************** 
 * 
 * From MPT_ApNumber.c
 * 
 ***************************************************************/

extern MPT_Status_t MPT_Init(MP_Env_pt env);
extern int  MPT_GetApIntFormat();
extern void (*MPT_InitCopyApInt)(MPT_Arg_t *dest, MPT_Arg_t src);
extern void (*MPT_DeleteApInt)(MPT_Arg_t apint);
extern void MPT_InitCopyApReal(MPT_Arg_t *dest, MPT_Arg_t src);
extern void MPT_DeleteApReal(MPT_Arg_t apreal);
MPT_Status_t MPT_GetApInt(MP_Link_pt link, MPT_Arg_t *arg);
MPT_Status_t MPT_PutApInt(MP_Link_pt link, MPT_Arg_t arg);
MPT_Status_t MPT_GetApReal(MP_Link_pt link, MPT_Arg_t *apreal);
MPT_Status_t MPT_PutApReal(MP_Link_pt link, MPT_Arg_t arg);

/*************************************************************** 
 * 
 * From MPT_Misc.c
 * 
 ***************************************************************/
inline MP_Boolean_t MPT_IsNode(MPT_Node_pt node,
                               MP_NodeType_t type,
                               MP_DictTag_t dict)
{
  return (node->type == type && node->dict == dict);
}

inline MP_Boolean_t MPT_IsNode(MPT_Node_pt node,
                               MP_NodeType_t type,
                               MP_DictTag_t dict,
                               MP_Common_t  cvalue)
{
  return (node->type == type && node->dict == dict &&
          MP_COMMON_T(node->nvalue) == cvalue);
}

inline MP_Boolean_t MPT_IsNode(MPT_Node_pt node,
                               MP_NodeType_t type,
                               MP_DictTag_t dict,
                               MP_Common_t  cvalue,
                               MP_NumChild_t nc)
{
  return (node->type == type && node->dict == dict &&
           MP_COMMON_T(node->nvalue) == cvalue && node->numchild == nc);
}

inline MPT_Annot_pt MPT_Annot(MPT_Node_pt node, MP_DictTag_t dict,
                              MP_AnnotType_t atype)
/* Searches for annot of type atype and dicionary tag dict in annots
   attached to node. If one is found, then the first one is
   returned, otherwise, NULL is returned */
{
  MP_NumAnnot_t numannot;
  MP_Uint32_t i;
  MPT_Annot_pt *annots, an;

  MPT_Assume(node != NULL);

  numannot = node->numannot;
  if (numannot == 0) return NULL;

  annots = node->annots;
  MPT_Assume(annots != NULL);

  for (i=0; i<numannot; i++)
  {
    an = annots[i];
    if (an->type == atype && an->dict == dict) return an;
  }

  return NULL;
}

inline MPT_Tree_pt MPT_AnnotValue(MPT_Node_pt node, MP_DictTag_t dict,
                                  MP_AnnotType_t atype)
{
  MPT_Annot_pt annot = MPT_Annot(node, dict, atype);
  if (annot != NULL && annot->value != NULL)
    return annot->value;
  return NULL;
}

#define MPT_ProtoAnnotValue(node) \
  MPT_AnnotValue(node, MP_ProtoDict, MP_AnnotProtoPrototype)

extern void MPT_MoveAnnots(MPT_Node_pt from, MPT_Node_pt to);
extern void MPT_RemoveAnnot(MPT_Node_pt node,
                            MP_DictTag_t dict,
                            MP_AnnotType_t type);
#define MPT_RemoveProtoAnnot(node) \
MPT_RemoveAnnot(node, MP_ProtoDict, MP_AnnotProtoPrototype)

extern MP_Boolean_t MPT_IsTrueProtoTypeSpec(MPT_Tree_pt typespec);
inline MPT_Tree_pt MPT_TrueProtoAnnotValue(MPT_Node_pt node)
{
  MPT_Tree_pt ptree = MPT_ProtoAnnotValue(node);
  if (ptree != NULL && MPT_IsTrueProtoTypeSpec(ptree))
    return ptree;
  
  return NULL;
}

extern void*       IMP_MemAlloc0Fnc(size_t size);
extern MPT_Tree_pt MPT_InitTree(MP_Common_t type,
                                MP_DictTag_t dict,
                                MPT_Arg_t nvalue,
                                MP_NumAnnot_t num_annot,
                                MP_NumChild_t num_child);
#define MPT_InitBasicTree(type, arg) MPT_InitTree(type, 0, arg, 0, 0)

extern void MPT_PushRecStruct(MPT_Tree_pt tree);
extern void MPT_PopRecStruct();
extern void MPT_PushRecUnion(MPT_Tree_pt tree);
extern void MPT_PopRecUnion();
extern MPT_Tree_pt MPT_RecStructTree;
extern MPT_Tree_pt MPT_RecUnionTree;

/*************************************************************** 
 * 
 * From MPT_TreeManips.c
 * 
 ***************************************************************/
extern MPT_Tree_pt MPT_UntypespecTree(MPT_Tree_pt tree);
extern MPT_Arg_pt MPT_UntypespecArgs(MPT_Arg_pt args,
                                     MP_NumChild_t nc,
                                     MPT_Tree_pt typespec);
extern MPT_Tree_pt MPT_UntypespecArg(MPT_Arg_t arg, MPT_Tree_pt typespec);

/*************************************************************** 
 * 
 * From MPT_PolyManips.c
 * 
 ***************************************************************/
extern MPT_Tree_pt MPT_DDP_2_ExpTree(MPT_Tree_pt tree);
extern MP_Boolean_t MPT_IsExpVectorTypespec(MPT_Tree_pt typespec,
                                            MP_NumChild_t *num_vars);

/*************************************************************** 
 * 
 * From MPT_ExternalData.c
 * 
 ***************************************************************/

extern void (*MPT_DeleteExternalData)(MPT_Arg_t edata);
extern void (*MPT_CpyExternalData)(MPT_Arg_t *src,
                                   MPT_Arg_t dest);
extern MPT_Status_t (*MPT_GetExternalData)(MP_Link_pt link,
                                           MPT_Arg_t *data,
                                           MPT_Node_pt node);
extern MPT_Status_t (*MPT_PutExternalData)(MP_Link_pt link,
                                           MPT_Arg_t data);
extern void MPT_DefaultDeleteExternalData(MPT_Arg_t edata);
extern void MPT_DefaultCpyExternalData(MPT_Arg_t *src,
                                       MPT_Arg_t dest);
extern MPT_Status_t MPT_DefaultGetExternalData(MP_Link_pt link,
                                               MPT_Arg_t *data,
                                               MPT_Node_pt node);
extern MPT_Status_t MPT_DefaultPutExternalData(MP_Link_pt link,
                                               MPT_Arg_t data);

/*************************************************************** 
 * 
 * from MPT_PutPoly.c
 * 
 ***************************************************************/
typedef void* MPT_ExternalPoly_t;
typedef void* MPT_ExternalMonomial_t;
typedef void* MPT_ExternalCoeff_t;
typedef void* MPT_ExternalIdeal_t;
typedef void* MPT_External2DMatrix_t;
typedef void* MPT_External2DPolyMatrix_t;


typedef struct MPT_PutPolyFncs_t
{
  MP_Sint32_t (*GetCharacteristic) (MPT_ExternalPoly_t poly);
  MP_Uint32_t (*GetNumberOfVariables) (MPT_ExternalPoly_t poly);
  char *      (*GetVarName) (MPT_ExternalPoly_t poly, MP_Uint32_t varnumber);
  MP_Common_t (*GetMonomialOrdering) (MPT_ExternalPoly_t poly);
  MP_Uint32_t (*GetNumberOfMonomials) (MPT_ExternalPoly_t poly);
  MP_Status_t (*PutCoeffFillExpVector) (MP_Link_pt link,
                                        MPT_ExternalPoly_t poly,
                                        MP_Uint32_t monomial,
                                        MP_Uint32_t varnumber,
                                        MP_Sint32_t *ex_pvector);
} MPT_PutPolyFncs_t;
typedef MPT_PutPolyFncs_t* MPT_PutPolyFncs_pt;

typedef struct MPT_PutIdealFncs_t
{
  MP_Uint32_t (*GetNumberOfPolys) (MPT_ExternalIdeal_t ideal);
  MPT_ExternalPoly_t (*GetPoly) (MPT_ExternalIdeal_t ideal,
                                 MP_Uint32_t polynumber);
  MPT_PutPolyFncs_pt polyfncs;
} MPT_PutIdealFncs_t;
typedef MPT_PutIdealFncs_t * MPT_PutIdealFncs_pt;

typedef struct MPT_Put2DMatrixFncs_t
{
  void (*GetDimension) (MPT_External2DMatrix_t matrix,
                        MP_Uint32_t *m,
                        MP_Uint32_t *n);
  MP_Status_t   (*PutElType) (MP_Link_pt link, MPT_External2DMatrix_t matrix);
  MP_Status_t (*PutEl) (MP_Link_pt link,
                        MPT_External2DMatrix_t matrix,
                        MP_Uint32_t m,
                        MP_Uint32_t n);
} MPT_Put2DMatrixFncs_t;
typedef MPT_Put2DMatrixFncs_t*  MPT_Put2DMatrixFncs_pt;

typedef struct MPT_Put2DPolyMatrixFncs_t
{
  void (*GetDimension) (MPT_External2DPolyMatrix_t matrix,
                        MP_Uint32_t *m,
                        MP_Uint32_t *n);
  MPT_ExternalPoly_t (*GetPoly)   (MPT_External2DPolyMatrix_t matrix,
                                   MP_Uint32_t m,
                                   MP_Uint32_t n);
  MPT_PutPolyFncs_pt polyfncs;
} MPT_Put2DPolyMatrixFncs_t;
typedef MPT_Put2DPolyMatrixFncs_t*  MPT_Put2DPolyMatrixFncs_pt;



extern MP_Status_t MPT_PutDDPoly(MP_Link_pt link,
                                 MPT_ExternalPoly_t poly,
                                 MPT_PutPolyFncs_pt polyfncs);
extern MP_Status_t MPT_PutPolyAnnots(MP_Link_pt link,
                                     MPT_ExternalPoly_t poly,
                                     MPT_PutPolyFncs_pt polyfncs);
extern MP_Status_t MPT_PutPolyData(MP_Link_pt link,
                                   MPT_ExternalPoly_t poly,
                                   MP_Uint32_t nummonimials,
                                   MPT_PutPolyFncs_pt polyfncs);
extern MP_Status_t MPT_PutDDPIdeal(MP_Link_pt link,
                                   MPT_ExternalIdeal_t ideal,
                                   MPT_PutIdealFncs_pt polyfncs);
extern MP_Status_t MPT_PutDense2DMatrix(MP_Link_pt link,
                                        MPT_External2DMatrix_t matrix,
                                        MPT_Put2DMatrixFncs_pt matrixfncs);
extern MP_Status_t MPT_Put2DPolyMatrix(MP_Link_pt link,
                                       MPT_External2DPolyMatrix_t matrix,
                                       MPT_Put2DPolyMatrixFncs_pt mfncts);

/*************************************************************** 
 * 
 * from MPT_GetPoly.c
 * 
 ***************************************************************/
extern MP_Boolean_t MPT_IsDDPTree(MPT_Tree_pt tree,
                                  MP_Sint32_t *characteristic,
                                  MPT_Tree_pt *var_tree,
                                  MP_Common_t *ordering);
extern MPT_Tree_pt MPT_GetVarNamesTree(MPT_Node_pt node,
                                       MP_NumChild_t num_vars);
extern MP_Boolean_t MPT_IsIdealTree(MPT_Tree_pt tree,
                                    MP_Sint32_t *characteristic,
                                    MPT_Tree_pt *var_tree,
                                    MP_Common_t *ordering);
  
/*************************************************************** 
 * 
 * Useful Macros
 * 
 ***************************************************************/
#define mpt_failr(cond)                              \
do {                                                 \
  MPT_Status_t _mpt_status = cond;                   \
  if (_mpt_status != MPT_Success) return _mpt_status;\
} while (0)

#define mp_failr(cond)                                              \
do                                                                  \
{                                                                   \
  if (((MP_Status_t) (cond)) != MP_Success) return MPT_SetError(MPT_MP_Failure);    \
} while (0)


#define mp_return(stmnt)                                            \
do                                                                  \
{                                                                   \
  if ((stmnt) != MP_Success)  return MPT_SetError(MPT_MP_Failure);  \
  else return MPT_Success;                                          \
}                                                                   \
while (0)

#define failr(cond)                                              \
do                                                                  \
{                                                                   \
  if ((cond) != MP_Success) return MP_Failure;                    \
} while (0)

/******************************************************************
 *
 * some handy defines
 *
 ******************************************************************/
#define MP_AnnotReqValTree  (MP_AnnotRequired | MP_AnnotValuated | MP_AnnotTreeScope)

#define MP_AnnotReqValNode  (MP_AnnotRequired | MP_AnnotValuated)

#endif /* _MPT_H_ */

