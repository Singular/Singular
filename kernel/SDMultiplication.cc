//This file contains the implementations for monomial/polynomial
//multiplications for letterplace.
//
//WARNING/TODO:
//I do not know, if the ideas in this file will always work. I
//do not yet know, if it was right to assume, that we always use
//the ..._Mult_...__T functions. Are there other Kandidates for
//r->p_Procs->..._Mult_... pointers???
//
//TODO PRIORITY TODO:
//review following functions:
//
//SetShortExpVector:  
//Kommt vor in Sektionen mit HAVE_SHIFTBBA_NONEXISTENT, also
//nicht kritisch...
//
//p_ExpVectorSub
//kommt in ksReducePoly vor und wird auf geshiftete Polynome
//angewendet, also sollte es funktionieren
//
//p_LmExpVectorAddIsOk
//kommt in ksReducePoly vor
//Stellen die das benutzen koennen wahrscheinlich ausgeklammert
//werden (hoffe ich), da wir uns um den exp bound keine Sorgen
//machen muessen. (Unsere Exponenten haben hoechstens Groesse 1.)
//
//p_ExpVectorAdd
//Benutzt an der auskommentierten Stelle von
//p_LmExpVectorAddIsOk - siehe Kommentar dazu
//
//p_GetExpDiff
//
//If some of them need adjustment: Ask Grischa to look them up;
//where in the bba are there used?
//
//TODO:
// · Write a decent Header for this file
//
// · Doublecheck Part 2 - (This should be done before debugging, 
//   but I know myself to well.)
// · Doublecheck Part 3
// · Create Part 4, in which old friends meet and we begin to
//   adjust all functions in the letterplace bba, which use
//   functions from Part 3
// · Part 4 seems to be created, now doublecheck it
//
// · Include this work into the right functions and invoke
//   InitSDMultiplication at the right place(s). Do not forget
//   to free r->omap .
// · adjust header files, (namespace) declarations, etc.
// · Test, if it works (as always I'm not too optimistic)
//
// · Optimization and Cleanup (This is the neverending Story)
// · Optimization Note: Shifts are yet very inefficient, we
//   could improve that; we may also be able to reduce the
//   number of shifts, or to improve our algorithms in such a
//   way, that they do not need to shift direktly, but
//   implicitly, which could be better with regard to
//   performance
// · Think about creating inlines
//
// · All this will never work...
// · Pray
//
//Now following: The lists from Grischa
/*
 Occurences of multiplication
 
 
 kspoly.cc:
 
 procedure          mult-command             	line
 
 ksReducePoly          
                    Tail_Minus_mm_Mult_qq     	139
                    
 ksReducePolySig 
                    pp_Mult_qq     		236
                    Tail_Minus_mm_Mult_qq       346
                    
 ksCreateSpoly
                    pp_Mult_mm_Noether	        448
                    pp_Mult_mm		        452
                    Tail_Minus_mm_Mult_qq       460
                    
 ksReducePolyTail   
                    uses ksReducePoly	      	512
                    Mult_nn			521
                    
 ksCreateShortSpoly
		    nMult			582, 583, 588, 594, 773, 774, 804, 809, 817, 826,
*/
/*
call in shiftDVec.cc of the procedures above:

procedure 			line

ksCreateSpoly			461(bba)

ksReducePoly			1167(redtail)
				1416(redHomog)
				1427(redHomog)
				1466(redHomog)
				1700(redBba)
				2421(redtailBba)
				
ksReducePolyTail		2421(redtailBba)
				1167(redtail)
*/



/* Part 1: General Tools - especially for dp case and the like */



/* Creates a Mapping: i -> pos, where i is an index of a       *
 * variable in a block of the letterplace polynomial and pos   *
 * the position of the order Field for the block. May not yet  *
 * work for other orderings than dp. The Mapping is stored to  *
 * r->omap, the size of the mapping to r->osize. omap[0] has a *
 * special meaning: it is the glocal order Field, thus the     *
 * totaldegree in the dp-Case.                                 * 
 * TODO: This has to be adapted to other orderings. And the    *
 * comment should be rewritten to be more understandable and   *
 * more accurate.                                              */
int ShiftDVec::InitOrderMapping( ring r )
{
  r->omap = (int *) omAlloc( (r->N+1) * sizeof(int) );

  for(int i = 1; i < r->OrdSize; ++i)
  {
    sro_ord* o=&(r->typ[i]);
    assume(o->ord_typ == ro_dp); //see comment above
    int a,e;
    a=o->data.dp.start;
    e=o->data.dp.end;
    for(int i=a;i<=e;i++) (r->omap)[i] = o->data.dp.place;
  }

  (r->omap)[0] = currRing->pOrdIndex;

  //BOCO TODO: This is a bit redundant...
  return r->osize = r->N+1;
}

/* Initializes the letterplace multiplication. See also        *
 * InitOrderMapping. Do not forget to free r->omap as soon, as *
 * it will no longer be used.                                  */
void ShiftDVec::InitSDMultiplication( ring r )
{
  for(int i = 1; i < r->OrdSize; ++i)
  {
    if( (r->typ[i]) != ro_dp )
    {
      r->p_ExpSum = &ShiftDVec::p_ExpSum_slow;
      return;
    }
  }

  r->p_ExpSum = &ShiftDVec::p_ExpSum_dp;
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



/* Part 2: replacements for p_MemSum__T .                      */



/* BOCO: This is our letterplace replacement for p_MemSum__T.  *
 * This function will return rt->exp = p->exp + s * q->exp,    *
 * where s is a shift in size of the number of exponents equal *
 * to on in p->exp . rt has to be allocated (for example with  *
 * p_AllocBin?).                                               */
void ShiftDVec::p_ExpSum_slow
  (poly rt, poly p, poly q, ring r, int lV)
{
  p_MemCopy_LengthGeneral(rt->exp, p->exp, r->ExpL_Size);

  //This represents the first index in the currently considered
  //block in rt->exp.
  long index_rt = p_Totaldeg(p, r) * lV + 1;

  long index_q = 1;
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q->exp, index_q+i, r) )
      {
        p_SetExp(rt->exp, index_rt+i, 1, r);
        index_rt += lV;
        if(index_rt > r->N) break; //looped through all vars
        index_q += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
  }

  p_Setm(rt,r);  //TODO: Maybe this is not yet nescessary
  return;
}

/* BOCO: This is our letterplace replacement for p_MemSum__T,  *
 * in case we have a dp-ordering on our blocks.                *
 * This function will return rt->exp = p->exp + s * q->exp,    *
 * where s is a shift in size of the number of exponents equal *
 * to on in p->exp . rt has to be allocated (for example with  *
 * p_AllocBin?).                                               */
void ShiftDVec::p_ExpSum_dp
  (poly rt, poly p, poly q, ring r, int lV)
{
  /* Du Gedicht                                              *
   *                                                         *
   * Wie gabst du in Wenigem,                                *
   * mir dich meiner Seele hin,                              *
   * gleichsam einem tiefen Fluss,                           *
   * welcher dunkel sich ergiesst.                           * 
   *                                                         *
   * Und in seinem Wasserbette,                              *
   * hunderte an einer Kette,                                *
   * Perlenkiese in sich traegt,                             *
   * ihren sich'ren Wert erwaegt.                            *
   *                                                         *
   * Ueberschaubar war die Laenge,                           *
   * dennoch sprachst du eine Menge                          *
   * kleiner Universen aus.                                  *
   *                                                         *
   * Und in diesem Wortgedraenge                             *
   * und mit glitzerndem Gehaenge                            *
   * kamst du furchtbar hoch hinaus.                         * 
   *                                                         *
   * Bo                                                      */
  p_MemCopy_LengthGeneral(rt->exp, p->exp, r->ExpL_Size);

  //This represents the first index in the currently considered
  //block in rt->exp.
  //long index_rt = p_Totaldeg(p, r) * lV + 1;
  long index_rt = p->exp[omap[0]] * lV + 1;

  long index_q = 1;
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q->exp, index_q+i, r) )
      {
        rt->exp[omap[index_rt+i]] = 1;
        p_SetExp(rt->exp, index_rt+i, 1, r);
        index_rt += lV;
        if(index_rt > r->N) break; //looped through all vars
        index_q += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
  }
  rt->exp[omap[0]] += q->exp[omap[0]];

  return;
}



/* Part 3: replacements of the multiplications functions       */



#if 0
/* BOCO: original comment and header from                      *
 * libpolys/polys/templates/pp_Mult_mm__T.cc                   */
/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    p, m
 *
 ***************************************************************/
LINKAGE poly pp_Mult_mm__T(poly p, const poly m, const ring ri, poly &last)
#else //BOCO: replacement
poly ShiftDVec::pp_Mult_mm__T
  (poly p, const poly m, const ring ri, poly &last, int lV)
#endif
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL)
  {
    last = NULL;      
    return NULL;
  }
  spolyrec rp;
#ifdef HAVE_ZERODIVISORS
  rp.next = NULL;
#endif
  poly q = &rp;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
#if 0 //BOCO: no longer needed
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
#endif
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero__T(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  number tmp;

  do
  {
    tmp = n_Mult__T(ln, pGetCoeff(p), ri);
#ifdef HAVE_ZERODIVISORS
    if (! n_IsZero__T(tmp, ri))
    {
#endif
      p_AllocBin( pNext(q), bin, ri);
      q = pNext(q);
      pSetCoeff0(q, tmp);

#if 0 //BOCO: original code
    p_MemSum__T(q->exp, p->exp, m_e, length);
#else //replacement
      ri->p_ExpSum(q, p, m, ri, lV);
#endif

      p_MemAddAdjust__T(q, ri);
#ifdef HAVE_ZERODIVISORS
    }
    else n_Delete__T(&tmp, ri);
#endif
    p = pNext(p);
  }
  while (p != NULL);
  last = q;
  pNext(q) = NULL;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}

#if 0
/* BOCO: original comment and header from                      *
 * libpolys/polys/templates/pp_Mult_mm_Noether__T.cc           */
/***************************************************************
 *
 *   Returns:  p*m, ll
 *   ll == pLength(p*m) , if on input ll < 0
 *   pLength(p) - pLength(p*m), if on input ll >= 0
 *   Const:    p, m
 *
 ***************************************************************/
LINKAGE poly pp_Mult_mm_Noether__T(poly p, const poly m, const poly spNoether, int &ll, const ring ri, poly &last)
#endif //BOCO: replacement
poly ShiftDVec::pp_Mult_mm_Noether__T
  ( poly p, const poly m, const poly spNoether, 
    int &ll, const ring ri, poly &last, int lV  )
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  assume(spNoether != NULL);
  if (p == NULL)
  {
    ll = 0;
    last = NULL;
    return NULL;
  }
  spolyrec rp;
  poly q = &rp, r;
  const unsigned long *spNoether_exp = spNoether->exp;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = ri->ordsgn);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero__T(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  int l = 0;

  do
  {
    p_AllocBin(r, bin, ri);

#if 0 //BOCO: original code
    p_MemSum__T(r->exp, p->exp, m_e, length);
#else //replacement
    ri->p_ExpSum(r, p, m, ri, lV);
#endif

    p_MemAddAdjust__T(r, ri);

    p_MemCmp__T(r->exp, spNoether_exp, length, ordsgn, goto Continue, goto Continue, goto Break);

    Break:
    p_FreeBinAddr(r, ri);
    break;

    Continue:
    l++;
    q = pNext(q) = r;
    pSetCoeff0(q, n_Mult__T(ln, pGetCoeff(p), ri));
    pIter(p);
  } while (p != NULL);

  if (ll < 0)
    ll = l;
  else
    ll = pLength(p);

  if (q != &rp)
    last = q;
  pNext(q) = NULL;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}

#if 0  
/* BOCO: original comment and header from                      *
 * libpolys/polys/templates/p_Minus_mm_Mult_qq__T.cc           */
/***************************************************************
 *
 * Returns:  p - m*q
 *           Shorter, where Shorter == Length(p) + Length(q) - Length(p - m*q);
 * Destroys: p
 * Const:    m, q
 *
 ***************************************************************/
LINKAGE poly p_Minus_mm_Mult_qq__T
  ( poly p, poly m, poly q, int& Shorter, 
    const poly spNoether, const ring r, poly &last )
#else
/* BOCO TODO: This function has yet to be written.             */
poly ShiftDVec::p_Minus_mm_Mult_qq__T
  ( poly p, poly m, poly q, int& Shorter, 
    const poly spNoether, const ring r, poly &last, int lV )
#endif
{
  p_Test(p, r);
  p_Test(q, r);
  p_LmTest(m, r);

#if PDEBUG > 0
  int l_debug = pLength(p) + pLength(q);
#endif

  Shorter = 0;
  // we are done if q == NULL || m == NULL
  if (q == NULL || m == NULL) return p;

  spolyrec rp;
  poly a = &rp,                    // collects the result
    qm = NULL;                     // stores q*m


  number tm = pGetCoeff(m),           // coefficient of m
    tneg = n_Neg__T(n_Copy__T(tm, r), r),    // - (coefficient of m)
    tb,                            // used for tm*coeff(a1)
    tc;                            // used as intermediate number


  int shorter = 0;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  const unsigned long* m_e = m->exp;
  omBin bin = r->PolyBin;

  if (p == NULL) goto Finish;           // return tneg*q if (p == NULL)

  pAssume(p_GetComp(q, r) == 0 || p_GetComp(m, r) == 0);

  AllocTop:
  p_AllocBin(qm, bin, r);

  SumTop:
#if 0 //BOCO: original code
  p_MemSum__T(qm->exp, q->exp, m_e, length);
#else //replacement
  r->p_ExpSum(qm, q, m, r, lV);
#endif
  p_MemAddAdjust__T(qm, r); //BOCO: Whatfor's this? Do we need it?

  CmpTop:
  // compare qm = m*q and p w.r.t. monomial ordering
  p_MemCmp__T(qm->exp, p->exp, length, ordsgn, goto Equal, goto Greater, goto Smaller );

  Equal:   // qm equals p
  tb = n_Mult__T(pGetCoeff(q), tm, r);
#ifdef HAVE_ZERODIVISORS
  if (!n_IsZero__T(tb,r)) {
#endif
  tc = pGetCoeff(p);
  if (!n_Equal__T(tc, tb, r))
  {
    shorter++;
    tc = n_Sub__T(tc, tb, r);
    n_Delete__T(&(pGetCoeff(p)), r);
    pSetCoeff0(p,tc); // adjust coeff of p
    a = pNext(a) = p; // append p to result and advance p
    pIter(p);
  }
  else
  { // coeffs are equal, so their difference is 0:
    shorter += 2;
    n_Delete__T(&tc, r);
    p = p_LmFreeAndNext(p, r);
  }
#ifdef HAVE_ZERODIVISORS
  }
  else
  { // coeff itself is zero
    shorter += 1;
  }
#endif
  n_Delete__T(&tb, r);
  pIter(q);
  if (q == NULL || p == NULL) goto Finish; // are we done ?
  // no, so update qm
  goto SumTop;


  Greater:
#ifdef HAVE_ZERODIVISORS
  tb = n_Mult__T(pGetCoeff(q), tneg, r);
  if (!n_IsZero__T(tb,r))
  {
#endif
    pSetCoeff0(qm, n_Mult__T(pGetCoeff(q), tneg, r));
    a = pNext(a) = qm;       // append qm to result and advance q
#ifdef HAVE_ZERODIVISORS
  }
  else
  {
    shorter++;
  }
  n_Delete__T(&tb, r);
#endif
  pIter(q);
  if (q == NULL) // are we done?
  {
    qm = NULL;
    goto Finish;
  }
  // construct new qm
  goto AllocTop;

  Smaller:
  a = pNext(a) = p;// append p to result and advance p
  pIter(p);
  if (p == NULL) goto Finish;
  goto CmpTop;


  Finish: // q or p is NULL: Clean-up time
  if (q == NULL) // append rest of p to result
  {
    pNext(a) = p;
    if (p == NULL) last = a;
  }
  else  // append (- m*q) to result
  {
    pSetCoeff0(m, tneg);
    last = a;
    if (spNoether != NULL) //BOCO: Don't know what this shall be
    {
      int ll = 0;
#if 0 //BOCO: original code
      pNext(a) = r->p_Procs->pp_Mult_mm_Noether(q, m, spNoether, ll, r, last);
#else //BOCO: replacement
      pNext(a) = 
        ShiftDVec::pp_Mult_mm_Noether__T
          (q, m, spNoether, ll, r, last, lV);
#endif
      shorter += ll;
    }
    else
    {
#if 0 //BOCO: original code
      pNext(a) = r->p_Procs->pp_Mult_mm(q, m, r, last);
#else //BOCO: replacement
      pNext(a) = ShiftDVec::pp_Mult_mm__T(q, m, r, last, lV);
#endif
#if 0 
//BOCO: We don't HAVE_RINGS (except for those under our eyes)
#ifdef HAVE_RINGS
      if (! rField_is_Domain(r))
      {
        shorter += pLength(q) - pLength(pNext(a));
      }
#endif
#endif
    }
    pSetCoeff0(m, tm);
  }

  n_Delete__T(&tneg, r);
  if (qm != NULL) p_FreeBinAddr(qm, r);
  Shorter = shorter;
  p_Test(pNext(&rp), r);
  return pNext(&rp);
}



/* Part 4: adjustment of the bba functions which make use of   *
 * one of the functions from Part 3                            */



// For debugging purposes we will snarf some variables from
// kspoly.cc . (additionally to all those hundreds of functions
// we already have stolen)
#ifdef KDEBUG
extern int red_count;
extern int create_count;
// define this if reductions are reported on TEST_OPT_DEBUG
#define TEST_OPT_DEBUG_RED
#endif

#if 0
/* BOCO: original comment and header from                      *
 * kernel/kspoly.cc                                            */
/***************************************************************
 *
 * Creates S-Poly of p1 and p2
 *
 *
 ***************************************************************/
void ksCreateSpoly(LObject* Pair,   poly spNoether,
                   int use_buckets, ring tailRing,
                   poly m1, poly m2, TObject** R)
#else //BOCO: replacement
void ShiftDVec::ksCreateSpoly
  ( LObject* Pair, poly spNoether, int use_buckets, 
    ring tailRing, poly m1, poly m2,  TObject** R, int lV )
#endif
{
#ifdef KDEBUG
  create_count++;
#endif
  kTest_L(Pair);
  poly p1 = Pair->p1;
  poly p2 = Pair->p2;
  poly last;
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

  // get m1 = LCM(LM(p1), LM(p2))/LM(p1)
  //     m2 = LCM(LM(p1), LM(p2))/LM(p2)
  if (m1 == NULL)
    k_GetLeadTerms(p1, p2, currRing, m1, m2, tailRing);

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
    l2 = -1;
#if 0 //BOCO: replaced
    a2 = tailRing->p_Procs->pp_Mult_mm_Noether(a2, m2, spNoether, l2, tailRing,last);
#else //replacement
    a2 = 
      ShiftDVec::pp_Mult_mm_Noether__T
        (a2, m2, spNoether, l2, tailRing, last, lV);
#endif
    assume(l2 == pLength(a2));
  }
  else
#if 0 //BOCO: replaced
    a2 = tailRing->p_Procs->pp_Mult_mm(a2, m2, tailRing,last);
#else //replacement
    a2 = ShiftDVec::pp_Mult_mm__T(a2, m2, tailRing, last, lV);
#endif
#ifdef HAVE_RINGS
  if (!(rField_is_Domain(currRing))) l2 = pLength(a2);
#endif

  Pair->SetLmTail(m2, a2, l2, use_buckets, tailRing, last);

  // get m2*a2 - m1*a1
#if 0 //BOCO: replaced
  Pair->Tail_Minus_mm_Mult_qq(m1, a1, l1, spNoether);
#else //replacement
  //defined in kInline.h
  Pair->Tail_Minus_mm_Mult_qq(m1, a1, l1, spNoether, lV);
#endif

  // Clean-up time
  Pair->LmDeleteAndIter();
  p_LmDelete(m1, tailRing);

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

  // the following is commented out: shrinking
#ifdef HAVE_SHIFTBBA_NONEXISTENT
  if (currRing->isLPring)
  {
    // assume? h->p in currRing
    Pair->GetP();
    poly qq = p_Shrink(Pair->p, currRing->isLPring, currRing);
    Pair->Clear(); // does the right things
    Pair->p = qq; 
    Pair->t_p = NULL;
    Pair->SetShortExpVector();
  }
#endif

}

#if 0
/* BOCO: original comment and header from                      *
 * kernel/kspoly.cc                                            */
/***************************************************************
 *
 * Reduces PR with PW
 * Assumes PR != NULL, PW != NULL, Lm(PW) divides Lm(PR)
 *
 ***************************************************************/
int ksReducePoly(LObject* PR,
                 TObject* PW,
                 poly spNoether,
                 number *coef,
                 kStrategy strat)
#else
int ShiftDVec::ksReducePoly
  ( LObject* PR, TObject* PW, 
    poly spNoether, number *coef, kStrategy strat )
#endif
{
#ifdef KDEBUG
  red_count++;
#ifdef TEST_OPT_DEBUG_RED
  if (TEST_OPT_DEBUG)
  {
    Print("Red %d:", red_count); PR->wrp(); Print(" with:");
    PW->wrp();
  }
#endif
#endif
  int ret = 0;
  ring tailRing = PR->tailRing;
  kTest_L(PR);
  kTest_T(PW);

  poly p1 = PR->GetLmTailRing();   // p2 | p1
  poly p2 = PW->GetLmTailRing();   // i.e. will reduce p1 with p2; lm = LT(p1) / LM(p2)
  poly t2 = pNext(p2), lm = p1;    // t2 = p2 - LT(p2); really compute P = LC(p2)*p1 - LT(p1)/LM(p2)*p2
  assume(p1 != NULL && p2 != NULL);// Attention, we have rings and there LC(p2) and LC(p1) are special
  p_CheckPolyRing(p1, tailRing);
  p_CheckPolyRing(p2, tailRing);

  pAssume1(p2 != NULL && p1 != NULL &&
           p_DivisibleBy(p2,  p1, tailRing));

  pAssume1(p_GetComp(p1, tailRing) == p_GetComp(p2, tailRing) ||
           (p_GetComp(p2, tailRing) == 0 &&
            p_MaxComp(pNext(p2),tailRing) == 0));

#ifdef HAVE_PLURAL
  assume(0); //BOCO: we don't HAVE_PLURAL
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

  p_ExpVectorSub(lm, p2, tailRing); // Calculate the Monomial we must multiply to p2

  if (tailRing != currRing)
  {
#if 0 
  /* BOCO: exp bound should never be violatet, since our
   * exponents can be 1 at maximum. Hence i hope it ise ok to
   * comment out this piece of code (otherwise i'm not sure, if
   * it will work with our multiplication adapted, shrink
   * cleaned letterplace version.)
   */
    // check that reduction does not violate exp bound
    while (PW->max != NULL && !p_LmExpVectorAddIsOk(lm, PW->max, tailRing))
    {
      // undo changes of lm
      p_ExpVectorAdd(lm, p2, tailRing);
      if (strat == NULL) return 2;
      if (! kStratChangeTailRing(strat, PR, PW)) return -1;
      tailRing = strat->tailRing;
      p1 = PR->GetLmTailRing();
      p2 = PW->GetLmTailRing();
      t2 = pNext(p2);
      lm = p1;
      p_ExpVectorSub(lm, p2, tailRing);
      ret = 1;
    }
#endif
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


  // and finally,
#if 0 //BOCO: replaced
  PR->Tail_Minus_mm_Mult_qq(lm, t2, PW->GetpLength() - 1, spNoether);
#else //replacement
  //defined in kInline.h
  PR->Tail_Minus_mm_Mult_qq
    (lm, t2, PW->GetpLength() - 1, spNoether, strat->lV);
#endif
  assume(PW->GetpLength() == pLength(PW->p != NULL ? PW->p : PW->t_p));
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
  return ret;
}

#if 0 //BOCO: original header from kspoly.cc
int ksReducePolyTail(LObject* PR, TObject* PW, poly Current, poly spNoether)
#else //replacement
int ShiftDVec::ksReducePolyTail
  (LObject* PR, TObject* PW, poly Current, poly spNoether, int lV)
#endif
{
  BOOLEAN ret;
  number coef;
  poly Lp =     PR->GetLmCurrRing();
  poly Save =   PW->GetLmCurrRing();

  kTest_L(PR);
  kTest_T(PW);
  pAssume(pIsMonomOf(Lp, Current));

  assume(Lp != NULL && Current != NULL && pNext(Current) != NULL);
  assume(PR->bucket == NULL);

  LObject Red(pNext(Current), PR->tailRing);
  TObject With(PW, Lp == Save);

  pAssume(!pHaveCommonMonoms(Red.p, With.p));
  ret = 
    ShiftDVec::ksReducePoly(&Red, &With, spNoether, &coef, lV);

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

  if (Lp == Save)
    With.Delete();

  // the following is commented out: shrinking
#ifdef HAVE_SHIFTBBA_NONEXISTENT
  if (currRing->isLPring)
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

  return ret;
}

#if 0 //BOCO: original header from kInline.h
KINLINE int ksReducePolyTail(LObject* PR, TObject* PW, LObject* Red)
#else //replacement
int ShiftDVec::ksReducePolyTail
  ( LObject* PR, TObject* PW, LObject* Red, int lV )
#endif
{
  BOOLEAN ret;
  number coef;

  assume(PR->GetLmCurrRing() != PW->GetLmCurrRing());
  Red->HeadNormalize();
  ret = ShiftDVec::ksReducePoly(Red, PW, NULL, &coef, lV);

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

#if 0 //BOCO: original header and comment from kspoly.cc
/*2
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is 0 (nNew)
*   1. a) in the case of coefficient ring, the coefficient is calculated
*   2. pNext is undefined
*/
//static void bbb() { int i=0; }
poly ksCreateShortSpoly(poly p1, poly p2, ring tailRing)
#else //replacement
poly ShiftDVec::ksCreateShortSpoly
  ( poly p1, poly p2, ring tailRing )
#endif
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  long c1=p_GetComp(p1, currRing),c2=p_GetComp(p2, currRing);
  long c;
  poly m1,m2;
  number t1 = NULL,t2 = NULL;
  int cm,i;
  BOOLEAN equal;

#ifdef HAVE_RINGS
  BOOLEAN is_Ring=rField_is_Ring(currRing);
  number lc1 = pGetCoeff(p1), lc2 = pGetCoeff(p2);
  if (is_Ring)
  {
    ksCheckCoeff(&lc1, &lc2, currRing->cf); // gcd and zero divisors
    if (a1 != NULL) t2 = nMult(pGetCoeff(a1),lc2);
    if (a2 != NULL) t1 = nMult(pGetCoeff(a2),lc1);
    while (a1 != NULL && nIsZero(t2))
    {
      pIter(a1);
      nDelete(&t2);
      if (a1 != NULL) t2 = nMult(pGetCoeff(a1),lc2);
    }
    while (a2 != NULL && nIsZero(t1))
    {
      pIter(a2);
      nDelete(&t1);
      if (a2 != NULL) t1 = nMult(pGetCoeff(a2),lc1);
    }
  }
#endif

  if (a1==NULL)
  {
    if(a2!=NULL)
    {
      m2=p_Init(currRing);
x2:
      for (i = (currRing->N); i; i--)
      {
        c = p_GetExpDiff(p1, p2,i, currRing);
        if (c>0)
        {
          p_SetExp(m2,i,(c+p_GetExp(a2,i,tailRing)),currRing);
        }
        else
        {
          p_SetExp(m2,i,p_GetExp(a2,i,tailRing),currRing);
        }
      }
      if ((c1==c2)||(c2!=0))
      {
        p_SetComp(m2,p_GetComp(a2,tailRing), currRing);
      }
      else
      {
        p_SetComp(m2,c1,currRing);
      }
      p_Setm(m2, currRing);
#ifdef HAVE_RINGS
      if (is_Ring)
      {
          nDelete(&lc1);
          nDelete(&lc2);
          nDelete(&t2);
          pSetCoeff0(m2, t1);
      }
      else
#endif
        nNew(&(pGetCoeff(m2)));
      return m2;
    }
    else
    {
#ifdef HAVE_RINGS
      if (is_Ring)
      {
        nDelete(&lc1);
        nDelete(&lc2);
        nDelete(&t1);
        nDelete(&t2);
      }
#endif
      return NULL;
    }
  }
  if (a2==NULL)
  {
    m1=p_Init(currRing);
x1:
    for (i = (currRing->N); i; i--)
    {
      c = p_GetExpDiff(p2, p1,i,currRing);
      if (c>0)
      {
        p_SetExp(m1,i,(c+p_GetExp(a1,i, tailRing)),currRing);
      }
      else
      {
        p_SetExp(m1,i,p_GetExp(a1,i, tailRing), currRing);
      }
    }
    if ((c1==c2)||(c1!=0))
    {
      p_SetComp(m1,p_GetComp(a1,tailRing),currRing);
    }
    else
    {
      p_SetComp(m1,c2,currRing);
    }
    p_Setm(m1, currRing);
#ifdef HAVE_RINGS
    if (is_Ring)
    {
      pSetCoeff0(m1, t2);
      nDelete(&lc1);
      nDelete(&lc2);
      nDelete(&t1);
    }
    else
#endif
      nNew(&(pGetCoeff(m1)));
    return m1;
  }
  m1 = p_Init(currRing);
  m2 = p_Init(currRing);
  loop
  {
    for (i = (currRing->N); i; i--)
    {
      c = p_GetExpDiff(p1, p2,i,currRing);
      if (c > 0)
      {
        p_SetExp(m2,i,(c+p_GetExp(a2,i,tailRing)), currRing);
        p_SetExp(m1,i,p_GetExp(a1,i, tailRing), currRing);
      }
      else
      {
        p_SetExp(m1,i,(p_GetExp(a1,i,tailRing)-c), currRing);
        p_SetExp(m2,i,p_GetExp(a2,i, tailRing), currRing);
      }
    }
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
        p_LmFree(m2,currRing);
#ifdef HAVE_RINGS
        if (is_Ring)
        {
          pSetCoeff0(m1, t2);
          nDelete(&lc1);
          nDelete(&lc2);
          nDelete(&t1);
        }
        else
#endif
          nNew(&(pGetCoeff(m1)));
        return m1;
      }
      else
      {
        p_LmFree(m1,currRing);
#ifdef HAVE_RINGS
        if (is_Ring)
        {
          pSetCoeff0(m2, t1);
          nDelete(&lc1);
          nDelete(&lc2);
          nDelete(&t2);
        }
        else
#endif
          nNew(&(pGetCoeff(m2)));
        return m2;
      }
    }
#ifdef HAVE_RINGS
    if (is_Ring)
    {
      equal = nEqual(t1,t2);
    }
    else
#endif
    {
      t1 = nMult(pGetCoeff(a2),pGetCoeff(p1));
      t2 = nMult(pGetCoeff(a1),pGetCoeff(p2));
      equal = nEqual(t1,t2);
      nDelete(&t2);
      nDelete(&t1);
    }
    if (!equal)
    {
      p_LmFree(m2,currRing);
#ifdef HAVE_RINGS
      if (is_Ring)
      {
          pSetCoeff0(m1, nSub(t1, t2));
          nDelete(&lc1);
          nDelete(&lc2);
          nDelete(&t1);
          nDelete(&t2);
      }
      else
#endif
        nNew(&(pGetCoeff(m1)));
      return m1;
    }
    pIter(a1);
    pIter(a2);
#ifdef HAVE_RINGS
    if (is_Ring)
    {
      if (a2 != NULL)
      {
        nDelete(&t1);
        t1 = nMult(pGetCoeff(a2),lc1);
      }
      if (a1 != NULL)
      {
        nDelete(&t2);
        t2 = nMult(pGetCoeff(a1),lc2);
      }
      while ((a1 != NULL) && nIsZero(t2))
      {
        pIter(a1);
        if (a1 != NULL)
        {
          nDelete(&t2);
          t2 = nMult(pGetCoeff(a1),lc2);
        }
      }
      while ((a2 != NULL) && nIsZero(t1))
      {
        pIter(a2);
        if (a2 != NULL)
        {
          nDelete(&t1);
          t1 = nMult(pGetCoeff(a2),lc1);
        }
      }
    }
#endif
    if (a2==NULL)
    {
      p_LmFree(m2,currRing);
      if (a1==NULL)
      {
#ifdef HAVE_RINGS
        if (is_Ring)
        {
          nDelete(&lc1);
          nDelete(&lc2);
          nDelete(&t1);
          nDelete(&t2);
        }
#endif
        p_LmFree(m1,currRing);
        return NULL;
      }
      goto x1;
    }
    if (a1==NULL)
    {
      p_LmFree(m1,currRing);
      goto x2;
    }
  }
}
