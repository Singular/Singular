/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id: mpsr_GetPoly.cc,v 1.8 1997-06-30 17:04:46 obachman Exp $ */

/***************************************************************
 *
 * File:       mpsr_GetPoly.cc
 * Purpose:    rotines which get polys and polynomails (i.e. ring) annotations
 * Author:     Olaf Bachmann (2/97)
 *
 *
 ***************************************************************/
#include "mod2.h"

#ifdef HAVE_MPSR
#include "limits.h"

#include "mpsr_Get.h"

#include "gmp.h"
#include "mmemory.h"
#include "tok.h"
#include "ring.h"
#include "ipid.h"
#include "longalg.h"
#include "maps.h"
#include "ideals.h"
#include "grammar.h"
#include "febase.h"
#include "modulop.h"

#include "mpsr_Tok.h"

#ifdef PARI_BIGINT_TEST
#include "MP_PariBigInt.h"

MP_Status_t IMP_MyGetApInt(MP_Link_pt link, MP_ApInt_t *apint)
{
  GEN pnum;
  mp_failr(IMP_GetApInt(link, (MP_ApInt_t *) &pnum));
  _pari_to_gmp(pnum, (mpz_ptr *) apint);

  return MP_Success;
}

#else

#define IMP_MyGetApInt IMP_GetApInt

#endif


/***************************************************************
 *
 * global variable definitions
 *
 ***************************************************************/

static mpsr_Status_t (*GetCoeff)(MP_Link_pt link, number *x);
static mpsr_Status_t (*GetAlgNumberNumber)(MP_Link_pt link, number *x);
static MP_Sint32_t gNalgvars = 0;
static MP_Sint32_t gNvars = 0;
static MP_Sint32_t *gTa = NULL;
static ring        currGetRing = NULL;


/***************************************************************
 *
 * prototype declarations
 *
 ***************************************************************/
static void        SetGetFuncs(ring r);
static mpsr_Status_t GetModuloNumber(MP_Link_pt link, number *a);
static mpsr_Status_t GetGaloisNumber(MP_Link_pt link, number *a);
static mpsr_Status_t GetFloatNumber(MP_Link_pt link, number *a);
static mpsr_Status_t GetApInt(MP_Link_pt link, mpz_ptr ap);
static mpsr_Status_t GetRationalNumber(MP_Link_pt link, number *a);
static mpsr_Status_t GetAlgNumber(MP_Link_pt link, number *a);
static mpsr_Status_t GetVarNumberAnnot(MPT_Node_pt node, ring r, BOOLEAN mv);
static mpsr_Status_t GetProtoTypeAnnot(MPT_Node_pt node, ring r, BOOLEAN mv,
                                     ring &subring);
static mpsr_Status_t GetOrderingAnnot(MPT_Node_pt node, ring r, BOOLEAN mv);
static mpsr_Status_t GetSimpleOrdering(MPT_Node_pt node, ring r, short i);
static mpsr_Status_t GetVarNamesAnnot(MPT_Node_pt node, ring r);
static mpsr_Status_t GetDefRelsAnnot(MPT_Node_pt node, ring r);

/***************************************************************
 *
 * Setting the global Put Functions
 *
 ***************************************************************/

static inline BOOLEAN IsCurrGetRing(ring r)
{
  if (r == currGetRing && r == currRing) return 1;
  else return 0;
}

static void SetGetFuncs(ring r)
{
  currGetRing = r;
  // first, we set the PutNumber function
  if (r->N != gNvars)
  {
    if (gTa != NULL)
      Free(gTa, (gNvars+1)*sizeof(MP_Sint32_t));

    gNvars = r->N;
    if (gNvars > 1)
      gTa = (MP_Sint32_t *) Alloc((gNvars+1)*sizeof(MP_Sint32_t));
    else
      gTa = NULL;
  }

  if ((r->ch) == 0)
    // rational numbers
    GetCoeff = GetRationalNumber;
  else if ((r->ch) > 1)
  {
    if (r->parameter == NULL)
      GetCoeff = GetModuloNumber;
    else
      GetCoeff = GetGaloisNumber;
  }
  else if ((r->ch) == -1)
    GetCoeff = GetFloatNumber;
  else
  {
    // now we come to algebraic numbers
    gNalgvars = rPar(r);
    GetCoeff = GetAlgNumber;
    if ((r->ch) < 0)
      // first, Z/p(a)
      GetAlgNumberNumber = GetModuloNumber;
    else
      GetAlgNumberNumber = GetRationalNumber;
  }
  
  // still need to set the global ring
  mpsr_SetCurrRing(r, TRUE);
}



/***************************************************************
 *
 * Routines for Getting coeffs 
 *
 ***************************************************************/
// we always Get modulo numbers without a node, since
// we have type-spec this before
static mpsr_Status_t GetModuloNumber(MP_Link_pt link, number *a)
{
  MP_Uint32_t x;
  mp_failr(IMP_GetUint32(link, &x));
  *a=npInit((int)x);
  return mpsr_Success;
}

static mpsr_Status_t GetGaloisNumber(MP_Link_pt link, number *a)
{
  mp_return(IMP_GetUint32(link, (MP_Uint32_t *) a));
}

static mpsr_Status_t GetFloatNumber(MP_Link_pt link, number *a)
{
   mp_return( IMP_GetReal32(link , (MP_Real32_t *) a));
}

static mpsr_Status_t GetApInt(MP_Link_pt link, mpz_ptr ap)
{
  MP_NodeType_t     node;
  MP_DictTag_t      dict;
  MP_NumChild_t     num_child;
  MP_NumAnnot_t     num_annots;
  MP_Common_t       cvalue;
  MP_Boolean_t      req = 0;
  
  mp_failr(IMP_GetNodeHeader(link,&node,&dict, &cvalue, &num_annots,
                             &num_child));

  if (node == MP_ApIntType)
  {
    mpz_init(ap);
    mp_failr(IMP_MyGetApInt(link, (MP_ApInt_t *) &ap));
  }
  else if (node == MP_Sint32Type || node == MP_Sint8Type)
  {
    MP_Sint32_t i;
    if (node == MP_Sint8Type)
      i = (int) ((signed char) cvalue);
    else
      mp_failr(IMP_GetSint32(link, &i));
    mpz_init_set_si((mpz_ptr) ap, i);
  }
  else if (node == MP_Uint32Type || node == MP_Uint8Type)
  {
    MP_Uint32_t i;
    if (node == MP_Uint8Type)
      i = cvalue;
    else
      mp_failr(IMP_GetUint32(link, &i));
    mpz_init_set_ui((mpz_ptr) ap, i);
  }
  else
    return mpsr_SetError(mpsr_WrongNodeType);
  
  if (num_annots > 0)
  {
    mpt_failr(MPT_SkipAnnots(link, num_annots, &req));
    if (req) return mpsr_SetError(mpsr_ReqAnnotSkip);
  }
  
  return mpsr_Success;
}
    
// This supposes that number is of char 0, i.e. a rational number
static mpsr_Status_t GetRationalNumber(MP_Link_pt link, number *x)
{
  MP_NodeType_t     node;
  MP_DictTag_t      dict;
  MP_NumChild_t     num_child;
  MP_NumAnnot_t     num_annots;
  MP_Sint32_t       i;
  MP_Common_t       cvalue;
  number            y;
  MP_Boolean_t      req;

  mp_failr(IMP_GetNodeHeader(link,&node,&dict, &cvalue, &num_annots,
                             &num_child));

  // start with the most frequent cases
  if (node == MP_Sint32Type) 
  {
    mp_failr(IMP_GetSint32(link, &i));
    *x = nlInit(i);
  }
  else if (node == MP_ApIntType)
  {
    mpz_ptr gnum;
    *x =  (number) Alloc0(sizeof(rnumber));
    y = (number) *x;
    y->s = 3;
    gnum = &(y->z);
    mpz_init(gnum);
    mp_failr(IMP_MyGetApInt(link, (MP_ApInt_t *) &gnum));
  }
  // fraction of numbers
  else if (node == MP_CommonOperatorType &&
           dict== MP_BasicDict &&
           cvalue == MP_CopBasicDiv)
  {
    if (num_annots > 0)
    {
      mpt_failr(MPT_SkipAnnots(link, num_annots, &req));
      if (req) return mpsr_SetError(mpsr_ReqAnnotSkip);
    }
    *x =  (number) Alloc0(sizeof(rnumber));
    y = (number) *x;
    y->s = 1;
    failr(GetApInt(link, &(y->z)));
    return GetApInt(link, &(y->n));
  }
  // check for some more esoteric cases
  else if (node == MP_Uint8Type)
    *x = nlInit(cvalue);
  else if (node == MP_Sint8Type)
    // be careful -- need to handle the value "-2", for example
    *x = nlInit((int) ((MP_Sint8_t) cvalue));
  else if (node == MP_Uint32Type)
  {
    MP_Uint32_t ui;
    mp_failr(IMP_GetUint32(link, &ui));
    // check whether u_int can be casted safely to int
    if (ui < INT_MAX)
      *x = nlInit(i);
    else
    {
      // otherwise, make an apint out of it
      *x =  (number) Alloc0(sizeof(rnumber));
      y = (number) *x;
      mpz_init_set_ui(&(y->z), ui);
      y->s = 3;
    }
  }
  else
    return mpsr_SetError(mpsr_WrongNodeType);
  
  if (num_annots > 0)
  {
    mpt_failr(MPT_SkipAnnots(link, num_annots, &req));
    if (req) return mpsr_SetError(mpsr_ReqAnnotSkip);
  }

  return mpsr_Success;
}

/***************************************************************
 *
 * Algebraic Numbers (a la Singular)
 *
 ***************************************************************/
static inline mpsr_Status_t GetAlgPoly(MP_Link_pt link, alg *p)
{
  MP_Uint32_t j, nm;
  int *exp;
  alg a;

  IMP_GetUint32(link, &nm);

  if (nm == 0)
  {
    *p = NULL;
    return mpsr_Success;
  }
  a = napNew();
  *p = a;

  failr(GetAlgNumberNumber(link, &(a->ko)));
  exp = &(a->e[0]);
  mp_failr(IMP_GetSint32Vector(link, (MP_Sint32_t **) &exp, naNumbOfPar));

  for (j=1; j<nm; j++)
  {
    a->ne = napNew();
    a = a->ne;
    failr(GetAlgNumberNumber(link, &(a->ko)));
    exp = &(a->e[0]);
    mp_failr(IMP_GetSint32Vector(link, (MP_Sint32_t **) &exp, naNumbOfPar));
  }
  a->ne = NULL;

  return mpsr_Success;
}

static mpsr_Status_t GetAlgNumber(MP_Link_pt link, number *a)
{
  lnumber b;
  MP_Uint32_t ut;

  // Get the union tag
  mp_failr(IMP_GetUint32(link, &ut));
  if (ut == 0)
  {
    *a = NULL;
    return mpsr_Success;
  }
  else if (ut == 1 || ut == 2)
  {
    // single number
    b = (lnumber) Alloc0(sizeof(rnumber));
    *a = (number) b;
    failr(GetAlgPoly(link, &(b->z)));
    if (ut == 2)
      return GetAlgPoly(link, &(b->n));
    else
      return mpsr_Success;
  }
  else
    return mpsr_SetError(mpsr_WrongUnionDiscriminator);
}

/***************************************************************
 *
 *  Getting polys
 *
 ***************************************************************/
mpsr_Status_t mpsr_GetPoly(MP_Link_pt link, poly &p, MP_Uint32_t nmon,
                         ring cring)
{
  poly pp;
  MP_Sint32_t i;
  MP_Uint32_t j;
  short *exp;

  if (! IsCurrGetRing(cring))
    SetGetFuncs(cring);
  
  if (nmon == 0)
  {
    p = NULL;
    return mpsr_Success;
  }
  
  pp = pNew();
  p = pp;
  failr(GetCoeff(link, &(pp->coef)));
  if (gNvars > 1)
  {
    mp_failr(IMP_GetSint32Vector(link, &gTa, gNvars));
    pp->exp[0] = 0;
    for (i=0, exp=&(pp->exp[1]); i<gNvars; i++)
      exp[i] = (short) gTa[i];
    pSetm(pp);

    for (j=1; j<nmon; j++)
    {
      pp->next = pNew();
      pp = pp->next;
      failr(GetCoeff(link, &(pp->coef)));
      mp_failr(IMP_GetSint32Vector(link, &gTa, gNvars));
      pp->exp[0] = 0;
      for (i=0, exp=&(pp->exp[1]); i<gNvars; i++)
        exp[i] = (short) gTa[i];
      pSetm(pp);
    }
  }
  else
  {
    mp_failr(IMP_GetSint32(link, &i));
    pp->exp[0] = 0;
    pp->exp[1] = (short) i;
    pSetm(pp);
    
    for (j=1; j<nmon; j++)
    {
      pp->next = pNew();
      pp = pp->next;
      failr(GetCoeff(link, &(pp->coef)));
      mp_failr(IMP_GetSint32(link, &i));
      pp->exp[0] = 0;
      pp->exp[1] = (short) i;
      pSetm(pp);
    }
  }

  pp->next = NULL;

  pTest(p);
  return mpsr_Success;
}

mpsr_Status_t mpsr_GetPolyVector(MP_Link_pt link, poly &p, MP_Uint32_t nmon,
                          ring cring)
{
  poly pp;
  MP_Sint32_t i, n1;
  MP_Uint32_t j;
  short *exp;

  if (!IsCurrGetRing(cring))
    SetGetFuncs(cring);
  
  n1 = gNvars + 1;
  if (nmon == 0)
  {
    p = NULL;
    return mpsr_Success;
  }
  
  pp = pNew();
  p = pp;
  failr(GetCoeff(link, &(pp->coef)));
  if (gNvars > 1)
  {
    mp_failr(IMP_GetSint32Vector(link, &gTa, n1));
    pp->exp[0] = 0;
    for (i=0, exp=&(pp->exp[0]); i<n1; i++)
      exp[i] = (short) gTa[i];
    pSetm(pp);

    for (j=1; j<nmon; j++)
    {
      pp->next = pNew();
      pp = pp->next;
      failr(GetCoeff(link, &(pp->coef)));
      mp_failr(IMP_GetSint32Vector(link, &gTa, n1));
      pp->exp[0] = 0;
      for (i=0, exp=&(pp->exp[0]); i<n1; i++)
        exp[i] = (short) gTa[i];
      pSetm(pp);
    }
  }
  else
  {
    mp_failr(IMP_GetSint32(link, &i));
    pp->exp[0] = (short) i;
    mp_failr(IMP_GetSint32(link, &i));
    pp->exp[1] = (short) i;
    pSetm(pp);
    
    for (j=1; j<nmon; j++)
    {
      pp->next = pNew();
      pp = pp->next;
      failr(GetCoeff(link, &(pp->coef)));
      mp_failr(IMP_GetSint32(link, &i));
      pp->exp[0] = (short) i;
      mp_failr(IMP_GetSint32(link, &i));
      pp->exp[1] = (short) i;
      pSetm(pp);
    }
  }
  pp->next = NULL;

  pTest(p);
  return mpsr_Success;
}

/***************************************************************
 *
 *  The Getting annotation buisness
 *
 ***************************************************************/
#define falser(x)                               \
do                                              \
{                                               \
  if (!(x)) return mpsr_Failure;                   \
}                                               \
while (0)

// We assume that the node is that of a DDP: This returns
// MP_Succcess, if annots of node can be used to construct a
// Singular ring (in which case r is the respective ring) or,
// MP_Failure, if not
mpsr_Status_t mpsr_GetRingAnnots(MPT_Node_pt node, ring &r, BOOLEAN &mv)
{
  sip_sring r1, *subring;
  poly minpoly = NULL;

  memset(&r1, 0, sizeof(sip_sring));

  r = NULL;
  if (MPT_FindAnnot(node, MP_PolyDict, MP_AnnotPolyModuleVector) != NULL)
    mv = 1;
  else
    mv = 0;

  // sets r->N
  if (GetVarNumberAnnot(node, &r1, mv) != mpsr_Success)
    Warn("GetVarNumberAnnot: using the one found in the prototype");

  // sets r->char and r->minpoly, r->parameter; if necessary
  failr(GetProtoTypeAnnot(node, &r1, mv, subring));

  // if we are still here, then we are successful in constructing the ring
  r = (ring) Alloc(sizeof(sip_sring));
  memcpy(r, &r1, sizeof(sip_sring));

  if (GetVarNamesAnnot(node, r) != mpsr_Success)
    Warn("GetVarNamesAnnot: using default variable names");

  if (GetOrderingAnnot(node,r, mv) != mpsr_Success)
    Warn("GetOrderingAnnot: using unspec ordering");

  if (GetDefRelsAnnot(node, r) != mpsr_Success)
    Warn("GetDefRelsAnnot: using no defining relations");

  // check on whether or not I have to set a minpoly
  if (subring != NULL)
  {
    if ((subring->qideal != NULL) &&
        ((minpoly = subring->qideal->m[0]) != NULL))
    {
      mpsr_SetCurrRing(r, TRUE);
      minpoly = maIMap(subring, minpoly);
      r->minpoly = minpoly->coef;
      pFree1(minpoly);
    }
    rKill(subring);
  }
    
  return mpsr_Success;
}


static mpsr_Status_t GetVarNumberAnnot(MPT_Node_pt node, ring r, BOOLEAN mv)
{
  MPT_Annot_pt annot = MPT_FindAnnot(node, MP_PolyDict, MP_AnnotPolyVarNumber);

  if (annot != NULL) 
  {
    if (annot->value != NULL && annot->value->node->type == MP_Uint32Type)
    {
      // Hm.. should check that r->N is not too big for Singular
      r->N = (short) MP_UINT32_T(annot->value->node->nvalue);
      if (mv) (r->N)--;
      return mpsr_Success;
    }
  }
  return mpsr_Failure;
}

          
static mpsr_Status_t GetProtoTypeAnnot(MPT_Node_pt node, ring r, BOOLEAN mv,
                                     ring &subring)
{
  MPT_Annot_pt annot = NULL;
  MPT_Tree_pt  val;
  MPT_Tree_pt  *ta;

  subring = NULL;
  
  // look for prototype annot
  if ((val = MPT_GetProtoTypespec(node)) == NULL)
    return mpsr_Failure;

  // check value of annot
  node = val->node;
  if (! (NodeCheck(node, MP_CommonOperatorType, MP_ProtoDict,
                   MP_CopProtoStruct) && node->numchild == 2))
    return mpsr_Failure;
  // get the two args of the value
  ta = (MPT_Tree_pt *) val->args;


  // We get the exponent vector specification first
  node = ta[1]->node;
  if (! (NodeCheck(node, MP_CommonMetaOperatorType, MP_ProtoDict,
                   MP_CopProtoArray) && node->numchild > 0))
    return mpsr_Failure;
  // check r->N and reset, if necessary
  if (mv) 
  {
    if (r->N != (int) (node->numchild - 1))
    {
      Warn("GetProtoAnnot: Inconsistent NumVars specification");
      r->N = (node->numchild -1);
    }
  }
  else
  {
    if (r->N != (int) node->numchild)
    {
      Warn("GetProtoAnnot: Inconsistent NumVars specification");
      r->N = (node->numchild);
    }
  }
  // check for type of exponent
  if ((val = MPT_GetProtoTypespec(node)) == NULL)
    return mpsr_Failure;
  
  node = val->node;
  falser(NodeCheck(node, MP_CommonMetaType, MP_ProtoDict, MP_CmtProtoIMP_Sint32));

  // consider the first arg -- which specify the coeffs
  val = ta[0];
  node = val->node;
  if (node->type == MP_CommonMetaType)
  {
    // char 0
    if (MP_COMMON_T(node->nvalue) == MP_CmtNumberRational &&
        node->dict == MP_NumberDict)
    {
      r->ch = 0;
      // Hmm ... we should check for the normalized annot
    }
    else if (MP_COMMON_T(node->nvalue) == MP_CmtProtoIMP_Uint32 &&
             node->dict == MP_ProtoDict &&
             (annot = MPT_FindAnnot(node,MP_NumberDict,MP_AnnotNumberModulos))
              != NULL)
    {
      // char p || GF(p,n)
      falser(annot->value != NULL &&
             annot->value->node->type == MP_Uint32Type);
      r->ch = MP_UINT32_T(annot->value->node->nvalue);

      if (MPT_FindAnnot(annot->value->node,
                        MP_NumberDict, MP_AnnotNumberIsPrime) == NULL)
      {
        // GF(p,n)
        falser((annot = MPT_FindAnnot(annot->value->node, 129,
                                  MP_AnnotSingularGalois)) != NULL &&
           (annot->value != NULL) &&
           (annot->value->node->type == MP_StringType));
        r->parameter = (char **)Alloc(sizeof(char *));
        r->parameter[0] = mstrdup(MP_STRING_T(annot->value->node->nvalue));
        r->P = 1;
      }
    }
    else if (MP_COMMON_T(node->nvalue) == MP_CmtProtoIMP_Real32 &&
             node->dict == MP_ProtoDict)
    {
      // floats
      r->ch = -1;
    }
    else
      return mpsr_SetError(mpsr_UnknownCoeffDomain);
    
    return mpsr_Success;
  }
  else
  {
    // alg numbers
    BOOLEAN mv2;
    int i;

    // DDP Frac Node check
    falser(NodeCheck(node, MP_CommonMetaOperatorType, MP_BasicDict,
                    MP_CopBasicDiv) &&
           node->numchild == 0);
    falser((val = MPT_GetProtoTypespec(node)) != NULL);
    node = val->node;
    mpsr_assume(node != NULL);
    falser(NodeCheck(node, MP_CommonMetaOperatorType, MP_PolyDict,
                     MP_CopPolyDenseDistPoly) &&
           node->numchild == 0);
    // GetRingAnnots
    failr(mpsr_GetRingAnnots(node, subring, mv2));
    // Check whether the ring can be "coerced" to an algebraic number
    falser(subring->ch >= 0 &&
           subring->order[0] == ringorder_lp &&
           subring->order[2] == 0 &&
           mv2 == FALSE);

    // Now do the coercion
    r->ch = (subring->ch == 1 ? 0 : - (subring->ch));
    r->parameter = (char **) Alloc((subring->N)*sizeof(char*));
    r->P = subring->N;
    for (i=0; i < subring->N; i++)
      r->parameter[i] = mstrdup(subring->names[i]);
    
    // everything is ok
    return mpsr_Success;
  }
}

static mpsr_Status_t GetVarNamesAnnot(MPT_Node_pt node, ring r)
{
  MPT_Annot_pt annot = MPT_FindAnnot(node, MP_PolyDict, MP_AnnotPolyVarNames);
  short num_vars = 0, N, lb, offset, nc;

  mpsr_assume(r != NULL);
  N = r->N;
  r->names = (char **) Alloc(N * sizeof(char *));
  
  // fill in varnames from the back
  if (annot != NULL && annot->value != NULL)
  {
    node = annot->value->node;
    nc = (short) node->numchild;
    if (NodeCheck(node, MP_CommonOperatorType, MP_ProtoDict, MP_CopProtoArray))
    {
      MPT_Tree_pt val = MPT_GetProtoTypespec(node);
      if (val != NULL &&
          NodeCheck(val->node, MP_CommonMetaType, MP_ProtoDict,
                    MP_CmtProtoIMP_Identifier))
      {
        MPT_Arg_pt arg_pt = annot->value->args;        
        lb = min(nc, N);
        offset = N - (short) nc;
        if (offset < 0) offset = 0;
        for (; num_vars < lb; num_vars++)
          r->names[offset + num_vars] =
            mstrdup(MP_STRING_T(arg_pt[num_vars]));
      }
    }
    else if (node->type == MP_IdentifierType)
    {
      r->names[N-1] = mstrdup(MP_STRING_T(annot->value->node->nvalue));
      num_vars = 1;
    }
  }

  // fill in all remaining varnames
  if (num_vars < N)
  {
    char vn[10];
    offset = N - num_vars;
    for (nc = 0; nc < offset; nc++)
    {
      sprintf(vn, "x(%d)", nc);
      r->names[nc] = mstrdup(vn);
    }
  }

  if (num_vars < N) return mpsr_Failure;
  else return mpsr_Success;
}

static mpsr_Status_t GetOrderingAnnot(MPT_Node_pt node, ring r, BOOLEAN mv)
{
  MPT_Annot_pt annot = MPT_FindAnnot(node, MP_PolyDict, MP_AnnotPolyOrdering);
  mpsr_Status_t status = mpsr_Success;

  if (annot == NULL || annot->value == NULL) status = mpsr_Failure;

  if (status == mpsr_Success) node =  annot->value->node;

  // Check for BlockOrdering
  if (status == mpsr_Success &&
      NodeCheck(annot->value->node, MP_CommonOperatorType,
               MP_BasicDict, MP_CopBasicList))
  {
    MP_NumChild_t nc = node->numchild, i;
    MPT_Tree_pt *tarray = (MPT_Tree_pt *) annot->value->args, *tarray2, tree;

    if (! mv) nc += 2; else nc++;
    r->block0 = (int *) Alloc0(nc*sizeof(int *));
    r->block1 = (int *) Alloc0(nc*sizeof(int *));
    r->wvhdl  = (short **) Alloc0(nc*sizeof(short *));
    r->order  = (int *) Alloc0(nc*sizeof(int *));

    if (! mv)
    {
      r->order[nc-2] = ringorder_C;
      nc = nc - 2;
    }
    else
      nc--;

    for (i=0; i<nc; i++)
    {
      tree = tarray[i];
      if (NodeCheck(tree->node, MP_CommonOperatorType,
                   MP_BasicDict, MP_CopBasicList) &&
          tree->node->numchild == 3)
      {
        tarray2 = (MPT_Tree_pt *) tree->args;
        if (GetSimpleOrdering(tarray2[0]->node, r, i) != mpsr_Success ||
            tarray2[1]->node->type != MP_Uint32Type ||
            tarray2[2]->node->type != MP_Uint32Type)
        {
          status = mpsr_Failure;
          break;
        }
        else
        {
          r->block0[i] = MP_SINT32_T(tarray2[1]->node->nvalue);
          r->block1[i] = MP_SINT32_T(tarray2[2]->node->nvalue);
        }
      }
      else
      {
          status = mpsr_Failure;
          break;
      }
    }

    if (status == mpsr_Success) status = mpsr_rSetOrdSgn(r);
    
    // Clean up if sth went wrong
    if (status == mpsr_Failure)
    {
      if (mv) nc++;
      Free(r->block0, nc*sizeof(int *));
      Free(r->block1, nc*sizeof(int *));
      Free(r->order, nc*sizeof(int *));
      Free(r->wvhdl, nc*sizeof(short *));
    }
    else
      return mpsr_Success;
  }

  // Either Simple Ordering, or sth failed from before
  r->wvhdl = (short **)Alloc0(3 * sizeof(short *));
  r->order = (int *) Alloc0(3 * sizeof(int *));
  r->block0 = (int *)Alloc0(3 * sizeof(int *));
  r->block1 = (int *)Alloc0(3 * sizeof(int *));
  r->order[1] = ringorder_C;
  r->block0[0] = 1;
  r->block1[0] = r->N;

  // Check for simple Ordering
  if (status == mpsr_Success)
  {
    status = GetSimpleOrdering(node, r, 0);
  }
  else
    r->order[0] = ringorder_unspec;
  
  return mpsr_rSetOrdSgn(r);
}

static mpsr_Status_t GetSimpleOrdering(MPT_Node_pt node, ring r, short i)
{
  if (node->type != MP_CommonConstantType)
    return mpsr_Failure;

  r->order[i] = mpsr_mp2ord(MP_UINT32_T(node->nvalue));
  if (r->order[i] == ringorder_unspec) return mpsr_Failure;

  MPT_Annot_pt annot = MPT_FindAnnot(node, MP_PolyDict, MP_AnnotPolyWeights);

  if (annot == NULL) return mpsr_Success;
  if (annot->value == NULL) return mpsr_Failure;

  node = annot->value->node;
  if (r->order[i] == ringorder_M)
  {
    if (! NodeCheck(node, MP_CommonOperatorType, MP_MatrixDict,
                   MP_CopMatrixDenseMatrix))
      return mpsr_Failure;
  }
  else
  {
    if (! NodeCheck(node, MP_CommonOperatorType, MP_MatrixDict,
                   MP_CopMatrixDenseVector))
      return mpsr_Failure;
  }

  MPT_Annot_pt annot2 = MPT_FindAnnot(node, MP_ProtoDict, MP_AnnotProtoPrototype);

  if (annot2 == NULL ||
      ! NodeCheck(annot2->value->node, MP_CommonMetaType, MP_ProtoDict,
                 MP_CmtProtoIMP_Sint32))
    return mpsr_Failure;

  MP_Uint32_t nc = node->numchild, j;
  MP_Sint32_t *w = (MP_Sint32_t *) annot->value->args;
  short *w2 = (short *) AllocL(nc*sizeof(short));
  
  r->wvhdl[i] = w2;
  for (j = 0; j < nc ; j++)
    w2[j] = w[j];

  return mpsr_Success;
}

static mpsr_Status_t GetDefRelsAnnot(MPT_Node_pt node, ring r)
{
  MPT_Annot_pt annot = MPT_FindAnnot(node, MP_PolyDict, MP_AnnotPolyDefRel);
  mpsr_leftv mlv;
  leftv lv;
  ring r1;

  if (annot == NULL) return mpsr_Success;

  node = annot->value->node;
  if (node->type != MPT_ExternalDataType) return mpsr_Failure;

  mlv = (mpsr_leftv) annot->value->args;
  r1 = mlv->r;
  lv = mlv->lv;

  if (! mpsr_RingEqual(r1, r)) return mpsr_Failure;

  if (lv->rtyp == POLY_CMD)
  {
    r->qideal = idInit(1,1);
    r->qideal->m[0] = (poly) lv->data;
    lv->data = NULL;
  }
  else if (lv->rtyp == IDEAL_CMD)
  {
    r->qideal = (ideal) lv->data;
    lv->data = NULL;
  }
  else return mpsr_Failure;
  
  return mpsr_Success;
}
  
extern mpsr_Status_t mpsr_rSetOrdSgn(ring r)
{
  short i = 0, order;
  r->OrdSgn = 1;

  while ((order = r->order[i]) != ringorder_no)
  {
    if (order == ringorder_ls ||
        order == ringorder_Ws ||
        order == ringorder_ws ||
        order == ringorder_Ds ||
        order == ringorder_ds)
    {
      r->OrdSgn = -1;
      return mpsr_Success;
    }
    if (order == ringorder_M)
    {
      int sz = r->block1[i] - r->block0[i] + 1, j, k=0;
      short *matrix = r->wvhdl[i];
      
      while (k < sz)
      {
        j = 0;
        while ((j < sz) && matrix[j*sz+k]==0) j++;
        if (j>=sz)
        {
          Warn("Matrix order not complete");
          r->OrdSgn = 0;
          return mpsr_Failure;
        }
        else if (matrix[j*sz+k]<0)
        {
          r->OrdSgn = -1;
          return mpsr_Success;
        }
        else
          k++;
      }
    }
    i++;
  }
  return mpsr_Success;
}

#endif
