/******************************************************************
 *
 * File:    MPT_Put.c
 * Purpose: Putting a MPT tree to an MP Link
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/
#include "MPT.h"

/* The following needs to be update */
#if 0 
MPT_Status_t MPT_PutTree(MP_Link_pt link, MPT_Tree_pt tree)
{
  if (tree != NULL)
  {
    MPT_Node_pt node = tree->node;
    MP_NodeType_t type = node->type;

    mpt_failr(MPT_PutNode(link, node));

    /* Put the args, if there are any */
    if (type == MP_CommonOperatorType || type == MP_OperatorType)
    {
      MP_Common_t nvalue = MP_COMMON_T(node->nvalue);
      MP_DictTag_t  ndict = node->dict;

      /* we might have to push the current tree onto the stack of
         recursive typespecs */
      if (ndict == MP_ProtoDict)
      {
        if (nvalue == MP_CopProtoRecUnion)
          MPT_PushRecUnion(tree);
        else if (nvalue == MP_CopProtoRecStruct)
          MPT_PushRecStruct(tree);
      }

      /* Put Args */
      mpt_failr(MPT_PutArgs(link, tree->args, node->numchild,
                            MPT_ProtoAnnotValue(node)));

      /* And, pop the recursive typesepcs again */
      if (ndict == MP_ProtoDict)
      {
        if (nvalue == MP_CopProtoRecUnion)
          MPT_PopRecUnion();
        else if (nvalue == MP_CopProtoRecStruct)
          MPT_PopRecStruct();
      }
      return MPT_Success;
    }
    else if (type == MPT_ExternalDataType)
      return MPT_PutExternalData(link, tree->args);
    /* else, it is a node which has no arguments */
    else return MPT_Success;
  }
  else
    return MPT_Success;
}

MPT_Status_t MPT_PutNode(MP_Link_pt link, MPT_Node_pt node)
{
  MP_NumAnnot_t numannot = node->numannot;
  MP_NodeType_t type = node->type;

  if (type != MPT_ExternalDataType)
  {
    /* Put the node Header */
    mp_failr(IMP_PutNodeHeader(link, type, node->dict, 
			       MP_COMMON_T(node->nvalue), 
                               numannot, node->numchild));

    /* Put the node value, if necessary */
    if (MP_IsStringBasedType(type) ||
        type == MP_OperatorType)
      mp_failr(IMP_PutString(link, MP_STRING_T(node->nvalue)));
    else if (MP_Is32BitNumericType(type))
      mp_failr(IMP_PutUint32(link, MP_UINT32_T(node->nvalue)));
    else if (MP_Is64BitNumericType(type))
      mpt_failr(IMP_PutReal64(link, MP_REAL64_T(node->nvalue)));
    else if (type == MP_ApIntType)
      mpt_failr(MPT_PutApInt(link, node->nvalue));
    else if (type == MP_ApRealType)
      mpt_failr(MPT_PutApReal(link, node->nvalue));
    else if (! MP_IsCommonType(type) || MP_Is8BitNumericType(type))
      return MPT_SetError(MPT_WrongNodeType);
    
    /* take care of annots, if there are any */
    if (numannot > 0)
    {
      MP_Uint32_t i;
      MPT_Annot_pt *annots = node->annots, annot;

      MPT_Assume(annots != NULL);
      for (i=0; i<numannot; i++)
      {
        annot = annots[i];
        MPT_Assume(annot != NULL);
        mp_failr(MP_PutAnnotationPacket(link, annot->dict, annot->type, 
                                        annot->flags));
        if (MP_IsAnnotValuated(annot->flags))
          mpt_failr(MPT_PutTree(link, annot->value));
      }
    }
  }

  return MPT_Success;
}
  

MPT_Status_t MPT_PutArgs(MP_Link_pt link, MPT_Arg_pt args, MP_NumChild_t nc,
                         MPT_Tree_pt typespec)
{
  MP_NumChild_t i;
  
  /* return, if args == NULL, i.e. node is a leave node of a tree */
  if (args == NULL || nc == 0) return MPT_Success;

  /* Check for non-prototyped  data */
  if (typespec == NULL)
  {
    for (i=0; i<nc; i++)
      mp_failr(MPT_PutTree(link, MPT_TREE_PT(args[i])));
    return MPT_Success;
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
      
      if (IMP_Is8BitMetaType(metatype))
        mp_return(IMP_Put8BitVector(link, MP_UINT8_PT(args), nc));
      else if (IMP_Is32BitNumericMetaType(metatype))
        mp_return(IMP_Put32BitVector(link, MP_UINT32_PT(args), nc));
      else if (IMP_Is64BitNumericMetaType(metatype))
        mp_return(IMP_Put8BitVector(link, MP_REAL64_PT(args), nc));
    }

    /* Everything else is handled item by item */
    for (i=0;i<nc; i++)
      mpt_failr(MPT_PutTypespecedArg(link, args[i], typespec));

    return MPT_Success;
  }
}


MPT_Status_t MPT_PutTypespecedArg(MP_Link_pt link, MPT_Arg_t arg,
                                  MPT_Tree_pt typespec)
{
  /* check for trivial cases */
  if (arg == NULL) return MPT_Success;

  if (typespec != NULL)
  {
    MPT_Node_pt typespec_node = typespec->node;
    MPT_Arg_pt   typespec_args = typespec->args;
    MP_DictTag_t  ndict = typespec_node->dict;
    MP_NodeType_t ntype = typespec_node->type;
    MP_NumChild_t nc =  typespec_node->numchild;
    MP_Common_t   cvalue = MP_COMMON_T(typespec_node->nvalue);

    /* check special Common Meta types from ProtoDict */
    if (ndict == MP_ProtoDict && ntype == MP_CommonMetaType)
    {
      /* do nothing for types with sizeof <= sizeof(MP_Arg_t) */
      if (IMP_Is32BitNumericMetaType(cvalue))
        mp_return(IMP_PutUint32(link, MP_UINT32_T(arg)));
      else if (IMP_Is8BitNumericMetaType(cvalue))
        mp_return(IMP_PutUint8(link, MP_UINT8_T(arg)));
      else if(IMP_Is64BitNumericMetaType(cvalue))
        mp_return(IMP_PutUint64(link, MP_REAL64_T(arg)));
      else if (cvalue == MP_CmtProtoIMP_ApInt)
        mp_return(MPT_PutApInt(link, arg));
      else if (cvalue == MP_CmtProtoIMP_ApReal)
        mp_return(MPT_PutApReal(link, arg));
      else if (IMP_IsStringBasedMetaType(cvalue))
        mp_return(IMP_PutString(link, MP_STRING_T(arg)));
      else if (cvalue == MP_CmtProtoRecStruct)
        return MPT_PutTypespecedData(link, arg, MPT_RecStructTree);
      else if (cvalue == MP_CmtProtoRecUnion)
        return MPT_PutTypespecedData(arg, MPT_RecUnionTree);
    }

    /* special MP Operators */
    if (ntype == MP_CommonOperatorType && ndict == MP_ProtoDict)
    {
      /* ProtoD:Union */
      if (cvalue == MP_CopProtoUnion || cvalue == MP_CopProtoRecUnion)
      {
        MPT_Union_pt un = MPT_UNION_PT(arg);
        MPT_Assume(un->tag <= nc);
        IMP_PutUint32(link, un->tag);
        if (un->tag > 0)
          return MPT_PutTypespecedArg(link, un->arg, typespec_args[un->tag -1]);
      }

      /* ProtoD:Struct */
      if (cvalue == MP_CopProtoStruct || cvalue == MP_CopProtoRecStruct)
      {
        MP_NumChild_t i;
        
        MPT_Arg_pt st_args = MPT_ARG_PT(arg);
        for (i=0; i<nc; i++)
          mpt_failr(MPT_PutTypespecedArg(link, st_args[i], typespec_args[i]));
        return MPT_Success;
      }
    }
  
    /* Meta Operators */
    if (ntype == MP_CommonMetaOperatorType || ntype == MP_MetaOperatorType)
    {
      MPT_DynArgs_pt dynargs = NULL;
      MPT_Arg_pt args;
      typespec = MPT_ProtoAnnotValue(typespec_node);
    
      if (nc == 0)
      {
        dynargs = MPT_DYNARGS_PT(arg);
        nc = dynargs->length;
        args = dynargs->args;
      }
      else
        args = MPT_ARG_PT(arg);

      return MPT_PutArgs(link, args, nc, typespec);
    }
  }
  /* Everything else is taken to be a tree */
  return MP_PutTree(link, MPT_TREE_PT(arg));
}

#endif /* needs to be updated */
MPT_Status_t MPT_PutTree(MP_Link_pt link, MPT_Tree_pt tree)
{
  if (tree != NULL)
  {
    MPT_Node_pt node = tree->node;
    MP_NodeType_t type = node->type;

    mpt_failr(MPT_PutNode(link, node));

    /* Put the args, if there are any */
    if (type == MP_CommonOperatorType || type == MP_OperatorType)
    {
      MP_Common_t nvalue = MP_COMMON_T(node->nvalue);
      MP_DictTag_t  ndict = node->dict;

      /* Put Args */
      return    MPT_PutArgs(link, tree->args, node->numchild,
                            MPT_ProtoAnnotValue(node));
      
    }
    else return MPT_Success;
  }
  else
    return MPT_Success;
}

MPT_Status_t MPT_PutNode(MP_Link_pt link, MPT_Node_pt node)
{
  MP_NumAnnot_t numannot = node->numannot;
  MP_NodeType_t type = node->type;

  if (type != MPT_ExternalDataType)
  {
    /* Put the node Header */
    mp_failr(IMP_PutNodeHeader(link, type, node->dict, 
                               MP_COMMON_T(node->nvalue), 
                               numannot, node->numchild));

    /* Put the node value, if necessary */
    if (MP_IsStringBasedType(type) ||
        type == MP_OperatorType)
      mp_failr(IMP_PutString(link, MP_STRING_T(node->nvalue)));
    else if (MP_Is32BitNumericType(type))
      mp_failr(IMP_PutUint32(link, MP_UINT32_T(node->nvalue)));
    else if (MP_Is64BitNumericType(type))
      mp_failr(IMP_PutReal64(link, MP_REAL64_T(node->nvalue)));
    else if (type == MP_ApIntType)
      mpt_failr(MPT_PutApInt(link, node->nvalue));
    else if (type == MP_ApRealType)
      mpt_failr(MPT_PutApReal(link, node->nvalue));
    else if (! MP_IsCommonType(type) || MP_Is8BitNumericType(type))
      return MPT_SetError(MPT_WrongNodeType);
    
    /* take care of annots, if there are any */
    if (numannot > 0)
    {
      MP_Uint32_t i;
      MPT_Annot_pt *annots = node->annots, annot;

      MPT_Assume(annots != NULL);
      for (i=0; i<numannot; i++)
      {
        annot = annots[i];
        MPT_Assume(annot != NULL);
        mp_failr(MP_PutAnnotationPacket(link, annot->dict, annot->type, 
                                        annot->flags));
        if (MP_IsAnnotValuated(annot->flags))
          mpt_failr(MPT_PutTree(link, annot->value));
      }
    }
  }
  return MPT_Success;
}
  

MPT_Status_t MPT_PutArgs(MP_Link_pt link, MPT_Arg_pt args, MP_NumChild_t nc,
                         MPT_Tree_pt typespec)
{
  MP_NumChild_t i;
  
  /* return, if args == NULL, i.e. node is a leave node of a tree */
  if (args == NULL || nc == 0) return MPT_Success;

  /* Check for non-prototyped  data */
  if (typespec == NULL)
  {
    for (i=0; i<nc; i++)
      mp_failr(MPT_PutTree(link, MPT_TREE_PT(args[i])));
    return MPT_Success;
  }
  else
  {
    /* not yet implemented */
    return MPT_Failure;
  }
}
