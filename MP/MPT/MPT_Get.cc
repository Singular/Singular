/******************************************************************
 *
 * File:    MPT_Get.c
 * Purpose: Routines which read data from an MP Link into an MPT Tree
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/

#include"MPT.h"
#include <string.h>
#include <memory.h>

#ifndef __64_BIT__
static MPT_Status_t MPT_GetReal64(MP_Link_pt link, MPT_Arg_t *oarg)
{
  MP_Real64_t *r64 = (MP_Real64_t*) IMP_MemAllocFnc(sizeof(MP_Real64_t));
  *oarg = r64;
  mp_return(IMP_GetReal64(link, r64));
}
#else
#define MPT_GetReal64(link, oarg)                        \
(IMP_GetReal64(link, (MP_Real64_pt) oarg) == MP_Success ?   \
 MPT_Success : MPT_SetError(MPT_MP_Failure));
#endif

/****************************************************************** 
 * FUNCTION:  MPT_GetTree
 * ARGUMENT:  link - pointer to a stream structure
 *            otree - pointer to a tree
 * RETURN:    MPT_Success, on success
 *            MPT error code, on error
 * PURPOSE:
 *
 * Retrieve an MP tree from the data stream associated with 
 * link and store it as an MPT tree. If everything went alright,
 * return MP_Success, otherwise, return the appropriate MPT error code.
 *
 ******************************************************************/
MPT_Status_t MPT_GetTree(MP_Link_pt link, MPT_Tree_pt *otree)
{
  MPT_Tree_pt tree;
  MPT_Status_t mpt_status;
  MPT_Node_pt node;
  MP_NodeType_t type;
  MP_Common_t nvalue  ;
  MP_DictTag_t  ndict;
  
  tree = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
  memset(tree, 0, sizeof(MPT_Tree_t));
  *otree = tree;

  /* First, get the node */
  mpt_failr(MPT_GetNode(link, &(tree->node)));

  node = tree->node;
  type = node->type;

  /* Get args, if necessary */
  if (type == MP_CommonOperatorType || type == MP_OperatorType)
  {
    /* Give the external application a shot to read in the args
       directly into its own data structures */
    mpt_status = MPT_GetExternalData(link, MPT_ARG_PT(&(tree->args)),
                                     tree->node);

    /* If args were read in directly, or an error occured while reading
       external args , return an error */
    if (mpt_status == MPT_Success)
    {
      tree->node->type = MPT_ExternalDataType;
      return MPT_Success;
    }
    else if (mpt_status !=  MPT_NotExternalData)
      return mpt_status;
    
    /* Otherwise, read in the args */
    /* However, we might have to push the current tree onto the stack
       of recursive typespecs */
    nvalue = MP_COMMON_T(node->nvalue);
    ndict = node->dict;
    
    if (ndict == MP_ProtoDict)
    {
      if (nvalue == MP_CopProtoRecUnion)
        MPT_PushRecUnion(tree);
      else if (nvalue == MP_CopProtoRecStruct)
        MPT_PushRecStruct(tree);
    }

    mpt_status = MPT_GetArgs(link, &(tree->args), node->numchild,
                             MPT_ProtoAnnotValue(node));

    if (ndict == MP_ProtoDict)
    {
      if (nvalue == MP_CopProtoRecUnion)
        MPT_PopRecUnion();
      else if (nvalue == MP_CopProtoRecStruct)
        MPT_PopRecStruct();
    }
    
    return mpt_status;
  }
  else
  {
    tree->args = NULL;
    return MPT_Success;
  }
}


MPT_Status_t MPT_GetNode(MP_Link_pt link, MPT_Node_pt *onode)
{
  MPT_Node_pt node = (MPT_Node_pt)IMP_MemAllocFnc(sizeof(MPT_Node_t));
  MP_NodeType_t type;
  MP_Common_t cvalue;
  memset(node, 0, sizeof(MPT_Node_t));
  
  *onode = node;

  /* Get the essential node info */
  mp_failr(IMP_GetNodeHeader(link, &(node->type),
                             &(node->dict),
                             &cvalue,
                             &(node->numannot),
                             &(node->numchild)));

  node->nvalue = MP_ARG_COMMON_T(cvalue);
  /* Get the node value, if necessary */
  type = node->type;

  if (MP_IsStringBasedType(type) ||
      type == MP_OperatorType ||
      type == MP_MetaOperatorType)
  {
    node->nvalue = NULL;
    mp_failr(IMP_GetStringBasedType(link, (char **) &(node->nvalue)));
  }
  else if (MP_Is32BitNumericType(type))
    mp_failr(IMP_GetUint32(link, MP_UINT32_PT(&(node->nvalue))));
  else if (MP_Is64BitNumericType(type))
    mpt_failr(MPT_GetReal64(link, &(node->nvalue)));
  else if (type == MP_ApIntType)
    mpt_failr(MPT_GetApInt(link, &(node->nvalue)));
  else if (type == MP_ApRealType)
    mpt_failr(MPT_GetApReal(link, &(node->nvalue)));
  else if (! (MP_IsCommonType(type) || MP_Is8BitNumericType(type)))
    return MPT_SetError(MPT_WrongNodeType);

  /* Get annots, if there are any */
  if (node->numannot > 0)
  {
    MP_Uint32_t i;
    node->annots
      = (MPT_Annot_pt *)IMP_MemAllocFnc(sizeof(MPT_Annot_pt)*node->numannot);

    for (i=0; i<node->numannot; i++)
      mpt_failr(MPT_GetAnnot(link, &(node->annots[i])));
  }
  else
    node->annots = NULL;

  return MPT_Success;
}

MPT_Status_t MPT_GetAnnot(MP_Link_pt link, MPT_Annot_pt *oannot)
{
  MPT_Annot_pt annot = (MPT_Annot_pt) IMP_MemAllocFnc(sizeof(MPT_Annot_t));
  memset(annot, 0, sizeof(MPT_Annot_t));
  *oannot = annot;

  /* Get fields of the annot */
  mp_failr(MP_GetAnnotationPacket(link,
                                  &(annot->dict),
                                  &(annot->type),
                                  &(annot->flags)));
  /* If it is valuated, then get the value */
  if (MP_IsAnnotValuated(annot->flags))
    return MPT_GetTree(link, &(annot->value));
  else
  {
    annot->value = NULL;
    return MPT_Success;
  }
}

MPT_Status_t MPT_GetArgs(MP_Link_pt link, MPT_Arg_pt *oargs, MP_NumChild_t nc,
                         MPT_Tree_pt typespec)
{
  MP_NumChild_t i;
  MPT_Arg_pt args;

  *oargs = NULL;

  if (nc == 0) return MPT_Success;
  
  /* Check for non-prototyped  data */
  if (typespec == NULL)
  {
    args = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
    *oargs = args;
    for (i=0; i<nc; i++)
      mpt_failr(MPT_GetTree(link, (MPT_Tree_pt *) &(args[i])));
  }
  else
  {
    /* Now we are in the realm of prototyped data */
    MPT_Node_pt pnode = typespec->node;
    MP_NodeType_t ptype = pnode->type;
    MP_DictTag_t  pdict = pnode->dict;

    /* Check only for arrays of basic numeric types */
    if (ptype == MP_CommonMetaType && pdict == MP_ProtoDict)
    {
      MP_Common_t metatype = MP_COMMON_T(pnode->nvalue);
      
      if (IMP_Is8BitNumericMetaType(metatype))
        mp_return(IMP_GetUint8Vector(link, (MP_Uint8_t **) oargs, nc));
      else if (IMP_Is32BitNumericMetaType(metatype))
        mp_return(IMP_GetUint32Vector(link, (MP_Uint32_t **) oargs, nc));
      else if (IMP_Is64BitNumericMetaType(metatype))
        mp_return(IMP_GetReal64Vector(link, (MP_Real64_t **) oargs, nc));
    }
  
    /* Everything else is handled item by item */
    args = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
    *oargs = args;
  
    for (i=0;i<nc; i++)
      mpt_failr(MPT_GetTypespecedArg(link, &(args[i]), typespec));
  }

  return MPT_Success;
}

MPT_Status_t MPT_GetTypespecedArg(MP_Link_pt link,MPT_Arg_t  *oarg,
                                  MPT_Tree_pt typespec)
{
  *oarg = NULL;

  if (typespec != NULL)
  {
    /* Non-trivial cases of typespeced data */
    MPT_Node_pt typespec_node = typespec->node;
    MPT_Arg_pt   typespec_args = typespec->args;
    MP_DictTag_t  ndict = typespec_node->dict;
    MP_NodeType_t ntype = typespec_node->type;
    MP_NumChild_t nc =  typespec_node->numchild;
    MP_Common_t   cvalue = MP_COMMON_T(typespec_node->nvalue);

    /* check special Common Meta types from ProtoDict */
    if (ndict == MP_ProtoDict && ntype == MP_CommonMetaType)
    {
      if (IMP_Is8BitNumericMetaType(cvalue))
        mp_return(IMP_GetUint8(link, MP_UINT8_PT(oarg)));
      else if (IMP_Is32BitNumericMetaType(cvalue))
        mp_return(IMP_GetUint32(link, MP_UINT32_PT(oarg)));
      else if (IMP_Is64BitNumericMetaType(cvalue))
        return MPT_GetReal64(link, oarg);
      else if (cvalue == MP_CmtProtoIMP_ApInt)
        return MPT_GetApInt(link, oarg);
      else if (cvalue == MP_CmtProtoIMP_ApReal)
        return MPT_GetApReal(link, oarg);
      else if (IMP_IsStringBasedMetaType(cvalue))
        mp_return(IMP_GetStringBasedType(link, (char **) oarg));
      /* Check for Rec Union and Rec Structure */
      else if (cvalue == MP_CmtProtoRecStruct)
        return MPT_GetTypespecedArg(link, oarg, MPT_RecStructTree);
      else if (cvalue == MP_CmtProtoRecUnion)
        return MPT_GetTypespecedArg(link, oarg, MPT_RecUnionTree);
    }

    /* special ProtoD Operators */
    if (ntype == MP_CommonOperatorType && ndict == MP_ProtoDict)
    {
      /* ProtoD:Union */
      if (cvalue == MP_CopProtoUnion || cvalue == MP_CopProtoRecUnion)
      {
        MPT_Union_pt mun = (MPT_Union_pt) IMP_MemAllocFnc(sizeof(MPT_Union_t));
        *oarg = (MPT_Arg_t) mun;
        mp_failr(IMP_GetUint32(link, &(mun->tag)));
      
        if (mun->tag == 0)
        {
          mun->arg = NULL;
          return MPT_Success;
        }
        else if (mun->tag <= nc)
          return MPT_GetTypespecedArg(link, &(mun->arg),
                                      MPT_TREE_PT(typespec_args[mun->tag-1]));
        else
          return MPT_SetError(MPT_WrongUnionDiscriminator);
      }

      /* ProtoD:Struct */
      if (cvalue == MP_CopProtoStruct || cvalue == MP_CopProtoRecStruct)
      {
        MPT_Arg_pt st_args = (MPT_Arg_pt)IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
        MP_NumChild_t i;
        
        *oarg = st_args;
        for (i=0; i<nc; i++)
          mpt_failr(MPT_GetTypespecedArg(link,&(st_args[i]),
                                         MPT_TREE_PT(typespec_args[i])));
        return MPT_Success;
      }
    }
  
    /* Meta Operators */
    if (ntype == MP_CommonMetaOperatorType || ntype == MP_MetaOperatorType)
    {
      MPT_Arg_pt *args;
      typespec = MPT_ProtoAnnotValue(typespec_node);
    
      if (nc == 0)
      {
        MPT_DynArgs_pt dynarg;

        mp_failr(IMP_GetUint32(link, &nc));
        dynarg = (MPT_DynArgs_pt) IMP_MemAllocFnc(sizeof(MPT_DynArgs_t));
        *oarg = dynarg;
        dynarg->length = nc;

        args = &(dynarg->args);
      }
      else
        args = (MPT_Arg_pt *) oarg;
    
      return MPT_GetArgs(link, args, nc, typespec);
    }
  }
  
  /* Everything else should be a tree */
  return MPT_GetTree(link, (MPT_Tree_pt *) oarg);
}



/******************************************************************
 *
 * Skipping over Data -- needs to be properly implemented
 *
 ******************************************************************/

MPT_Status_t MPT_SkipAnnots(MP_Link_pt link, MP_NumAnnot_t na,
                            MP_Boolean_t *req)
{
  MP_Uint32_t i;
  MP_Boolean_t lreq = 0;

  for (i=0; i<na; i++)
  {
    mpt_failr(MPT_SkipAnnot(link, req));
    lreq = (lreq || *req);
  }

  *req = lreq;
  return MPT_Success;
}

MPT_Status_t MPT_SkipAnnot(MP_Link_pt link, MP_Boolean_t *req)
{
  MP_AnnotType_t atype;
  MP_DictTag_t dtag;
  MP_AnnotFlags_t flags;

  mp_failr(MP_GetAnnotationPacket(link, &dtag, &atype, &flags));

  *req = MP_IsAnnotRequired(flags);
  
  if (MP_IsAnnotValuated(flags))
    return MPT_SkipTree(link);
  else
    return MPT_Success;
}

MPT_Status_t MPT_SkipTree(MP_Link_pt link)
{
  MPT_Tree_pt tree;

  mpt_failr(MPT_GetTree(link, &tree));
  MPT_DeleteTree(tree);

  return MPT_Success;
}

MPT_Status_t MPT_SkipArgs(MP_Link_pt link, MPT_Node_pt node)
{
  MPT_Arg_pt args;
  MP_NumChild_t nc = node->numchild;
  MPT_Tree_pt typespec = MPT_ProtoAnnotValue(node);
  
  mpt_failr(MPT_GetArgs(link, &args, nc, typespec));
  MPT_DeleteArgs(args, nc, typespec);
  return MPT_Success;
}


    
    
      
      
  
