/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id$ */

/***************************************************************
 *
 * File:       mpsr_PutPoly.cc
 * Purpose:    rotines which put polys and polynomials (i.e. ring) annotations
 * Author:     Olaf Bachmann (10/95)
 *
 * Change History (most recent first):
 *  o 1/97 obachman
 *    Updated routines to MP and MPP v1.1
 *
 ***************************************************************/
#include <Singular/mod2.h>

#ifdef HAVE_MPSR

#include <Singular/mpsr_Put.h>
#include <Singular/mpsr_Tok.h>
#include "longalg.h"
#include <omalloc.h>
#include <kernel/ring.h>
#include <kernel/polys.h>
//#include "ipid.h"

#ifdef PARI_BIGINT_TEST
#include <Singular/MP_PariBigInt.h>
MP_Status_t MP_MyPutApIntPacket(MP_Link_pt link, MP_ApInt_t mp_apint,
                                MP_NumAnnot_t num_annots)
{
  GEN pnum = _gmp_to_pari((mpz_ptr) mp_apint);
  return MP_PutApIntPacket(link, (MP_ApInt_t) pnum, num_annots);
}

#else

#define MP_MyPutApIntPacket MP_PutApIntPacket

#endif

/***************************************************************
 *
 * global variable definitions
 *
 ***************************************************************/

static mpsr_Status_t (*PutCoeff)(MP_Link_pt link, number x);
static mpsr_Status_t (*PutAlgAlgNumber)(MP_Link_pt link, number x);
static MP_Uint32_t gNalgvars = 0;
static MP_Sint32_t gNvars = 0;
static ring        CurrPutRing = NULL;

MP_Sint32_t *gTa = NULL;
MP_Sint32_t gTa_Length = 0;

/***************************************************************
 *
 * prototype declarations
 *
 ***************************************************************/
static void        SetPutFuncs(ring r);
static mpsr_Status_t PutModuloNumber(MP_Link_pt link, number a);
static mpsr_Status_t PutFloatNumber(MP_Link_pt link, number a);
static mpsr_Status_t PutRationalNumber(MP_Link_pt link, number a);
static mpsr_Status_t PutAlgPoly(MP_Link_pt link, napoly a, ring ar);
static mpsr_Status_t PutAlgNumber(MP_Link_pt link, number a);

static mpsr_Status_t PutVarNamesAnnot(MP_Link_pt link, ring r);
static mpsr_Status_t PutVarNumberAnnot(MP_Link_pt link, ring r, BOOLEAN mv);
static mpsr_Status_t PutOrderingAnnot(MP_Link_pt link, ring r, BOOLEAN mv);
static mpsr_Status_t PutSimpleOrdering(MP_Link_pt link, ring r, short index);
static mpsr_Status_t PutProtoTypeAnnot(MP_Link_pt link, ring r, BOOLEAN mv);
static mpsr_Status_t PutMinPolyAnnot(MP_Link_pt link, ring r);
static mpsr_Status_t PutDefRelsAnnot(MP_Link_pt link, ring r);


/***************************************************************
 *
 * Setting the global Put Functions
 *
 ***************************************************************/
static void SetPutFuncs(ring r)
{
  CurrPutRing = r;
  // first, we set the PutNumber function
  gNvars = r->N;
  mpsr_InitTempArray(gNvars+1);

  if (rField_is_Q(r))
    // rational numbers
    PutCoeff= PutRationalNumber;
  else if (rField_is_Zp(r) || rField_is_GF(r))
    // Form our point of view, ModuloP numbers and numbers from
    // GF(p,n) are the same, here. They only differ in the annots
    PutCoeff = PutModuloNumber;
  else if (rField_is_R(r))
    PutCoeff = PutFloatNumber;
  else
  {
    // now we come to algebraic numbers
    gNalgvars = rPar(r);
    mpsr_InitTempArray(gNalgvars);
    PutCoeff = PutAlgNumber;
    if (rField_is_Zp_a(r))
      // first, Z/p(a)
      PutAlgAlgNumber = PutModuloNumber;
    else
      PutAlgAlgNumber = PutRationalNumber;
  }
}



/***************************************************************
 *
 * modulo numbers (i.e. char p, or p^n)
 *
 ***************************************************************/
// we always put modulo numbers without a header, since
// we have type-spec this before
static mpsr_Status_t PutModuloNumber(MP_Link_pt link, number a)
{
  mp_return(IMP_PutUint32(link, (MP_Uint32_t) a));
}

static mpsr_Status_t PutFloatNumber(MP_Link_pt link, number a)
{
  mp_return(IMP_PutReal32(link , Number_2_Real32(a)));
}

/***************************************************************
 *
 * rational (i.e. char 0) numbers
 *
 ***************************************************************/
// This supposes that number is of char 0, i.e. a rational number
static mpsr_Status_t PutRationalNumber(MP_Link_pt link, number a)
{
  // Check for case number == NULL == 0
  if (a == NULL)
    mp_return( MP_PutSint32Packet(link, 0, 0));

  // check for SR_INT type
  if (SR_HDL(a) & SR_INT)
    mp_return( MP_PutSint32Packet(link, SR_TO_INT(a), 0));

  // check for single GMP integer
  if ((a->s) == 3)
    mp_return(MP_MyPutApIntPacket(link, (MP_ApInt_t) &(a->z), 0));

  // Now we have a fraction
  // normalize, if necessary
//   if ((a->s) == 0)
//   {
// //    nlNormalize(a);
//     return PutRationalNumber(link, a);
//   }

  // send number itself
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicDiv,
                                      0,
                                      2));
  // and now sent nominator and denominator
  mp_failr(MP_MyPutApIntPacket(link, (MP_ApInt_t) &(a->z), 0));
  mp_return(MP_MyPutApIntPacket(link, (MP_ApInt_t) &(a->n), 0));
}

/***************************************************************
 *
 * Algebraic Numbers (a la Singular)
 *
 ***************************************************************/
static inline MP_Uint32_t GetPlength(napoly a)
{
  MP_Uint32_t i = napLength(a);
  return i;
}

static mpsr_Status_t PutAlgNumber(MP_Link_pt link, number a)
{
  lnumber b;

  if (a == NULL)
    mp_return(IMP_PutUint32(link, 0));

  b = (lnumber) a;
  if ((b->n) == NULL) // NULL means there is only one number
  {
    // hence, we use the first case of the union
    mp_failr(IMP_PutUint32(link, 1));
    mp_failr(IMP_PutUint32(link, GetPlength(b->z)));
    return PutAlgPoly(link, b->z, CurrPutRing->algring);
  }
  else
  {
    // we use the 2nd case of the union
    mp_failr(IMP_PutUint32(link, 2));
    mp_failr(IMP_PutUint32(link, GetPlength(b->z)));
    failr(PutAlgPoly(link, b->z, CurrPutRing->algring));
    mp_failr(IMP_PutUint32(link, GetPlength(b->n)));
    return PutAlgPoly(link, b->n, CurrPutRing->algring);
  }
}

// this is very similar to putting a Poly
static mpsr_Status_t PutAlgPoly(MP_Link_pt link, napoly a, ring ar)
{
  unsigned int i;
  int *exp;

  if (gNalgvars > 1)
    while (a != NULL)
    {
      failr(PutAlgAlgNumber(link, napGetCoeff(a)));
      for (i=0; i<gNalgvars; i++)
        gTa[i] = p_GetExp((poly)a,i+1,ar);
      mp_failr(IMP_PutSint32Vector(link, gTa, gNalgvars));
      pIter(a);
    }
  else
  {
    while (a != NULL)
    {
      failr(PutAlgAlgNumber(link, napGetCoeff(a)));
      IMP_PutSint32(link, (MP_Sint32_t) p_GetExp((poly)a,1,ar));
      pIter(a);
    }
  }
  return mpsr_Success;
}

/***************************************************************
 *
 *  Putting polys
 *
 ***************************************************************/
mpsr_Status_t mpsr_PutPolyData(MP_Link_pt link, poly p, ring cring)
{
  //if (cring != CurrPutRing)
    SetPutFuncs(cring);

#ifdef MPSR_DEBUG
  if (currRing == cring)
    pTest(p);
#endif

  if (gNvars > 1)
  {
    short i;
    MP_Sint32_t *ta1 = &(gTa[1]);

    while (p != NULL)
    {
      failr(PutCoeff(link, pGetCoeff(p)));
      for (i=1; i<=gNvars; i++)
        gTa[i] = (MP_Sint32_t) pGetExp(p, i);
      mp_failr(IMP_PutSint32Vector(link, ta1, gNvars));
      pIter(p);
    }
  }
  else
    while (p != NULL)
    {
      failr(PutCoeff(link, pGetCoeff(p)));
      IMP_PutSint32(link, (MP_Sint32_t) pGetExp(p,1));
      pIter(p);
    }
  return mpsr_Success;
}

mpsr_Status_t mpsr_PutPolyVectorData(MP_Link_pt link, poly p, ring cring)
{
  short i, n1;

  if (cring != CurrPutRing)
    SetPutFuncs(cring);

  if (gNvars > 1)
  {
    short i, n1= gNvars + 1;
    while (p != NULL)
    {
      failr(PutCoeff(link, pGetCoeff(p)));
      gTa[0] = pGetComp(p);
      for (i=1; i< n1; i++)
        gTa[i] = (MP_Sint32_t) pGetExp(p,i);
      mp_failr(IMP_PutSint32Vector(link, gTa, n1));
      pIter(p);
    }
  }
  else
    while (p != NULL)
    {
      failr(PutCoeff(link, pGetCoeff(p)));
      IMP_PutSint32(link, (MP_Sint32_t) pGetComp(p));
      IMP_PutSint32(link, (MP_Sint32_t) pGetExp(p,1));
      pIter(p);
    }
  return mpsr_Success;
}

/***************************************************************
 *
 *  The putting annotation buisness
 *
 ***************************************************************/
int mpsr_GetNumOfRingAnnots(ring r, BOOLEAN mv)
{
  short annots = 4;

  if (mv) annots++;
  if (r->qideal != NULL) annots++;
  // we sent the minpoly with the coefficient ring
  if ((r->minpoly != NULL) && (r->parameter == NULL)) annots++;

  return annots;
}

mpsr_Status_t mpsr_PutRingAnnots(MP_Link_pt link, ring r, BOOLEAN mv)
{
  failr(PutProtoTypeAnnot(link,r, mv));
  failr(PutVarNumberAnnot(link, r, mv));
  failr(PutVarNamesAnnot(link,r));
  failr(PutOrderingAnnot(link,r, mv));

  if (mv)
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_PolyDict,
                                    MP_AnnotPolyModuleVector,
                                    MP_AnnotRequired));
  // Hmm .. this is not according to a "proper" Singular ring,
  // but to be used in a recursive call of mpsr_PutRingAnnots
  if (r->minpoly != NULL && r->parameter == NULL && r->ch > 0)
    failr(PutMinPolyAnnot(link,r));

  if (r->qideal != NULL)
    return PutDefRelsAnnot(link, r);

  return mpsr_Success;
}

static mpsr_Status_t PutProtoTypeAnnot(MP_Link_pt link, ring r,
                                       BOOLEAN mv)
{
  // each element of the poly is a
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  // Monom represented as a struct of 2 elements
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_ProtoDict,
                                      MP_CopProtoStruct,
                                      0,
                                      2));

  // First element is the coefficient
  if (rField_is_Q(r))
  {
    // rational numbers
    mp_failr(MP_PutCommonMetaTypePacket(link,
                                        MP_NumberDict,
                                        MP_CmtNumberRational,
                                        1));
    // are always normalized (and made that way, if necessary)
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_NumberDict,
                                    MP_AnnotNumberIsNormalized,
                                    0));
  }
  else if (rField_is_Zp(r) || rField_is_GF(r))
  {
    // modulo p numbers
    // are communicated as IMP_Uint32's
    mp_failr(MP_PutCommonMetaTypePacket(link,
                                    MP_ProtoDict,
                                    MP_CmtProtoIMP_Uint32,
                                    1));
    // but are taken as modulo numbers
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_NumberDict,
                                    MP_AnnotNumberModulos,
                                    MP_AnnotValuated));
    // with Modulo
    mp_failr(MP_PutUint32Packet(link, rInternalChar(r), 1));
    if (r->parameter == NULL)
    {
      // which is (in our case) always a prime number
      mp_failr(MP_PutAnnotationPacket(link,
                                      MP_NumberDict,
                                      MP_AnnotNumberIsPrime,
                                      0));
    }
    else
    {
      mp_failr(MP_PutAnnotationPacket(link,
                                      MP_SingularDict,
                                      MP_AnnotSingularGalois,
                                      MP_AnnotValuated));
      mp_failr(MP_PutStringPacket(link, r->parameter[0], 0));
    }
  }
  else if (rField_is_R(r))
  {
    // floats
    mp_failr(MP_PutCommonMetaTypePacket(link,
                                    MP_ProtoDict,
                                    MP_CmtProtoIMP_Real32,
                                    0));
  }
  else
  {
    // alg numbers
    // create temporary ring for describing the coeeficient domain
    ring alg_r = r->algring;
    alg_r->minpoly = r->minpoly;

    // Algebraic numbers are
    // a fraction of two Dense Dist Polys
    mp_failr(MP_PutCommonMetaOperatorPacket(link,
                                            MP_BasicDict,
                                            MP_CopBasicDiv,
                                            1,
                                            0));
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_ProtoDict,
                                    MP_AnnotProtoPrototype,
                                    MP_AnnotReqValNode));
    mp_failr(MP_PutCommonMetaOperatorPacket(link,
                                            MP_PolyDict,
                                            MP_CopPolyDenseDistPoly,
                                            mpsr_GetNumOfRingAnnots(alg_r, 0),
                                            0));
    failr(mpsr_PutRingAnnots(link, alg_r, 0));

    // destroy temporary ring
    alg_r->minpoly = NULL;
  }

  // second element is the exponent vector
  mp_failr(MP_PutCommonMetaOperatorPacket(link,
                                          MP_ProtoDict,
                                          MP_CopProtoArray,
                                          1,
                                          (mv ? r->N + 1 : r->N)));
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  mp_return(MP_PutCommonMetaTypePacket(link,
                                   MP_ProtoDict,
                                   MP_CmtProtoIMP_Sint32,
                                   0));
}


static mpsr_Status_t PutVarNamesAnnot(MP_Link_pt link, ring r)
{
  // first, we put the annot packet, with flags (1, 0, 1, 0)
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyVarNames,
                                  MP_AnnotValuated));
  // now, the varnames follow
  if ((r->N) == 1)
  {
    // only 1 varname
    mp_return(MP_PutIdentifierPacket(link, MP_SingularDict, *(r->names), 0));
  }
  else
  {
    // more than one varname
    char **names = r->names;
    int i, n = r->N;

    mp_failr(MP_PutCommonOperatorPacket(link,
                                        MP_ProtoDict,
                                        MP_CopProtoArray,
                                        1,
                                        r->N));
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_ProtoDict,
                                    MP_AnnotProtoPrototype,
                                    MP_AnnotReqValNode));

    mp_failr(MP_PutCommonMetaTypePacket(link,
                                    MP_ProtoDict,
                                    MP_CmtProtoIMP_Identifier,
                                    0));
    for (i=0; i<n; i++)
      mp_failr(IMP_PutString(link, names[i]));

    return mpsr_Success;
  }
}

static mpsr_Status_t PutVarNumberAnnot(MP_Link_pt link, ring r, BOOLEAN mv)
{
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyVarNumber,
                                  MP_AnnotValuated));
  mp_return(MP_PutUint32Packet(link, (mv ? r->N + 1 : r->N), 0));
}


static mpsr_Status_t PutOrderingAnnot(MP_Link_pt link, ring r, BOOLEAN mv)
{
  int index = 0, nblocks = 0;
  int *order = r->order;

  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyOrdering,
                                  MP_AnnotValuated));

  // let's see whether we have a simple ordering to sent
  if ((mv == 0) && (order[1] == ringorder_no))
  {
    // ring does not contain a component entry:
    return PutSimpleOrdering(link, r, 0);
  }
  if ((mv == 0) && (order[2] == ringorder_no))
  {
    if (order[1] == ringorder_C || order[1] == ringorder_c)
      index = 0;
    else
      index = 1;

    return PutSimpleOrdering(link, r, index);
  }

  // No -- so we are dealing with a product ordering
  // First, let's find out how many blocks there are
  while (order[nblocks] != ringorder_no) nblocks++;
  // if we deal with a non-vector, we are not going to sent
  // the vector ordering
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicList,
                                      0,
                                      (mv ? nblocks : nblocks - 1)));

  for (index = 0; index < nblocks; index ++)
    // Do not sent vector ordering, if we are sending pure polys
    if (! (mv == 0 && (order[index] == ringorder_C ||
                       order[index] == ringorder_c)))
    {
      // a product ordering is sent as triple
      mp_failr(MP_PutCommonOperatorPacket(link,
                                          MP_BasicDict,
                                          MP_CopBasicList,
                                          0,
                                          3));
      // first element is the simple ordering
      failr(PutSimpleOrdering(link, r, index));
      // second and third for covered variables
      mp_failr(MP_PutUint32Packet(link, (MP_Uint32_t) r->block0[index], 0));
      mp_failr(MP_PutUint32Packet(link, (MP_Uint32_t) r->block1[index], 0));
    }

  return mpsr_Success;
}


static mpsr_Status_t PutSimpleOrdering(MP_Link_pt link, ring r, short index)
{
  MP_Uint32_t vlength = 0, i;

  // find out the length of the weight-vector
  if (r->wvhdl && r->wvhdl[index] != NULL)
    vlength = r->block1[index] - r->block0[index] + 1;

  mp_failr(MP_PutCommonConstantPacket(link,
                                      MP_PolyDict,
                                      mpsr_ord2mp(r->order[index]),
                                      (vlength == 0 ? 0 : 1)));

  if (vlength == 0) return mpsr_Success;

  // deal with the weights
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyWeights,
                                  MP_AnnotValuated));
  if (r->order[index] == ringorder_M)
  {
    // put the matrix header
    mp_failr(MP_PutCommonOperatorPacket(link,
                                        MP_MatrixDict,
                                        MP_CopMatrixDenseMatrix,
                                        2,
                                        vlength*vlength));
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_ProtoDict,
                                    MP_AnnotProtoPrototype,
                                    MP_AnnotReqValNode));
    mp_failr(MP_PutCommonMetaTypePacket(link,
                                    MP_ProtoDict,
                                    MP_CmtProtoIMP_Sint32,
                                    0));
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_MatrixDict,
                                    MP_AnnotMatrixDimension,
                                    MP_AnnotReqValNode));
    mp_failr(MP_PutCommonOperatorPacket(link,
                                        MP_BasicDict,
                                        MP_CopBasicList,
                                        0, 2));
    mp_failr(MP_PutSint32Packet(link, (MP_Sint32_t) vlength, 0));
    mp_failr(MP_PutSint32Packet(link, (MP_Sint32_t) vlength, 0));
    vlength *= vlength;
  }
  else
  {
    // vector header
    mp_failr(MP_PutCommonOperatorPacket(link,
                                        MP_MatrixDict,
                                        MP_CopMatrixDenseVector,
                                        1,
                                        vlength));
    mp_failr(MP_PutAnnotationPacket(link,
                                    MP_ProtoDict,
                                    MP_AnnotProtoPrototype,
                                    MP_AnnotReqValNode));
    mp_failr(MP_PutCommonMetaTypePacket(link,
                                    MP_ProtoDict,
                                    MP_CmtProtoIMP_Sint32,
                                    0));
  }

  // weights are all what remains
  for (i=0; i<vlength; i++)
    mp_failr(IMP_PutSint32(link, (MP_Sint32_t) r->wvhdl[index][i]));

  return mpsr_Success;
}

static mpsr_Status_t PutMinPolyAnnot(MP_Link_pt link, ring r)
{
  mpsr_assume(r->minpoly != NULL && r->ch > 0 && r->parameter == NULL);

  number minpoly = r->minpoly;
  r->minpoly = 0;

  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyDefRel,
                                  MP_AnnotValuated));
  mp_failr(MP_PutCommonOperatorPacket(link,
                                      MP_PolyDict,
                                      MP_CopPolyDenseDistPoly,
                                      5,
                                      GetPlength( ((lnumber) minpoly)->z)));
  failr(PutProtoTypeAnnot(link, r, 0));
  failr(PutVarNamesAnnot(link, r));
  failr(PutVarNumberAnnot(link, r,0));
  failr(PutOrderingAnnot(link, r, 0));
  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyIrreducible,
                                  0));

  // need to set PutAlgAlgnumber and gNalgVars
  CurrPutRing = r;
  if (rField_is_Zp(r) || rField_is_GF(r))
    PutAlgAlgNumber = PutModuloNumber;
  else
    PutAlgAlgNumber = PutRationalNumber;
  gNalgvars = r->N;

  r->minpoly = minpoly;
  return PutAlgPoly(link, ((lnumber) minpoly)->z, r);
}


static mpsr_Status_t PutDefRelsAnnot(MP_Link_pt link, ring r)
{
  int i, idn;
  ideal id;

  id = r->qideal;
  r->qideal = NULL;

  mp_failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyDefRel,
                                  MP_AnnotValuated));
  failr(mpsr_PutIdeal(link, id, r));
  r->qideal = id;
  return mpsr_Success;
}

#endif
