/******************************************************************
 *
 * File:    MPT_Misc.c
 * Purpose: Some Miscellaneaus routines for dealing with MP Trees
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 12/96
 *
 * Change History (most recent first):
 *     
 ******************************************************************/
#include "MPT.h"
#include "string.h"

/*************************************************************** 
 * 
 * Routines around annotations
 * 
 ***************************************************************/

MP_Boolean_t MPT_IsTrueProtoTypeSpec(MPT_Tree_pt typespec)
{
  if (typespec != NULL)
  {
    MPT_Node_pt node = typespec->node;

    if (node->dict == MP_ProtoDict)
    {
      MP_Common_t cvalue = MP_COMMON_T(node->nvalue);
      
      if (node->type == MP_CommonMetaType && IMP_IsBasicMetaType(cvalue))
        return MP_TRUE;
      else if (node->type == MP_CommonOperatorType &&
               (cvalue == MP_CopProtoStruct ||
                cvalue == MP_CopProtoRecStruct ||
                cvalue == MP_CopProtoUnion ||
                cvalue == MP_CopProtoRecUnion))
        return MP_TRUE;
    }

    if (node->type == MP_CommonMetaOperatorType ||
        node->type == MP_MetaOperatorType)
      return MP_TRUE;
  }
  return MP_FALSE;
}

/* Removes the first Prototype annotation it finds */
void MPT_RemoveAnnot(MPT_Node_pt node, MP_DictTag_t dict, MP_AnnotType_t type)
{
  if (node != NULL)
  {
    MP_Sint32_t na = node->numannot;

    if (na > 0)
    {
      MP_Sint32_t i;
      MPT_Annot_pt *annots = node->annots;
      MP_Sint32_t found = -1;

      for (i=0; i<na; i++)
      {
        if (annots[i]->dict == dict && annots[i]->type == type)
        {
          found = i;
          break;
        }
      }

      if (found > 0)
      {
        if (na > 1)
        {
          MPT_Annot_pt *nannots
            =(MPT_Annot_pt*)IMP_MemAllocFnc(sizeof(MPT_Annot_pt)*(na-1));

          for (i=0; i<found; i++)
            nannots[i] = annots[i];

          for (i=found+1; i<na; i++)
            nannots[i-1] = annots[i];

          node->annots = nannots;
        }
        else
          node->annots = 0;

        node->numannot = na -1;
        IMP_MemFreeFnc(annots, na*sizeof(MPT_Annot_pt));
      }
    }
  }
}

    
void MPT_MoveAnnots(MPT_Node_pt from, MPT_Node_pt to)
{
  MP_NumAnnot_t n_from = from->numannot,
    n_to = to->numannot,
    n_new = n_from + n_to,
    i, j;

  MPT_Annot_pt *an_from = from->annots,
    *an_to = to->annots,
    *an_new;

  if (n_from == 0) return;

  if (n_to == 0)
  {
    to->numannot = n_from;
    to->annots = an_from;
  }
  else
  {
    an_new = (MPT_Annot_pt *) IMP_MemAllocFnc(n_new*sizeof(MPT_Annot_pt));

    for (i=0; i<n_from; i++)
      an_new[i] = an_from[i];

    for (j=0; i<n_new; i++, j++)
      an_new[i] = an_to[j];

    IMP_MemFreeFnc(an_from, n_from*sizeof(MPT_Annot_pt));
    IMP_MemFreeFnc(an_to, n_to*sizeof(MPT_Annot_pt));

    to->numannot = n_new;
    to->annots = an_new;
  }
    
  from->numannot = 0;
  from->annots = NULL;
}

  
/*************************************************************** 
 * 
 * Routines for easy tree initalizations
 * 
 ***************************************************************/

void* IMP_MemAlloc0Fnc(size_t size)
{
  void* got = IMP_MemAllocFnc(size);
  if (got != NULL) memset(got, 0, size);
  return got;
}
  
MPT_Tree_pt MPT_InitTree(MP_Common_t type,
                         MP_DictTag_t dict,
                         MPT_Arg_t nvalue,
                         MP_NumAnnot_t numannot,
                         MP_NumChild_t numchild)
{
  MPT_Tree_pt tree = (MPT_Tree_pt) IMP_MemAllocFnc(sizeof(MPT_Tree_t));
  MPT_Node_pt node = (MPT_Node_pt) IMP_MemAllocFnc(sizeof(MPT_Node_t));

  tree->node = node;

  node->type = type;
  node->dict = dict;
  node->nvalue = nvalue;
  node->numannot = numannot;
  node->numchild = numchild;

  if (numannot > 0)
    node->annots
      = (MPT_Annot_pt *) IMP_MemAlloc0Fnc(numannot*sizeof(MPT_Annot_pt));
  else
    node->annots = 0;

  if (numchild > 0)
    tree->args = (MPT_Arg_pt) IMP_MemAlloc0Fnc(numchild*sizeof(MPT_Arg_t));
  else
    tree->args = NULL;

  return tree;
}

  
  

/*************************************************************** 
 * 
 * Routines implementing a stack of void * poiters
 * 
 ***************************************************************/
typedef struct Cell_t Cell_t;
typedef Cell_t * Cell_pt;

struct Cell_t
{
  MPT_Tree_pt   el;
  Cell_pt       next;
};

static void Push(Cell_pt *stack, MPT_Tree_pt el)
{
  Cell_pt nc = (Cell_pt) IMP_MemAllocFnc(sizeof(Cell_t));
  nc->el = el;
  nc->next = *stack;
  *stack = nc;
}

static void Pop(Cell_pt *stack)
{
  Cell_pt head = *stack;
  
  *stack = head->next;
  IMP_MemFreeFnc(head, sizeof(Cell_t));
}


/* the variables which hold the recursive Struct and Union stacks */
/* Used for recursive pointers */
static Cell_pt Union_Stack = NULL;
static Cell_pt Struct_Stack = NULL;

MPT_Tree_pt MPT_RecStructTree = NULL, MPT_RecUnionTree = NULL;

void MPT_PushRecStruct(MPT_Tree_pt tree)
{
  Push(&Struct_Stack, tree);
  MPT_RecStructTree = tree;
}

void MPT_PopRecStruct()
{
  Pop(&Struct_Stack);
  if (Struct_Stack == NULL)
    MPT_RecStructTree = NULL;
  else
    MPT_RecStructTree = Struct_Stack->el;
}

void MPT_PushRecUnion(MPT_Tree_pt tree)
{
  Push(&Union_Stack, tree);
  MPT_RecUnionTree = tree;
}

void MPT_PopRecUnion()
{
  Pop(&Union_Stack);
  if (Union_Stack == NULL)
    MPT_RecUnionTree = NULL;
  else
    MPT_RecUnionTree = Union_Stack->el;
  
}






