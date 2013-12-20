/*! @file
 *
 *  @brief This File contains the Implemenations for
 *         Monomial/Polynomial Multiplications for ShiftDVec
 *         Letterplace
 *
 *  \remarks
 *    These multiplications functions need unshifted polynomials.
 *    The pairs were formerly stored as shifted/unshifted, but
 *    considering the actual multiplication, it is more efficient
 *    not to shift the polynomial (and this will even reduce the
 *    memory needed for storing pairs).
 *
 *  \todo there were many other comments in a previous version of
 *        this file; maybe we should review them. ( see commit
 *        166cd4a953fee0ab72c072d6006afaf1177055ee )
 *  \todo comment the procedures in this file properly
 *  \todo I forgot so much...
 *  \todo better explanation of procedures and their parameters
 */

#include <kernel/SDMultiplication.h>
#include <kernel/SDkutil.h>
#include <kernel/SDDebug.h>


// Part 1: General Tools - especially for dp case and the like


/** Mapping used to speed up Multiplication
 *
 *  Creates a Mapping: i -> pos, where i is an index of a
 *  variable in a block of the letterplace polynomial and pos the
 *  position of the order Field for the block. May not yet work
 *  for other orderings than dp. The Mapping is stored to
 *  r->omap, the size of the mapping to r->osize. omap[0] has a
 *  special meaning: it is the glocal order Field, thus the
 *  totaldegree in the dp-Case.                                
 *
 *  \todo This has to be adapted to other orderings. And the
 *        comment should be rewritten to be more understandable
 *        and more accurate. note: There seems to be no Problem
 *        with other orderings - hopefully, but I am yet not
 *        sure.
 */
int ShiftDVec::InitOrderMapping( ring r )
{
  r->omap = (int *) OMALLOC( r->N+1, int );
  
  for(int i = 1; i < r->OrdSize; ++i)
  {
    sro_ord* o=&(r->typ[i]);
    //assume(o->ord_typ == ro_dp); //see comment above
    int a,e;
    a=o->data.dp.start;
    e=o->data.dp.end;
    for(int i=a;i<=e;i++) (r->omap)[i] = o->data.dp.place;
  }
  
  (r->omap)[0] = currRing->pOrdIndex;
  
  //BOCO TODO: This is a bit redundant...
  return r->osize = r->N+1;
}

/** Initializes the ShiftDVec Letterplace multiplication.
 *
 * See also InitOrderMapping. Do not forget to free r->omap as
 * soon, as it will no longer be used.
 *
 * \todo better explanation ?
 */
void ShiftDVec::InitSDMultiplication
  ( ring r, SD::kStrategy strat )
{
  r->isLPring = strat->get_lV(); 
  //BOCO: this should have already been set by
  //makeLetterplaceRing, but it isnt :(

  r->p_ExpSum = &SD::p_ExpSum_slow;

#if 0 //BOCO: Well, I hope all works fine even without ro_dp...
  for(int i = 1; i < r->OrdSize; ++i)
  {
    if( r->typ[i].ord_typ != ro_dp )
    {
      r->p_ExpSum = &SD::p_ExpSum_slow;
      return;
    }
  }
#endif

  //BOCO: TODO: QUESTION: 
  //Should we reset that after the bba?
  r->p_Procs->pp_Mult_mm =
    r->p_Procs->LPDV__pp_Mult_mm;
  r->p_Procs->pp_Mult_mm_Noether =
    r->p_Procs->LPDV__pp_Mult_mm_Noether;
  r->p_Procs->p_Minus_mm_Mult_qq =
    r->p_Procs->LPDV__p_Minus_mm_Mult_qq;

  InitOrderMapping(r);
}

#if 0 //Not used at the moment
//BOCO: Get the vth exponent in exp (derived from p_GetExp)
long ShiftDVec::_GetExp(long* exp, ring r, const int v)
{
pAssume2(v>0 && v <= r->N);
pAssume2(r->VarOffset[v] != -1);

int pos  =(r->VarOffset[v] & 0xffffff);
int hbyte= (r->VarOffset[v] >> 24); // the highest byte
int bitpos = hbyte & 0x3f; // last 6 bits
long bitmask = BitMask(r->bitmask, hbyte >> 6);

return (exp[pos] >> bitpos) & bitmask;
}
#endif


// Part 2: replacements for p_MemSum__T .
// And other helper functions (mainly for ksReduce...)


/** @brief This is the ShiftDVec Letterplace replacement for
 *         p_MemSum__T - the slow version.
 *
 * This function will return rt->exp = p->exp + s * q->exp, where
 * s is a shift in size of the number of exponents equal to on in
 * p->exp . rt has to be allocated (for example with
 * p_AllocBin?).
 * 
 * This is a slow version, but it should be applicable in most
 * cases.
 *
 * \todo doxygen explanation of parameters ?
 */
void ShiftDVec::p_ExpSum_slow
(poly rt, poly p, poly q, ring r)
{
  int lV = r->isLPring;
  
  //This represents the first index in the currently considered
  //block in rt->exp.
  long index_rt = p_Totaldegree(p, r) * lV + 1;
  
  long index_q = 1;
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q, index_q+i, r) )
      {
        p_SetExp(rt, index_rt+i, 1, r);
        index_rt += lV;
        if(index_rt > r->N) break; //looped through all vars
        index_q += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
  }
  
  //TODO: Maybe should be done later to reduce overhead
  p_Setm(rt,r);
  
  return;
}

/** @brief This is the ShiftDVec Letterplace replacement for
 *         p_MemSum__T - the fast version.
 *
 * This function will return rt->exp = p->exp + s * q->exp, where
 * s is a shift in size of the number of exponents equal to on in
 * p->exp . rt has to be allocated (for example with
 * p_AllocBin?).
 * 
 * This is a fast version, which was tested for dp ordering, but
 * may be applicable to other orderings as well (without
 * warranty).
 *
 * \todo doxygen explanation of parameters ?
 */
void ShiftDVec::p_ExpSum_dp
(poly rt, poly p, poly q, ring r)
{
  //This represents the first index in the currently considered
  //block in rt->exp.
  //long index_rt = p_Totaldegree(p, r) * lV + 1;
  int lV = r->isLPring;
  long index_rt = p->exp[r->omap[0]] * lV + 1;
  
  long index_q = 1;
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q, index_q+i, r) )
      {
        rt->exp[r->omap[index_rt+i]] = 1;
        p_SetExp(rt, index_rt+i, 1, r);
        index_rt += lV;
        if(index_rt > r->N) break; //looped through all vars
        index_q += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
  }
  rt->exp[r->omap[0]] += q->exp[r->omap[0]];
  
  return;
}

/** get the cofactors (left and right) of a divistion
 * 
 * p will become ml and vice versa (Thus p is modified!)
 * ml will be a new poly
 *
 * \todo better explanation of function and parameters
 * \remarks
 *    - Assumes divides_p is shifted correctly
 *    - may be (speed) improvable, but I don't think that matters
 *      to much at the place where it is used - I at least hope
 *      so...
 */
void ShiftDVec::get_division_cofactors
  ( poly p, poly divides_p, 
    int divides_p_shift, poly* ml, poly* mr, ring r )
{
  int lV = r->isLPring;
  p_ExpVectorSub(p, divides_p, r);
  long index_p = 
    (divides_p->exp[r->omap[0]] + divides_p_shift) * lV + 1;
  *mr = P_INIT(r);
  if(index_p > r->N) {*ml = p; return; }
  long index_mr = 1;
  { //this is a loop
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(int i = 0; i < lV; ++i)
      if( p_GetExp(p, index_p+i, r) )
      {
        (*mr)->exp[r->omap[index_mr+i]] = 1;
        p_SetExp(*mr, index_mr+i, 1, r);

        // p shall become ml, so substract the exponent
        p->exp[r->omap[index_p+i]] = 0;
        p_SetExp(p, index_p+i, 0, r);

        index_p += lV;
        if(index_p > r->N) break; //looped through all vars
        index_mr += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
    }

  *ml = p;
}

//BOCO:
//This function does what it seems to do, or not.
//We should store shift whereever possible to avoid use of this
//function. Maybe there is a better heuristik than starting
//testing from the beginning - but I get off the point...
int ShiftDVec::get_shift_of(poly p, ring r)
{
  int lV = r->isLPring;
  long deg_p = p->exp[r->omap[0]] * lV;

  //TODO: Is it ok, to define the shift of a poly equal to zero
  //as zero ? :
  if(deg_p == 0) return 0;

  long index_p = 0;

  //find the polynomial
  { //this is a loop
    nexttry: ;
    index_p += deg_p;
    assume(index_p <= r->N);
    for(int i = 0; i < lV; ++i)
      if( p_GetExp(p, index_p-i, r) )
        goto endloop; //found the poly
    goto nexttry; //poly not yet found

    endloop: ;
  }

  //find the first empty block of the polynomial 
  //(could be improved or merged with the loop above)
  { //this is a loop
    nexttry2: ;

    index_p -= lV;
    if(index_p == 0) return 0; //first block nonempty => shift=0
    for(int i = 0; i < lV; ++i)
      if( p_GetExp(p, index_p-i, r) )
        goto nexttry2; //block not empty

    return index_p/lV; //found empty block, calculate shift
  }
}


/* Part 2: adjustments of various functions which invoke some */
/* of the multiplications functions .                         */



int ShiftDVec::red_count = 0; //count of reductions
int ShiftDVec::create_count = 0;  //count of spoly creations

/***************************************************************
*
* Reduces PR with SPW
* Assumes PR != NULL, SPW != NULL, Lm(SPW) divides Lm(PR)
* SPW is the shifted UPW
*
***************************************************************/
int ShiftDVec::ksReducePoly(LObject* PR,
               TObject* UPW,
               TObject* SPW,
               poly spNoether,
               number *coef,
               SD::kStrategy strat)
{
#ifdef TEST_OPT_DEBUG_RED
  if (TEST_OPT_DEBUG)
  {
    Print("Red %d:", red_count); PR->wrp(); Print(" with:");
    PW->wrp();
  }
#endif
  int ret = 0;
  ring tailRing = PR->tailRing;
  kTest_L(PR);
  kTest_T(UPW);
  poly p1 = PR->GetLmTailRing();   // p2 | p1
  poly p2 = SPW->GetLmTailRing();   // i.e. will reduce p1 with p2; lm = LT(p1) / LM(p2)
  deBoGriPrint(p1, "p1: ", 4096, currRing, PR->tailRing);
  deBoGriPrint(p2, "p1: ", 4096, currRing, PR->tailRing);
  poly t2 = pNext(UPW->GetLmTailRing()), lm = p1;    // t2 = p2 - LT(p2); really compute P = LC(p2)*p1 - LT(p1)/LM(p2)*p2
  assume(p1 != NULL && p2 != NULL);// Attention, we have rings and there LC(p2) and LC(p1) are special
  p_CheckPolyRing(p1, tailRing);
  p_CheckPolyRing(p2, tailRing);

  pAssume1(p2 != NULL && p1 != NULL &&
           p_DivisibleBy(p2,  p1, tailRing));

  pAssume1(p_GetComp(p1, tailRing) == p_GetComp(p2, tailRing) ||
           (p_GetComp(p2, tailRing) == 0 &&
            p_MaxComp(pNext(p2),tailRing) == 0));

#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    // for the time being: we know currRing==strat->tailRing
    // no exp-bound checking needed 
    // (only needed if exp-bound(tailring)<exp-b(currRing))
    if (PR->bucket!=NULL)  nc_kBucketPolyRed(PR->bucket, p2,coef);
    else
    {
      poly _p = (PR->t_p != NULL ? PR->t_p : PR->p);
      assume(_p != NULL);
      nc_PolyPolyRed(_p, p2,coef, currRing);
      if (PR->t_p!=NULL) PR->t_p=_p; else PR->p=_p;
      PR->pLength=0; // usually not used, GetpLength re-computes it if needed
    }
    return 0;
  }
#endif
  if (t2==NULL)           // Divisor is just one term, therefore it will
  {                       // just cancel the leading term
    PR->LmDeleteAndIter();
    if (coef != NULL) *coef = n_Init(1, tailRing);
    return 0;
  }

#if 0  //BOCO: For LP we need a left and a right cofactor
  p_ExpVectorSub(lm, p2, tailRing); // Calculate the Monomial we must multiply to p2
#else
  poly mr;
  int shift_of_p2 = SD::get_shift_of(p2, tailRing);
  //BOCO:
  //lm: lm of poly which shall be reduced
  //    will become left cofactor
  //p2: divisor of lm
  //mr: will become right cofactor
  SD::get_division_cofactors
    (lm, p2, shift_of_p2, &lm, &mr, tailRing);
#endif

  if (tailRing != currRing)
  {
    // check that reduction does not violate exp bound
    // BOCO: TODO: think about it!
    while( SPW->max != NULL &&
           !p_LmExpVectorAddIsOk(lm, SPW->max, tailRing) )
    {
      //BOCO: hopefully this loop is not executed very often...
      //otherwise: have to build a method called
      //unget_division_cofactors...
      // undo changes of lm
      p_ExpVectorAdd(lm, p2, tailRing);
      uint shift_mr = shift_of_p2 + p2->exp[tailRing->omap[0]];
      poly mr_shifted= PMLP_SHIFT(mr, shift_mr, strat, tailRing);
      P_DELETE(&mr, tailRing);
      p_ExpVectorAdd(lm, mr_shifted, tailRing);
      P_DELETE(&mr_shifted, tailRing);

      if (strat == NULL) return 2;
      if (! kStratChangeTailRing(strat, PR, SPW)) return -1;
      tailRing = strat->tailRing;
      p1 = PR->GetLmTailRing();
      p2 = SPW->GetLmTailRing();
      t2 = pNext(UPW->GetLmTailRing());
      lm = p1;
      SD::get_division_cofactors
        (lm, p2, shift_of_p2, &lm, &mr, tailRing);
      ret = 1;
    }
  }

  // take care of coef buisness
  if (! n_IsOne(pGetCoeff(p2), tailRing))
  {
    number bn = pGetCoeff(lm);
    number an = pGetCoeff(p2);
    int ct = ksCheckCoeff(&an, &bn, tailRing->cf);    // Calculate special LC
    p_SetCoeff(lm, bn, tailRing);
    if ((ct == 0) || (ct == 2))
      PR->Tail_Mult_nn(an);
    if (coef != NULL) *coef = an;
    else n_Delete(&an, tailRing);
  }
 else
  {
    if (coef != NULL) *coef = n_Init(1, tailRing);
  }


  // and finally 
  // BOCO: TODO: Do we have to say UPW->GetLength() ?
  // (so what does GetLength() return...)
  // BOCO: TODO Memory: Which polys have to deleted afterwards
  // or before? head of t2? mr? else ? ... care for that!!!
#if 0 //BOCO: replaced because of two sided multiplication for LP
  PR->Tail_Minus_mm_Mult_qq
    (lm, t2, SPW->GetpLength() - 1, spNoether);
#else
  //BOCO: the following could lead to unwanted side effects, but
  //I want to get rid of dPolyErrors p_LmTest in debug version
  assume( p_SetCoeff(mr, nInit(1), tailRing) );
  PR->Tail_Minus_mml_Mult_qq_Mult_mmr
    (lm, t2, mr, SPW->GetpLength() - 1, spNoether);
#endif
  assume(SPW->GetpLength() == pLength(SPW->p != NULL ? SPW->p : SPW->t_p));
  PR->LmDeleteAndIter();

  // the following is commented out: shrinking
#ifdef HAVE_SHIFTBBA_NONEXISTENT
  if ( (currRing->isLPring) && (!strat->homog) )
  {
    // assume? h->p in currRing
    PR->GetP();
    poly qq = p_Shrink(PR->p, currRing->isLPring, currRing);
    PR->Clear(); // does the right things
    PR->p = qq;
    PR->t_p = NULL;
    PR->SetShortExpVector();
  }
#endif

#if defined(KDEBUG) && defined(TEST_OPT_DEBUG_RED)
  if (TEST_OPT_DEBUG)
  {
    Print(" to: "); PR->wrp(); Print("\n");
  }
#endif
  deBoGriPrint("", 4096); deBoGriPrint("Reduced Poly: ", 4096); 
  deBoGriPrint
    (PR->t_p, "PR->t_p: ", 4096, tailRing, tailRing);
  return ret;
}

/** Creation of the S-Polynomial ShiftDVec Letterplace Version
 *
 *  There are S-Polynomials everywhere! They fly around, pierce
 *  through the matter of our world and disappear in the infinite
 *  silence of dark and untidy areas beneath our workstations.
 */
void ShiftDVec::ksCreateSpoly( LObject* Pair, poly spNoether,
                               int use_buckets,ring tailRing,
                               poly m1, poly m2,
                               TObject** R, kStrategy strat )
{
  kTest_L(Pair);
  poly p1 = Pair->p1;
  poly p2 = Pair->p2;
  Pair->tailRing = tailRing;

  assume(p1 != NULL);
  assume(p2 != NULL);
  assume(tailRing != NULL);

  poly a1 = pNext(p1), a2 = pNext(p2);
  number lc1 = pGetCoeff(p1), lc2 = pGetCoeff(p2);
  int co=0, ct = ksCheckCoeff(&lc1, &lc2, currRing->cf); // gcd and zero divisors

  int l1=0, l2=0;

  if (p_GetComp(p1, currRing)!=p_GetComp(p2, currRing))
  {
    if (p_GetComp(p1, currRing)==0)
    {
      co=1;
      p_SetCompP(p1,p_GetComp(p2, currRing), currRing, tailRing);
    }
    else
    {
      co=2;
      p_SetCompP(p2, p_GetComp(p1, currRing), currRing, tailRing);
    }
  }

  if (m1 == NULL)
    SD::k_GetLeadTerms( p1, p2,
                        Pair->SD_LExt()->shift_p2,
                        currRing, m1, m2, tailRing, strat );

  pSetCoeff0(m1, lc2);
  pSetCoeff0(m2, lc1);  // and now, m1 * LT(p1) == m2 * LT(p2)

  if (R != NULL)
  {
    if (Pair->i_r1 == -1)
    {
      l1 = pLength(p1) - 1;
    }
    else
    {
      l1 = (R[Pair->i_r1])->GetpLength() - 1;
    }
    if (Pair->i_r2 == -1)
    {
      l2 = pLength(p2) - 1;
    }
    else
    {
      l2 = (R[Pair->i_r2])->GetpLength() - 1;
    }
  }

  // get m2 * a2
  if (spNoether != NULL)
  {
    l1 = -1;
    a1 = tailRing->p_Procs->pp_Mult_mm_Noether( a1, m1,
                                                spNoether,
                                                l1, tailRing );
    assume(l1 == pLength(a1));
  }
  else a1 = tailRing->p_Procs->pp_Mult_mm( a1, m1, tailRing );

#ifdef HAVE_RINGS
  if (!(rField_is_Domain(currRing))) l1 = pLength(a1);
#endif

  // TODO: adding m1 to a1:
  // Why is that done, is seems to be unnecessary?
  Pair->SetLmTail(m1, a1, l1, use_buckets, tailRing);

  // get a1*m1 - m2*a2
  Pair->Tail_Minus_mm_Mult_qq(m2, a2, l2, spNoether);

  // Clean-up time
  Pair->LmDeleteAndIter();
  P_LMDELETE(m2, tailRing);

  if (co != 0)
  {
    if (co==1)
    {
      p_SetCompP(p1,0, currRing, tailRing);
    }
    else
    {
      p_SetCompP(p2,0, currRing, tailRing);
    }
  }
}

//BOCO: used in redtail
int ShiftDVec::ksReducePolyTail
  ( LObject* PR, TObject* UPW, TObject* SPW, 
    poly Current, poly spNoether, SD::kStrategy strat )
{
  BOOLEAN ret;
  number coef;
  poly Lp =     PR->GetLmCurrRing();
  poly Save =   SPW->GetLmCurrRing();

  kTest_L(PR);
  kTest_T(UPW);
  pAssume(pIsMonomOf(Lp, Current));

  assume( Lp != NULL &&
          Current != NULL &&
          pNext(Current) != NULL );

  assume(PR->bucket == NULL);

  LObject Red(pNext(Current), PR->tailRing);
  TObject SWith(SPW, Lp == Save);
  TObject UWith(UPW, Lp == Save);

  pAssume(!pHaveCommonMonoms(Red.p, SWith.p));
  ret = SD::ksReducePoly
    (&Red, &UWith, &SWith, spNoether, &coef, strat);

  if (!ret)
  {
    if (! n_IsOne(coef, currRing))
    {
      pNext(Current) = NULL;
      if (Current == PR->p && PR->t_p != NULL)
        pNext(PR->t_p) = NULL;
      PR->Mult_nn(coef);
    }

    n_Delete(&coef, currRing);
    pNext(Current) = Red.GetLmTailRing();
    if (Current == PR->p && PR->t_p != NULL)
      pNext(PR->t_p) = pNext(Current);
  }
  if (Lp == Save)  //BOCO TODO: Do we miss braces here ???
   SWith.Delete();
   UWith.Delete();

  return ret;
}

//BOCO: used in redtailBba (kutil.cc)
int ShiftDVec::ksReducePolyTail
  ( LObject* PR, TObject* UPW, TObject* SPW, LObject* Red )
{
  BOOLEAN ret;
  number coef;


  assume(PR->GetLmCurrRing() != UPW->GetLmCurrRing());
  Red->HeadNormalize();
#if 0 //original
  ret = ksReducePoly(Red, PW, NULL, &coef);
#else //replacement
  ret = 
    SD::ksReducePoly(Red, UPW, SPW, NULL, &coef);
#endif

  if (!ret)
  {
    if (! n_IsOne(coef, currRing->cf))
    {
      PR->Mult_nn(coef);
      // HANNES: mark for Normalize
    }
    n_Delete(&coef, currRing->cf);
  }
  return ret;
}


/* Part 3: others...                                          */


/** @brief Some debug check concerning creation of S-Polynomials,
 *         I guess.
 *
 *  There was nearly no change to it in ShiftDVec Letterplace.
 */
BOOLEAN ShiftDVec::kCheckSpolyCreation
  ( LObject *L, SD::kStrategy strat, poly &m1, poly &m2 )
{
  if (strat->overflow) return FALSE;
  assume(L->p1 != NULL && L->p2 != NULL);
  // shift changes: from 0 to -1
  assume(L->i_r1 >= -1 && L->i_r1 <= strat->tl);
  assume(L->i_r2 >= -1 && L->i_r2 <= strat->tl);
  assume(strat->tailRing != currRing);

  //BOCO: only Change: using our k_GetLeadTerms adaption
  if(
  !SD::k_GetLeadTerms( L->p1, L->p2,
                       L->SD_LExt()->shift_p2,
                       currRing, m1, m2, strat->tailRing, strat )
  ) return FALSE;

  // shift changes: extra case inserted
  if ((L->i_r1 == -1) || (L->i_r2 == -1) )
  {
    return TRUE;
  }
  poly p1_max = (strat->R[L->i_r1])->max;
  poly p2_max = (strat->R[L->i_r2])->max;

  if ((p1_max != NULL && !p_LmExpVectorAddIsOk(m1, p1_max, strat->tailRing)) ||
      (p2_max != NULL && !p_LmExpVectorAddIsOk(m2, p2_max, strat->tailRing)))
  {
    P_LMFREE(m1, strat->tailRing);
    P_LMFREE(m2, strat->tailRing);
    m1 = NULL;
    m2 = NULL;
    return FALSE;
  }
  return TRUE;
}


/** @brief get the monomials which one has to multiply to the
 *         tail terms of the S-polynomial to build it
 *
 * This calculates (except for m1 being not shifted, see remark):
 *   - m1 = LCM(LM(p1), LM(shift*p2))/LM(p1) and
 *   - m2 = LCM(LM(p1), LM(shift*p2))/LM(shift*p2)
 *
 * \remark m1 and m2 will always be unshifted polynomials,
 *         although m1 would theoretically be shifted. This is
 *         due to the implementation of our improved
 *         multiplication.
 *
 * \todo explain parameters and maybe improve explanation from
 *       above
 */
BOOLEAN ShiftDVec::k_GetLeadTerms
  ( const poly p1,
    const poly p2, uint shift_p2, const ring p_r,
    poly &m1, poly &m2, const ring m_r, SD::kStrategy strat )
{
  int lV = strat->lV;

  p_LmCheckPolyRing(p1, p_r);
  p_LmCheckPolyRing(p2, p_r);

  long x;
  m1 = P_INIT(m_r);
  m2 = P_INIT(m_r);

  //BOCO: small warning: polys should correspond to a 
  //non-center overlap - otherwise the loops may not complete

  long j = 1;
  long k = 1;
  long exp_in_blocks_before;

  // get m1 by looping over p1 for shift blocks
  for( uint block = 0; block < shift_p2; ++block )
  {
    for(long i = 0; i < lV; ++i, ++j)
      if( (x = p_GetExp(p1, j, p_r)) > 0 )
      {
        if (x > (long) m_r->bitmask) goto false_return;
        p_SetExp(m2, j, 1, m_r);
        j = j + (lV - (j-1) % lV); //We found a nonzero exponent,
        break; //thus we can move to the next block
      }
  }

  // skip part where p1 and p2 have the overlap, d.i. the part
  // where p1 and shift*p2 have their common divisor
  while( p_GetExp(p1, j, p_r) == p_GetExp(p2, k, p_r) ){++j;++k;}

  exp_in_blocks_before = (((k-1) / lV) * lV);

  while( k < p_r->N )
  {
    for(long i = 0; i < lV; ++k, ++i)
      if( (x = p_GetExp(p2, k, p_r)) > 0 )
      {
        if (x > (long) m_r->bitmask) goto false_return;
        p_SetExp(m1, k-exp_in_blocks_before, 1, m_r);
        k = k + (lV - (k-1) % lV);// We found a nonzero exponent,
        break; //thus we can move to the next block
      }
  }

  p_Setm(m1, m_r);
  p_Setm(m2, m_r);
  return TRUE;

  false_return:
  P_LMFREE(m1, m_r);
  P_LMFREE(m2, m_r);
  m1 = m2 = NULL;
  return FALSE;
}

/** creates the leading term of the S-polynomial of p1 and p2
 *
 * does not destroy p1 and p2
 *
 * this is version for the shift free letterplace implementation
 * (d.i. p1 and p2 are not shifted, but the shift for p2 is
 *  supplied in shift_p2)
 *
 * \remarks
 *   1. the coefficient is 0 (nNew)
 *   2. in the case of coefficient ring, the coefficient is
 *      calculated
 *   3. pNext is undefined
 *   4. for the letterplace version I removed the
 *      #ifdef HAVE_RINGS parts everywhere
 *   5. this function could be implemented to be more efficient
 *
 *  \todo This function is too long and contains gotos leaping
 *        over many lines, making it difficult to figure out the
 *        control flow; with other words: it's confusing for
 *        everyone who is not the author. To my humble mind, this
 *        situation should be improved. But not now and not by
 *        me, I'm tired of it... -> this function should be split
 *        into subparts
 *  \todo it should be explained for future readers of the source
 *        code of this function, what m1 and m2 are
 */
poly ShiftDVec::ksCreateShortSpoly( poly p1, poly p2,
                                    uint shift_p2,
                                    int lV, ring tailRing )
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  long c1=p_GetComp(p1, currRing),c2=p_GetComp(p2, currRing);
  long c;
  poly m1,m2;
  number t1 = NULL,t2 = NULL;
  int cm,i;
  BOOLEAN equal;
  long sh_off = lV * shift_p2;
  long p1_off = lV * p_Totaldegree(p1, currRing);

  if (a1==NULL)
  {
    if(a2!=NULL)
    {
      m2=P_INIT(currRing);
x2:
      // copy first part (of size sh_off) of p1 to m2
      // and append a2
      p_lmCopy( p1, currRing, m2, currRing, lV, sh_off );
      p_lmAppend( m2, sh_off, currRing, a2, 0, tailRing, lV );
      if ((c1==c2)||(c2!=0))
      {
        p_SetComp(m2,p_GetComp(a2,tailRing), currRing);
      }
      else
      {
        p_SetComp(m2,c1,currRing);
      }
      p_Setm(m2, currRing);
        nNew(&(pGetCoeff(m2)));
      return m2;
    }
    else
    {
      return NULL;
    }
  }
  if (a2==NULL)
  {
    m1=P_INIT(currRing);
x1:
    long a1_off = p_lmCopy( a1, tailRing,
                            m1, currRing, lV ) * lV;
    p_lmAppend( m1, a1_off, currRing,
                p2, p1_off-sh_off, currRing, lV );

    if ((c1==c2)||(c1!=0))
         p_SetComp(m1,p_GetComp(a1,tailRing),currRing);
    else p_SetComp(m1,c2,currRing);

    p_Setm(m1, currRing);
    nNew(&(pGetCoeff(m1)));
    return m1;
  }
  loop
  {
    m1 = P_INIT(currRing);
    m2 = P_INIT(currRing);
    long a1_off = p_lmCopy( a1, tailRing,
                            m1, currRing, lV );
    a1_off *= lV; // in letterplace we have to 'shift' by deg(a1)
    // mult. m1 by the correct and correctly shifted part of p2
    p_lmAppend( m1, a1_off, currRing,
                p2, p1_off-sh_off, currRing, lV );

    // copy first part (of size sh_off) of p1 to m2 and append a2
    p_lmCopy( p1, currRing, m2, currRing, lV, sh_off );
    p_lmAppend( m2, sh_off, currRing, a2, 0, tailRing, lV );

    if(c1==c2)
    {
      p_SetComp(m1,p_GetComp(a1, tailRing), currRing);
      p_SetComp(m2,p_GetComp(a2, tailRing), currRing);
    }
    else
    {
      if(c1!=0)
      {
        p_SetComp(m1,p_GetComp(a1, tailRing), currRing);
        p_SetComp(m2,c1, currRing);
      }
      else
      {
        p_SetComp(m2,p_GetComp(a2, tailRing), currRing);
        p_SetComp(m1,c2, currRing);
      }
    }
    p_Setm(m1,currRing);
    p_Setm(m2,currRing);
    cm = p_LmCmp(m1, m2,currRing);
    if (cm!=0)
    {
      if(cm==1)
      {
        P_LMFREE(m2,currRing);
          nNew(&(pGetCoeff(m1)));
        return m1;
      }
      else
      {
        P_LMFREE(m1,currRing);
          nNew(&(pGetCoeff(m2)));
        return m2;
      }
    }
    {
      t1 = nMult(pGetCoeff(a2),pGetCoeff(p1));
      t2 = nMult(pGetCoeff(a1),pGetCoeff(p2));
      equal = nEqual(t1,t2);
      nDelete(&t2);
      nDelete(&t1);
    }
    if (!equal)
    {
      P_LMFREE(m2,currRing);
        nNew(&(pGetCoeff(m1)));
      return m1;
    }
    pIter(a1);
    pIter(a2);
    if (a2==NULL)
    {
      P_LMFREE(m2,currRing);
      if (a1==NULL)
      {
        P_LMFREE(m1,currRing);
        return NULL;
      }
      goto x1;
    }
    if (a1==NULL)
    {
      P_LMFREE(m1,currRing);
      goto x2;
    }
  }
}

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
