/******************************************************************
 *
 * File:    MPT_TreeManips.c
 * Purpose: Syntactic Manipulation of MPT Trees
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 6/97
 *
 ******************************************************************/
#include "MPT.h"

static MPT_Tree_pt MPT_FindProtoPrototypespec(MPT_Node_pt node);
static MPT_Tree_pt MPT_DeleteNullArgs(MPT_Tree_pt tree);
static void MPT_UntypespecNode(MPT_Node_pt node);

/*******************************************************************
 *
 * Untypespecing a tree
 *
 *******************************************************************/

MPT_Tree_pt MPT_UntypespecTree(MPT_Tree_pt tree)
{
  if (tree != NULL)
  {
    MPT_Node_pt node = tree->node;
    MPT_Assume(node != NULL);

    /* Untypespec real composit trees, only, if there are any */
    if (node->type == MP_CommonOperatorType ||
        node->type == MP_OperatorType)
    {
      MPT_Tree_pt typespec = MPT_ProtoAnnotValue(tree->node);
      tree->args = MPT_UntypespecArgs(tree->args, node->numchild, typespec);
      MPT_UntypespecNode(tree->node);
      tree = MPT_DeleteNullArgs(tree);
    }
    else
      MPT_UntypespecNode(tree->node);
  }
  return tree;
}

static MPT_Tree_pt MPT_FindProtoPrototypespec(MPT_Node_pt node)
{
  MPT_Tree_pt typespec = MPT_ProtoAnnotValue(node);

  if (typespec != NULL && MPT_IsTrueProtoTypeSpec(typespec))
    return typespec;
  else
    return NULL;
}
  
static MPT_Tree_pt MPT_DeleteNullArgs(MPT_Tree_pt tree)
{
  if (tree != NULL)
  {
    MPT_Node_pt node = tree->node;
    MPT_Assume(node != NULL);

    if (node->type == MP_CommonOperatorType ||
        node->type == MP_OperatorType)
    {
      MPT_Arg_pt args = tree->args;
      MP_NumChild_t nc = node->numchild, i, j, nullargs = 0;

      for (i=0; i<nc; i++)
      {
        if (args[i] == NULL) nullargs++;
      }

      if (nullargs > 0)
      {
        node->numchild = nc - nullargs;
        if (nullargs == nc)
        {
          IMP_MemFreeFnc(args, nc*sizeof(MPT_Arg_pt));
          tree->args = NULL;
        }
        else
        {
          MPT_Arg_pt new_args
            = (MPT_Arg_pt) IMP_MemAllocFnc((nc - nullargs)*sizeof(MPT_Arg_t));

          for (i=0, j=0; i<nc; i++)
          {
            if (args[i] != NULL)
            {
              new_args[j] = args[i];
              j++;
            }
          }

          IMP_MemFreeFnc(args, nc*sizeof(MPT_Arg_t));
          tree->args = new_args;
        }
      }

      /* Now decide whether or not to cancel an entire tree */
      if (node->numchild == 0 &&
          node->dict == MP_ProtoDict &&
          (node->numannot == 0 ||
           (node->numannot == 1 && MPT_ProtoAnnotValue(node) != NULL)))
        
      {
        /* which we will only do for operators from the protodict
           which have no additional annots, except for the prototype
           annot */
        MPT_DeleteTree(tree);
        tree = NULL;
      }
    }
  }
  return tree;
}
        
static void MPT_UntypespecNode(MPT_Node_pt node)
{
  MP_Sint32_t i, j;
  MP_Sint32_t numannot;
  MPT_Annot_pt *annots, an;
  MP_Sint32_t found;

  MPT_Assume(node != NULL);

  numannot = node->numannot;
  if (numannot == 0) return;

  annots = node->annots;
  MPT_Assume(annots != NULL);
  
  for (found=0, i=0; i< numannot; i++)
  {
    an = annots[i];
    if (an->type == MP_AnnotProtoPrototype && an->dict == MP_ProtoDict)
    {
      MPT_DeleteTree(an->value);
      IMP_MemFreeFnc(an, sizeof(MPT_Annot_t));
      annots[i] = NULL;
      found++;
    }
    else if (MP_IsAnnotValuated(an->flags))
    {
      an->value = MPT_UntypespecTree(an->value);
      if (an->value == NULL)
      {
        IMP_MemFreeFnc(an, sizeof(MPT_Annot_t));
        annots[i] = NULL;
        found++;
      }
    }
  }

  if (found > 0)
  {
    if (numannot > found)
    {
      MPT_Annot_pt *new_annots
        =(MPT_Annot_pt*)IMP_MemAllocFnc((numannot-found)*sizeof(MPT_Annot_pt));

      for (i=0, j=0; i<numannot; i++)
      {
        if (annots[i] != NULL)
        {
          new_annots[j] = annots[i];
          j++;
        }
      }
      node->numannot = numannot-found;
      node->annots = new_annots;
      IMP_MemFreeFnc(annots, numannot*sizeof(MPT_Annot_pt));
    }
    else
    {
      node->numannot = 0;
      node->annots = NULL;
      IMP_MemFreeFnc(annots, numannot*sizeof(MPT_Annot_pt));
    }
  }
}
        
MPT_Arg_pt MPT_UntypespecArgs(MPT_Arg_pt args,
                              MP_NumChild_t nc,
                              MPT_Tree_pt typespec)
{
  MP_NumChild_t i;

  /* return, if args == NULL, i.e. node is a leave node of a tree */
  if (args == NULL || nc == 0) return NULL;

  /* Check for non-prototyped  data */
  if (typespec == NULL)
  {
    for (i=0; i<nc; i++)
      MPT_UntypespecTree(MPT_TREE_PT(args[i]));

    return args;
  }
  else
  {
    MPT_Node_pt pnode = typespec->node;
    MP_NodeType_t ptype = pnode->type;
    MP_DictTag_t  pdict = pnode->dict;
    MP_Common_t metatype = MP_COMMON_T(pnode->nvalue);

    if (ptype == MP_CommonMetaType && pdict == MP_ProtoDict &&
        (IMP_Is32BitNumericMetaType(metatype) ||
         IMP_Is8BitNumericMetaType(metatype) ||
         IMP_Is64BitNumericMetaType(metatype)))
    {
      /* Here we assume that sizeof(MP_Uint32_t) == sizeof(MPT_Arg_t) */
      MP_Common_t new_type = MP_CmtProto_2_MPType(metatype);
      MPT_Tree_pt new_tree = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
      MPT_Tree_pt n_tree;
      new_tree->args = NULL;
      MPT_CpyNode(&(new_tree->node), pnode);
      new_tree->node->type = new_type;
      MPT_UntypespecTree(new_tree);
      
      if (IMP_Is32BitNumericMetaType(metatype))
      {
        MP_Uint32_t *uint32_array = MP_UINT32_PT(args);
        new_tree->node->nvalue = MP_ARG_UINT32_T(uint32_array[0]);
        args[0] = MPT_ARG_T(new_tree);
        for (i=1; i<nc; i++)
        {
          MPT_CpyTree(&n_tree, new_tree);
          n_tree->node->nvalue = MP_ARG_UINT32_T(uint32_array[i]);
          args[i] = MPT_ARG_T(n_tree);
        }
        return args;
      }
      else if (IMP_Is8BitNumericMetaType(metatype))
      {
        MP_Uint8_t *uint8_array = MP_UINT8_PT(args);
        MPT_Arg_pt new_args
          = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
        new_tree->node->nvalue = MP_ARG_UINT8_T(uint8_array[0]);
        new_args[0] = MPT_ARG_T(new_tree);
        for (i=1; i<nc; i++)
        {
          MPT_CpyTree(&n_tree, new_tree);
          n_tree->node->nvalue = MP_ARG_UINT8_T(uint8_array[i]);
          new_args[i] = MPT_ARG_T(n_tree);
        }
        IMP_MemFreeFnc(args, nc*sizeof(MP_Uint8_t));
        return new_args;
      }
      else
      {
        MP_Real64_t *real64_array = MP_REAL64_PT(args);
        MPT_Arg_pt new_args
          = (MPT_Arg_pt) IMP_MemAllocFnc(nc*sizeof(MPT_Arg_t));
#ifndef __64_BIT__
        new_tree->node->nvalue
          = (MPT_Arg_t) IMP_MemAllocFnc(sizeof(MP_Real64_t));
#endif
        MP_REAL64_T(new_tree->node->nvalue) = real64_array[i];
        new_args[0] = MPT_ARG_T(new_tree);
        for (i=1; i<nc; i++)
        {
          MPT_CpyTree(&n_tree, new_tree);
          MP_REAL64_T(n_tree->node->nvalue) = real64_array[i];
          args[i] = MPT_ARG_T(n_tree);
        }
        IMP_MemFreeFnc(args, nc*sizeof(MP_Real64_t));
        return new_args;
      }
    }
    
    /* else we go step by step */
    for (i=0; i<nc; i++)
      args[i] = MPT_ARG_T(MPT_UntypespecArg(args[i], typespec));

    return args;
  }
}

MPT_Tree_pt MPT_UntypespecArg(MPT_Arg_t arg, MPT_Tree_pt typespec)
{
  if (typespec != NULL)
  {
    MPT_Node_pt typespec_node = typespec->node;
    MPT_Arg_pt   typespec_args = typespec->args;
    MP_DictTag_t  ndict = typespec_node->dict;
    MP_NodeType_t ntype = typespec_node->type;
    MP_NumChild_t nc =  typespec_node->numchild;
    MP_Common_t   cvalue = MP_COMMON_T(typespec_node->nvalue);
    MPT_Tree_pt new_tree = NULL;
    
    /* check special Common Meta types from ProtoDict */
    if (ndict == MP_ProtoDict && ntype == MP_CommonMetaType &&
        IMP_IsBasicMetaType(cvalue))
    {
      MPT_CpyTree(&new_tree, typespec);
      new_tree->args = NULL;
      new_tree->node->type = MP_CmtProto_2_MPType(cvalue);
      new_tree->node->dict = 0;
      new_tree->node->nvalue = arg;
      MPT_UntypespecTree(new_tree);
      return new_tree;
    }

    /* this takes care of recursive NULL pointers */
    if (arg == NULL) return NULL;
    
    /* special MP Operators */
    if (ntype == MP_CommonOperatorType && ndict == MP_ProtoDict)
    {
      /* ProtoD:Union */
      if (cvalue == MP_CopProtoUnion || cvalue == MP_CopProtoRecUnion)
      {
        MPT_Union_pt mun = MPT_UNION_PT(arg);
        MPT_Assume(mun->tag <= nc);
        if (mun->tag > 0)
          new_tree = MPT_UntypespecArg(mun->arg, 
                                       MPT_TREE_PT(typespec_args[mun->tag-1]));

        IMP_MemFreeFnc(mun, sizeof(MPT_Union_t));
        return new_tree;
      }

      /* ProtoD:Struct */
      if (cvalue == MP_CopProtoStruct || cvalue == MP_CopProtoRecStruct)
      {
        MP_NumChild_t i;
        MPT_Arg_pt st_args = MPT_ARG_PT(arg);
        new_tree = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
        MPT_CpyNode(&(new_tree->node), typespec->node);
        MPT_UntypespecNode(new_tree->node);
        new_tree->args = st_args;
        for (i=0; i<nc; i++)
          st_args[i] = MPT_UntypespecArg(st_args[i], 
                                         MPT_TREE_PT(typespec_args[i]));
        return MPT_DeleteNullArgs(new_tree);
      }
    }
  
    /* Meta Operators */
    if (ntype == MP_CommonMetaOperatorType || ntype == MP_MetaOperatorType)
    {
      MPT_DynArgs_pt dynargs = NULL;
      MPT_Arg_pt args;
      MPT_Tree_pt prototypespec = MPT_ProtoAnnotValue(typespec_node);
      
      if (nc == 0)
      {
        dynargs = MPT_DYNARGS_PT(arg);
        nc = dynargs->length;
        args = dynargs->args;
      }
      else
        args = MPT_ARG_PT(arg);

      MPT_CpyTree(&new_tree, typespec);
      new_tree->node->numchild = nc;
      if (ntype == MP_CommonMetaOperatorType) 
        new_tree->node->type = MP_CommonOperatorType;
      else
        new_tree->node->type = MP_OperatorType;
        
      new_tree->args = MPT_UntypespecArgs(args, nc, prototypespec);

      MPT_UntypespecNode(new_tree->node);
      new_tree = MPT_DeleteNullArgs(new_tree);

      if (dynargs != NULL)
        IMP_MemFreeFnc(dynargs, sizeof(MPT_DynArgs_t));
      return new_tree;
    }
  }

  /* Everything else is taken to be a tree */
  return MPT_UntypespecTree(MPT_TREE_PT(arg));
}

