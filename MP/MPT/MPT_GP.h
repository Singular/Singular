/******************************************************************
 *
 * File:    MPT_GP.h
 * Purpose: Main Header file for MP implementation of GP classes
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 11/98
 *
 ******************************************************************/
#ifndef _MPT_GP_H_
#define _MPT_GP_H_

#include <MPT.h>
#include <GP.h>

/******************************************************************
 *                                                                     
 *  Forward typedefs of pointers
 *   
 ******************************************************************/
typedef class MPT_GP_t  * MPT_GP_pt;
typedef class MPT_GP_Atom_t * MPT_GP_Atom_pt;
typedef class MPT_GP_Comp_t * MPT_GP_Comp_pt;
typedef class MPT_GP_Poly_t * MPT_GP_Poly_pt;
typedef class MPT_GP_MvPoly_t       * MPT_GP_MvPoly_pt;
typedef class MPT_GP_DistMvPoly_t   * MPT_GP_DistMvPoly_pt;
typedef class MPT_GP_Ordering_t * MPT_GP_Ordering_pt;

/******************************************************************
 *                                                                     
 *  Base classes for objects and specs
 *   
 ******************************************************************/
class MPT_Top_t
{
public:
  void* operator new(size_t size)
    {return IMP_MemAllocFnc(size);}
  void operator delete(void* p, size_t s)
    {IMP_MemFreeFnc(p,s);}

  virtual ~MPT_Top_t() {}
};
  
class MPT_GP_t : public virtual GP_t, public MPT_Top_t 
{

public:
  virtual GP_Atom_pt    Atom() {return NULL;}
  virtual GP_Comp_pt    Comp() {return NULL;}
  virtual GP_Poly_pt    Poly() {return NULL;}
  virtual ~MPT_GP_t() {}

protected:
  MPT_GP_t()                         {_tnode = NULL;}
  MPT_GP_t(MPT_Node_pt tnode)     {_tnode = tnode;}
  MPT_Node_pt _tnode;
};
MPT_GP_pt MPT_GetGP(MPT_Node_pt tnode);
MPT_GP_pt MPT_GetGP(MPT_Tree_pt tree);

typedef class MPT_GP_Iterator_t : public GP_Iterator_t, public MPT_Top_t
{
public:
  MPT_GP_Iterator_t(MPT_Node_pt tnode);
  MPT_GP_Iterator_t();
  virtual long  N()                       {return _n;}
  virtual void* Next();
  virtual void  Reset(const void* data);
  ~MPT_GP_Iterator_t() {}

private:
  long _i;
  long _n;
  MPT_Arg_pt _args;
  bool _dynamic;
} * MPT_GP_Iterator_pt;

typedef class MPT_GP_ValueIterator_t : public MPT_GP_Iterator_t
{
public:
  MPT_GP_ValueIterator_t(MPT_Tree_pt tree, long vtype);
  MPT_GP_ValueIterator_t();
  void* Next();

private:
  MPT_Tree_pt _prototype;
  long _vtype;
};
bool MPT_GP_IsValueIterator(MPT_Tree_pt tree, long vtype = -1);

  
MPT_GP_Atom_pt      MPT_GetGP_Atom(MPT_Node_pt tnode);
MPT_GP_Comp_pt      MPT_GetGP_Comp(MPT_Node_pt tnode);
MPT_GP_Poly_pt      MPT_GetGP_Poly(MPT_Node_pt tnode);
MPT_GP_MvPoly_pt    MPT_GetGP_MvPoly(MPT_Node_pt tnode);
MPT_GP_DistMvPoly_pt MPT_GetGP_DistMvPoly(MPT_Node_pt tnode);
MPT_GP_Ordering_pt MPT_GetGP_Ordering(MPT_Tree_pt o_tree, long nvars);

/////////////////////////////////////////////////////////////////////
///
/// Ordering
///
/////////////////////////////////////////////////////////////////////
class MPT_GP_Ordering_t : public GP_Ordering_t
{
  friend MPT_GP_Ordering_pt MPT_GetGP_Ordering(MPT_Tree_pt o_tree, long nvars);
  
public:
  
  // type 
  GP_OrderingType_t OrderingType();
  GP_OrderingType_t OrderingType(const void* block);

  // weights (and number of weights), for weighted orderings
  GP_Iterator_pt WeightsIterator();
  GP_Iterator_pt WeightsIterator(const void* block);

  // blocks of orderings and number of blocks for product orderings 
  GP_Iterator_pt BlockOrderingIterator();

  // only relevant for blocks
  long BlockLength(const void* block);

protected:
  MPT_GP_Ordering_t(MPT_Tree_pt otree);
  
private:
  MPT_Tree_pt _otree;
  MPT_GP_Iterator_t _block_iterator;
  MPT_GP_ValueIterator_t _weights_iterator;
  MPT_GP_ValueIterator_t _block_weights_iterator;
};
      

/////////////////////////////////////////////////////////////////////
///
/// Atoms
///
/////////////////////////////////////////////////////////////////////
class MPT_GP_Atom_t : public GP_Atom_t, public MPT_GP_t
{
  friend MPT_GP_Atom_pt MPT_GetGP_Atom(MPT_Node_pt tnode);
  friend MPT_GP_Comp_pt MPT_GetGP_Comp(MPT_Node_pt tnode);

public:
  GP_Atom_pt        Atom()          {return this;}
  GP_AtomType_t     AtomType()      {return _atype;}
  GP_AtomEncoding_t AtomEncoding()  {return _aencoding;} 
  void*             AtomModulus()   {return _modulus;} 
  GP_AtomEncoding_t AtomEncoding(const void* data);

  unsigned long     AtomUlong(const void* data); 
  signed long       AtomSlong(const void* data); 
  float             AtomFloat(const void* data);
  double            AtomDouble(const void* data);
  const void*       AtomGmpApInt(const void* data); 
  const void*       AtomPariApInt(const void* data);

protected:
  MPT_GP_Atom_t(MPT_Node_pt tnode, 
                GP_AtomType_t atype, GP_AtomEncoding_t aencoding,
                MPT_Arg_t modulus = NULL) 
      : MPT_GP_t(tnode) 
    {_atype = atype; _aencoding = aencoding; _modulus = modulus;}

private:
  GP_AtomType_t _atype;
  GP_AtomEncoding_t _aencoding;
  MPT_Arg_t _modulus;
};

/////////////////////////////////////////////////////////////////////
///
/// Composites
///
/////////////////////////////////////////////////////////////////////
class MPT_GP_Comp_t : public GP_Comp_t, public MPT_GP_t
{
  friend MPT_GP_Comp_pt MPT_GetGP_Comp(MPT_Node_pt tnode);

protected:
  MPT_GP_Comp_t(MPT_Node_pt tnode, GP_CompType_t ctype, MPT_GP_pt elements) 
      : MPT_GP_t(tnode), _iterator(tnode)
    {_ctype = ctype; _elements = elements;}
  
public:
  GP_Comp_pt        Comp()          {return this;}
  GP_CompType_t     CompType()      {return _ctype;}
  GP_pt             Elements()      {return _elements;}

  GP_Iterator_pt    ElementDataIterator(const void* data)
    {_iterator.Reset(data); return &(_iterator);}

  ~MPT_GP_Comp_t()   {delete _elements;}

private:
  GP_CompType_t _ctype;
  MPT_GP_pt _elements;
  MPT_GP_Iterator_t _iterator;
};

class MPT_GP_MatrixComp_t : public MPT_GP_Comp_t
{
  friend MPT_GP_Comp_pt MPT_GetGP_Comp(MPT_Node_pt tnode);

public:
  void MatrixDimension(long &dx, long &dy)
   {dx = _dx; dy = _dy;} 
  
protected:
  MPT_GP_MatrixComp_t(MPT_Node_pt tnode, MPT_GP_pt elements,     
                      long dx, long dy) 
      : MPT_GP_Comp_t(tnode, GP_MatrixCompType, elements)
    {_dx = dx; _dy = dy;}

private:
  long _dx;
  long _dy;
};

/////////////////////////////////////////////////////////////////////
///
/// Polys
///
/////////////////////////////////////////////////////////////////////
class MPT_GP_Poly_t : public virtual GP_Poly_t, public MPT_GP_t
{
  friend MPT_GP_Poly_pt MPT_GetGP_Poly(MPT_Node_pt tnode);

protected:
  MPT_GP_Poly_t(MPT_Node_pt tnode, MPT_GP_pt coeffs) 
      : MPT_GP_t(tnode)
    {_coeffs = coeffs;}
  MPT_GP_pt _coeffs;

public:
  GP_Poly_pt            Poly()      {return this;}
  virtual GP_UvPoly_pt  UvPoly()    {return NULL;} 
  virtual GP_MvPoly_pt  MvPoly()    {return NULL;} 

  GP_pt         Coeffs()            {return _coeffs;} 
  bool          IsFreeModuleVector();

  void*         MinPoly()           {return NULL;}                  // TBC
  GP_Property_t IsIrreducible()     {return GP_IsUnknownProperty;}  // TBC

  ~MPT_GP_Poly_t()  {delete _coeffs;}

};

class MPT_GP_MvPoly_t : public virtual GP_MvPoly_t, public MPT_GP_Poly_t
{
  friend MPT_GP_MvPoly_pt MPT_GetGP_MvPoly(MPT_Node_pt tnode);
  
public:
  GP_MvPoly_pt              MvPoly()        {return this;}
  virtual GP_DistMvPoly_pt  DistMvPoly()    {return NULL;}
  virtual GP_RecMvPoly_pt   RecMvPoly()     {return NULL;}
  
  long              NumberOfVars()  {return _nvars;}
  GP_Iterator_pt    VarNamesIterator() 
    {
      if (_vname_iterator != NULL) _vname_iterator->Reset(_tnode); 
      return (GP_Iterator_pt) _vname_iterator;
    }
  
  ~MPT_GP_MvPoly_t() {if (_vname_iterator != NULL) delete _vname_iterator;}

protected:
  MPT_GP_MvPoly_t(MPT_Node_pt tnode, MPT_GP_pt coeffs, long nvars);
  long _nvars;
  MPT_GP_Iterator_pt _vname_iterator;
};



class MPT_GP_DistMvPoly_t 
  : public virtual GP_DistMvPoly_t, public MPT_GP_MvPoly_t
{
  friend MPT_GP_DistMvPoly_pt MPT_GetGP_DistMvPoly(MPT_Node_pt tnode);
  
public:
  // only DenseDistMvPoly is implemented, at the moment
  GP_DistMvPoly_pt  DistMvPoly()            {return this;}
  GP_DistMvPolyType_t DistMvPolyType()      {return GP_DenseDistMvPolyType;}
  GP_Ordering_pt HasOrdering()              {return _has_ordering;}
  GP_Ordering_pt ShouldHaveOrdering()       {return _should_have_ordering;}
      
  GP_Iterator_pt MonomIterator(const void* data)
    {_monom_iterator.Reset(data); return &(_monom_iterator);}
  void*         Coeff(const void* monom);
  void          ExpVector(const void* monom, long* &expvector);
  
  ~MPT_GP_DistMvPoly_t()
    {
      if (_has_ordering != NULL) delete _has_ordering;
      if (_should_have_ordering != NULL) delete _should_have_ordering;
    }

protected:
  MPT_GP_DistMvPoly_t(MPT_Node_pt tnode, MPT_GP_pt coeffs, long nvars,
                      MPT_GP_Ordering_pt has_ordering,
                      MPT_GP_Ordering_pt should_have_ordering);

private:
  MPT_GP_Ordering_pt _has_ordering;
  MPT_GP_Ordering_pt _should_have_ordering;
  MPT_GP_Iterator_t _monom_iterator;
};


#endif // _MPT_GP_H_



