/******************************************************************
 *
 * File:    MP_PutGP.h
 * Purpose: Puts an GP Object to an MP link
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 11/98
 *
 ******************************************************************/

#include "GP.h"
#include "MP.h"


MP_Status_t MP_PutGP_Spec(MP_Link_pt link, GP_pt gp, 
                          bool meta = TRUE, void* data = NULL)
{
  switch(gp->Type())
  {
      case GP_AtomType:
        return MP_PutGP_AtomSpec(link, gp->Atom(), meta, data);
        
      case GP_PolyType:
        return MP_PutGP_PolySpec(link, gp->Poly(), meta, data);
        
      case GP_CompType:
        return MP_PutGP_CompSpec(link, gp->Comp(), meta, data);
        
      default:
        return MP_SetError(link, MP_ExternalError);
  }
}

MP_Status_t MP_PutGP_Data(MP_Link_pt link, GP_pt gp, void* data)
{
  switch(gp->Type())
  {
      case GP_AtomType:
        return MP_PutGP_AtomData(link, gp->Atom(), data);
        
      case GP_PolyType:
        return MP_PutGP_PolyData(link, gp->Poly(), data);
        
      case GP_CompType:
        return MP_PutGP_CompData(link, gp->Comp(), data);
        
      default:
        return MP_SetError(link, MP_ExternalError);
  }
}


/////////////////////////////////////////////////////////////////////
///
/// Atoms
///
/////////////////////////////////////////////////////////////////////
MP_Status_t MP_PutGP_AtomSpec(MP_Link_pt link, GP_Atom_pt gp, 
                              bool meta = TRUE, void* data = NULL)
{
  GP_AtomEncoding aencoding = gp->AtomEncoding();
  GP_AtomType_t atype = AtomType();
  MP_Common_t mtype;
  MP_DictType_t dict = MP_ProtoDict;
  MP_NumAnnot_t numannots = 0;

  if (aencoding == GP_DynamicAtomEncoding && meta == FALSE)
    aencoding = gp->DynamicAtomEncoding(data);
  
  switch(aencoding)
  {
      case GP_SlongAtomEncoding:
      case GP_SintAtomEncoding:
        mtype = MP_CmtProtoIMP_Sint32;
        break;
        
      case GP_UintAtomEncoding:
      case GP_UlongAtomEncoding:
        mtype = MP_CmtProtoIMP_Uint32;
        break;

      case GP_FloatAtomEncoding:
        mtype = MP_CmtProtoIMP_Real32; break;
        
      case GP_DoubleAtomEncoding:
        mtype = MP_CmtProtoIMP_Real64; break;

      case GP_IncrApIntAtomEncoding:
      case GP_DecrApIntAtomEncoding:
      case GP_GmpApIntAtomEncoding:
      case GP_PariApIntAtomEncoding:
        mtype =  MP_CmtProtoIMP_ApInt; break;
        
      case GP_DynamicAtomEncoding:
      {
        dict = MP_NumberDict;
        switch(atype)
        {
            case GP_IntegerAtomType:
              mtype = MP_CmtNumberInteger; break;
              
            case GP_RealAtomType:
              mtype = MP_CmtNumberReal; break;
              
            case GP_CharPAtomType:
              mtype = MP_CmtNumberCharP; break;
              
            case GP_ModuloAtomType:
              mtype = MP_CmtNumberModulo; break;
              
            default:
              return MP_SetError(link, MP_ExternalError);
        }
        break;
      }
      
      default:
        return MP_SetError(link, MP_ExternalError);
  }

  if (atype == GP_CharPAtomType || atype == GP_ModuloAtomType)
    numannots = 1;

  if (meta == TRUE)
    ERR_CHK(IMP_PutNodeHeader(link,MP_CommonMetaType,dict,mtype,numannots,0));
  else
    ERR_CHK(IMP_PutNodeHeader(link, 
                              MP_CmtProt_2_MPType(mtype), 0, 0, numannots, 0));

  if (numannots == 1)
  {
    MP_PutAnnotationPacket(link,
                           MP_NumberDict,
                           MP_AnnotNumberModulos,
                           MP_AnnotValuated);
    if (atype == GP_ModuloAtomType)
      numannots = 0;
    
    if (mtype == MP_CmtProtoIMP_Sint32)
      ERR_CHK(MP_PutSint32Packet(link, 
                                 (MP_Sint32_t) gp->AtomModulus(), numannots));
    else if (mtype == MP_CmtProtoIMP_Uint32)
      ERR_CHK(MP_PutUint32Packet(link, 
                                 (MP_Uint32_t) gp->AtomModulus(), numannots));
    else if (mtype == MP_CmtProtoIMP_ApInt)
      ERR_CHK(MP_PutApIntPacket(link, 
                                (MP_ApInt_t) gp->AtomModulus(), numannots));
    else
      return MPT_SetError(link, MPT_ExternalError);
    
    if (numannots == 1)
      ERRCHK(MP_PutAnnotationPacket(link,
                                      MP_NumberDict,
                                      MP_AnnotNumberIsPrime,
                                    0));
  }
  
  return MP_Success;
}

MP_Status_t MP_PutGP_AtomData(MP_Link_pt link, GP_Atom_pt gp, void* data)
{
  GP_AtomEncoding aencoding = gp->AtomEncoding();

  if (aencoding == GP_DynamicAtomEncoding)
    aencoding = gp->DynamicAtomEncoding(data);
  
  switch(aencoding)
  {
      case GP_UlongAtomEncoding:
        return IMP_PutUint32(link, (MP_Uint32_t) gp->AtomUlong(data));
        
      case GP_UintAtomEncoding:
        return IMP_PutUint32(link, (MP_Uint32_t) gp->AtomUint(data));
        
      case GP_SlongAtomEncoding:
        return IMP_PutSint32(link, (MP_Sint32_t) gp->AtomSlong(data));
        
      case GP_SintAtomEncoding:
        return IMP_PutSint32(link, (MP_Sint32_t) gp->AtomSint(data));

      case GP_FloatAtomEncoding:
        return IMP_PutReal32(link, (MP_Real32_t) gp->AtomFloat(data));
        
      case GP_DoubleAtomEncoding:
        return IMP_PutReal64(link, (MP_Real64_t) gp->AtomDouble(data));

      case GP_PariApIntAtomEncoding:
        // Hmm .. this should be implemented more carefully
        return IMP_PutApInt(link, (MP_ApInt_t) gp->AtomPariApInt(data));
        
      case GP_GmpApIntAtomEncoding:
        return IMP_PutApInt(link, (MP_ApInt_t) gp->AtomGmpApInt(data));

      case GP_GmpApRealAtomEncoding:
        return IMP_PutApReal(link, (MP_ApInt_t) gp->AtomGmpApReal(data));

      default:
        return MP_SetError(link, MP_ExternalError);
  }
}

/////////////////////////////////////////////////////////////////////
///
/// Composites
///
/////////////////////////////////////////////////////////////////////
MP_Status_t MP_PutGP_CompSpec((MP_Link_pt link, GP_Comp_pt gp, 
                              bool meta = TRUE, void* data = NULL)
{
  GP_CompType_t ctype = CompType();
  MP_NodeType_t ntype = MP_CommonMetaOperatorType;
  MP_DictTag_t dict;
  MP_Common_t cval;
  MP_NumAnnot_t numannot = 1;
  MP_NumChild_t numchild = 0;
  

  if (ctype == GP_RationalCompType ||
      ctype == GP_ComplexCompType)
  {
    if (meta == TRUE)
      ERR_CHK(MP_PutCommonMetaTypePacket(link, MP_NumberDict, 
                                         (ctype == GP_RationalCompType ?
                                          MP_CmtNumberRational :
                                          MP_CmtNumberComplex), 0));
    return MP_Success;
  }
  
  if (meta == FALSE)
  {
    numchild = gp->ElementIterator()->N();
    ntype = MP_CommonOperatorType;
  }
  
  switch (ctype)
  {
      case GP_IdealCompType:
        dict = MP_PolyDict;
        cval = MP_CopPolyIdeal;
        break;

      case GP_ModuleCompType:
        dict = MP_PolyDict;
        cval = MP_CopPolyModule;
        break;

      case GP_QuotientCompType:
        dict = MP_BasicDict;
        cval = MP_CopBasicDiv;
        break;
        
      case GP_VectorCompType:
        dict = MP_MatrixDict;
        cval = MP_CopMatrixDenseVector;
        break;
        
      case GP_MatrixCompType:
        dict = MP_MatrixDict;
        cval = MP_CopMatrixDenseMatrix;
        numannot++;
        break;
        
      default:
        return MP_SetError(link, MP_ExternalError);
  }
  ERR_CHK(IMP_PutNodeHeader(link, ntype, dict, cval, numannot, numchild));
  ERR_CHK(MP_PutAnnotationPacket(link, MP_ProtoDict, MP_AnnotProtoProtoType,
                                 MP_AnnotReqValNode));
  ERR_CHK(MP_PutGP_Spec(link, gp->Elements()));
  if (ctype == GP_MatrixCompType)
  {
    int dx, dy;
    gp->
    
    ERR_CHK(MP_PutAnnotationPacket(link,
                                   MP_MatrixDict,
                                   MP_AnnotMatrixDimension,
                                   MP_AnnotReqValNode));
    ERR_CHK(MP_PutCommonOperatorPacket(link,
                                        MP_BasicDict,
                                        MP_CopBasicList,
                                        0, 2));
    
    mp_failr(MP_PutUint32Packet(link, (MP_Uint32_t) , 0));
    mp_failr(MP_PutUint32Packet(link, (MP_Uint32_t) vlength, 0));
  
  
}
bool GP_Comp_t::IsCompDataOk(const void* data)
{
  GP_Iterator_pt it = ElementDataIterator(data);
  GP_pt elements = Elements();
  GP_CompType_t ctype = CompType();
  int i, n;
  
  
  if (it == NULL) return false;
  n = it->N();
  if (n < 0) return false;
  
  switch (CompType())
  {
      case GP_MatrixCompType:
      {
        int dx, dy;
        MatrixDimension(data, dx, dy);
        if (dx < 0 || dy < 0) return false;
        if (dx*dy != n) return false;
        break;
      }
      
      case GP_RationalCompType:
      case GP_QuotientCompType:
      case GP_ComplexCompType:
        if (n == 0 || n > 2) return false;
        break;
        
      case GP_UnknownCompType:
        return false;
        
      default:
        break;
  }

  for (i=0; i<n; i++)
    if (elements->IsDataOk(it->Next()) == false) return false;
  return true;
}


/////////////////////////////////////////////////////////////////////
///
/// Polys
///
/////////////////////////////////////////////////////////////////////
bool GP_Poly_t::IsPolySpecOk()
{
  void* mpoly;

  switch(PolyType())
  {
      case GP_UvPolyType:
        if (UvPoly() != NULL && UvPoly()->IsUvPolySpecOk()) break;
        return false;

      case GP_MvPolyType:
        if (MvPoly() != NULL && MvPoly()->IsMvPolySpecOk()) break;
        return false;

      default:
        return false;
  }
  if (Coeffs()->IsSpecOk() == false) return false;
  
  mpoly = MinPoly();
  if (mpoly != NULL) return IsPolyDataOk(mpoly);
  return true;
}
bool GP_Poly_t::IsPolyDataOk(const void* data)
{
  
  switch(PolyType())
  {
      case GP_UvPolyType:
        return UvPoly() != NULL && UvPoly()->IsUvPolyDataOk(data);

      case GP_MvPolyType:
        return MvPoly() != NULL && MvPoly()->IsMvPolyDataOk(data);

      default:
        return false;
  }
}


/////////////////////////////////////////////////////////////////////
///
/// Univariate Polys
///
/////////////////////////////////////////////////////////////////////
bool GP_UvPoly_t::IsUvPolySpecOk()
{
  if (UvPolyType() == GP_UnknownUvPolyType) return false;
  return true;
}

bool GP_UvPoly_t::IsUvPolyDataOk(const void* data)
{
  GP_Iterator_pt it = TermIterator(data);
  bool isSparse = (UvPolyType() == GP_SparseUvPolyType);
  GP_pt coeff = Coeffs();

  int i, n;
  void* term;
  
  
  if (it == NULL) return false;
  n = it->N();
  
  if (n < 0) return false;
  
  for (i = 0; i<n; i++)
  {
    term = it->Next();
    if (isSparse) 
    {
      if (coeff->IsDataOk(ExpCoeff(term)) == false ||
          ExpValue(term) < 0) return false;
    }
    else
    {
      if (coeff->IsDataOk(term) == false) return false;
    }
  }
  return true;
}


/////////////////////////////////////////////////////////////////////
///
/// Multivariate Polys
///
/////////////////////////////////////////////////////////////////////
bool GP_MvPoly_t::IsMvPolySpecOk()
{
  if (NumberOfVars() <= 0) 
    return false;
  
  switch(MvPolyType())
  {
      case GP_DistMvPolyType:
        return DistMvPoly() != NULL && DistMvPoly()->IsDistMvPolySpecOk();
        
      case GP_RecMvPolyType:
        return RecMvPoly() != NULL && RecMvPoly()->IsRecMvPolySpecOk();

      default:
        return false;
  }
}
bool GP_MvPoly_t::IsMvPolyDataOk(const void* data)
{
  switch(MvPolyType())
  {
      case GP_DistMvPolyType:
        return DistMvPoly() != NULL && DistMvPoly()->IsDistMvPolyDataOk(data);
        
      case GP_RecMvPolyType:
        return RecMvPoly() != NULL && RecMvPoly()->IsRecMvPolyDataOk(data);

      default:
        return false;
  }
}


/////////////////////////////////////////////////////////////////////
///
/// Distributed multivariate polys
///
/////////////////////////////////////////////////////////////////////
bool GP_DistMvPoly_t::IsDistMvPolySpecOk()
{

  if (DistMvPolyType() == GP_UnknownDistMvPolyType) return false;

  GP_Ordering_pt has_ordering = HasOrdering();
  GP_Ordering_pt should_ordering = ShouldHaveOrdering();
  
  return 
    (has_ordering == NULL || (has_ordering->IsOk(NumberOfVars()))) &&
    (should_ordering == NULL || (should_ordering->IsOk(NumberOfVars())));
}
bool GP_DistMvPoly_t::IsDistMvPolyDataOk(const void* data)
{
  GP_DistMvPolyType_t type = DistMvPolyType();
  GP_Iterator_pt    monoms = MonomIterator(data);
  GP_pt             coeffs = Coeffs();
  void*             monom;
  int i, n,j,       nvars  = NumberOfVars();
  GP_Iterator_pt    expvector = NULL;

  if (type ==  GP_UnknownDistMvPolyType) return false;
  if (monoms == NULL) return false;
  if (nvars <= 0) return false;
  
  n = monoms->N();
  
  if (n < 0) return false;
  
  for (i=0; i<n; i++)
  {
    monom = monoms->Next();
    if (coeffs->IsDataOk(Coeff(monom)) == false) return false;
    
    if (type == GP_SparseDistMvPolyType)
    {
      int m;
      void* exp;

      if (expvector == NULL)  
      {
        expvector = ExpVectorIterator(monom);
        if (expvector == NULL) return false;
      }
      else expvector->Reset(monom);
      
      m = expvector->N();
      if (m < 0) return false;
      
      for (j = 0; j < m; j++)
      {
        exp = expvector->Next();
        if (ExpValue(exp) < 0 || ExpNumber(exp) < 0 || ExpNumber(exp) >= nvars)
          return false;
      }
    }
    else
    {
      int* evector = NULL;
      ExpVector(monom, evector);
      
      if (evector == NULL) return NULL;
      for (j = 0; j<nvars; j++)
      {
        if (evector[j] < 0) return false;
      }
    }
  }
  return true;
}


/////////////////////////////////////////////////////////////////////
///
/// recursive multivariate polys
///
/////////////////////////////////////////////////////////////////////
bool GP_RecMvPoly_t::IsRecMvPolySpecOk()
{
  return true;
}
bool GP_RecMvPoly_t::IsRecMvPolyDataOk(const void* data)
{
  if (IsNull(data)) return true;
  if (IsCoeff(data)) return Coeffs()->IsDataOk(data);

  return (
    Variable(data) >= 0 && Variable(data) < NumberOfVars() &&
    Exponent(data) > 0 &&
    IsRecMvPolyDataOk(AddSubPoly(data)) &&
    IsRecMvPolyDataOk(MultSubPoly(data))
    );
}


/////////////////////////////////////////////////////////////////////
///
/// Orderings
///
/////////////////////////////////////////////////////////////////////
bool GP_Ordering_t::IsBlockOrderingOk(const void* block_ordering)
{
  int low, high;
  
  BlockLimits(block_ordering, low, high);
  
  if (low < 0 || high < low) return false;
  
  switch (OrderingType(block_ordering))
  {
      case GP_UnknownOrdering:
      case GP_ProductOrdering: 
        return false;

      case GP_MatrixOrdering:
      {
        GP_Iterator_pt iter = WeightsIterator();
        if (iter == NULL) return false;
        if (iter->N() != (high - low)*(high - low)) return false;
        return true;
      }
        
      default:
        return true;
  }
}
    
bool GP_Ordering_t::IsOk(const int nvars)
{
  if (nvars <= 0) return false;
  
  switch(OrderingType())
  {
      case GP_UnknownOrdering:
        return false;
        
      case GP_VectorOrdering:
      case GP_IncrCompOrdering:
      case GP_DecrCompOrdering:
        // incomplete orderings are no good
        return false;
        
      case GP_MatrixOrdering:
      {
        GP_Iterator_pt iter = WeightsIterator();
        if (iter == NULL) return false;
        if (iter->N() != nvars*nvars) return false;
        return true;
      }
      
      case GP_ProductOrdering:
      {
        GP_Iterator_pt iter = BlockOrderingIterator();
        int i, n = 0, low, high;
        bool found_zero=false, found_nvars=false;
        void* block_ordering;
        
        if (iter !=NULL) 
        {
          for (i=0, n = iter->N(); i<n; i++)
          {
            block_ordering = iter->Next();
            if (! IsBlockOrderingOk(block_ordering)) return false;
            BlockLimits(block_ordering, low, high);
          
            if (low < 0 || high >= nvars) return false;

            // Hmm.. we should check fo complete coverage of all
            // variables However, we would need to allocate memory to do
            // so, and I'd like to avoid this here
            if (low == 0) found_zero = true;
            if (high == nvars -1) found_nvars = true;
          }
        }
        return (found_zero && found_nvars);
      }
      
      default:
        return true;
  }
}

    
