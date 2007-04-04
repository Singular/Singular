/******************************************************************
 *
 * File:    MP_GP.h
 * Purpose: Implementation for MP realization of GP classes
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 11/98
 *
 ******************************************************************/

#include "MPT_GP.h"
/////////////////////////////////////////////////////////////////////
///
/// Iterator:
//  BUG: Iterator does not work for array of Real64, or Uint8
///
/////////////////////////////////////////////////////////////////////
MPT_GP_Iterator_t::MPT_GP_Iterator_t(MPT_Node_pt tnode)
{
  MPT_Assume(tnode != NULL &&
             (tnode->type == MP_CommonOperatorType || 
              tnode->type == MP_CommonMetaOperatorType));

  if (tnode->type == MP_CommonMetaOperatorType &&
      tnode->numchild == 0)
  {
    _n = -1;
    _dynamic = true;
  }
  else
  {
    _n = tnode->numchild;
    _dynamic = false;
  }

  _i = _n+1;
  _args = NULL;
}

MPT_GP_Iterator_t::MPT_GP_Iterator_t()
{
  _n = -1;
  _dynamic = false;
  _i = -2;
  _args = NULL;
}

void MPT_GP_Iterator_t::Reset(const void* data)
{
  MPT_Assume(data != NULL);
  if (_dynamic)
  {
    MPT_DynArgs_pt dynargs = MPT_DYNARGS_PT(data);
    _n = dynargs->length;
    _args = dynargs->args;
  }
  else
    _args = MPT_ARG_PT(data);

  _i = 0;
}

void* MPT_GP_Iterator_t::Next()
{
  if (_i < _n) 
    return _args[_i++];
  return NULL;
}

bool MPT_GP_IsValueIterator(MPT_Tree_pt tree, long vtype)
{
  if (tree == NULL || tree->node->type != MP_CommonOperatorType)
    return false;
  MPT_Tree_pt proto = MPT_TrueProtoAnnotValue(tree->node);

  if (proto != NULL)
  {
    if (vtype < 0 || 
        (proto->node->type == MP_CommonMetaType && 
         proto->node->dict == MP_ProtoDict && 
         vtype == MP_CmtProto_2_MPType(MP_COMMON_T(proto->node->nvalue))))
      return true;
    else 
      return false;
  }
  if (vtype >= 0)
  {
    long nc = tree->node->numchild, i;
    MPT_Arg_pt args;
  
    for (i=0; i<nc; i++)
    {
      if (vtype != MP_COMMON_T(MPT_TREE_PT(tree->args[i])->node->type))
        return false;
    }
  }
  return true;
}

MPT_GP_ValueIterator_t::MPT_GP_ValueIterator_t(MPT_Tree_pt tree, long vtype) 
    : MPT_GP_Iterator_t(tree->node)
{
  MPT_Assume(MPT_GP_IsValueIterator(tree, vtype));
  _prototype = MPT_TrueProtoAnnotValue(tree->node);
  _vtype = vtype;
}

MPT_GP_ValueIterator_t::MPT_GP_ValueIterator_t() 
    : MPT_GP_Iterator_t()
{
  _vtype = -1;
  _prototype = NULL;
}

void* MPT_GP_ValueIterator_t::Next()
{
  void* arg = MPT_GP_Iterator_t::Next();
  
  if (_prototype == NULL) arg = MPT_TREE_PT(arg)->node->nvalue;
  return arg;
}


/////////////////////////////////////////////////////////////////////
///
/// Top - level GP
///
/////////////////////////////////////////////////////////////////////

MPT_GP_pt MPT_GetGP(MPT_Node_pt tnode)
{
  MPT_Assume(tnode != NULL);
  MPT_GP_pt gp;
  if ((gp = MPT_GetGP_Atom(tnode)) != NULL) return gp;
  if ((gp = MPT_GetGP_Comp(tnode)) != NULL) return gp;
  if ((gp = MPT_GetGP_Poly(tnode)) != NULL) return gp;
  return NULL;
}

MPT_GP_pt MPT_GetGP(MPT_Tree_pt tree)
{
  if (tree == NULL) return NULL;
  return MPT_GetGP(tree->node);
}

/////////////////////////////////////////////////////////////////////
///
/// Atoms
///
/////////////////////////////////////////////////////////////////////
static GP_AtomEncoding_t MP_Type_2_GP_AtomEncoding(MP_Common_t mptype)
{
  if (MP_IsIntegerType(mptype))
  {
    if (MP_IsFixedIntegerType(mptype))
    {
      if (mptype == MP_Sint32Type)
        return GP_SlongAtomEncoding;
      if (mptype == MP_Uint32Type)
        return GP_UlongAtomEncoding;
      if (mptype == MP_Sint8Type)
        return GP_SlongAtomEncoding;
      MPT_Assume(mptype == MP_Uint8Type);
      return GP_UlongAtomEncoding;
    }
    else
    {
      long format = MPT_GetApIntFormat();
      if (format == MP_GMP) return GP_GmpApIntAtomEncoding;
      if (format == MP_PARI) return GP_PariApIntAtomEncoding;
    }
  }
  else if (MP_IsRealType(mptype))
  {
    if (mptype == MP_Real32Type) return GP_FloatAtomEncoding;
    if (mptype == MP_Real64Type) return GP_DoubleAtomEncoding;
    if (mptype == MP_ApRealType) return GP_ApRealAtomEncoding;
  }
  return GP_UnknownAtomEncoding;
}

MPT_GP_Atom_pt MPT_GetGP_Atom(MPT_Node_pt tnode)
{
  if (tnode == NULL) return NULL;
  
  MP_NodeType_t ntype = tnode->type;
  GP_AtomType_t atype;
  MPT_Arg_t modulus = NULL;
  MPT_Node_pt _tnode = NULL;
  
  if (ntype == MP_CommonMetaType && tnode->dict == MP_ProtoDict)
  {
    _tnode = tnode;
    ntype = MP_CmtProto_2_MPType(MP_COMMON_T(tnode->nvalue));
  }
  
  if (MP_IsRealType(ntype)) atype = GP_RealAtomType;
  else if (MP_IsIntegerType(ntype))
  {
    atype = GP_IntegerAtomType;

    MPT_Tree_pt mtree = MPT_AnnotValue(tnode,
                                       MP_NumberDict, MP_AnnotNumberModulos);
    if (mtree != NULL && ntype == mtree->node->type)
    {
      modulus = mtree->node->nvalue;
      if (MPT_Annot(mtree->node,MP_NumberDict,MP_AnnotNumberIsPrime) != NULL)
        atype = GP_CharPAtomType;
      else
        atype = GP_ModuloAtomType;
    }
  }
  else 
    return NULL;
  
  return new MPT_GP_Atom_t(_tnode, atype, 
                           MP_Type_2_GP_AtomEncoding(ntype), 
                           modulus);
}
 
GP_AtomEncoding_t MPT_GP_Atom_t::AtomEncoding(const void* data)
{
  if (_aencoding != GP_DynamicAtomEncoding) return _aencoding;
  MPT_Assume(_tnode == NULL && data != NULL);
  return MP_Type_2_GP_AtomEncoding(((MPT_Tree_pt) data)->node->type);
}

unsigned long MPT_GP_Atom_t::AtomUlong(const void* data) 
{
  MPT_Assume(AtomEncoding(data) == GP_UlongAtomEncoding);

  return (unsigned long) 
    (_tnode != NULL ? data : ((MPT_Tree_pt) data)->node->nvalue);
}
signed long MPT_GP_Atom_t::AtomSlong(const void* data) 
{
  MPT_Assume(AtomEncoding(data) == GP_SlongAtomEncoding);

  return (long) 
    (_tnode != NULL ? data : ((MPT_Tree_pt) data)->node->nvalue);
}
float MPT_GP_Atom_t::AtomFloat(const void* data)
{
  MPT_Assume(AtomEncoding(data) == GP_FloatAtomEncoding);
  if (_tnode != NULL)
    return *((float*) &data);
  return *((float*) &(((MPT_Tree_pt) data)->node->nvalue));
}
double MPT_GP_Atom_t::AtomDouble(const void* data)
{
  MPT_Assume(AtomEncoding(data) == GP_DoubleAtomEncoding);

  return (double) 
    MP_REAL64_T((_tnode!=NULL ? data : ((MPT_Tree_pt) data)->node->nvalue));
}
const void* MPT_GP_Atom_t::AtomGmpApInt(const void* data)
{
  MPT_Assume(AtomEncoding(data) == GP_GmpApIntAtomEncoding);

  return (_tnode != NULL ? data : ((MPT_Tree_pt) data)->node->nvalue);
}
const void* MPT_GP_Atom_t::AtomPariApInt(const void* data)
{
  MPT_Assume(AtomEncoding(data) == GP_PariApIntAtomEncoding);

  return (_tnode != NULL ? data : ((MPT_Tree_pt) data)->node->nvalue);
}


/////////////////////////////////////////////////////////////////////
///
/// Composite Types
///
/////////////////////////////////////////////////////////////////////
static GP_CompType_t 
MP_TypeDictOp_2_GP_CompType(long type,
                            MP_DictTag_t dict, MP_Common_t cvalue) 
{
  if (type == MP_CommonOperatorType || type == MP_CommonMetaOperatorType)
  {
    if (dict == MP_PolyDict)
    {
      if (cvalue == MP_CopPolyIdeal)
        return GP_IdealCompType;
      if (cvalue == MP_CopPolyModule)
        return GP_ModuleCompType;
    }
    else if (dict == MP_MatrixDict)
    {
      if (cvalue == MP_CopMatrixDenseMatrix) return GP_MatrixCompType;
      if (cvalue == MP_CopMatrixDenseVector) return GP_VectorCompType;
    }
    else if (dict == MP_ProtoDict && cvalue == MP_CopProtoArray)
    {
      return GP_VectorCompType;
    }
    else if (dict == MP_BasicDict)
    {
      if (cvalue == MP_CopBasicDiv) return GP_QuotientCompType;
      if (cvalue == MP_CopBasicComplex) return GP_ComplexCompType;
    }
  }
  else if (dict == MP_NumberDict && type == MP_CommonMetaType)
  {
    if (cvalue == MP_CmtNumberRational) return GP_RationalCompType;
    if (cvalue == MP_CmtNumberComplex) return GP_ComplexCompType;
  }
  return GP_UnknownCompType;
}

MPT_GP_Comp_pt MPT_GetGP_Comp(MPT_Node_pt tnode)
{
  if (tnode == NULL) return NULL;
  GP_CompType_t comptype 
    =   MP_TypeDictOp_2_GP_CompType(tnode->type, tnode->dict,
                                    MP_COMMON_T(tnode->nvalue));
  
  if (comptype == GP_UnknownCompType) return NULL;
  
  MPT_GP_pt elements = MPT_GetGP(MPT_ProtoAnnotValue(tnode));
  
  if (elements == NULL)
  {
    if (comptype == GP_RationalCompType)
      elements = new MPT_GP_Atom_t(NULL, 
                                   GP_IntegerAtomType, GP_DynamicAtomEncoding);
    else if (comptype == GP_ComplexCompType)
     elements = new MPT_GP_Atom_t(NULL, 
                                  GP_RealAtomType, GP_DynamicAtomEncoding);
    else
      return NULL;
  }
  
  if (comptype == GP_RationalCompType)  
  {
    if (elements->Type() != GP_AtomType ||
        ((MPT_GP_Atom_pt) elements)->AtomType() != GP_IntegerAtomType)
      goto null_return;
  }
  else if (comptype == GP_ComplexCompType)
  {
    if (elements->Type() != GP_AtomType ||
        ((MPT_GP_Atom_pt) elements)->AtomType() != GP_RealAtomType)
      goto null_return;
  }
  else if (comptype == GP_IdealCompType)
  {
    if (elements->Type() != GP_PolyType ||
        ((MPT_GP_Poly_pt) elements)->IsFreeModuleVector())
      goto null_return;
  }
  else if (comptype == GP_ModuleCompType)
  {
    if (elements->Type() != GP_PolyType ||
        ! ((MPT_GP_Poly_pt) elements)->IsFreeModuleVector())
      goto null_return;
  }
  else if (comptype == GP_MatrixCompType)
  {
    MPT_Tree_pt tree = MPT_AnnotValue(tnode, 
                                      MP_MatrixDict,
                                      MP_AnnotMatrixDimension);
    MPT_Tree_pt ctree;
    long dx, dy;
    
    if (tree == NULL || tree->node->type != MP_CommonOperatorType ||
        tree->node->numchild != 2) 
      goto null_return;
    ctree = MPT_TREE_PT(tree->args[0]);
    if (ctree == NULL) goto null_return;
    if (ctree->node->type != MP_Sint32Type) 
      dx = MP_SINT32_T(ctree->node->nvalue);
    else if  (ctree->node->type != MP_Uint32Type) 
      dx = MP_UINT32_T(ctree->node->nvalue);
    else goto null_return;

    ctree  = MPT_TREE_PT(tree->args[1]);
    if (ctree->node->type != MP_Sint32Type) 
      dy = MP_SINT32_T(ctree->node->nvalue);
    else if  (ctree->node->type != MP_Uint32Type) 
      dy = MP_UINT32_T(ctree->node->nvalue);
    else goto null_return;
    return new MPT_GP_MatrixComp_t(tnode, elements, dx, dy);
  }
  return new MPT_GP_Comp_t(tnode, comptype, elements);
  
  null_return:
  delete elements;
  return NULL;
}

/////////////////////////////////////////////////////////////////////
///
/// Polys
///
/////////////////////////////////////////////////////////////////////
MPT_GP_Poly_pt MPT_GetGP_Poly(MPT_Node_pt tnode)
{
  return MPT_GetGP_MvPoly(tnode);
}

bool MPT_GP_Poly_t::IsFreeModuleVector()
{
  if (MPT_Annot(_tnode,
                MP_PolyDict,
                MP_AnnotPolyModuleVector) != NULL)
    return true;
  return false;
}

MPT_GP_MvPoly_pt MPT_GetGP_MvPoly(MPT_Node_pt tnode)
{
  return MPT_GetGP_DistMvPoly(tnode);
}

MPT_GP_MvPoly_t::MPT_GP_MvPoly_t(MPT_Node_pt tnode, 
                                 MPT_GP_pt coeffs, 
                                 long nvars)
    : MPT_GP_Poly_t(tnode, coeffs)
{
  _nvars = nvars;
  MPT_Tree_pt vtree = MPT_AnnotValue(tnode,
                                     MP_PolyDict,
                                     MP_AnnotPolyVarNames);

  if (vtree != NULL && MPT_GP_IsValueIterator(vtree, MP_StringType))
  {
    _vname_iterator = new MPT_GP_ValueIterator_t(vtree, MP_StringType);
    _vname_iterator->Reset(vtree->args);
  }
  else
    _vname_iterator = NULL;
}

static GP_DistMvPolyType_t 
MPT_GetGP_DistMvPolyType(MPT_Node_pt tnode,
                         MPT_GP_pt &coeffs,
                         long &nvars)
{
  MPT_Assume(tnode != NULL);
  MPT_Node_pt node;
  MPT_Tree_pt val;
  MPT_Arg_pt  ta;
  
  // a DDP consists of monoms
  if ((val =  MPT_ProtoAnnotValue(tnode)) == NULL)
    return GP_UnknownDistMvPolyType;
  
  node = val->node;
  // a Monom is a tuple of (coeff, expvector)
  if (! (MPT_IsNode(node, MP_CommonOperatorType, MP_ProtoDict,
                    MP_CopProtoStruct, 2)))
    return GP_UnknownDistMvPolyType;
  
  // an expvector is a fixed-length array 
  ta = val->args;
  node = MPT_TREE_PT(ta[1])->node;
  if (!MPT_IsNode(node, MP_CommonMetaOperatorType, MP_ProtoDict,
                  MP_CopProtoArray) 
      || node->numchild <= 0)
    return GP_UnknownDistMvPolyType;
  else
    nvars = node->numchild;
  // of ints 
  if ((val = MPT_ProtoAnnotValue(node)) == NULL)
    return GP_UnknownDistMvPolyType;
  node = val->node;
  if (!MPT_IsNode(node,  MP_CommonMetaType, MP_ProtoDict, 
                  MP_CmtProtoIMP_Sint32))
    return GP_UnknownDistMvPolyType;

  // Look for coeffs
  coeffs = MPT_GetGP( MPT_TREE_PT(ta[0]));
  if (coeffs == NULL) return GP_UnknownDistMvPolyType;
  // Hmm should check that coeffs are Poly or Atom or Rational/Complex
  else  return  GP_DenseDistMvPolyType;
}
  
MPT_GP_DistMvPoly_pt MPT_GetGP_DistMvPoly(MPT_Node_pt tnode)
{
  if (tnode == NULL) return NULL;

  MPT_GP_pt coeffs;
  long nvars;
  
  if (MPT_GetGP_DistMvPolyType(tnode, coeffs, nvars) 
      != GP_DenseDistMvPolyType)
    return NULL;

  return new 
    MPT_GP_DistMvPoly_t(tnode, coeffs, nvars,
                        MPT_GetGP_Ordering(
                          (MPT_Node_t*)MPT_AnnotValue(tnode,
                                         MP_PolyDict,
                                         MP_AnnotPolyOrdering), 
                          nvars), 
                        MPT_GetGP_Ordering(
                          (MPT_Node_t*)MPT_AnnotValue(tnode,
                                         MP_PolyDict,
                                         MP_AnnotShouldHavePolyOrdering),
                          nvars));
  
}

MPT_GP_DistMvPoly_t::MPT_GP_DistMvPoly_t(MPT_Node_pt tnode,
                                         MPT_GP_pt coeffs, long nvars,
                                         MPT_GP_Ordering_pt has_ordering,
                                         MPT_GP_Ordering_pt should_ordering)
    : MPT_GP_MvPoly_t(tnode, coeffs, nvars), _monom_iterator(tnode)
{
  _has_ordering = has_ordering;
  _should_have_ordering = should_ordering;
}


void* MPT_GP_DistMvPoly_t::Coeff(const void* monom)
{
  return (MPT_ARG_PT(monom))[0];
}

void MPT_GP_DistMvPoly_t::ExpVector(const void* monom, long* &expvector)
{
  MP_Sint32_t* ev = MP_SINT32_PT((MPT_ARG_PT(monom))[1]);
  
  if (expvector != NULL)
  {
    long i;
    for (i=0; i < _nvars; i++) 
      expvector[i] = ev[i];
  }
  else
    expvector = (long*) ev;
}

/////////////////////////////////////////////////////////////////////
///
/// Ordering
///
/////////////////////////////////////////////////////////////////////
MPT_GP_Ordering_t::MPT_GP_Ordering_t(MPT_Tree_pt otree) 
{
  _otree = otree;
}

MPT_GP_Ordering_pt   MPT_GetGP_Ordering(MPT_Tree_pt o_tree, long nvars)  
{
  if (o_tree != NULL)
  {
    MPT_GP_Ordering_pt ordering = new MPT_GP_Ordering_t(o_tree);
    if (ordering->IsOk(nvars)) 
      return ordering;
    delete ordering;
  }
  return NULL;
}

GP_OrderingType_t MP_CcPolyOrdering_2_GP_OrderingType(MP_Common_t cc)
{
  switch(cc)
  {
      case  MP_CcPolyOrdering_Lex           : return GP_LexOrdering;
      case  MP_CcPolyOrdering_RevLex        : return GP_RevLexOrdering;
      case  MP_CcPolyOrdering_DegRevLex     : return GP_DegRevLexOrdering;
      case  MP_CcPolyOrdering_DegLex        : return GP_DegLexOrdering;
      case  MP_CcPolyOrdering_NegLex        : return GP_NegLexOrdering;
      case  MP_CcPolyOrdering_NegRevLex     : return GP_NegRevLexOrdering;
      case  MP_CcPolyOrdering_NegDegRevLex  : return GP_NegDegRevLexOrdering; 
      case  MP_CcPolyOrdering_NegDegLex     : return GP_NegDegLexOrdering;
      case  MP_CcPolyOrdering_Matrix        : return GP_MatrixOrdering;
      case  MP_CcPolyOrdering_IncComp       : return GP_IncrCompOrdering;
      case  MP_CcPolyOrdering_DecComp       : return GP_DecrCompOrdering;

      default : return GP_UnknownOrdering;
  }
}

GP_OrderingType_t MPT_GP_Ordering_t::OrderingType()
{
  if (MPT_IsNode(_otree->node, MP_CommonConstantType, MP_PolyDict))
    return 
      MP_CcPolyOrdering_2_GP_OrderingType(MP_COMMON_T(_otree->node->nvalue));
  else  if (MPT_IsNode(_otree->node, MP_CommonOperatorType, MP_BasicDict,
                       MP_CopBasicList))
    return GP_ProductOrdering;
  else
    return GP_UnknownOrdering;
}

GP_OrderingType_t MPT_GP_Ordering_t::OrderingType(const void* block)
{
  if (block == NULL) return GP_UnknownOrdering;
  MPT_Tree_pt  btree = ((MPT_Tree_pt) block);
  
  if (MPT_IsNode(btree->node, MP_CommonOperatorType, MP_BasicDict,
                 MP_CopBasicList, 3)) 
  {
    btree = MPT_TREE_PT(btree->args[0]);
    if (btree != NULL && 
        MPT_IsNode(btree->node, MP_CommonConstantType, MP_PolyDict))
      return 
        MP_CcPolyOrdering_2_GP_OrderingType(MP_COMMON_T(btree->node->nvalue));
  }
  return GP_UnknownOrdering;
}

GP_Iterator_pt MPT_GP_Ordering_t::WeightsIterator()
{
  GP_OrderingType_t otype = OrderingType();
  
  if (otype == GP_UnknownOrdering ||
      otype == GP_IncrCompOrdering ||
      otype == GP_DecrCompOrdering ||
      otype == GP_ProductOrdering)
    return NULL;
  
  MPT_Tree_pt wtree = MPT_AnnotValue(_otree->node,
                                     MP_PolyDict,
                                     MP_AnnotPolyWeights);
  if (wtree == NULL || ! MPT_GP_IsValueIterator(wtree, MP_Sint32Type))
    return NULL;
  _weights_iterator = MPT_GP_ValueIterator_t(wtree, MP_Sint32Type);
  return &(_weights_iterator);
}

GP_Iterator_pt MPT_GP_Ordering_t::WeightsIterator(const void* block)
{
  if (block == NULL) return NULL;

  GP_OrderingType_t otype = OrderingType(block);
  if (otype == GP_UnknownOrdering ||
      otype == GP_IncrCompOrdering ||
      otype == GP_DecrCompOrdering ||
      otype == GP_ProductOrdering)
    return NULL;

  MPT_Tree_pt btree = MPT_AnnotValue(_otree->node,
                                     MP_PolyDict,
                                     MP_AnnotPolyWeights);

  if (btree == NULL || ! MPT_GP_IsValueIterator(btree, MP_Sint32Type)) 
    return NULL;
  _block_weights_iterator = MPT_GP_ValueIterator_t(btree, MP_Sint32Type);
  
  return &(_block_weights_iterator);
}

GP_Iterator_pt MPT_GP_Ordering_t::BlockOrderingIterator()
{
  GP_OrderingType_t otype = OrderingType();
  
  if (otype != GP_ProductOrdering) return NULL;
  
  _block_iterator =  MPT_GP_Iterator_t(_otree->node);
  _block_iterator.Reset(_otree->args);
  return &(_block_iterator);
}

long MPT_GP_Ordering_t::BlockLength(const void* block)
{
  long low = -1;
  long high = -2;

  if (block == NULL) return -1;
  
  GP_OrderingType_t otype = OrderingType(block);
  if (otype == GP_UnknownOrdering ||
      otype == GP_ProductOrdering)  
    return -1;
  
  MPT_Tree_pt btree = MPT_TREE_PT(block);

  if (btree != NULL &&
      MPT_IsNode(btree->node, MP_CommonOperatorType, MP_BasicDict,
                 MP_CopBasicList, 3))
  {
    MPT_Tree_pt ctree = MPT_TREE_PT(btree->args[1]);
    if (ctree == NULL) return -1;
    if (ctree->node->type != MP_Sint32Type) 
      low = MP_SINT32_T(ctree->node->nvalue);
    else if  (ctree->node->type != MP_Uint32Type) 
      low = MP_UINT32_T(ctree->node->nvalue);
    else return -1;

    ctree  = MPT_TREE_PT(btree->args[2]);
    if (ctree == NULL) return -1;
    if (ctree->node->type != MP_Sint32Type) 
      high = MP_SINT32_T(ctree->node->nvalue);
    else if  (ctree->node->type != MP_Uint32Type) 
      high = MP_UINT32_T(ctree->node->nvalue);
    else return -1;
  }

  return high - low;
}

    
                                   
    
     
  


  
  
  
    










