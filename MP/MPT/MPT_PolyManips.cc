/******************************************************************
 *
 * File:    MPT_PolyManips.c
 * Purpose: Syntactic Manipulation of polynomials in MPT Trees
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 6/97
 *
 ******************************************************************/
#include "MPT.h"

static MPT_Arg_pt MPT_DDP_2_ExpTree_Args(MPT_Arg_pt args,
                                         MP_NumChild_t nc,
                                         MPT_Tree_pt typespec);
static MPT_Arg_t MPT_DDP_2_ExpTree_Arg(MPT_Arg_t arg, MPT_Tree_pt typespec);
static void MPT_DDP_2_ExpTree_Node(MPT_Node_pt node);
static MP_Boolean_t MPT_HasDDPAnnots(MPT_Node_pt node,
                                     MPT_Tree_pt *var_tree,
                                     MPT_Tree_pt *coeff_typespec);
static MPT_Tree_pt MPT_DDP_2_ExpTree_Data(MPT_Arg_pt args,
                                          MP_NumChild_t nm,
                                          MPT_Tree_pt var_tree,
                                          MPT_Tree_pt coeff_typespec);

/*******************************************************************
 *
 * DDP ->ExprTree
 *
 *******************************************************************/

/* For the time being, we only convert Polys and ops which have
   prototyped'ed polys as arguments */
MPT_Tree_pt MPT_DDP_2_ExpTree(MPT_Tree_pt tree)
{
  if (tree != NULL)
  {
    MP_Uint32_t i;
    MPT_Node_pt node = tree->node;
    MPT_Assume(node != NULL);

    if (node->type == MP_CommonOperatorType ||
        node->type == MP_OperatorType)
    {
      MPT_Tree_pt typespec =  MPT_ProtoAnnotValue(tree->node);
      MPT_Tree_pt var_tree, coeff_typespec;

      /* Are we a ddp tree ? */
      if (node->type == MP_CommonOperatorType &&
          node->dict == MP_PolyDict &&
          MP_COMMON_T(node->nvalue) == MP_CopPolyDenseDistPoly &&
          MPT_HasDDPAnnots(node, &var_tree, &coeff_typespec))
      {
        /* then do a conversion */
        MPT_Tree_pt ddp_tree = MPT_DDP_2_ExpTree_Data(tree->args,
                                                      node->numchild,
                                                      var_tree,
                                                      coeff_typespec);
        tree->args = NULL;
        MPT_DeleteTree(tree);
        MPT_DeleteTree(var_tree);
        tree = ddp_tree;
      }
      /* Are we typespec'ed */
      else if (typespec != NULL)
      {
        MPT_Node_pt pnode = typespec->node;

        /* with DDP's ? */
        if (pnode->type == MP_CommonMetaOperatorType &&
            pnode->dict == MP_PolyDict &&
            MP_COMMON_T(pnode->nvalue) == MP_CopPolyDenseDistPoly &&
            MPT_HasDDPAnnots(pnode, &var_tree, &coeff_typespec))
        {
          /* Yes, so convert them */
          MPT_DynArgs_pt dynarg;
            
          for (i=0; i<node->numchild; i++)
          {
            dynarg = MPT_DYNARGS_PT(tree->args[i]);
            tree->args[i] = MPT_DDP_2_ExpTree_Data(dynarg->args,
                                                   dynarg->length,
                                                   var_tree,
                                                   coeff_typespec);
            IMP_MemFreeFnc(dynarg, sizeof(MPT_DynArgs_t));
          }
          MPT_RemoveProtoAnnot(node);
        }
        else
        {
          /* No, we do nothing, yet -- should go down recursively */
        }
      }
      else
      {
        /* No prototyped args, so decend recursively */
        for (i=0; i<node->numchild; i++)
          tree->args[i] = MPT_DDP_2_ExpTree(MPT_TREE_PT(tree->args[i]));
      }
    }

    /* Last but not least, do things with annots */
    node = tree->node;
    for (i=0; i<node->numannot; i++)
    {
      if (MP_IsAnnotValuated(node->annots[i]->flags))
        node->annots[i]->value = MPT_DDP_2_ExpTree(node->annots[i]->value);
    }
    
  }
  return tree;
}

static MP_Boolean_t MPT_HasDDPAnnots(MPT_Node_pt node,
                                     MPT_Tree_pt *var_tree,
                                     MPT_Tree_pt *coeff_typespec)
{
  MPT_Tree_pt typespec = MPT_ProtoAnnotValue(node);

  if (typespec != NULL)
  {
    MPT_Node_pt pnode = typespec->node;
    MP_NumChild_t numvars;
    MPT_Annot_pt vn;

    if (pnode->type == MP_CommonOperatorType &&
        pnode->dict == MP_ProtoDict &&
        MP_COMMON_T(pnode->nvalue) == MP_CopProtoStruct &&
        pnode->numchild == 2 &&
        MPT_IsExpVectorTypespec(MPT_TREE_PT((typespec->args)[1]), &numvars) &&
        (vn = MPT_Annot(node,MP_PolyDict,MP_AnnotPolyVarNumber)) != NULL)
    {
      *coeff_typespec = MPT_TREE_PT((typespec->args)[0]);
      *var_tree=MPT_GetVarNamesTree(node,MP_UINT32_T(vn->value->node->nvalue));
      return MP_TRUE;
    }
  }

  return MP_FALSE;
}

MP_Boolean_t MPT_IsExpVectorTypespec(MPT_Tree_pt typespec,
                                     MP_NumChild_t *num_vars)
{
  MPT_Node_pt node = typespec->node;

  if (node->type == MP_CommonMetaOperatorType &&
      node->dict == MP_ProtoDict &&
      MP_COMMON_T(node->nvalue) == MP_CopProtoArray &&
      node->numchild > 0)
  {
    *num_vars = node->numchild;
    typespec = MPT_ProtoAnnotValue(node);
    if (typespec != NULL)
    {
      node = typespec->node;
      if (node->type == MP_CommonMetaType &&
          node->dict == MP_ProtoDict &&
          IMP_IsIntegerMetaType(MP_COMMON_T(node->nvalue)))
        return MP_TRUE;
    }
  }
  return MP_FALSE;
}


static MPT_Tree_pt MPT_DDP_2_ExpTree_Data(MPT_Arg_pt args,
                                          MP_NumChild_t nm,
                                          MPT_Tree_pt var_tree,
                                          MPT_Tree_pt coeff_typespec)
{
  MP_NumChild_t i, j, k, non_zero;
  MPT_Tree_pt ntree = NULL, ctree = NULL, mtree = NULL, ptree;
  MPT_Arg_pt vars = var_tree->args;
  MP_NumChild_t num_vars = var_tree->node->numchild;
  MPT_Arg_pt monomial = NULL, nargs, margs;
  MP_Sint32_t *m_vector;
  
  
  if (nm == 0) return MPT_InitBasicTree(MP_Sint32Type, MPT_ARG_T(0));

  ntree = MPT_InitTree(MP_CommonOperatorType,
                       MP_BasicDict,
                       MPT_ARG_T(MP_CopBasicAdd),
                       0,
                       nm);
  nargs = ntree->args;

  for (i=0; i<nm; i++)
  {
    monomial = MPT_ARG_PT(args[i]);

    /* convert coeff */
    ctree=MPT_UntypespecArg(monomial[0], coeff_typespec);

    /* Get Exponent vector */
    m_vector = MP_SINT32_PT(monomial[1]);

    /* count non-zero entries */
    for (j=0, non_zero=0; j<num_vars; j++)
      if (m_vector[j] != 0) non_zero++;

    if (non_zero > 0)
    {
      /* if there are any, then make a mult node */
      mtree = MPT_InitTree(MP_CommonOperatorType,
                           MP_BasicDict,
                           MPT_ARG_T(MP_CopBasicMult),
                           0,
                           non_zero + 1);
      margs = mtree->args;
      margs[0] = MPT_ARG_T(ctree);

      for (j=0, k=1; k<=non_zero; j++)
      {
        if (m_vector[j] != 0)
        {
          if (m_vector[j] == 1)
          {
            /* if exponent == 1, simply use the variable */
            MPT_CpyTree(&ptree, MPT_TREE_PT(vars[j]));
            margs[k] = MPT_ARG_T(ptree);
          }
          else
          {
            /* make a x^n node */
            ptree = MPT_InitTree(MP_CommonOperatorType,
                                 MP_BasicDict,
                                 MPT_ARG_T(MP_CopBasicPow),
                                 0,   
                                 2);
            
            MPT_CpyTree(&ctree, MPT_TREE_PT(vars[j]));
            ptree->args[0] = MPT_ARG_T(ctree);
            ptree->args[1]=
              MPT_ARG_T(MPT_InitBasicTree(MP_Sint32Type,
                                          MPT_ARG_T(m_vector[j])));
            margs[k] = ptree;
          }
          k++;
        }
      }
    }
    else
    {
      mtree = ctree;
    }
    nargs[i] = MPT_ARG_T(mtree);
    
    /* Clean up */
    IMP_MemFreeFnc(m_vector, num_vars*sizeof(MP_Sint32_t));
    IMP_MemFreeFnc(monomial, 2*sizeof(MPT_Arg_t));
  }

  if (nm == 1)
  {
    nargs[0] = NULL;
    MPT_DeleteTree(ntree);
    ntree = mtree;
  }

  IMP_MemFreeFnc(args, nm*sizeof(MPT_Arg_t));
  
  return ntree;
}
  
      
    
  
          
       
           
  
