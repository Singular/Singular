/*! @file
 *
 * @brief Creation of new pairs and applying criterions on them
 *
 * This file is part of the Letterplace ShiftDVec Project
 *
 * @author Grischa Studzinski
 * @author Benjamin Schnitzler benjaminschnitzler@googlemail.com
 *
 * @copyright see main copyright notice for Singular
 */

#include <SDBase.new.h>

/** @brief Enter a pair corresponding to a self overlap into
 *         strat->B and return a pointer to the pair entered. 
 *
 *  The pair entered will be stored in an ShiftDVec::LObject say
 *  L. L will contain the two Letterplace polynomials L.p1 and
 *  L.p2 (as pointers, no new memory is allocated for them).
 *  These polynomials will build the S-polynomial
 *  L.p1 - L.shift_p2*L.p2, where L.shift_p2 is the shift
 *  defining the overlap the pair (L.p1, L.p2 and
 *  L.shift_p2*L.p2 the polynomial obtained by shifting L.p2 by
 *  L.shift_p2 .
 *
 *  The code of this function was obtained by modifying
 *  void enterOnePairNormal( int i,poly p,
 *                           int ecart, int isFromQ,
 *                           kStrategy strat, int atR = -1)
 *  ( Note to myself:
 *    No change to it in current singular Spielwiese since
 *    obtaining the copy )
 *
 *  @param[in]  p1        left  polynomial in overlap
 *  @param[in]  p2        right polynomial in overlap
 *  @param[in]  atR1      position of p1 in R
 *  @param[in]  atR2      position of p2 in R
 *  @param[in]  shift     the shift to apply to p2
 *  @param[in]  ecart1    TODO: what's that? do we need it?
 *  @param[in]  ecart2    TODO: what's that? do we need it?
 *  @param[in]  isFromQ1  TODO: what's that? do we need it?
 *  @param[in]  isFromQ2  TODO: what's that? do we need it?
 *  @param[in]  strat     TODO: explain that in a central place
 *
 * TODO: implement this!
 * TODO: test this in doxygen!
 */
LObject* ShiftDVec::enterOnePair
  ( poly p1, int atR1,
    int isFromQ1, int ecart1,
    poly p2, uint shift, int atR2,
    int isFromQ2, int ecart2, SD::kStrategy strat )
{
  if (strat->interred_flag) return NULL;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif

  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

  //TODO: SD::pLcm is yet to implement ( with HAVE_RATRING! )
  assume(0)
  SD::pLcm( p1, p2, shift, Lp.lcm, currRing );
  pSetm(Lp.lcm);

  /*
   * the pair (S[i],p) enters B if the spoly != 0
   */

  /*-  compute the short s-polynomial -*/

  if (strat->fromT && !TEST_OPT_INTSTRATEGY) pNorm(p2);

  if ((p1==NULL) || (p2==NULL))
    { if(p2){pDelete(&p2);} return NULL; }

  if ((strat->fromQ!=NULL) && (isFromQ1!=0) && (isFromQ2!=0))
    Lp.p=NULL;
  else
  {
    assume(!rIsPluralRing(currRing));
    Lp.p = ksCreateShortSpoly(p1, p2, strat->tailRing);
  }
  if (Lp.p == NULL){ if (Lp.lcm!=NULL) pLmFree(Lp.lcm); }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = p1;
    Lp.p2 = p2;
    Lp.shift_p2 = shift;

    assume(!rIsPluralRing(currRing))
    assume(pNext(Lp.p)==NULL); // TODO: this may be violated
                               // whenever ext.prod.crit. for
                               // Lie alg. is used
    pNext(Lp.p) = strat->tail; // !!!

    Lp.i_r1 = atR1;
    Lp.i_r2 = atR2;

    //grico: still unsure about Ecart, since we do not use Mora,
    //       I think its not needed
    strat->initEcartPair( &Lp, p1, p2, ecart1, ecart2 );

    if (TEST_OPT_INTSTRATEGY)
      if (!rIsPluralRing(currRing)) nDelete(&(Lp.p->coef));

    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    assume(strat->R[Lp.i_r1]->p == Lp.p1);
    Lp.SD_Object_Extension = NULL;
    return SD::enterL(&strat->B,&strat->Bl,&strat->Bmax,&Lp,l);
    //grico: pair is first saved to B, will be merged into L at
    //the end
  }

  return NULL;
}

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
