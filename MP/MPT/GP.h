/******************************************************************
 *
 * File:    GP.h
 * Purpose: Main Header file for abstract poly classes
 * Author:  Olaf Bachmann (obachman@mathematik.uni-kl.de)
 * Created: 11/96
 *
 ******************************************************************/

#ifndef _GP_H_
#define _GP_H_

/***************************************************************
 *
 * some general definitions
 * 
 ***************************************************************/

#ifndef NULL
#define NULL (0)
#endif

enum GP_Property_t
{
  GP_IsUnknownProperty,
  GP_IsTrueProperty,
  GP_IsFalseProperty
};

/******************************************************************
 *                                                                     
 *  Forward declaration of pointers
 *   
 ******************************************************************/
typedef class GP_t              * GP_pt;
typedef class GP_Atom_t         * GP_Atom_pt;
typedef class GP_Comp_t         * GP_Comp_pt;
typedef class GP_Poly_t         * GP_Poly_pt;
typedef class GP_UvPoly_t       * GP_UvPoly_pt;
typedef class GP_MvPoly_t       * GP_MvPoly_pt;
typedef class GP_DistMvPoly_t   * GP_DistMvPoly_pt;
typedef class GP_RecMvPoly_t    * GP_RecMvPoly_pt;
typedef class GP_Ordering_t     * GP_Ordering_pt;

/******************************************************************
 *                                                                     
 *  Top-level classes: GP, Object, and Iterator
 *   
 ******************************************************************/

enum GP_Type_t
{
  GP_UnknownType,
  GP_AtomType, 
  GP_PolyType, 
  GP_CompType  
};

class GP_t
{
public:
  // has to say what kind of specification it is
  virtual GP_Type_t     Type()      {return GP_UnknownType;}
  virtual GP_Atom_pt    Atom()      {return NULL;}
  virtual GP_Comp_pt    Comp()      {return NULL;}
  virtual GP_Poly_pt    Poly()      {return NULL;}
  
  // traverses through the spec tree and checks everything for
  // semantic correctness
  bool IsSpecOk();
  bool IsDataOk(const void* data);
  bool IsOk(const void* data) {return IsSpecOk() && IsDataOk(data);}
};

typedef class GP_Object_t
{
public:
  
} * GP_Object_pt;

typedef class GP_Iterator_t
{
public:
  virtual int   N()                     = 0;
  virtual void* Next()                  = 0;
  virtual void  Reset(const void* data) = 0;
  virtual ~GP_Iterator_t() {}
  
      
} * GP_Iterator_pt;


/******************************************************************
 *                                                                     
 *  Second-level derived classes: Atom, Composite, Polynomial 
 *   
 ******************************************************************/
//////////////////////////////////////////////////////////
// Atoms
//
typedef enum 
{
  GP_UnknownAtomType,
  GP_IntegerAtomType,  // Integers 
  GP_RealAtomType,     // Real
  GP_CharPAtomType,    // Mod p numbers, where p is a prime number
  GP_ModuloAtomType    // and, Mod m numbers, where m is an integer > 0.
} GP_AtomType_t;

typedef enum 
{
  GP_UnknownAtomEncoding,
  GP_DynamicAtomEncoding,
  GP_UintAtomEncoding,
  GP_SintAtomEncoding,
  GP_UlongAtomEncoding,
  GP_SlongAtomEncoding,
  GP_FloatAtomEncoding,
  GP_DoubleAtomEncoding,
  GP_IncrApIntAtomEncoding,
  GP_DecrApIntAtomEncoding,
  GP_GmpApIntAtomEncoding,
  GP_PariApIntAtomEncoding,
  GP_ApRealAtomEncoding
} GP_AtomEncoding_t;
    
class GP_Atom_t : virtual public GP_t
{
public:
  GP_Type_t Type()  {return GP_AtomType;}
  bool  IsAtomSpecOk();
  bool  IsAtomDataOk(const void* data);

  // the following need to be implemented by a child of GP_Atom_t
  virtual GP_AtomType_t AtomType()  = 0;
  // GP_UnknownAtomEncoding: means can only be determined from the atom itself
  virtual GP_AtomEncoding_t AtomEncoding()        = 0;
  // for GP_CharPAtomType and GP_ModuloAtomType
  // this returns the modulus, otherwise, this returns 0
  // Encoding of return is equivalent to that of AtomEncoding()
  virtual void* GP_AtomModulus()    {return NULL;}

  // the actual encoding for Atom whose AtomEncoding() is 
  // GP_DynamicAtomEncoding
  virtual GP_AtomEncoding_t AtomEncoding(const void* data) 
    {return GP_UnknownAtomEncoding;}
  
  // getting the value of Atoms
  virtual unsigned int  AtomUint(const void* data)  {return 0;}
  virtual signed int    AtomSint(const void* data)  {return 0;}
  virtual unsigned long AtomUlong(const void* data) {return 0;}
  virtual signed long   AtomSlong(const void* data) {return 0;}
  virtual float         AtomFloat(const void* data) {return 0.0;}
  virtual double        AtomDouble(const void* data){return 0.0;}

  virtual   unsigned int   AtomApIntLength(const void* data)   {return 0;}
  virtual   signed int     AtomApIntSign(const void* data)     {return 0;}
  virtual   void           AtomApInt(const void* data, unsigned long* apint) {}
  virtual   const unsigned long*  AtomApInt(const void* data) {return NULL;}
  virtual   const void*    AtomGmpApInt(const void* data) {return NULL;}
  virtual   const void*    AtomPariApInt(const void* data) {return NULL;}
};



//////////////////////////////////////////////////////////
// Composites
//
typedef enum 
{
  GP_UnknownCompType,
  GP_RationalCompType,
  GP_ComplexCompType,
  GP_IdealCompType,
  GP_ModuleCompType,
  GP_QuotientCompType,
  GP_VectorCompType,
  GP_MatrixCompType 
} GP_CompType_t;

class GP_Comp_t : virtual public GP_t
{
public:
  GP_Type_t Type()  {return GP_CompType;}   
  bool  IsCompSpecOk();
  bool  IsCompDataOk(const void* data);
  
  virtual   GP_CompType_t   CompType()  = 0;
  // Spec of the elements 
  virtual   GP_pt    Elements()         = 0;
  // Iterator over Elmenets
  virtual   GP_Iterator_pt  ElementDataIterator(const void* data)  = 0;
    
  virtual void MatrixDimension(const void* data, int &dx, int &dy) 
    {dx = -1; dy = -1;}
};


////////////////////////////////////////////////////////////////////
// Last, but not least, polynomials:
// We distinguish further in
//
typedef enum 
{
  GP_UnknownPolyType,
  GP_UvPolyType,  // univariate polynomials
  GP_MvPolyType   // and, multivariate polynomials
} GP_PolyType_t;

class GP_Poly_t : virtual public GP_t
{
public:
  GP_Type_t Type()  {return GP_PolyType;}
  bool  IsPolySpecOk();
  bool  IsPolyDataOk(const void* data);

  virtual GP_PolyType_t PolyType()      {return GP_UnknownPolyType;}
  virtual GP_UvPoly_pt  UvPoly()        {return NULL;}
  virtual GP_MvPoly_pt  MvPoly()        {return NULL;}
  
  virtual GP_pt   Coeffs()              = 0;

  virtual void*          MinPoly()          {return NULL;}
  virtual GP_Property_t  IsIrreducible()    {return GP_IsUnknownProperty;}
  // a specification whether the first variable of the poly is to be
  // interpreted as index of a free vector generator -- i.e. whether
  // poly is actually a vector over a free module 
  virtual bool      IsFreeModuleVector()        {return false;}
};


////////////////////////////////////////
// Univariate Polys
//
enum GP_UvPolyType_t
{
  GP_UnknownUvPolyType,
  GP_DenseUvPolyType,  // dense univariate polys
  GP_SparseUvPolyType, // sparse univariate polys
};

class GP_UvPoly_t : virtual public GP_Poly_t
{
public:
  GP_PolyType_t    PolyType()              {return GP_UvPolyType;}
  bool  IsUvPolySpecOk();
  bool  IsUvPolyDataOk(const void* data);
  
  virtual GP_UvPolyType_t  UvPolyType()                 = 0;

  // NULL means varname is "unknown"
  virtual char*     VarName()   {return NULL;}

  // For DenseUvPoly, a term is simple a coeff
  // For SparseUvPoly, a term is a tuple of (coeff, exponent)
  virtual GP_Iterator_pt TermIterator(const void* data)  = 0;

  // The next two functions are only relevant for Sparse Univariate polys
  virtual void* ExpCoeff(const void* term)     {return NULL;}
  virtual int   ExpValue(const void* term)     {return -1;}
};



////////////////////////////////////////
// Multivariate Polys
//
typedef enum 
{
  GP_UnknownMvPolyType,
  GP_DistMvPolyType,
  GP_RecMvPolyType
} GP_MvPolyType_t;

class GP_MvPoly_t : virtual public GP_Poly_t
{
public:
  GP_PolyType_t     PolyType()     {return GP_MvPolyType;}
  bool  IsMvPolySpecOk();
  bool  IsMvPolyDataOk(const void* data);

  virtual GP_MvPolyType_t   MvPolyType()    {return GP_UnknownMvPolyType;}
  virtual GP_DistMvPoly_pt  DistMvPoly()    {return NULL;}
  virtual GP_RecMvPoly_pt   RecMvPoly()     {return NULL;}
  
  // the number of variables of the poly
  virtual int   NumberOfVars()              = 0;
  // iterator for variable names
  virtual GP_Iterator_pt    VarNamesIterator() {return NULL;}
};

////////////////////////////////////////
// Distributed Multivariate Polys
//
typedef enum 
{
  GP_UnknownDistMvPolyType,
  GP_DenseDistMvPolyType,
  GP_SparseDistMvPolyType,
} GP_DistMvPolyType_t;


class GP_DistMvPoly_t : virtual public GP_MvPoly_t
{
public:
  GP_MvPolyType_t MvPolyType()  {return GP_DistMvPolyType;}
  bool  IsDistMvPolySpecOk();
  bool  IsDistMvPolyDataOk(const void* data);
  
  virtual GP_DistMvPolyType_t DistMvPolyType()              = 0;

  virtual GP_Ordering_pt    HasOrdering() {return NULL;}
  virtual GP_Ordering_pt    ShouldHaveOrdering() {return NULL;}

  virtual GP_Iterator_pt    MonomIterator(const void* data) = 0;

  // extracting Coeff from a monom
  virtual void* Coeff(const void* monom)                    = 0;

  // for DenseDistPoly, we simply let the Exp Vector be filled
  virtual void       ExpVector(const void* monom, int* &expvector) {}

  // for SparseDistPolys, we need another iterator
  virtual GP_Iterator_pt ExpVectorIterator(const void* monom) {return NULL;}
  // from which we can extract the (number, value) tuple
  virtual int ExpValue(void* exp)  {return -1;}
  virtual int ExpNumber(void* exp) {return -1;}
};


class GP_RecMvPoly_t : virtual public GP_MvPoly_t
{
public:
  GP_MvPolyType_t MvPolyType()  {return GP_RecMvPolyType;}
  bool  IsRecMvPolySpecOk();
  bool  IsRecMvPolyDataOk(const void* data);

  // A RecMvPoly is either 
  // NULL -- i.e. the zero polynom, or
  // a Coefficient
  virtual   bool   IsNull(const void* data)   {return data == NULL;}     
  virtual   bool   IsCoeff(const void* data)      = 0;
  virtual   void*  Coeff(const void* data)        = 0;
  // or a term of the form v^e*MutlSubPoly() + AddSubPoly()
  // index of variable v ( in range 0 to VarNumber()-1)
   virtual   int Variable(const void* data)       = 0;    
  // exponent e of v
   virtual   int Exponent(const void* data)       = 0;
  // multiplicative subpoly of v
   virtual   void*    MultSubPoly(const void* data)   = 0;
  // additive submonom of v
   virtual   void*    AddSubPoly(const void*  data)   = 0;
};


/******************************************************************
 *                                                                     
 * Let's come to monomial orderings
 *   
 ******************************************************************/

typedef enum 
{
  GP_UnknownOrdering,

  // either simple orderings like global orderings (which are complete)
  GP_LexOrdering, 
  GP_RevLexOrdering,
  GP_DegLexOrdering,       // can have pos. integers as weights
  GP_DegRevLexOrdering,    // can have pos. integers as weights

  // local orderings (are complete)
  GP_NegLexOrdering,
  GP_NegRevLexOrdering,
  GP_NegDegLexOrdering,    // can have pos. integers as weights
  GP_NegDegRevLexOrdering, // can have pos. integers as weights

  // matrix ordering (is complete)
  GP_MatrixOrdering,      
  // extra weight vector ordering (is incomplete)
  GP_VectorOrdering,

  // module orderings (are incomplete)
  GP_IncrCompOrdering,  
  GP_DecrCompOrdering,

  // and, product orderings, which are "compositions" of blocks of
  // simple orderings
  GP_ProductOrdering
} GP_OrderingType_t;

class GP_Ordering_t
{
public:
  bool IsOk(const int nvars);
  
  // type 
  virtual GP_OrderingType_t OrderingType() = 0;
  GP_OrderingType_t OrderingType(const void* block) 
    {return GP_UnknownOrdering;}

  // weights (and number of weights), for weighted orderings
  virtual GP_Iterator_pt WeightsIterator() {return NULL;}
  virtual  GP_Iterator_pt WeightsIterator(const void* block) {return NULL;} 

  // blocks of orderings and number of blocks for product orderings 
  virtual GP_Iterator_pt BlockOrderingIterator() {return NULL;}

  virtual void BlockLimits(const void* block, int &low, int &high) 
    {low = -1; high = 0;}

private:
  bool IsBlockOrderingOk(const void* block_ordering);
};


#if 0
/***************************************************************
 ***************************************************************
 **
 ** Some other helpful classes
 ** 
 ***************************************************************
 ***************************************************************/
// using CompleteVarNames, you do not have to bother any more about
// having less variable names then actual variables
class GP_CompleteVarNames_t 
{
  char  tmp[20];
  char* prefix;
  MP_Sint32_t vardiff;
  MP_Uint32_t varnamesnumber;
  GP_MvPoly_pt pspec;

public:
  GP_CompleteVarNames_t(GP_MvPoly_pt polyspec, char* prefix);
  MP_NumChild_t     VarNamesNumber();
  // if there are more variables then variable names, VarName uses the
  // given prefix and constructs a varname by appending the number to
  // the prefix
  char*             VarName(MP_NumChild_t i);
};


/***************************************************************
 ***************************************************************
 **
 ** Some useful macros and utilities
 ** 
 ***************************************************************
 ***************************************************************/

inline GP_OrderingType_t GP_CcPolyOrdering_2_OrderingType(MP_Common_t cc)
{
  switch(cc)
  {
      case  MP_CcPolyOrdering_Lex           : return GP_LexOrdering;
      case  MP_CcPolyOrdering_DegRevLex     : return GP_DegRevLexOrdering;
      case  MP_CcPolyOrdering_DegLex        : return GP_DegLexOrdering;
      case  MP_CcPolyOrdering_NegLex        : return GP_NegLexOrdering;
      case  MP_CcPolyOrdering_NegDegRevLex  : return GP_NegDegRevLexOrdering; 
      case  MP_CcPolyOrdering_NegDegLex     : return GP_NegDegLexOrdering;
      case  MP_CcPolyOrdering_Vector        : return GP_VectorOrdering;
      case  MP_CcPolyOrdering_Matrix        : return GP_MatrixOrdering;
      case  MP_CcPolyOrdering_IncComp        : return GP_IncCompOrdering;
      case  MP_CcPolyOrdering_DecComp       : return GP_DecCompOrdering;

      default : return GP_UnknownOrdering;
  }
}

inline MP_Common_t
GP_OrderingType_2_CcPolyOrdering(GP_OrderingType_t ordering)
{
  switch(ordering)
  {
      case GP_LexOrdering          : return MP_CcPolyOrdering_Lex;
      case GP_DegRevLexOrdering    : return MP_CcPolyOrdering_DegRevLex;
      case GP_DegLexOrdering       : return MP_CcPolyOrdering_DegLex;
      case GP_NegLexOrdering       : return MP_CcPolyOrdering_NegLex;
      case GP_NegDegLexOrdering    : return MP_CcPolyOrdering_NegDegLex;
      case GP_NegDegRevLexOrdering : return MP_CcPolyOrdering_NegDegRevLex;
      case GP_VectorOrdering       : return MP_CcPolyOrdering_Vector; 
      case GP_MatrixOrdering       : return MP_CcPolyOrdering_Matrix;
      case GP_IncCompOrdering      : return MP_CcPolyOrdering_IncComp;
      case GP_DecCompOrdering      : return MP_CcPolyOrdering_DecComp;

      default: return MP_CcPolyOrdering_Unknown;
  }
}

extern MP_Sint32_t* g_GP_TmpVector;
extern MP_Uint32_t  g_GP_TmpVectorLength;

inline MP_Sint32_t* GP_GetTmpVector(MP_NumChild_t l)
{
  if (l < g_GP_TmpVectorLength)
  {
    if (g_GP_TmpVector != NULL)
      g_GP_TmpVector = (MP_Sint32_t*)
        IMP_MemReallocFnc(g_GP_TmpVector,
                          l*sizeof(MP_Sint32_t),
                          g_GP_TmpVectorLength*sizeof(MP_Sint32_t));
    else
      g_GP_TmpVector = (MP_Sint32_t*) IMP_MemAllocFnc(l*sizeof(MP_Sint32_t));
    
      g_GP_TmpVectorLength = l;
  }   
  return g_GP_TmpVector;
}
                                                 
#ifndef NDEBUG
#include <MPT.h>
#define GP_Assume(x)   MPT_Assume(x)
#define ERR_RTR(x)                                                            \
do                                                                            \
{                                                                             \
  MP_Status_t _status = x;                                                    \
  if (x != MP_Success)                                                        \
  {                                                                           \
    fprintf(stderr, "MP_Failure return in %s, Line %d, ErrorCode %d\n, Expression\n%s\n", \
            __FILE__, __LINE__, _status, #x);                                          \
    return _status;                                                                 \
  }                                                                           \
} while(0)
#else
#define GP_Assume(x)   ((void)0)
#define ERR_RTR(x)
do                                              \
{                                               \
  MP_Status_t _status = x;                      \
  if (x != MP_Success) return _status;                \
} while(0)
#endif

#endif

#endif // _GP_H_

