/******************************************************************
 *
 * File:    MPT_Tree.c
 * Purpose: Copying/Deleting MPT Trees
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/
#include "MPT.h"
#include <string.h>

#ifndef __64_BIT__
#define MPT_Delete64BitType(arg) IMP_MemFreeFnc(arg, sizeof(MP_Real64_t))
static void  MPT_Cpy64BitType(MPT_Arg_t *odest, MPT_Arg_t src)
{
  MP_Real64_t *r64_pt = (MP_Real64_t *) IMP_MemAllocFnc(sizeof(MP_Real64_t));
  *r64_pt = MP_REAL64_T(src);
  *odest = r64_pt;
}
#else
#define MPT_Delete64BitType(arg) ((void *) 0)
#define MPT_Cpy64BitType(dest, src) *(dest) = src
#endif

/*******************************************************************
 *
 * Deleting a tree
 *
 *******************************************************************/

void MPT_DeleteTree(MPT_Tree_pt tree)
{
  if (tree != NULL)
  {
    MPT_Assume(tree->node != NULL);

    /* Delete the args, if there are any */
    if (tree->node->type == MP_CommonOperatorType ||
        tree->node->type == MP_OperatorType)
      MPT_DeleteArgs(tree->args, tree->node->numchild,
                     MPT_ProtoAnnotValue(tree->node));
    /* Check for external data */
    else if (tree->node->type == MPT_ExternalDataType)
      MPT_DeleteExternalData(tree->args);
    /* else, it is a node which has no arguments */

    MPT_DeleteNode(tree->node);

    IMP_MemFreeFnc(tree, sizeof(MPT_Tree_t));
  }
}
  

void MPT_DeleteNode(MPT_Node_pt node)
{
  MP_NumAnnot_t numannot = node->numannot;
  MP_NodeType_t type = node->type;
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
      if (MP_IsAnnotValuated(annot->flags))
        MPT_DeleteTree(annot->value);
      IMP_MemFreeFnc(annot, sizeof(MPT_Annot_t));
    }

    IMP_MemFreeFnc(node->annots, numannot*sizeof(MPT_Annot_pt));
  }

  /* delet nvalue, if necessary */
  if (node->nvalue != NULL)
  {
    if (MP_IsStringBasedType(type) ||
        type == MP_OperatorType ||
        type == MP_MetaOperatorType)
      IMP_RawMemFreeFnc(node->nvalue);
    else if (type == MP_ApIntType)
      MPT_DeleteApInt(node->nvalue);
    else if (type == MP_ApRealType)
      MPT_DeleteApReal(node->nvalue);
    else if (MP_Is64BitNumericType(type))
      MPT_Delete64BitType(node->nvalue);
  }
  
  IMP_MemFreeFnc(node, sizeof(MPT_Node_t));
}

void MPT_DeleteArgs(MPT_Arg_pt args, MP_NumChild_t nc,
                    MPT_Tree_pt typespec)
{
  MP_NumChild_t i;
  MP_Uint32_t size = 0;
  
  /* return, if args == NULL, i.e. node is a leave node of a tree */
  if (args == NULL || nc == 0) return;

  /* Check for non-prototyped  data */
  if (typespec == NULL)
  {
    for (i=0; i<nc; i++)
      MPT_DeleteTree(MPT_TREE_PT(args[i]));
    size = nc*sizeof(MPT_Arg_t);
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
        size = nc*sizeof(MP_Uint8_t);
      else if (IMP_Is32BitNumericMetaType(metatype))
        size = nc*sizeof(MP_Uint32_t);
      else if (IMP_Is64BitNumericMetaType(metatype))
        size = nc*sizeof(MP_Real64_t);
    }

    /* Everything else is handled item by item */
    if (size == 0)
    {
      for (i=0;i<nc; i++)
        MPT_DeleteTypespecedArg(args[i], typespec);
      size = nc*sizeof(MPT_Arg_t);
    }
  }
  
  /* Delete Args */
  IMP_MemFreeFnc(args, size);
}


void MPT_DeleteTypespecedArg(MPT_Arg_t arg, MPT_Tree_pt typespec)
{
  /* check for trivial cases */
  if (arg == NULL) return;

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
      /* do nothing for types with sizeof <= sizeof(MPT_Arg_t) */
      if (IMP_Is32BitNumericMetaType(cvalue) ||
          IMP_Is8BitNumericMetaType(cvalue))
        return;

      if (cvalue == MP_CmtProtoIMP_ApInt)
      {
        MPT_DeleteApInt(arg);
        return;
      }
      else if (cvalue == MP_CmtProtoIMP_ApReal)
      {
        MPT_DeleteApReal(arg);
        return;
      }
      else if (IMP_Is64BitNumericMetaType(cvalue))
      {
        MPT_Delete64BitType(arg);
        return;
      }
      else if (IMP_IsStringBasedMetaType(cvalue))
      {
        IMP_RawMemFreeFnc(arg);
        return;
      }
      else if (cvalue == MP_CmtProtoRecStruct)
      {
        MPT_DeleteTypespecedArg(arg, MPT_RecStructTree);
        return;
      }
      else if (cvalue == MP_CmtProtoRecUnion)
      {
        MPT_DeleteTypespecedArg(arg, MPT_RecUnionTree);
        return;
      }
    }

    /* special MP Operators */
    if (ntype == MP_CommonOperatorType && ndict == MP_ProtoDict)
    {
      /* ProtoD:Union */
      if (cvalue == MP_CopProtoUnion || cvalue == MP_CopProtoRecUnion)
      {
        MPT_Union_pt mun = MPT_UNION_PT(arg);
        MPT_Assume(mun->tag <= nc);
        if (mun->tag > 0)
          MPT_DeleteTypespecedArg(mun->arg, 
                                  MPT_TREE_PT(typespec_args[mun->tag -1]));
        IMP_MemFreeFnc(mun, sizeof(MPT_Union_t));
        return;
      }

      /* ProtoD:Struct */
      if (cvalue == MP_CopProtoStruct || cvalue == MP_CopProtoRecStruct)
      {
        MP_NumChild_t i;
        MPT_Arg_pt st_args = MPT_ARG_PT(arg);
        for (i=0; i<nc; i++)
          MPT_DeleteTypespecedArg(st_args[i], MPT_TREE_PT(typespec_args[i]));
        IMP_MemFreeFnc(arg, nc*sizeof(MPT_Arg_t));
        return;
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

      MPT_DeleteArgs(args, nc, typespec);
      if (dynargs != NULL)
        IMP_MemFreeFnc(dynargs, sizeof(MPT_DynArgs_t));
      return;
    }
  }

  /* Everything else is taken to be a tree */
  MPT_DeleteTree(MPT_TREE_PT(arg));
}

/*******************************************************************
 *
 * Copying a tree
 *
 *******************************************************************/

void MPT_CpyTree(MPT_Tree_pt *otree, MPT_Tree_pt src)
{
  if (src == NULL)
    *otree = NULL;
  else
  {
    MPT_Tree_pt dest = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
    MPT_Node_pt snode = src->node;
    MP_NodeType_t stype = snode->type;

    *otree = dest;
    MPT_CpyNode(&(dest->node), src->node);
    
    /* Copy Args, if necessary */
    if (stype == MP_CommonOperatorType || stype == MP_OperatorType)
    {
      MP_Common_t nvalue = MP_COMMON_T(snode->nvalue);
      MP_DictTag_t  ndict = snode->dict;

      /* we might have to push the current tree onto the stack of
         recursive typespecs */
      if (ndict == MP_ProtoDict)
      {
        if (nvalue == MP_CopProtoRecUnion)
          MPT_PushRecUnion(dest);
        else if (nvalue == MP_CopProtoRecStruct)
          MPT_PushRecStruct(dest);
      }

      /* Copy Args */
      MPT_CpyArgs(&(dest->args), src->args,
                  snode->numchild, MPT_ProtoAnnotValue(snode)); 

      /* And, pop the recursive typesepcs again */
      if (ndict == MP_ProtoDict)
      {
        if (nvalue == MP_CopProtoRecUnion)
          MPT_PopRecUnion();
        else if (nvalue == MP_CopProtoRecStruct)
          MPT_PopRecStruct();
      }
    }
    /* No args */
    else if (stype == MPT_ExternalDataType)
      MPT_CpyExternalData(MPT_ARG_PT(&(dest->args)), src->args);
    else
      dest->args = NULL;
  }
}

void MPT_CpyNode(MPT_Node_pt *onode, MPT_Node_pt node)
{
  MPT_Node_pt dest = (MPT_Node_pt) IMP_MemAllocFnc(sizeof(MPT_Node_t));
  MP_NumAnnot_t numannot = node->numannot;
  MP_NodeType_t type = node->type;

  *onode = dest;
  memcpy(dest, node, sizeof(MPT_Node_t));

  /* take care of annots, if there are any */
  if (numannot > 0)
  {
    MP_Uint32_t i;
    MPT_Annot_pt annot, dannot,
      *annots = node->annots, 
      *dannots = (MPT_Annot_pt*)IMP_MemAllocFnc(numannot*sizeof(MPT_Annot_pt));
    dest->annots = dannots;

    for (i=0; i<numannot; i++)
    {
      annot = annots[i];
      MPT_Assume(annot != NULL);

      dannot = (MPT_Annot_pt) IMP_MemAllocFnc(sizeof(MPT_Annot_t));
      dannots[i] = dannot;

      memcpy(dannot, annot, sizeof(MPT_Annot_t));
      if (MP_IsAnnotValuated(annot->flags))
        MPT_CpyTree(&(dannot->value), annot->value);
    }
  }

  /* cpy nvalue, if necessary */
  if (MP_IsStringBasedType(type) ||
      type == MP_OperatorType ||
      type == MP_MetaOperatorType)
  {
    dest->nvalue
      = IMP_RawMemAllocFnc(
        (strlen(MP_STRING_T(node->nvalue))+1)*sizeof(char));
    strcpy((char*) dest->nvalue, (char*) node->nvalue);
  }
  else if (type == MP_ApIntType)
    MPT_InitCopyApInt(&(dest->nvalue), node->nvalue);
  else if (type == MP_ApRealType)
    MPT_InitCopyApReal(&(dest->nvalue), node->nvalue);
  else if (type == MP_Real64Type)
    MPT_Cpy64BitType(&(dest->nvalue), node->nvalue);
}

void MPT_CpyArgs(MPT_Arg_pt *oargs, MPT_Arg_pt args,
                 MP_NumChild_t nc, MPT_Tree_pt typespec)
{
  /* return, if args == NULL, i.e. node is a leave node of a tree */
  if (args == NULL || nc == 0)
    *oargs = NULL;
  else
  {
    MP_NumChild_t i;
    MPT_Arg_pt dargs;

    /* Check for non-prototyped  data */
    if (typespec == NULL)
    {
      dargs = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
      *oargs = dargs;
    
      for (i=0; i<nc; i++)
        MPT_CpyTree((MPT_Tree_pt *) &(dargs[i]), MPT_TREE_PT(args[i]));
    }
    else
    {
      /* Now we are in the realm of prototyped data */
      MPT_Node_pt pnode = typespec->node;
      MP_NodeType_t ptype = pnode->type;
      MP_DictTag_t  pdict = pnode->dict;
      MP_Uint32_t size = 0;

      /* Check only for arrays of basic numeric types */
      if (ptype == MP_CommonMetaType && pdict == MP_ProtoDict)
      {
        MP_Common_t metatype = MP_COMMON_T(pnode->nvalue);
      
        if (IMP_Is8BitNumericMetaType(metatype))
          size = nc*sizeof(MP_Uint8_t);
        else if (IMP_Is32BitNumericMetaType(metatype))
          size = nc*sizeof(MP_Uint32_t);
        else if (IMP_Is64BitNumericMetaType(metatype))
          size = nc*sizeof(MP_Real64_t);
      }
  
      /* Everything else is handled item by item */
      if (size == 0)
      {
        dargs = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
        *oargs = dargs;
    
        for (i=0;i<nc; i++)
          MPT_CpyTypespecedArg(&(dargs[i]), args[i], typespec);
      }
      else
      {
        dargs = (MPT_Arg_pt) IMP_MemAllocFnc(size);
        *oargs = dargs;
        memcpy(dargs, args, size);
      }
    }
  }
}

  

void MPT_CpyTypespecedArg(MPT_Arg_t *oarg,
                          MPT_Arg_t arg, MPT_Tree_pt typespec)
{
  /* check for trivial cases */
  if (arg == NULL)
  {
    *oarg = NULL;
    return;
  }

  if (typespec != NULL)
  {
    /* Non-trivial cases of typespeced data */
    MPT_Node_pt typespec_node = typespec->node;
    MPT_Arg_pt   typespec_args = typespec->args;
    MP_DictTag_t  ndict = typespec_node->dict;
    MP_NodeType_t ntype = typespec_node->type;
    MP_NumChild_t nc =  typespec_node->numchild;
    MP_Common_t   cvalue = MP_COMMON_T(typespec_node->nvalue);

    *oarg = arg;
    /* check special Common Meta types from ProtoDict */
    if (ndict == MP_ProtoDict && ntype == MP_CommonMetaType)
    {
      /* do nothing for types with sizeof <= sizeof(MPT_Arg_t) */
      if (IMP_Is32BitNumericMetaType(cvalue) ||
          IMP_Is8BitNumericMetaType(cvalue))
        return;

      if (cvalue == MP_CmtProtoIMP_ApInt)
      {
        MPT_InitCopyApInt(oarg, arg);
        return;
      }
      else if (cvalue == MP_CmtProtoIMP_ApReal)
      {
        MPT_InitCopyApReal(oarg, arg);
        return;
      }
      else if (IMP_Is64BitNumericMetaType(cvalue))
      {
        MPT_Cpy64BitType(oarg, arg);
        return;
      }
      else if (IMP_IsStringBasedMetaType(cvalue))
      {
        *oarg = IMP_RawMemAllocFnc((strlen(MP_STRING_T(arg))+1)*sizeof(char));
        strcpy(MP_STRING_T(*oarg), MP_STRING_T(arg));
        return;
      }
      else if (cvalue == MP_CmtProtoRecStruct)
      {
        MPT_CpyTypespecedArg(oarg, arg, MPT_RecStructTree);
        return;
      }
      else if (cvalue == MP_CmtProtoRecUnion)
      {
        MPT_CpyTypespecedArg(oarg, arg, MPT_RecUnionTree);
        return;
      }
    }

    /* special MP Operators */
    if (ntype == MP_CommonOperatorType && ndict == MP_ProtoDict)
    {
      /* ProtoD:Union */
      if (cvalue == MP_CopProtoUnion || cvalue == MP_CopProtoRecUnion)
      {
        MPT_Union_pt mun = MPT_UNION_PT(arg),
          dun = (MPT_Union_pt) IMP_MemAllocFnc(sizeof(MPT_Union_t));

        MPT_Assume(mun->tag <= nc);
        dun->tag = mun->tag;
        *oarg = (MPT_Arg_t) dun;
        if (mun->tag > 0)
          MPT_CpyTypespecedArg(&(dun->arg), mun->arg,
                               MPT_TREE_PT(typespec_args[mun->tag-1]));
        else
          dun->arg = NULL;
        return;
      }

      /* ProtoD:Struct */
      if (cvalue == MP_CopProtoStruct || cvalue == MP_CopProtoRecStruct)
      {
        MPT_Arg_pt st_args = MPT_ARG_PT(arg),
          dst_args = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
        MP_NumChild_t i;

        *oarg = dst_args;
        for (i=0; i<nc; i++)
          MPT_CpyTypespecedArg(&(dst_args[i]), st_args[i], 
                               MPT_TREE_PT(typespec_args[i]));
        return;
      }
    }
  
    /* Meta Operators */
    if (ntype == MP_CommonMetaOperatorType || ntype == MP_MetaOperatorType)
    {
      MPT_Arg_pt args, *dargs;
      typespec = MPT_ProtoAnnotValue(typespec_node);
    
      if (nc == 0)
      {
        MPT_DynArgs_pt dynargs = MPT_DYNARGS_PT(arg), ddynargs;
      
        nc = dynargs->length;
        ddynargs = (MPT_DynArgs_pt) IMP_MemAllocFnc(sizeof(MPT_DynArgs_t));
        *oarg = ddynargs;
        ddynargs->length = nc;

        args = dynargs->args;
        dargs = &(ddynargs->args);
      }
      else
      {
        args = MPT_ARG_PT(arg);
        dargs = (MPT_Arg_pt *) oarg;
      }
    
      MPT_CpyArgs(dargs, args, nc, typespec);
      return;
    }
  }

  /* Everything else is taken to be a tree */
  MPT_CpyTree((MPT_Tree_pt *) oarg, MPT_TREE_PT(arg));
}

