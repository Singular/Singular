//This file contains the implementations for monomial/polynomial
//multiplications for letterplace.
//
//WARNING/TODO:
//I do not know, if the ideas in this file will always work. I
//do not yet know, if it was right to assume, that we always use
//the ..._Mult_...__T functions. Are there other Kandidates for
//r->p_Procs->..._Mult_... pointers???
//
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
//
//TODO Additional: -> This is now done i hope (more or less)
//review following functions:
//-> See list of exp vector things
//
//SetShortExpVector:  
//Wir benutzen momentan den SEV nicht (spaeter vielleicht ???)
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
//TODO: Kommt noch an so einigen Stellen vor und sollte
//wahrscheinlich aus dem Code entfernt werden, aber das mache
//ich vielleicht ein ander Mal -> vielleicht auch erst beim
//Debugging (z.B. in der kCheckSpolyCreation) -> s. Grischas
//dritte Liste
//
//p_ExpVectorAdd
//Benutzt an der auskommentierten Stelle von
//p_LmExpVectorAddIsOk - siehe Kommentar dazu
//Ich denke hier geht es hauptsaechlich darum herauszufinden, ob
//der exponent nicht zu gross ist, um ihn speichern zu koennen.
//Das kann bei uns nicht passieren, da wir nur Exponenten der
//Groesse maximal 1 haben, aber vielleicht sollte man sich das
//nochmal genauer anschauen.
//TODO: Kommt auch noch an anderen Stellen vor, z.B. in der
//kutil.cc, aber vielleicht funktioniert das dort auch ->
//spaeter drum kuemmern
//
//p_GetExpDiff
//Adjustment to ksCreateSpoly -- done
//
//p_GetExp/p_SetExp
//Adjustment to ksCreateSpoly -- done
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
ksCreateShortSpoly
nc_ksCreateShortSpoly
*/
/* List for ExpVector things
Those in Brackets are not used in our letterplace version of
the Bba

function		file		line

SetShortExpVector	kspoly.cc	153, 488, 543 (360)
			kutil.cc	9400 (3118, 5308, 5406)
			shiftDVec.cc	1336(so close!), 1526, 2289
			kstd2.cc	319, 336, 399, 473, 539, 639, 687, 777, 842, 945, 1023, 1123, 2795, 3094, 3138
			tgb.cc		(1867, 1914, 3018, 3065, 4703)
p_ExpVectorSub		kspoly.cc	99, 115, (227, 306, 322 )
p_LmExpVectorAddIsOk	kspoly.cc	104, (311)
			kutil.cc	7936, 7937, 7963, 7964
p_ExpVectorAdd		kspoly.cc	107, (314)
			kutil.cc	5866, 5909
			fast_mult.cc	(449)
			kstd2.cc	1842, 1852
p_GetExpDiff		kspoly.cc	607, 659, 697
			ringgb.cc	(71)
*/

#include <kutil.h>  //because of include order
#include <SDMultiplication.h>

#include <polys/monomials/p_polys.h>


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
void ShiftDVec::InitSDMultiplication( ring r, kStrategy strat )
{
  r->isLPring = strat->lV; 
  //BOCO: this should have already been set by
  //makeLetterplaceRing, but it isnt :(

  r->p_ExpSum = &ShiftDVec::p_ExpSum_slow;

  for(int i = 1; i < r->OrdSize; ++i)
  {
    if( r->typ[i].ord_typ != ro_dp )
    {
      r->p_ExpSum = &ShiftDVec::p_ExpSum_slow;
      return;
    }
  }

  //BOCO: TODO: QUESTION: 
  //Do we have to reset that after the bba?
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



/* Part 2: replacements for p_MemSum__T .                      */



/* BOCO: This is our letterplace replacement for p_MemSum__T.  *
 * This function will return rt->exp = p->exp + s * q->exp,    *
 * where s is a shift in size of the number of exponents equal *
 * to on in p->exp . rt has to be allocated (for example with  *
 * p_AllocBin?).                                               */
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



/* Part 2: replacements for Various functions which invoke some *
 * of the multiplications functions .                           */



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
                 kStrategy strat)
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

  p_ExpVectorSub(lm, p2, tailRing); // Calculate the Monomial we must multiply to p2

  if (tailRing != currRing)
  {
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
      t2 = pNext(UPW->GetLmTailRing());
      lm = p1;
      p_ExpVectorSub(lm, p2, tailRing);
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


  // and finally,
  PR->Tail_Minus_mm_Mult_qq(lm, t2, PW->GetpLength() - 1, spNoether);
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

/***************************************************************
 *
 * Creates S-Poly of p1 and p2
 *
 *
 ***************************************************************/
void ShiftDVec::ksCreateSpoly(LObject* Pair,   poly spNoether,
                   int use_buckets, ring tailRing,
                   poly m1, poly m2, TObject** R)
{
#ifdef KDEBUG
  create_count++;
#endif
  kTest_L(Pair);
  poly p1 = Pair->p1;
  poly p2 = Pair->p2;
  Pair->tailRing = tailRing;

  assume(p1 != NULL);
  assume(p2 != NULL);
  assume(tailRing != NULL);

  poly a1 = pNext(p1), a2 = strat->R[Pair.i_r2]->p;
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
    a2 = tailRing->p_Procs->pp_Mult_mm_Noether(a2, m2, spNoether, l2, tailRing);
    assume(l2 == pLength(a2));
  }
  else
    a2 = tailRing->p_Procs->pp_Mult_mm(a2, m2, tailRing);
#ifdef HAVE_RINGS
  if (!(rField_is_Domain(currRing))) l2 = pLength(a2);
#endif

  Pair->SetLmTail(m2, a2, l2, use_buckets, tailRing);
  // get m2*a2 - m1*a1
  Pair->Tail_Minus_mm_Mult_qq(m1, a1, l1, spNoether);

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
