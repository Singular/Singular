/******************************************************************
 *
 * File:    MPT_GetPoly.c
 * Purpose: Routines which support Get side of polynomial communications
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 6/97
 *
 ******************************************************************/
#include "MPT.h"

static MP_Boolean_t MPT_GetDDPAnnots(MPT_Node_pt node,
                                     MP_Sint32_t *characteristic,
                                     MPT_Tree_pt *var_tree,
                                     MP_Common_t *ordering);
static MP_Sint32_t MPT_GetCharacterisitic(MPT_Tree_pt typespec);
static MP_Common_t MPT_GetOrdering(MPT_Node_pt node);
static MP_Boolean_t MPT_IsDDPMetaTree(MPT_Tree_pt tree,
                                      MP_Sint32_t *characteristic,
                                      MPT_Tree_pt *var_tree,
                                      MP_Common_t *ordering);

MP_Boolean_t MPT_IsDDPTree(MPT_Tree_pt tree,
                           MP_Sint32_t *characteristic,
                           MPT_Tree_pt *var_tree,
                           MP_Common_t *ordering)
{
  MPT_Node_pt node;

  if (tree == NULL) return MP_FALSE;

  node = tree->node;
  if (node->type != MP_CommonOperatorType ||
      node->dict != MP_PolyDict ||
      node->nvalue != MPT_ARG_T(MP_CopPolyDenseDistPoly))
    return MP_FALSE;

  return MPT_GetDDPAnnots(node, characteristic, var_tree, ordering);
}

static MP_Boolean_t MPT_GetDDPAnnots(MPT_Node_pt node,
                                     MP_Sint32_t *characteristic,
                                     MPT_Tree_pt *var_tree,
                                     MP_Common_t *ordering)
{
  MPT_Tree_pt typespec = MPT_ProtoAnnotValue(node);

  if (typespec != NULL)
  {
    MPT_Node_pt pnode = typespec->node;
    MP_NumChild_t numvars;

    if (pnode->type == MP_CommonOperatorType &&
        pnode->dict == MP_ProtoDict &&
        MP_COMMON_T(pnode->nvalue) == MP_CopProtoStruct &&
        pnode->numchild == 2 &&
        MPT_IsExpVectorTypespec(MPT_TREE_PT((typespec->args)[1]), &numvars) &&
        ((*characteristic
          = MPT_GetCharacterisitic(MPT_TREE_PT((typespec->args)[0]))) >= 0))
    {
      *var_tree = MPT_GetVarNamesTree(node, numvars);
      *ordering = MPT_GetOrdering(node);
      return MP_TRUE;
    }
  }
  return MP_FALSE;
}

static MP_Sint32_t MPT_GetCharacterisitic(MPT_Tree_pt typespec)
{
  if (typespec != NULL)
  {
    MPT_Node_pt node = typespec->node;
    
    if (node->type == MP_CommonMetaType)
    {
      if (node->dict == MP_NumberDict &&
          node->nvalue == MPT_ARG_T(MP_CmtNumberRational))
        return 0;
      else if (node->dict == MP_ProtoDict &&
               node->nvalue == MPT_ARG_T(MP_CmtProtoIMP_Uint32))
      {
        MPT_Annot_pt annot = MPT_Annot(node,
                                           MP_NumberDict,
                                           MP_AnnotNumberModulos);
        if (annot != NULL)
        {
          node = annot->value->node;
          if (node->type == MP_Uint32Type)
            return MP_SINT32_T(node->nvalue);
        }
      }
    }
  }
  return -1;
}

static MP_Common_t MPT_GetOrdering(MPT_Node_pt node)
{
  MPT_Annot_pt annot = MPT_Annot(node, MP_PolyDict, MP_AnnotPolyOrdering);

  if (annot != NULL && annot->value != NULL)
  {
    node = annot->value->node;
    if (node->type == MP_CommonConstantType)
      return MP_COMMON_T(node->nvalue);
  }

  return MP_CcPolyOrdering_Unknown;
}

MPT_Tree_pt MPT_GetVarNamesTree(MPT_Node_pt node,
                                MP_NumChild_t num_vars)
{
  MPT_Tree_pt vtree = NULL;
  MPT_Annot_pt
    vn_annot = MPT_Annot(node, MP_PolyDict, MP_AnnotPolyVarNames);


  if (vn_annot != NULL && MP_IsAnnotValuated(vn_annot->flags))
  {
    MPT_CpyTree(&vtree, vn_annot->value);
    vtree = MPT_UntypespecTree(vtree);
  }

  /* There might be just one variable or no vn_annot */
  if (vtree == NULL || vtree->node->type != MP_CommonOperatorType)
  {
    MPT_Tree_pt temp_tree = vtree;
    vtree = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
    vtree->node = (MPT_Node_pt) IMP_MemAllocFnc(sizeof(MPT_Node_t));
    vtree->node->type = MP_CommonOperatorType;
    vtree->node->dict = MP_BasicDict;
    vtree->node->numannot = 0;
    vtree->node->annots = NULL;
    vtree->node->nvalue = MP_ARG_COMMON_T(MP_CopBasicList);
    if (temp_tree != NULL)
    {
      vtree->node->numchild = 1;
      vtree->args = (MPT_Arg_pt) IMP_MemAllocFnc(sizeof(MPT_Arg_t));
      vtree->args[0] = MPT_ARG_T(temp_tree);
    }
    else
    {
      vtree->node->numchild = 0;
      vtree->args = NULL;
    }
  }
  else
  {
    vtree->node->dict = MP_BasicDict;
    vtree->node->nvalue = MPT_ARG_T(MP_CopBasicList);
  }
  
  
  if (vtree->node->numchild < num_vars)
  {
    MPT_Arg_pt nargs = (MPT_Arg_pt)IMP_MemAllocFnc(num_vars*sizeof(MPT_Arg_t));
    MP_Sint32_t i, j;
    MPT_Tree_pt ntree;
    MPT_Node_pt nnode;

    for (i=num_vars-1, j = vtree->node->numchild - 1; j >= 0; j--, i--)
      nargs[i] = vtree->args[j];

    for (; i >= 0; i--)
    {
      ntree = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
      nnode = (MPT_Node_pt) IMP_MemAllocFnc(sizeof(MPT_Node_t));
      ntree->node = nnode;
      ntree->args = NULL;
      nnode->type = MP_IdentifierType;
      nnode->dict = 0;
      nnode->numannot = 0;
      nnode->annots = NULL;
      nnode->nvalue = (MPT_Arg_t) IMP_RawMemAllocFnc(12*sizeof(char));
      sprintf((char *) nnode->nvalue, "x%ld", i);
      nargs[i] = MPT_ARG_T(ntree);
    }

    IMP_MemFreeFnc(vtree->args, (vtree->node->numchild)*sizeof(MPT_Arg_t));
    vtree->node->numchild = num_vars;
    vtree->args = nargs;
  }
  return vtree;
}

MP_Boolean_t MPT_IsIdealTree(MPT_Tree_pt tree,
                             MP_Sint32_t *characteristic,
                             MPT_Tree_pt *var_tree,
                             MP_Common_t *ordering)
{
  MPT_Node_pt node;

  if (tree == NULL) return MP_FALSE;

  node = tree->node;
  if (node->type != MP_CommonOperatorType ||
      node->dict != MP_PolyDict ||
      node->nvalue != MPT_ARG_T(MP_CopPolyIdeal))
    return MP_FALSE;

  return MPT_IsDDPMetaTree(MPT_ProtoAnnotValue(node),
                           characteristic,
                           var_tree,
                           ordering);
}

static MP_Boolean_t MPT_IsDDPMetaTree(MPT_Tree_pt tree,
                                      MP_Sint32_t *characteristic,
                                      MPT_Tree_pt *var_tree,
                                      MP_Common_t *ordering)
{
  MPT_Node_pt node;
  MPT_Annot_pt annot;

  if (tree == NULL) return MP_FALSE;
  node = tree->node;

  if (node->type != MP_CommonMetaOperatorType ||
      node->dict != MP_PolyDict ||
      node->nvalue != MPT_ARG_T(MP_CopPolyDenseDistPoly) ||
      node->numchild != 0)
    return MP_FALSE;

  return MPT_GetDDPAnnots(node, characteristic, var_tree, ordering);
}

  

  


