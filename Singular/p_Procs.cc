/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs.cc
 *  Purpose: implementation of primitive procs for polys
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Procs.cc,v 1.15 2000-10-16 12:06:37 obachman Exp $
 *******************************************************************/
#include <string.h>

#include "mod2.h"


/***************************************************************
 *  
 * Configurations
 * 
 *******************************************************************/

/***************************************************************
 Here is how it works:
 At run-time, SetProcs is used to choose the appropriate PolyProcs 
              based on the ring properies.
 At generate-time, SetProcs is used to generate all 
              possible PolyProcs.
 Which PolyProcs are generated/used can be controled by values of 
 HAVE_FAST_P_PROCS, HAVE_FAST_LENGTH, HAVE_FAST_ORD, and FAST_FIELD
 
 At generate-time, the file p_Procs.inc is generated,
 which provides implementations of the p_Procs, based on
 the p_*_Templates.cc and header files which provide the respective
 macros.

 At run-time, a fast proc is set/choosen if found/generated, else 
 a general proc is set/choosen.
 *******************************************************************/

// Set HAVE_FAST_P_PROCS to:
//   0 -- only FieldGeneral_LengthGeneral_OrdGeneral
//   1 -- plus FieldZp_Length*_OrdGeneral procs
//   2 -- plus FieldZp_Length*_Ord* procs
//   3 -- plus FieldQ_Length*_Ord*
//   4 -- plus FieldGeneral_Length*_OrdGeneral procs
//   5 -- all Field*_Length*_Ord* procs
#ifdef NDEBUG
const int HAVE_FAST_P_PROCS = 3;
#else
const int HAVE_FAST_P_PROCS = 0;
#endif

// Set HAVE_FAST_FIELD to:
//   0 -- only FieldGeneral
//   1 -- special cases for FieldZp
//   2 -- plus special cases for FieldQ
//   nothing else is implemented, yet
const int HAVE_FAST_FIELD = 2;

// Set HAVE_FAST_LENGTH to:
//   0 -- only LengthGeneral
//   1 -- special cases for length <= 1
//   2 -- special cases for length <= 2
//   3 -- special cases for length <= 4
//   4 -- special cases for length <= 8
const int HAVE_FAST_LENGTH = 3;

// Set HAVE_FAST_ORD to:
//  0  -- only OrdGeneral
//  1  -- special for ords with n_min <= 1
//  2  -- special for ords with n_min <= 2
//  3  -- special ords for with n_min <= 3
//  4  -- special for all ords
const int HAVE_FAST_ORD = 4;

// undefine this, if ExpLSize always equals CompLSize
#define HAVE_LENGTH_DIFF
// Set HAVE_FAST_ZERO_ORD to:
//  0 -- no zero ords are considered 
//  1 -- only ZeroOrds for OrdPosNomogPosZero, OrdNomogPosZero, OrdPomogNegZero
//  2 -- ZeroOrds for all
const int HAVE_FAST_ZERO_ORD = 2;

// Predicate which returns true if alloc/copy/free of numbers is
// like that of Zp
#define ZP_COPY_FIELD(field) \
  (field == FieldZp || field == FieldGF || field == FieldR)

/***************************************************************
 *  
 * Definitions of our fields, lengths, ords, procs we work with
 * 
 *******************************************************************/

// Here are the different parameters for setting the PolyProcs:

// If you add/remove things from here, also remeber to adjust the
// respective *_2_String 
typedef enum p_Field
{
  FieldGeneral = 0,
  FieldZp,          
  FieldR,
  FieldGF,
  FieldQ,
#if HAVE_MORE_FIELDS_IMPLEMENTED
  FieldLong_R,
  FieldLong_C,
  FieldZp_a,
  FieldQ_a,
#endif
  FieldUnknown
};
typedef enum p_Length // Length of exponent vector in words
{
  LengthGeneral = 0, // n >= 1
  LengthEight,       // n == 8   
  LengthSeven,
  LengthSix,
  LengthFive,
  LengthFour,
  LengthThree,
  LengthTwo,
  LengthOne,
  LengthUnknown
};
typedef enum p_Ord  
{                   
  OrdGeneral = 0,   
                    //     ordsgn   
                    //  0   1   i   n-1 n   n_min   Example
  OrdPomog,         //  +   +   +   +   +   1       (lp,C)
  OrdNomog,         //  -   -   -   -   -   1       (ls, c), (ds, c)
#define ORD_MAX_N_1 OrdNomog

#ifdef HAVE_LENGTH_DIFF
  OrdPomogZero,     //  +   +   +   +   0   2       (Dp, C), Even vars
  OrdNomogZero,     //  -   -   -   -   0   2       (ds, c), Even vars
#endif

  OrdNegPomog,      //  -   +   +   +   +   2       (c, lp), (Ds, c)
  OrdPomogNeg,      //  +   +   +   +   -   2       (lp, c)
#define ORD_MAX_N_2 OrdPomogNeg

  OrdPosNomog,      //  +   -   -   -   +   3       (dp, c) (for n == 2, use PomogNeg)
  OrdNomogPos,      //  -   -   -   -   +   3       (ls, C) (for n == 2, use NegPomog)

#ifdef HAVE_LENGTH_DIFF
  OrdNegPomogZero,  //  -   +   +   +   0   3       (c, lp), (Ds, c)
  OrdPomogNegZero,  //  +   +   +   -   0   3       (lp, c)
#endif

  OrdPosPosNomog,   //  +   +   -   -   -   3       (C, dp)
  OrdPosNomogPos,   //  +   -   -   -   +   3       (dp, C)
  OrdNegPosNomog,   //  -   +   -   -   -   3       (c, dp)
#define ORD_MAX_N_3 OrdNegPosNomog

#ifdef HAVE_LENGTH_DIFF
  OrdNomogPosZero,  //  -   -   -   +   0   4       (ls, C) (for n == 3, use NegPomogZero)
  OrdPosNomogZero,  //  +   -   -   -   0   4       (dp, c) (for n == 3, use PomogNegZero)

  OrdPosPosNomogZero,// +   +   -   -   0   4       (C, dp)
  OrdPosNomogPosZero,// +   -   -   +   0   4       (dp, C)
  OrdNegPosNomogZero,// -   +   -   -   0   4       (c, dp)
#endif

  OrdUnknown
};

typedef enum p_Proc
{
  p_Copy_Proc = 0,
  p_Delete_Proc,
  p_ShallowCopyDelete_Proc,
  p_Mult_nn_Proc,
  pp_Mult_nn_Proc,
  pp_Mult_mm_Proc,
  p_Mult_mm_Proc,
  p_Add_q_Proc,
  p_Minus_mm_Mult_qq_Proc,
  p_Neg_Proc,
  pp_Mult_Coeff_mm_DivSelect_Proc,
  p_Unknown_Proc
};

// *_2_String conversions of these enums
// they are only needed at for GENERATE_P_PROCS or RDEBUG
#if defined(GENERATE_P_PROCS) || defined(RDEBUG)
char* p_FieldEnum_2_String(p_Field field)
{
  switch(field)
  {
      case FieldGeneral: return "FieldGeneral";
      case FieldZp: return "FieldZp";          
      case FieldR: return "FieldR";
      case FieldGF: return "FieldGF";
      case FieldQ: return "FieldQ";
#if HAVE_MORE_FIELDS_IMPLEMENTED
      case FieldLong_R: return "FieldLong_R";
      case FieldLong_C: return "FieldLong_C";
      case FieldZp_a: return "FieldZp_a";
      case FieldQ_a: return "FieldQ_a";
#endif
      case FieldUnknown: return "FieldUnknown";
  }
  return "NoField_2_String";
}

char* p_LengthEnum_2_String(p_Length length)
{
  switch(length)
  {
      case LengthGeneral: return "LengthGeneral"; 
      case LengthEight: return "LengthEight";       
      case LengthSeven: return "LengthSeven";
      case LengthSix: return "LengthSix";
      case LengthFive: return "LengthFive";
      case LengthFour: return "LengthFour";
      case LengthThree: return "LengthThree";
      case LengthTwo: return "LengthTwo";
      case LengthOne: return "LengthOne";
      case LengthUnknown: return "LengthUnknown";
  }
  return "NoLength_2_String";
}

char* p_OrdEnum_2_String(p_Ord ord)
{
  switch(ord)
  {
      case OrdGeneral: return "OrdGeneral";   
      case OrdPomog: return "OrdPomog";         
      case OrdNomog: return "OrdNomog";         
      case OrdNegPomog: return "OrdNegPomog";      
      case OrdPomogNeg: return "OrdPomogNeg";      
      case OrdPosNomog: return "OrdPosNomog";      
      case OrdNomogPos: return "OrdNomogPos";      
      case OrdPosPosNomog: return "OrdPosPosNomog";   
      case OrdPosNomogPos: return "OrdPosNomogPos";   
      case OrdNegPosNomog: return "OrdNegPosNomog";   
#ifdef HAVE_LENGTH_DIFF
      case OrdNegPomogZero: return "OrdNegPomogZero";  
      case OrdPomogNegZero: return "OrdPomogNegZero";  
      case OrdPomogZero: return "OrdPomogZero";     
      case OrdNomogZero: return "OrdNomogZero";     
      case OrdNomogPosZero: return "OrdNomogPosZero";  
      case OrdPosNomogZero: return "OrdPosNomogZero";  
      case OrdPosPosNomogZero: return "OrdPosPosNomogZero";
      case OrdPosNomogPosZero: return "OrdPosNomogPosZero";
      case OrdNegPosNomogZero: return "OrdNegPosNomogZero";
#endif
      case OrdUnknown: return "OrdUnknown";
  }
  return "NoOrd_2_String";
}

char* p_ProcEnum_2_String(p_Proc proc)
{
  switch(proc)
  {
      case p_Copy_Proc: return "p_Copy_Proc";
      case p_Delete_Proc: return "p_Delete_Proc";
      case p_ShallowCopyDelete_Proc: return "p_ShallowCopyDelete_Proc";
      case p_Mult_nn_Proc: return "p_Mult_nn_Proc";
      case pp_Mult_nn_Proc: return "pp_Mult_nn_Proc";
      case pp_Mult_mm_Proc: return "pp_Mult_mm_Proc";
      case p_Mult_mm_Proc: return "p_Mult_mm_Proc";
      case p_Add_q_Proc: return "p_Add_q_Proc";
      case p_Minus_mm_Mult_qq_Proc: return "p_Minus_mm_Mult_qq_Proc";
      case p_Neg_Proc: return "p_Neg_Proc";
      case pp_Mult_Coeff_mm_DivSelect_Proc: return "pp_Mult_Coeff_mm_DivSelect_Proc";
      case p_Unknown_Proc: return "p_Unknown_Proc";
  }
  return "NoProc_2_String";
}
#endif // defined(GENERATE_P_PROCS) || defined(RDEBUG)


#ifdef GENERATE_P_PROCS
#include "dError.c"
#endif

/***************************************************************
 *  
 * 
 * Deal with OrdZero
 * 
 *******************************************************************/
#ifdef HAVE_LENGTH_DIFF
static inline int IsZeroOrd(p_Ord ord)
{
  return (ord == OrdPomogZero || ord == OrdNomogZero ||
          ord == OrdNegPomogZero || ord == OrdPosNomogZero ||
          ord == OrdPomogNegZero || ord == OrdNomogPosZero ||
          ord == OrdPosNomogPosZero || ord == OrdPosPosNomogZero ||
          ord == OrdNegPosNomogZero);
}

static inline p_Ord ZeroOrd_2_NonZeroOrd(p_Ord ord, int strict)
{
  if (IsZeroOrd(ord))
  {
    switch (ord)
    {
        case OrdPomogZero:      return OrdPomog;
        case OrdNomogZero:      return OrdNomog;
        case OrdNegPomogZero:   return OrdNegPomog;
        case OrdPosNomogZero:   return OrdPosNomog;
        case OrdPosPosNomogZero:    return OrdPosPosNomog;
        case OrdNegPosNomogZero:    return OrdNegPosNomog;
        default:    
          if (strict) return OrdGeneral;
          else if (ord == OrdPomogNegZero) return OrdPomogNeg;
          else if (ord == OrdNomogPosZero) return OrdNomogPos;
          else if (ord == OrdPosNomogPosZero) return OrdPosNomogPos;
          else return OrdGeneral;
    }
  }
  else
  {
    return ord;
  }
}
#else
#define IsZeroOrd(ord) 0
#define ZeroOrd_2_NonZeroOrd(ord) (ord)
#endif

/***************************************************************
 *  
 * Filters which are applied to field/length/ord, before a proc is 
 * choosen
 * 
 *******************************************************************/
static inline void FastP_ProcsFilter(p_Field &field, p_Length &length, p_Ord &ord)
{
  if (HAVE_FAST_P_PROCS >= 5) return;
  
  if (HAVE_FAST_P_PROCS < 3 && field == FieldQ)
    field = FieldGeneral;
  
  if ((HAVE_FAST_P_PROCS == 0) ||
      (HAVE_FAST_P_PROCS <= 4 && field != FieldZp && field != FieldQ))
  {
    field = FieldGeneral;
    length = LengthGeneral;
    ord = OrdGeneral;
    return;
  }
  if (HAVE_FAST_P_PROCS == 1 || (HAVE_FAST_P_PROCS == 4 && field != FieldZp))
    ord = OrdGeneral;
}

static inline void FastFieldFilter(p_Field &field)
{
  if (HAVE_FAST_FIELD <= 0 || 
      (HAVE_FAST_FIELD == 1 && field != FieldZp) ||
      (field != FieldZp && field != FieldQ))
    field = FieldGeneral;
}
      
static inline void FastLengthFilter(p_Length &length)
{
  if ((HAVE_FAST_LENGTH == 3 && length <= LengthFive) ||
      (HAVE_FAST_LENGTH == 2 && length <= LengthFour) ||
      (HAVE_FAST_LENGTH == 1 && length <= LengthTwo) ||
      (HAVE_FAST_LENGTH <= 0))
  {
    length = LengthGeneral;
  }
}

static inline void FastOrdFilter(p_Ord &ord)
{
  if ((HAVE_FAST_ORD == 3 && ord >= OrdNomogPosZero) ||
      (HAVE_FAST_ORD == 2 && ord >= OrdPosNomog) ||
      (HAVE_FAST_ORD == 1 && ord >= OrdPomogZero) ||
      (HAVE_FAST_ORD <= 0))
    ord = OrdGeneral;
}

static inline void FastOrdZeroFilter(p_Ord &ord)
{
  if (IsZeroOrd(ord))
  {
    if ((HAVE_FAST_ZERO_ORD == 1 && (ord != OrdPosNomogPosZero && 
                                     ord != OrdNomogPosZero && 
                                     ord != OrdPomogNegZero)) ||
        (HAVE_FAST_ZERO_ORD <= 0))
      ord = ZeroOrd_2_NonZeroOrd(ord, 1);
  }
}

static inline void NCopy__Filter(p_Field &field)
{
  if (ZP_COPY_FIELD(field)) field = FieldZp;
}

// in p_Add_q, p_MemCmp works with CompLSize,
// hence, we do not need to consider ZeroOrds
static inline void p_Add_q__Filter(p_Length &length, p_Ord &ord)
{
  if (IsZeroOrd(ord))
  {
    ord = ZeroOrd_2_NonZeroOrd(ord, 0);
    if (length > LengthGeneral)
    {
      length = (p_Length) ((int)length + 1);
    }
  }
}

static inline void FastProcFilter(p_Proc proc, p_Field &field, p_Length &length, 
                                   p_Ord &ord)
{
  switch(proc)
  {
      case p_Add_q_Proc:
        p_Add_q__Filter(length, ord);
        break;
        
      case p_Copy_Proc:
      case p_Delete_Proc:
        NCopy__Filter(field);
        break;
        
      default: break;
  }

  FastOrdFilter(ord);
  FastLengthFilter(length);
  FastFieldFilter(field);
  FastP_ProcsFilter(field, length, ord);
}

// returns 1 if combination of field/length/ord is invalid
static inline int IsValidSpec(p_Field field, p_Length length, p_Ord ord)
{
  if (field == FieldUnknown || length == LengthUnknown || ord == OrdUnknown)
    return 0;

  if (length >= LengthThree && // i.e. 1, 2, or 3
      ord > ORD_MAX_N_3)       //  i.e. OrdNomogPosZero and below
    return 0;
  
  if (length >= LengthTwo &&    // i.e. 1 or 2
      ord > ORD_MAX_N_2)        // i.e. PosNomog and below
    return 0;

  if (length == LengthOne && 
      ord > ORD_MAX_N_1)           // i.e. PosPomogZero and below
    return 0;

  // we cover everything for length <= two
  if (ord == OrdGeneral && length >= LengthTwo)
    return 0;
  return 1;
}

 
static inline int index(p_Field field, p_Length length)
{
  return field*LengthUnknown + length;
}

static inline int index(p_Field field, p_Length length, p_Ord ord)
{
  return field*LengthUnknown*OrdUnknown + length*OrdUnknown + ord;
}

static inline int index(p_Proc proc, p_Field field, p_Length length, p_Ord ord)
{
  switch(proc)
  {
      case p_Delete_Proc:
      case p_Mult_nn_Proc:
      case p_Neg_Proc:
        return field;
        
      case p_ShallowCopyDelete_Proc:
        return length;
        
      case p_Copy_Proc:
      case pp_Mult_mm_Proc:
      case p_Mult_mm_Proc:
      case pp_Mult_nn_Proc:
      case pp_Mult_Coeff_mm_DivSelect_Proc:
        return index(field, length);

      case p_Add_q_Proc:
      case p_Minus_mm_Mult_qq_Proc:
        return index(field, length, ord);
        
      default:
        assume(0);
        return -1;
  }
}

// The procedure which does the work of choosing/generating a
// set of poly procs
static void SetProcs(p_Field field, p_Length length, p_Ord ord);


#ifndef GENERATE_P_PROCS 

/***************************************************************
 * 
 * Runtime stuff
 *
 ***************************************************************/
#include "structs.h"
#include "p_polys.h"
#include "ring.h"
#include "p_Procs.h"
#include "p_Numbers.h"
#include "p_MemCmp.h"
#include "p_MemAdd.h"
#include "p_MemCopy.h"
#ifdef NDEBUG
#include "p_Procs.inc"
#else
#include "p_Procs_debug.inc"
#endif

// the rest is related to getting the procs
static inline p_Field p_FieldIs(ring r)
{
  if (rField_is_Zp(r)) return FieldZp;          
  if (rField_is_R(r)) return FieldR;
  if (rField_is_GF(r)) return FieldGF;
  if (rField_is_Q(r)) return FieldQ;
#ifdef HAVE_MORE_FIELDS_IMPLEMENTED
  if (rField_is_long_R(r)) return FieldLong_R;
  if (rField_is_long_C(r)) return FieldLong_C;
  if (rField_is_Zp_a(r)) return FieldZp_a;
  if (rField_is_Q_a(r)) return FieldQ_a;
#endif
  return FieldGeneral;
}

static inline p_Length p_LengthIs(ring r)
{
  assume(r->ExpLSize > 0);
  if (r->ExpLSize == 1) return LengthOne;
  if (r->ExpLSize == 2) return LengthTwo;
  if (r->ExpLSize == 3) return LengthThree;
  if (r->ExpLSize == 4) return LengthFour;
  if (r->ExpLSize == 5) return LengthFive;
  if (r->ExpLSize == 6) return LengthSix;
  if (r->ExpLSize == 7) return LengthSeven;
  if (r->ExpLSize == 8) return LengthEight;
  return LengthGeneral;
}

static inline int p_IsNomog(long* sgn, int l)
{
  int i;
  for (i=0;i<l;i++)
    if (sgn[i] > 0) return 0;
  
  return 1;
}

static inline int p_IsPomog(long* sgn, int l)
{
  int i;
  for (i=0;i<l;i++)
    if (sgn[i] < 0) return 0;
  return 1;
}

static inline p_Ord p_OrdIs(ring r)
{
  long* sgn = r->ordsgn;
  long l = r->ExpLSize;
  int zero = 0;
  
  if (sgn[l-1] == 0) 
  {
    l--;
    zero = 1;
  }
  
  // we always favour the pomog cases
  if (p_IsPomog(sgn,l)) return (zero ? OrdPomogZero : OrdPomog);
  if (p_IsNomog(sgn,l)) return (zero ? OrdNomogZero : OrdNomog);
  
  assume(l > 1);
  
  if (sgn[0] == -1 && p_IsPomog(&sgn[1], l-1))
    return (zero ? OrdNegPomogZero : OrdNegPomog);
  if (sgn[l-1] == -1 && p_IsPomog(sgn, l-1))
    return (zero ? OrdPomogNegZero : OrdPomogNeg);

  if (sgn[0] == 1 && p_IsNomog(&sgn[1], l-1)) 
    return (zero ? OrdPosNomogZero : OrdPosNomog);
  if (sgn[l-1] == 1 && p_IsNomog(sgn, l-1))
    return (zero ? OrdNomogPosZero : OrdNomogPos);

  assume(l > 2);
  
  if (sgn[0] == 1 && sgn[1] == 1 && p_IsNomog(&sgn[2], l-2))
    return (zero ? OrdPosPosNomogZero : OrdPosPosNomog);

  if (sgn[0] == 1 && sgn[l-1] == 1 && p_IsNomog(&sgn[1], l-2))
    return (zero ? OrdPosNomogPosZero : OrdPosNomogPos);

  if (sgn[0] == -1 && sgn[1] == 1 && p_IsNomog(&sgn[2], l-2))
    return (zero ? OrdNegPosNomogZero : OrdNegPosNomog);

  return OrdGeneral;
}
  
static p_Procs_s *_p_procs;
// Choose a set of p_Procs
void p_SetProcs(ring r, p_Procs_s* p_Procs)
{
  p_Field     field = p_FieldIs(r);
  p_Length    length = p_LengthIs(r);
  p_Ord       ord = p_OrdIs(r);
  
  assume(p_Procs != NULL);
#ifdef RDEBUG
  memset(p_Procs, 0, sizeof(p_Procs_s));
#endif
  _p_procs = p_Procs;
  assume(IsValidSpec(field, length, ord));

  SetProcs(field, length, ord);
  assume(
    (p_Procs->p_Delete != NULL) &&
    (p_Procs->p_ShallowCopyDelete != NULL) &&
    (p_Procs->p_Mult_nn != NULL) &&
    (p_Procs->pp_Mult_nn != NULL) &&
    (p_Procs->p_Copy != NULL) &&
    (p_Procs->pp_Mult_mm != NULL) &&
    (p_Procs->p_Mult_mm != NULL) &&
    (p_Procs->p_Add_q != NULL) &&
    (p_Procs->p_Neg != NULL) &&
    (p_Procs->pp_Mult_Coeff_mm_DivSelect != NULL) &&
    (p_Procs->p_Minus_mm_Mult_qq != NULL));
}

#ifdef RDEBUG 
void p_Debug_GetSpecNames(const ring r, char* &field, char* &length, char* &ord)
{
  p_Field     e_field = p_FieldIs(r);
  p_Length    e_length = p_LengthIs(r);
  p_Ord       e_ord = p_OrdIs(r);
  
  field  = p_FieldEnum_2_String(p_FieldIs(r));
  length = p_LengthEnum_2_String(p_LengthIs(r));
  ord    = p_OrdEnum_2_String(p_OrdIs(r));
}
// like SetProcs, only that names are set
static int set_names = 0;
void p_Debug_GetProcNames(const ring r, p_Procs_s* p_Procs)
{
  set_names = 1;
  p_SetProcs(r, p_Procs);
  set_names = 0;
}
#endif // RDEBUG

#define __SetProc(what, type, field, length, ord) \
  _p_procs->what = (what##_Proc_Ptr) what##_Proc_##type [index(what##_Proc, field, length, ord)]

#define ___SetProc(what, field, length, ord) __SetProc(what, funcs, field, length, ord)

#ifdef RDEBUG
#define _SetProc(what, field, length, ord)      \
do                                              \
{                                               \
  if (set_names)                                \
    __SetProc(what, names, field, length, ord); \
  else                                          \
    ___SetProc(what, field, length, ord);       \
}                                               \
while(0)
#else
#define _SetProc ___SetProc
#endif

#else /* GENERATE_P_PROCS */
/***************************************************************
 * 
 * generate time stuff
 *
 ***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*** generated_p_procs;

inline int AlreadyHaveProc(p_Proc proc, p_Field field, p_Length length, p_Ord ord)
{
  return (generated_p_procs[proc])[index(proc, field, length, ord)] != 0;
}

const char* macros_field[] = {"n_Copy","n_Delete", "n_Mult", "n_Add", "n_Sub", "n_IsZero", "n_Equal" , "n_Neg", NULL};

const char* macros_length[] =
{"p_MemCopy", "p_MemAdd", "p_MemSum", NULL};

const char* macros_length_ord[] = {"p_MemCmp", NULL};
int DummyProcs = 0;

void AddProc(const char* s_what, p_Proc proc, p_Field field, p_Length length, p_Ord ord)
{
  int i;
  const char* s_length = p_LengthEnum_2_String(length);
  const char* s_ord = p_OrdEnum_2_String(ord);
  const char* s_field = p_FieldEnum_2_String(field);
  char* s_full_proc_name = (char*) malloc(200);
  
  sprintf(s_full_proc_name, "%s__%s_%s_%s", s_what, s_field, s_length, s_ord);
             
  (generated_p_procs[proc])[index(proc, field, length, ord)] = s_full_proc_name;
  
  // define all macros 
  printf("\n// definition of %s\n", s_full_proc_name);
  i = 0;
  while (macros_field[i] != NULL)
  {
    printf("#undef %s\n#define %s\t%s_%s\n", 
           macros_field[i], macros_field[i],  macros_field[i], s_field);
    i++;
  }
  i = 0;
  while (macros_length[i] != NULL)
  {
    printf("#undef %s\n#define %s\t%s_%s\n", 
           macros_length[i], macros_length[i], macros_length[i], s_length);
    i++;
  }
  i = 0;
  while (macros_length_ord[i] != NULL)
  {
    printf("#undef %s\n#define %s\t%s_%s_%s\n", 
           macros_length_ord[i], macros_length_ord[i], macros_length_ord[i], s_length, s_ord);
    i++;
  }

  // define DECLARE_LENGTH
  printf("#undef DECLARE_LENGTH\n");
  if (length != LengthGeneral)
    printf("#define DECLARE_LENGTH(what) ((void)0)\n");
  else
    printf("#define DECLARE_LENGTH(what) what\n");
  
  // define DECLARE_ORDSGN
  printf("#undef DECLARE_ORDSGN\n");
  if (ord != OrdGeneral)
    printf("#define DECLARE_ORDSGN(what) ((void)0)\n");
  else
    printf("#define DECLARE_ORDSGN(what) what\n");

  // define pp_Mult_mm, for p_Minus_mm_Mult_qq
  if (strcmp(s_what, "p_Minus_mm_Mult_qq") == 0)
  {
    printf("#undef pp_Mult_mm\n");
    printf("#define pp_Mult_mm pp_Mult_mm__%s_%s_OrdGeneral\n", s_field, s_length);
  }
  printf("#undef %s\n#define %s %s\n", s_what, s_what, s_full_proc_name);
  printf("#include \"%s__Template.cc\"\n", s_what);
  printf("#undef %s\n#undef pp_Mult_mm\n", s_what);
}

void GenerateProc(const char* s_what, p_Proc proc, p_Field field, p_Length length, p_Ord ord)
{
  if (! AlreadyHaveProc(proc, field, length, ord))
    AddProc(s_what, proc, field, length, ord);
}

int main()
{
  int field;
  int length; 
  int ord;
  int i, j;
  int NumberOfHaveProcs = 0;
  
  
  printf("/* -*-c++-*- */\n");
  printf("/***************************************************************\n");
  printf(" This file was generated automatically by p_Procs.cc: DO NOT EDIT\n\n");
  printf(" This file provides the needed implementation of p_Procs\n");
  printf(" See the end for a summary. \n");
  printf("*******************************************************************/\n");

  generated_p_procs = (char***) malloc(p_Unknown_Proc*sizeof(char**));
  for (i=0; i<p_Unknown_Proc; i++)
  {
    generated_p_procs[i] = 
      (char**) calloc(index((p_Proc)i, FieldUnknown, LengthUnknown, OrdUnknown), sizeof(char*));
  }
  
  // set default procs
  for (field = 0; field < (int) FieldUnknown; field++)
  {
    for (length=0; length < (int) LengthUnknown; length++)
    {
      for (ord=0; ord < (int)OrdUnknown; ord++)
      {
        if (IsValidSpec((p_Field) field, (p_Length) length, (p_Ord) ord))
            SetProcs((p_Field) field, (p_Length) length, (p_Ord) ord);
      }
    }
  }

  printf("
/***************************************************************
  Names of procs for RDEBUG */
#ifdef RDEBUG\n");

  for (i=0; i<p_Unknown_Proc; i++)
  {
    printf("static const char* %s_names[] = {", p_ProcEnum_2_String((p_Proc)i));
    for (j=0;j<index((p_Proc)i, FieldUnknown, LengthUnknown, OrdUnknown); j++)
    {
      char* s = (generated_p_procs[i])[j];
      if (s != 0)
      {
        printf("\n\"%s\",", s);
      }
      else
        printf("0,");
          
    }
    printf("\n};\n");
  }
  printf("
#endif // RDEBUG


/***************************************************************/
/* Tables for lookup of procedures: */\n");

  for (i=0; i<p_Unknown_Proc; i++)
  {
    printf("static const %s_Ptr %s_funcs[] = {", p_ProcEnum_2_String((p_Proc)i), p_ProcEnum_2_String((p_Proc)i));
    for (j=0;j<index((p_Proc)i, FieldUnknown, LengthUnknown, OrdUnknown); j++)
    {
      char* s = (generated_p_procs[i])[j];
      if (s != 0)
      {
        NumberOfHaveProcs++;
        printf("\n%s,", s);
      }
      else
        printf("0,");
          
    }
    printf("\n};\n");
  }
  printf("
/***************************************************************
 * Summary: 
 *   HAVE_FAST_P_PROCS  = %d, 
 *   HAVE_FAST_FIELD    = %d, 
 *   HAVE_FAST_LENGTH   = %d, 
 *   HAVE_FAST_ORD      = %d, 
 *   HAVE_FAST_ZERO_ORD = %d
 *   
 *   Generated PolyProcs= %d
 *
 *******************************************************************/\n",
         HAVE_FAST_P_PROCS, HAVE_FAST_FIELD, HAVE_FAST_LENGTH, HAVE_FAST_ORD, HAVE_FAST_ZERO_ORD,
         NumberOfHaveProcs);
}

#define _SetProc(what, field, length, ord) \
      GenerateProc(#what, what##_Proc, field, length, ord)
#endif // GENERATE_P_PROCS

/***************************************************************
 * 
 * Setting the procedures
 *
 ***************************************************************/

#define SetProc(what, field, length, ord)                   \
do                                                          \
{                                                           \
  p_Field t_field = field;                                  \
  p_Ord t_ord = ord;                                        \
  p_Length t_length = length;                               \
  FastProcFilter(what##_Proc, t_field, t_length, t_ord);    \
  _SetProc(what, t_field, t_length, t_ord);                 \
}                                                           \
while (0)
  
static void SetProcs(p_Field field, p_Length length, p_Ord ord)
{
  SetProc(p_Delete, field, LengthGeneral, OrdGeneral);
  SetProc(p_Mult_nn, field, LengthGeneral, OrdGeneral);
  SetProc(pp_Mult_nn, field, length, OrdGeneral);
  SetProc(p_ShallowCopyDelete, FieldGeneral, length, OrdGeneral);
  SetProc(p_Copy, field, length, OrdGeneral);
  SetProc(pp_Mult_mm, field, length, OrdGeneral);
  SetProc(p_Mult_mm, field, length, OrdGeneral);
  SetProc(p_Add_q, field, length, ord);
  SetProc(p_Minus_mm_Mult_qq, field, length, ord);
  SetProc(p_Neg, field, LengthGeneral, OrdGeneral);
  SetProc(pp_Mult_Coeff_mm_DivSelect, field, length, OrdGeneral);
}



