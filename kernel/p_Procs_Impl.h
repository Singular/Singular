/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Impl.h
 *  Purpose: implementation of primitive procs for polys
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *******************************************************************/
#ifndef P_PROCS_IMPL_H
#define P_PROCS_IMPL_H

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

 At run-time, a fast proc is set/chosen if found/generated, else
 a general proc is set/chosen.
 *******************************************************************/

// Define HAVE_FAST_P_PROCS to:
//   0 -- only FieldGeneral_LengthGeneral_OrdGeneral
//   1 -- plus FieldZp_Length*_OrdGeneral procs
//   2 -- plus FieldZp_Length*_Ord* procs
//   3 -- plus FieldQ_Length*_Ord*
//   4 -- plus FieldGeneral_Length*_OrdGeneral procs
//   5 -- all Field*_Length*_Ord* procs
#ifndef HAVE_FAST_P_PROCS
#define HAVE_FAST_P_PROCS 0
#endif

// Define HAVE_FAST_FIELD to:
//   0 -- only FieldGeneral
//   1 -- special cases for FieldZp
//   2 -- plus special cases for FieldQ
//   nothing else is implemented, yet
#ifndef HAVE_FAST_FIELD
#define HAVE_FAST_FIELD 0
#endif

// Define HAVE_FAST_LENGTH to:
//   0 -- only LengthGeneral
//   1 -- special cases for length <= 1
//   2 -- special cases for length <= 2
//   3 -- special cases for length <= 4
//   4 -- special cases for length <= 8
#ifndef HAVE_FAST_LENGTH
#define HAVE_FAST_LENGTH 0
#endif

// Define HAVE_FAST_ORD to:
//  0  -- only OrdGeneral
//  1  -- special for ords with n_min <= 1
//  2  -- special for ords with n_min <= 2
//  3  -- special ords for with n_min <= 3
//  4  -- special for all ords
#ifndef HAVE_FAST_ORD
#define HAVE_FAST_ORD 0
#endif

// Define HAVE_FAST_ZERO_ORD to:
//  0 -- no zero ords are considered
//  1 -- only ZeroOrds for OrdPosNomogPosZero, OrdNomogPosZero, OrdPomogNegZero
//  2 -- ZeroOrds for all
#ifndef HAVE_FAST_ZERO_ORD
#define HAVE_FAST_ZERO_ORD 0
#endif

// undefine this, if ExpL_Size always equals CompLSize
#define HAVE_LENGTH_DIFF


// Predicate which returns true if alloc/copy/free of numbers is
// like that of Zp (rings should use GMP in future)
#ifdef HAVE_RINGS
#define ZP_COPY_FIELD(field) \
  (field == FieldZp || field == FieldGF || field == FieldR)
#else
#define ZP_COPY_FIELD(field) \
  (field == FieldZp || field == FieldGF || field == FieldR)
#endif

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
  FieldQ,
  FieldR,
  FieldGF,
#if HAVE_MORE_FIELDS_IMPLEMENTED
  FieldLong_R,
  FieldLong_C,
  FieldZp_a,
  FieldQ_a,
#endif
#ifdef HAVE_RINGS
  RingGeneral,
#endif
  FieldUnknown
} p_Field;
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
} p_Length;
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
} p_Ord;

typedef enum p_Proc
{
  p_Copy_Proc = 0,
  p_Delete_Proc,
  p_ShallowCopyDelete_Proc,
  p_Mult_nn_Proc,
  pp_Mult_nn_Proc,
  pp_Mult_mm_Proc,
  pp_Mult_mm_Noether_Proc,
  p_Mult_mm_Proc,
  p_Add_q_Proc,
  p_Minus_mm_Mult_qq_Proc,
  p_Neg_Proc,
  pp_Mult_Coeff_mm_DivSelect_Proc,
  pp_Mult_Coeff_mm_DivSelectMult_Proc,
  p_Merge_q_Proc,
  p_kBucketSetLm_Proc,
  p_Unknown_Proc
} p_Proc;

static inline const char* p_FieldEnum_2_String(p_Field field)
{
  switch(field)
  {
      case FieldGeneral: return "FieldGeneral";
      case FieldZp: return "FieldZp";
      case FieldQ: return "FieldQ";
      case FieldR: return "FieldR";
      case FieldGF: return "FieldGF";
#if HAVE_MORE_FIELDS_IMPLEMENTED
      case FieldLong_R: return "FieldLong_R";
      case FieldLong_C: return "FieldLong_C";
      case FieldZp_a: return "FieldZp_a";
      case FieldQ_a: return "FieldQ_a";
#endif
#ifdef HAVE_RINGS
      case RingGeneral: return "RingGeneral";
#endif
      case FieldUnknown: return "FieldUnknown";
  }
  return "NoField_2_String";
}

static inline const char* p_LengthEnum_2_String(p_Length length)
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

static inline const char* p_OrdEnum_2_String(p_Ord ord)
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

static inline const char* p_ProcEnum_2_String(p_Proc proc)
{
  switch(proc)
  {
      case p_Copy_Proc: return "p_Copy_Proc";
      case p_Delete_Proc: return "p_Delete_Proc";
      case p_ShallowCopyDelete_Proc: return "p_ShallowCopyDelete_Proc";
      case p_Mult_nn_Proc: return "p_Mult_nn_Proc";
      case pp_Mult_nn_Proc: return "pp_Mult_nn_Proc";
      case pp_Mult_mm_Proc: return "pp_Mult_mm_Proc";
      case pp_Mult_mm_Noether_Proc: return "pp_Mult_mm_Noether_Proc";
      case p_Mult_mm_Proc: return "p_Mult_mm_Proc";
      case p_Add_q_Proc: return "p_Add_q_Proc";
      case p_Minus_mm_Mult_qq_Proc: return "p_Minus_mm_Mult_qq_Proc";
      case p_Neg_Proc: return "p_Neg_Proc";
      case pp_Mult_Coeff_mm_DivSelect_Proc: return "pp_Mult_Coeff_mm_DivSelect_Proc";
      case pp_Mult_Coeff_mm_DivSelectMult_Proc: return "pp_Mult_Coeff_mm_DivSelectMult_Proc";
      case p_Merge_q_Proc: return "p_Merge_q_Proc";
      case p_kBucketSetLm_Proc: return "p_kBucketSetLm_Proc";
      case p_Unknown_Proc: return "p_Unknown_Proc";
  }
  return "NoProc_2_String";
}

static inline int p_ProcDependsOn_Field(p_Proc proc)
{
  if (proc == p_ShallowCopyDelete_Proc ||
      proc == p_Merge_q_Proc)
    return 0;
  return 1;
}

static inline int p_ProcDependsOn_Ord(p_Proc proc)
{
  switch(proc)
  {
      case p_Add_q_Proc:
      case p_Minus_mm_Mult_qq_Proc:
      case pp_Mult_mm_Noether_Proc:
      case p_kBucketSetLm_Proc:
      case p_Merge_q_Proc:
        return 1;

      default:
        return 0;
  }
}

static inline int p_ProcDependsOn_Length(p_Proc proc)
{
  switch(proc)
  {
      case p_Delete_Proc:
      case p_Mult_nn_Proc:
      case p_Neg_Proc:
        return 0;

      default:
        return 1;
  }
}

// returns string specifying the module into which the p_Proc
// should go
static inline const char* p_ProcField_2_Module(p_Proc proc,  p_Field field)
{
  if (! p_ProcDependsOn_Field(proc))
    return "FieldIndep";
  else
  {
    if (field > FieldQ) field = FieldGeneral;
    return p_FieldEnum_2_String(field);
  }
}

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
 * chosen
 *
 *******************************************************************/
#ifdef p_Procs_Static
static inline void StaticKernelFilter(p_Field &field, p_Length &length,
                                      p_Ord &ord, const p_Proc proc)
{
  // simply exclude some things
  if ((proc == pp_Mult_mm_Noether_Proc || proc == p_kBucketSetLm_Proc) &&
#ifdef HAVE_RINGS
      (field != RingGeneral) &&
#endif
      (field != FieldZp))
  {
    field = FieldGeneral;
    length = LengthGeneral;
    ord = OrdGeneral;
  }
}
#endif

static inline void FastP_ProcsFilter(p_Field &field, p_Length &length, p_Ord &ord, const p_Proc proc)
{
  if (HAVE_FAST_P_PROCS >= 5) return;

  if (HAVE_FAST_P_PROCS < 3 && field == FieldQ)
    field = FieldGeneral;

  if ((HAVE_FAST_P_PROCS == 0) ||
      (HAVE_FAST_P_PROCS <= 4 && field != FieldZp && field != FieldQ &&
       proc != p_Merge_q_Proc))
  {
#ifdef HAVE_RINGS
    if (field != RingGeneral)
#endif
    field = FieldGeneral;
    length = LengthGeneral;
    ord = OrdGeneral;
    return;
  }
  if (HAVE_FAST_P_PROCS == 1 ||
      (HAVE_FAST_P_PROCS == 4 && field != FieldZp && proc != p_Merge_q_Proc))
    ord = OrdGeneral;
}

static inline void FastFieldFilter(p_Field &field)
{
  if (HAVE_FAST_FIELD <= 0 ||
      (HAVE_FAST_FIELD == 1 && field != FieldZp) ||
      (field != FieldZp && field != FieldQ))
#ifdef HAVE_RINGS
    if (field != RingGeneral)
#endif
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

static inline void pp_Mult_mm_Noether_Filter(p_Field &field,
                                             p_Length &length, p_Ord &ord)
{
  if (ord == OrdPomog
      || ord == OrdPomogZero
      || (ord == OrdPomogNeg && length > LengthTwo)
#ifdef HAVE_LENGTH_DIFF
      || (ord == OrdPomogZero)
      || (ord == OrdPomogNegZero && length > LengthThree)
#endif
      )
  {
    // all the other orderings might occur (remember Mixed Orderings!)
#ifdef HAVE_RINGS
    if (field != RingGeneral)
#endif
    field = FieldGeneral;
    ord = OrdGeneral;
    length = LengthGeneral;
  }
}

static inline void FastProcFilter(p_Proc proc, p_Field &field,
                                  p_Length &length, p_Ord &ord)
{
  switch(proc)
  {
      case p_Add_q_Proc:
      case p_Merge_q_Proc:
        p_Add_q__Filter(length, ord);
        break;

      case p_Copy_Proc:
      case p_Delete_Proc:
        NCopy__Filter(field);
        break;

      case pp_Mult_mm_Noether_Proc:
        pp_Mult_mm_Noether_Filter(field, length, ord);
        break;

        case pp_Mult_Coeff_mm_DivSelectMult_Proc:
          if (length == LengthOne || length == LengthTwo)
          {
#ifdef HAVE_RINGS
            if (field != RingGeneral)
#endif
            field = FieldGeneral;
            length = LengthGeneral;
            ord = OrdGeneral;
            return;
          }
          break;

      default: break;
  }

  FastOrdFilter(ord);
  FastOrdZeroFilter(ord);
  FastLengthFilter(length);
  FastFieldFilter(field);
  FastP_ProcsFilter(field, length, ord, proc);
#ifdef p_Procs_Static
  StaticKernelFilter(field, length, ord, proc);
#endif
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


static inline int index(p_Length length, p_Ord ord)
{
  return length*OrdUnknown + ord;
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
      case pp_Mult_Coeff_mm_DivSelectMult_Proc:
        return index(field, length);

      case p_Add_q_Proc:
      case p_Minus_mm_Mult_qq_Proc:
      case pp_Mult_mm_Noether_Proc:
      case p_kBucketSetLm_Proc:
        return index(field, length, ord);

      case p_Merge_q_Proc:
        return index(length, ord);

      default:
        assume(0);
        return -1;
  }
}



/***************************************************************
 *
 * Macros for setting procs -- these are used for
 * generation and setting
 *
 ***************************************************************/

#define SetProc(what, field, length, ord)                   \
do                                                          \
{                                                           \
  p_Field t_field = field;                                  \
  p_Ord t_ord = ord;                                        \
  p_Length t_length = length;                               \
  FastProcFilter(what##_Proc, t_field, t_length, t_ord);    \
  DoSetProc(what, t_field, t_length, t_ord);                \
}                                                           \
while (0)                                                   \

#define SetProcs(field, length, ord)                                    \
do                                                                      \
{                                                                       \
  SetProc(p_Copy, field, length, OrdGeneral);                           \
  SetProc(p_Delete, field, LengthGeneral, OrdGeneral);                  \
  SetProc(p_ShallowCopyDelete, FieldGeneral, length, OrdGeneral);       \
  SetProc(p_Mult_nn, field, LengthGeneral, OrdGeneral);                 \
  SetProc(pp_Mult_nn, field, length, OrdGeneral);                       \
  SetProc(pp_Mult_mm, field, length, OrdGeneral);                       \
  SetProc(p_Mult_mm, field, length, OrdGeneral);                        \
  SetProc(p_Minus_mm_Mult_qq, field, length, ord);                      \
  SetProc(pp_Mult_mm_Noether, field, length, ord);                      \
  SetProc(p_Add_q, field, length, ord);                                 \
  SetProc(p_Neg, field, LengthGeneral, OrdGeneral);                     \
  SetProc(pp_Mult_Coeff_mm_DivSelect, field, length, OrdGeneral);       \
  SetProc(pp_Mult_Coeff_mm_DivSelectMult, field, length, OrdGeneral);   \
  SetProc(p_Merge_q, FieldGeneral, length, ord);                        \
  SetProc(p_kBucketSetLm, field, length, ord);                          \
}                                                                       \
while (0)

#ifdef NV_OPS
#define SetProcs_nv(field, length, ord)                                 \
do                                                                      \
{                                                                       \
  SetProc(p_Delete, field, LengthGeneral, OrdGeneral);                  \
  SetProc(p_ShallowCopyDelete, FieldGeneral, length, OrdGeneral);       \
  SetProc(p_Copy, field, length, OrdGeneral);                           \
  SetProc(p_Add_q, field, length, ord);                                 \
  SetProc(p_kBucketSetLm, field, length, ord);                          \
  SetProc(p_Neg, field, LengthGeneral, OrdGeneral);                     \
  SetProc(p_Merge_q, FieldGeneral, length, ord);                        \
}                                                                       \
while (0)
#endif

#endif // P_PROCS_IMPL_H

