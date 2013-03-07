//Needed because fo include order; should rather be included in
//ring.h
#include <kernel/mod2.h>

//For various declarations
#include <kernel/febase.h>
#include <kernel/ideals.h>
#include <misc/options.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <polys/weight.h>
#include <kernel/timer.h>
#include <kernel/polys.h>
#include <kernel/longrat.h>

#include <kernel/kutil.h>
#include <kernel/kutil2.h> //already included in kutil.h
#include <kernel/shiftDVec.h>

//now our adapted multiplications for:
//- ksCreateShortSpoly
//- etc. ?
#include <kernel/SDMultiplication.h>


//No debug version at the moment 
//TODO: (why did I shut that down?)
#undef KDEBUG

#undef HAVE_RINGS //We do not have rings
#undef HAVE_PLURAL //Nope

/* TODO/Ideas:
 * A lot !!!
 * - Lots of functions have to be testet thoroughfully, as they
 *   were heavily modified. This is especially true for those
 *   of the Gebauer-Moeller criterions.
 * - The code has partly to be rewritten, maybe split up into
 *   smaller functions, optimized, cleared to be more concise
 *   and commented better, to be more comprehensive.
 * - Lots of things can possibly be optimized. There should be a
 *   table to record shifts of polynomials, so, that they are
 *   only once created and can be freed with the help of this
 *   table correctly. (Memory leaks are probably a big problem
 *   in the momentary state of the code.)
 * - In the chain criterion we always test, if we have a valid
 *   pair; with some sophisticated tricks we may be able, to
 *   just know the pairs which are valid. (These pairs were
 *   already created in the enterRight/LeftOverlaps functions.)
 *   Maybe again, this table could help, plus some links.     
 * - It would be handy, to store the totaldegree for each
 *   polynomial, as this is often used.
 * - The lcm often violates the degree and so do the shifted
 *   polys. We could avoid this, by not saving the shifted
 *   polys, but only by saving the unshifted and the shift. We
 *   would have only to care about the tail term. This idea
 *   needs some reconfiguring and new lcm tests, which are
 *   partly implemented. However, as there had to be some major
 *   change, whilst we do not know, if this really has some big
 *   advantages, this idea is cancelled for the forseeable
 *   future.                                                  */


#if 0 //BOCO: original code and comment
/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L(via initenterpairs)
*superfluous elements in S will be deleted
*/
void enterpairs (poly h,int k,int ecart,int pos,kStrategy strat, int atR)
#else //replacement
/* BOCO: 
 * (s[0],s*h),...,(s[k],s*h) will be put to the pairset 
 * L(via initenterpairs) for every shift s, where s*h is not
 * menacing the uptodeg boundary. superfluous elements in S will
 * be deleted
 *
 * BOCO - additional comment:
 * pos seems to be the position of h in S (see bba function)
 */
void ShiftDVec::enterpairs 
  (LObject* H,int k,int ecart,int pos,kStrategy strat, int atR)
{
  namespace SD = ShiftDVec;
#endif
  int j=pos;

#ifdef HAVE_RINGS
  assume (!rField_is_Ring(currRing));
#endif

#if 0 //BOCO: original code and comment
  initenterpairs(h,k,ecart,0,strat, atR);
#else //replacement
  SD::initenterpairs(H,k,ecart,0,strat, atR);
#endif

  if ( (!strat->fromT)
  && ((strat->syzComp==0)
    ||(pGetComp(H->p)<=strat->syzComp)))
  {
    //Print("start clearS k=%d, pos=%d, sl=%d\n",k,pos,strat->sl);
    unsigned long h_sev = pGetShortExpVector(H->p);
    loop
    {
      if (j > k) break;
#if 0 //BOCO: original code and comment
      clearS(h,h_sev, &j,&k,strat);
#else //replacement
      SD::clearS(H,h_sev, &j,&k,strat);
#endif
      j++;
    }
    //Print("end clearS sl=%d\n",strat->sl);
  }
 // PrintS("end enterpairs\n");
}


#if 0 //BOCO: original code and comment from kInline.h
KINLINE void clearS (poly p, unsigned long p_sev, int* at, int* k,
                    kStrategy strat)
#else //replacement
/* Cares for shifts of p, as the divisibleBy functions do care
 * for shifts. */
void ShiftDVec::clearS 
  (LObject* H, unsigned long p_sev, int* at, int* k, kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
  assume(p_sev == pGetShortExpVector(H->p));
  if (strat->noClearS) return;
#if 0 //BOCO: original code (replaced)
    /*BOCO: pLmShortDivisibleBy is a macro and defined by:
     *
     * #define pLmShortDivisibleBy(a, sev_a, b, not_sev_b) \
     *  p_LmShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
     *
     * in polys.h.
     */
  if (!pLmShortDivisibleBy(p,p_sev, strat->S[*at], ~ strat->sevS[*at])) return;
#else //replacement

  TObject T(strat->S[*at], currRing, strat->tailRing);  
  /* BOCO: maybe dvec and TObject already exist
   * -> we should improve that! */

  uint shift = SD::p_LmShortDivisibleBy
            (&T,p_sev, H, ~ strat->sevS[*at], currRing);

  //BOCO: if T.dvec does not exist, then T.p ist a constant
  //      monomial, in which case shift will be UINT_MAX-1
  assume(!H->dvec || shift != UINT_MAX-1);

  //BOCO: if T.dvec == NULL, then T.p is a constant, which
  //TODO: implies R = 1 and we could stop here
  if ( H->dvec && shift > UINT_MAX-2 ) return;
#endif
  deleteInS((*at),strat);
  (*at)--;
  (*k)--;
}


/* grico:
 * construct new pairs of the form (h,s*p) and store as an
 * LObject dropped some if clauses regariding the product
 * criterion check initenterpairs for old code to compare the
 * implementations */
void ShiftDVec::initenterpairs
  ( LObject* H, int k, int ecart, 
    int isFromQ, kStrategy strat, int atR )
{
  assume(strat->R[atR]->p == H->p);
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering initenterpairs", "Leaving initenterpairs", 1 );

  namespace SD = ShiftDVec;
  BOOLEAN new_pair=FALSE;

  if ( (strat->syzComp==0) || (pGetComp(H->p)<=strat->syzComp) ) 
    //grico: this doesn't seem to be a wrong assumpiton, so I
    //kept it
  { 
    /* BOCO: 
     *  First we will find all overlaps of H with each
     *  element of S. We use very large arrays of arrays. This
     *  could be partially improved, by applying the
     *  Gebauer-Moeller criterion as soon as possible. Maybe, we 
     *  could change the findRightOverlaps function accordingly.*/
 
    //BOCO: arrays for right-overlaps
    uint** r_overlaps = 
      k < 0 ? NULL : (uint**)omAlloc((k+1)*sizeof(uint*));
    if(r_overlaps){
      loGriToFile
        ( "omAlloc for r_overlaps in initenterpairs ",
          (k+1)*sizeof(uint*), 1024, (void*) r_overlaps );
    }

    //Sizes for above arrays
    uint* r_ovl_sizes = 
      k < 0 ? NULL : (uint*)omAlloc((k+1)*sizeof(uint));
    if(r_overlaps){
      loGriToFile
        ( "omAlloc for r_ovl_sizes in initenterpairs ",
          (k+1)*sizeof(uint*), 1024, (void*) r_ovl_sizes );
    }

    //BOCO: arrays for right-overlaps
    uint** l_overlaps = 
      k < 0 ? NULL : (uint**)omAlloc((k+1)*sizeof(uint*));
    if(l_overlaps){
      loGriToFile
        ( "omAlloc for l_overlaps in initenterpairs ",
          (k+1)*sizeof(uint*), 1024, (void*) l_overlaps );
    }

    //Sizes for above arrays
    uint* l_ovl_sizes = 
      k < 0 ? NULL : (uint*)omAlloc((k+1)*sizeof(uint));
    if(l_overlaps){
      loGriToFile
        ( "omAlloc for l_ovl_sizes in initenterpairs",
          (k+1)*sizeof(uint*), 1024, (void*) l_ovl_sizes );
    }

    //BOCO: Find the overlaps
    for (int i = 0; i <= k; i++)
    {
      r_ovl_sizes[i] = 
        SD::findRightOverlaps
          ( H, strat->S_2_T(i), 
            strat->lV, strat->uptodeg, &(r_overlaps[i]) );

      l_ovl_sizes[i] = 
        SD::findRightOverlaps
          ( strat->S_2_T(i), H,
            strat->lV, strat->uptodeg, &(l_overlaps[i]) );
    }

    //BOCO: We are also interested in overlaps of H with itself.
    uint* s_overlaps;
    uint  s_ovl_size = 
      SD::findRightOverlaps
        ( H,H,strat->lV, strat->uptodeg, &s_overlaps);

    //BOCO: Before entering new pairs, we want to use the
    //Gebauer-Moeller criterion, to filter some pairs, we do
    //not need to enter.
    //Why does that not work in debug Version???
    if( TEST_OPT_LPDVGEBMOELL )
    {
      SD::GebauerMoeller
        ( s_overlaps, s_ovl_size, 
          r_overlaps, r_ovl_sizes, 
          l_overlaps, l_ovl_sizes, H, k, strat );
    }

    enterLeftOverlaps
      (H,strat,k, l_overlaps, l_ovl_sizes, isFromQ, ecart, atR);

    enterRightOverlaps
      (H,strat,k, r_overlaps, r_ovl_sizes, isFromQ, ecart, atR);

    enterSelfOverlaps
      (H, strat, s_overlaps, s_ovl_size, isFromQ, ecart, atR);

    //BOCO: Free arrays with overlaps
    for (int i = 0; i <= k; i++)
    {
      if( r_ovl_sizes[i] )
      {
        loGriToFile
          ( "omFreeSize for r_overlaps[i] in initenterpairs",
            r_ovl_sizes[i]*sizeof(uint),1024,(void*)r_overlaps[i] );
        omFreeSize
          ((ADDRESS)r_overlaps[i], sizeof(uint)*r_ovl_sizes[i]);
      }

      if( l_ovl_sizes[i] )
      {
        loGriToFile
          ( "omFreeSize for l_overlaps[i] in initenterpairs",
            l_ovl_sizes[i]*sizeof(uint),1024,(void*)l_overlaps[i] );
        omFreeSize
          ((ADDRESS)l_overlaps[i], sizeof(uint)*l_ovl_sizes[i]);
      }
    }

    if(k >= 0)
    {
      loGriToFile
        ( "omFreeSize for r_overlaps in initenterpairs",
          (k+1)*sizeof(uint*), 1024, (void*) r_overlaps  );
      omFreeSize
        ((ADDRESS)r_overlaps, sizeof(uint*)*(k+1));

      loGriToFile
        ( "omFreeSize for l_overlaps in initenterpairs",
          (k+1)*sizeof(uint*), 1024, (void*) l_overlaps  );
      omFreeSize
        ((ADDRESS)l_overlaps, sizeof(uint*)*(k+1));

      loGriToFile
        ( "omFreeSize for l_ovl_sizes in initenterpairs",
          (k+1)*sizeof(uint*), 1024, (void*) l_ovl_sizes  );
      omFreeSize
        ((ADDRESS)l_ovl_sizes, sizeof(uint)*(k+1));

      loGriToFile
        ( "omFreeSize for r_ovl_sizes in initenterpairs",
          (k+1)*sizeof(uint*), 1024, (void*) r_ovl_sizes  );
      omFreeSize
        ((ADDRESS)r_ovl_sizes, sizeof(uint)*(k+1));
    }

    if( s_ovl_size )
    {
      loGriToFile
        ( "omFreeSize for s_overlaps in initenterpairs",
          s_ovl_size*sizeof(uint), 1024, (void*)s_overlaps );
      omFreeSize
        ((ADDRESS)s_overlaps, sizeof(uint)*s_ovl_size);
    }
  }

#if 0
#if DEBOGRI > 0 
  if( ShiftDVec::debogri == 1024 )
  {
    //BOCO: Watch new Elements of L
    for(int i = 0; i <= strat->Bl; ++i)
    {
      deBoGriPrint(strat->B[i].p2, "B[i].p2 ", 1024);
    }
  }
#endif
#endif


#if 0 //BOCO: chainCrit now inside enterSelfOverlaps as GM3
      //The chainCrit had to be change heavily. There is not
      //much similiarity to our chainCrit. Even the theory
      //underwent some more or less big changes.
  if (new_pair) {SD::chainCrit(H,ecart,strat);}
#else
  SD::kMergeBintoL(strat); //merges the B set into the L set
  //This was formerly done in the chainCrit.
#endif
}

void ShiftDVec::DeBoGriTestGM
  (TSet tset, int k, TObject* H, int uptodeg, int lVblock)
{
 namespace SD = ShiftDVec;

/*

    //BOCO: arrays for right-overlaps
    uint** r_overlaps =
      k < 0 ? NULL : (uint**)omAlloc((k+1)*sizeof(uint*));

    if(r_overlaps){
      loGriToFile
        ( "omAlloc for r_overlaps in initenterpairs ",
          (k+1)*sizeof(uint*), 1024, (void*) r_overlaps );
    }

    //Sizes for above arrays
    uint* r_ovl_sizes =
      k < 0 ? NULL : (uint*)omAlloc((k+1)*sizeof(uint));

    if(r_overlaps){
      loGriToFile
        ( "omAlloc for r_ovl_sizes in initenterpairs ",
          (k+1)*sizeof(uint*), 1024, (void*) r_ovl_sizes );
    }

    //BOCO: arrays for left-overlaps
    uint** l_overlaps =
      k < 0 ? NULL : (uint**)omAlloc((k+1)*sizeof(uint*));

    if(l_overlaps){
      loGriToFile
        ( "omAlloc for l_overlaps in initenterpairs ",
          (k+1)*sizeof(uint*), 1024, (void*) l_overlaps );
    }

    //Sizes for above arrays
    uint* l_ovl_sizes =
      k < 0 ? NULL : (uint*)omAlloc((k+1)*sizeof(uint));

    if(l_overlaps){
      loGriToFile
        ( "omAlloc for l_ovl_sizes in initenterpairs",
          (k+1)*sizeof(uint*), 1024, (void*) l_ovl_sizes );
    }

    for (int i = 0; i <= k; i++)
    {
      r_ovl_sizes[i] =
        SD::findRightOverlaps
          ( H, tset[i],
            lVBlock, uptodeg, &(r_overlaps[i]) );

      l_ovl_sizes[i] =
        SD::findRightOverlaps
          ( tset[i], H,
            lVBlock, uptodeg, &(l_overlaps[i]) );
    }
*/

    //thats the first test we do
    //BOCO: We are also interested in overlaps of H with itself.
    uint* s_overlaps;
    uint  s_ovl_size =
      SD::findRightOverlaps
        ( H,H,lVblock, uptodeg, &s_overlaps);

  // Case 1 & 2
  for( uint i = 0; i < s_ovl_size; ++i )
  {
    //self with self and self

   Print("Testing selfoverlaps of ");
   pWrite(H->p);
   Print("with overlap %d \n",s_overlaps[i]);
   if  (GMTest( 
        H, H, s_overlaps[i], H, s_overlaps,
        s_ovl_size, s_overlaps, s_ovl_size ))
        {Print("GMTest Case 1 will return true.\n");}
   else {Print("GMTest Case 1 will return false.\n");}

    //self with right and left
   /* for(int l = 0; l <= k && s_overlaps[i]; ++l)
    {
      TObject* Right = tset[l];
      Print("Consider selfoverlap %d with element ", s_overlaps[i]);
      pWrite(tset[l]->p);
      Print(with right and leftoverlap %d and %d./n",r_overlaps[l],l_overlaps[l]);

      if (GMTest
          ( H, H, s_overlaps[i], Right, r_overlaps[l],
            r_ovl_sizes[l], l_overlaps[l], l_ovl_sizes[l] ))
        {Print("GMTest Case 2 will return true.\n");}
   else {Print("GMTest Case 2 will return false.\n");}
    }*/
   //later
  }

/*
//for later use the other cases
//include computation of left and right overlaps
  // Case 3 & 5
  for(int l = 0; l <= k; ++l)
  {
    //right with self and right
    for(uint i = 0; i < sizesRightOvls[l]; ++i)
    {
      TObject* Right = tset[l];
      Print("Consider rightoverlap %d with element",r_overlaps[l][i]);
      pWrite(tset[l]->p);
      Print(with self and right overlaps./n");
      if (GMTest
        ( H, Right, r_overlaps[l][i], H, s_overlaps,
          s_ovl_size, r_overlaps[l], r_ovl_sizes[l] ))
      {Print("GMTest Case 3 will return true./n");}
      else (Print("GMTest Case 3 will return false./n");)
    }

    //left with left and self
    for(uint i = 0; i < sizesLeftOvls[l]; ++i)
    {
      TObject* Left = tset[l];
      Print("Consider leftoverlap %d with element ",l_overlaps[l][i]);
      pWrite(tset[l]->p);
      Print(with left and self overlaps./n");
      if (GMTest
        ( Left, H, l_overlaps[l][i], H, s_overlaps,
          s_ovl_size, l_overlaps[l], l_ovl_sizes[l] ))
       {Print("GMTest Case 5 will return true./n");}
      else {Print("GMTest Case 5 will return false./n");}
    }
  }

 // Case 4: right with right and ?
 // xxxxxx
 //    yyyyyyyyy   Right1
 //   zzzzz        Right2
  for(int i = 0; i <= k; ++i)
  {
    for(uint j = 0; j < r_ovl_sizes[i]; ++j)
    {
      for(int m = 0; m <= k; ++m)
      {
        for(uint l = 0; l < r_ovl_sizes[m]; ++l)
        {
          uint shRight2 = r_overlaps[m][l];
          if( !shRight2 ) continue;
          uint shRight1 = r_overlaps[i][j];
          if( !shRight1 ) goto end_of_middle_loop_1;
          TObject* Right1 = tset[i];
          TObject* Right2 = tset[m];
          if( shRight2 >= shRight1 ||
              shRight2 + Right2->GetDVsize() >
              shRight1 + Right1->GetDVsize()   ) break;
          if( GMTest(H,Right1,Right2, shRight1,shRight2, NULL) )
            {rightOvls[i][j] = 0; goto end_of_middle_loop_1;}
          //not working as of yet, have to check with Bo
        }
      }

      end_of_middle_loop_1:;
    }
  }

  // Case 6: left with ? and left
  // yyyyyy     Left1
  //    xxxxxx
  //   zzzzz    Left2
  for(int i = 0; i <= k; ++i)
  {
    for(uint j = 0; j < l_ovl_sizes[i]; ++j)
    {
      for(int m = 0; m <= k; ++m)
      {
        for(uint l = 0; l < l_ovl_sizes[m]; ++l)
        {
          uint shLeft2 = l_overlaps[m][l];
          uint shLeft1 = l_overlaps[i][j];
          if( !shLeft2 ) continue;
          if( !shLeft1 ) goto end_of_middle_loop_2;
          TObject* Left1 = tset[i];
          TObject* Left2 = tset[m];
          if( shLeft2 > shLeft1 ||
              shLeft1 - Left1->GetDVsize() <=
              shLeft2 - Left1->GetDVsize()    ) break;

          if( GMTest(Left1,H,Left2, shLeft1,shLeft2, NULL) )
            {leftOvls[i][j] = 0; break;}

          //not working yet
        }
      }

      end_of_middle_loop_2:;
    }
  }
*/

/*
      //BOCO: Free arrays with overlaps
    for (int j = 0; i <= k; i++)
    {
      if( r_ovl_sizes[j] )
      {
        loGriToFile
          ( "omFreeSize for r_overlaps[i] in initenterpairs",
            r_ovl_sizes[j]*sizeof(uint),1024,(void*)r_overlaps[j] );

        omFreeSize
          ((ADDRESS)r_overlaps[j], sizeof(uint)*r_ovl_sizes[j]);
      }

      if( l_ovl_sizes[j] )
      {
        loGriToFile
          ( "omFreeSize for l_overlaps[i] in initenterpairs",
            l_ovl_sizes[j]*sizeof(uint),1024,(void*)l_overlaps[j] );

        omFreeSize
          ((ADDRESS)l_overlaps[j], sizeof(uint)*l_ovl_sizes[j]);
      }
    }
*/

    if( s_ovl_size )
    {
      loGriToFile
        ( "omFreeSize for s_overlaps in initenterpairs",
          s_ovl_size*sizeof(uint), 1024, (void*)s_overlaps );

      omFreeSize
        ((ADDRESS)s_overlaps, sizeof(uint)*s_ovl_size);
    }
}


/* BOCO: 
 *
 * This is Gebauer-Moeller for the non-commutative case.
 * It is known as pCompareChain for the commutative case.
 * It is used in enterSelfOverlaps. This is the Version, which
 * can be used for filtering overlaps, before creating
 * Pairs and entering these into the L-set.
 *
 * Define h1 := lm(H2->p), similar h2, h3. h1 is assumed to be a
 * polynomial or its leading monomial, if appropriate.
 * shift2 * h2 and shift3 * h3 are assumed to be right-overlaps
 * of h1, with shift2 > shift3 > 0.  Be aware, that this
 * automatically assures, that shift*h2 does not divide h1 or
 * shift*h3.  It is also assumed, that deg(shift2 * h2) >
 * deg(shift3 * h3), where deg(p) denotes the highest index of
 * the letterplace variables of p. 
 *
 * We will test, if shift3*h3 divides lcm(h1, shift2*h2). If so,
 * we will return true and the Gebauer-Moeller Criterion applies
 * for (h1, h2) (Thus, this pair does not need to be created for
 * obtaining our Groebner-Basis.) The pairs allowing not to
 * consider (h1, shift * h2), if the criterion applies, are (h1,
 * shift * h2) and (shift * h2, h3). One must be sure, that
 * these exist, or that it is possible to apply this criterion
 * for some other reason.
 *
 * This function has to be checked!                           */
bool ShiftDVec::GMTestRight
  ( TObject* H1, TObject* H2, TObject* H3, 
    uint shift2, uint shift3, kStrategy strat, ring r )
{
  /* We will do that by comparing the dvecs of h2 and h3. We
   * assume, that h2 and h3 have at least overlap of degree one
   * with h1, and that the overlap of h3 with h1 is bigger than
   * the overlap of h2 with h1 - This is all due to our function
   * prerequisistes. We only need to find out, if the part of
   * h3, not overlapping h1 is prefix of the part of h2, not
   * overlapping h1; consider the following diagram with
   * monospaced font as an example:
   * xyxyxyx     | h1
   *     xyxyxyx | h2
   *   xyxyxyx   | h3
   * h1 has overlap with h2: xyx
   * h2 has overlap with h3: xyxyx
   * We need to proof:
   * yx prefixes yxyx
   * (In this case, it is true of course.) 
   * This is equivalent of showing, that the part of the DVec of
   * h3, corresponding to the non-overlapping part of h1,
   * prefixes the part of the DVec of h3 corresponding to the
   * non-overlapping part of h1.                               
   * If you have a more concise, or better way to express these
   * thoughts - feel free to contact the developers of this
   * piece of code.                                           */
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering GMTestRight the other version.", 
      "Leaving GMTestRight the other version.", 128 );

  assume(shift2 > shift3 && shift3 > 0);

  deBoGriPrint
    ("The following pair will be considered for cancellation:", 128);
  deBoGriPrint(H1->p, "h1: ", 128);
  deBoGriPrint(H2->p, shift2, strat, "shift2*h2 ", 128);
  deBoGriPrint
    ("The pair might be cancelled because of: ", 128);
  deBoGriPrint(H3->p, shift3, strat, "shift3*h3: ", 128);

  int degH1 = H1->GetDVsize(); 
  int degH3 = H3->GetDVsize(); 

  //assume(degH3 + shift3 < H2->GetDVsize() + shift2);
  assume(degH3 + shift3 <= H2->GetDVsize() + shift2);

  int minCmpDV2 = degH1 - shift2;
  int minCmpDV3 = degH1 - shift3;
  int cmpLength = degH3 + shift3 - degH1;

  if( H2->cmpDVecProper
      (H3, minCmpDV2, minCmpDV3, cmpLength, strat->lV) == 0 )
  {
    deBoGriPrint("GMTestRight will return true.", 128);
    return true;
  }

  deBoGriPrint("GMTestRight will return false.", 128);
  return false;
}

/* BOCO: 
 *
 * This function is somehow similar to the GMTestRight function,
 * but somehow different. (I know, this is a great comment, so
 * please don't chop of my head.)
 *
 * This function has to be checked!                           */
bool ShiftDVec::GMTestLeft
  ( TObject* H1, TObject* H2, TObject* H3, 
    uint shift2, uint shift3, kStrategy strat, ring r )
{
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering GMTest the other version.", 
      "Leaving GMTest the other version.", 128 );

  //assume(shift2 > shift3 && shift3 > 0);
  assume(shift2 > shift3);
  assume(H1->GetDVsize() < H2->GetDVsize() + shift2);
  assume(H3->GetDVsize() + shift3 < H2->GetDVsize() + shift2);

  deBoGriPrint
    ("The following pair will be considered for cancellation:", 128);
  deBoGriPrint(H1->p, "h1: ", 128);
  deBoGriPrint(H2->p, shift2, strat, "shift2*h2 ", 128);
  deBoGriPrint
    ("The pair might be cancelled because of: ", 128);
  deBoGriPrint(H3->p, shift3, strat, "shift3*h3: ", 128);

  int minCmpDV1 = shift3;
  int minCmpDV3 = 0;
  int cmpLength = shift2 - shift3;

  if( H1->cmpDVecProper
      (H3, minCmpDV1, minCmpDV3, cmpLength, strat->lV) == 0 )
  {
    deBoGriPrint("GMTestLeft will return true.", 128);
    return true;
  }

  deBoGriPrint("GMTestLeft will return false.", 128);
  return false;
}

/* Suppose, we have three polynomials - h1, h2, h3 and that
 * these polynomials correspond to some TObjects H1, H2 and H3,
 * respectively.
 * This function supposes h1 to be right overlapped by h2, where
 * sH2 is the shift for h2, corresponding to the overlap.
 * ovlH1H3 is assumed to be an array of shifts s, which
 * correspond to right overlaps of h1 by s*h3. And ovlH3H2 is
 * assumed to be an array of shifts s, which correspond to right
 * overlaps of h3 by s*h2. These shifts are
 * assumed to be stored in ascending order in ovlH1H3
 * respectively ovlH3H2. 
 * If sH2 == 0, this function will return false immediately. If
 * there are 0-shifts in numOvlH1H3 or numOvlH3H2, they will be
 * ignored.
 * GMTest will test, if there exists a shift s in ovlH1H3,
 * such that s*h3 divides lcm(h1, sH2 * h2). If so, it will
 * return true, otherwise false. 
 * TODO: Special cases: h1 | h3 and h2 | h3                   */
bool ShiftDVec::GMTest
  ( TObject* H1, TObject* H2, uint sH2, TObject* H3, 
    uint* ovlH1H3, uint numOvlH1H3, 
    uint* ovlH3H2, uint numOvlH3H2                   )
{
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering GMTest", "Leaving GMTest", 4 );

#if DEBOGRI > 0
  static int counter;
  ++counter;
  deBoGriPrint(counter, "Der Counter", 128+2);
#endif

  if( sH2 == 0 ) {
    loGriToFile("GMTest return 1 ", counter, 4, NULL);
    return false;
  }

  assume( H1->GetDVsize() > 0 );
  assume( H2->GetDVsize() > 0 );
  assume( H3->GetDVsize() > 0 );

  //maxShiftOfH3 = min(sizeOfLcmDVec - sizeOfH3DVec, sH2 - 1)
  //sizeOfLcmDVec = sizeOfH2DVec + sH2 - 1
  uint maxShiftOfH3;
  uint minShiftOfH3;
  if( H3->GetDVsize() > H2->GetDVsize() ) //TODO: case h2 | h3
    maxShiftOfH3 = H2->GetDVsize() + sH2 - H3->GetDVsize();
  else
    maxShiftOfH3 = sH2 - 1;

  int i = 0;
  if( H3->GetDVsize() > H1->GetDVsize() ) //TODO: case h1 | h3
    minShiftOfH3 = 1;
  else
  {
    minShiftOfH3 = H1->GetDVsize() - H3->GetDVsize() + 1;
    for(; ovlH1H3[i] && minShiftOfH3 > ovlH1H3[i]; ++i)
      if(i >= numOvlH1H3)
      {
        loGriToFile("GMTest return 2 ", counter, 4, NULL);
        return false;
      }
  }

  int j;
  deBoGriPrint("TestTest1", 2);
  for( j = numOvlH3H2-1; j >= 0; --j )
    if( ovlH3H2[j] && ovlH3H2[j] + minShiftOfH3 <= sH2 ) break;
  if(j < 0) {
    loGriToFile("GMTest return 3 ", counter, 4, NULL);
    return false;
  }


  deBoGriPrint("TestTest2", 2);
  if(ovlH1H3[i] > maxShiftOfH3) {
    loGriToFile("GMTest return 4 ", counter, 4, NULL);
    return false;
  }

  uint sH3;
  deBoGriPrint("TestTest3", 2);
  while( sH3 = sH2 - ovlH3H2[j] != ovlH1H3[i] )
  {
    if(sH3 > maxShiftOfH3) {
      loGriToFile("GMTest return 5 ", counter, 4, NULL);
      deBoGriPrint("TestTest4", 2);
      return false;
    }

    while(ovlH1H3[i++] < sH3 || (i < numOvlH3H2) && !ovlH1H3[i]);
    if(i >= numOvlH1H3) {
      loGriToFile("GMTest return 6 ", counter, 4, NULL);
      deBoGriPrint("TestTest5", 2);
      return false;
    }
    if(ovlH1H3[i] > maxShiftOfH3) {
      deBoGriPrint("TestTest6", 2);
      loGriToFile("GMTest return 7 ", counter, 4, NULL);
      return false;
    }

    for(;j >= 0; --j)
      if(ovlH3H2[j] && sH2 - ovlH3H2[j] >= ovlH1H3[i]) break;
    if(j < 0){
      deBoGriPrint("TestTest7", 2);
      loGriToFile("GMTest return 8 ", counter, 4, NULL);
      return false;
    }
  }
  deBoGriPrint("TestTest8", 2);

  loGriToFile("GMTest return 9 ", counter, 4, NULL);

  deBoGriPrint("May filter Pair H1, s2*H2.", 128+2);
  deBoGriPrint("With Pair H1, s3*H3.", 128+2);
  deBoGriPrint("And Pair s3*H3, s2*H2.", 128+2);
  deBoGriPrint(H1->p, "H1: ", 128+2);
  deBoGriPrint(H2->p, "H2: ", 128+2);
  deBoGriPrint(H3->p, "H3: ", 128+2);
  deBoGriPrint(sH2, "s2: ", 128+2);
  deBoGriPrint(sH3, "s3: ", 128+2);

  return true;
}

/* BOCO: TODO: explain this $?&X/§! function!
 *
 * BOCO: hints:
 * (xxxxxx is our H)
 *
 * Case 1
 * self with self and self
 * xxxxxx
 *    xxxxxx
 *  xxxxxx
 * 
 * Case 2
 * self with right and left
 * xxxxxx
 *    xxxxxx
 *  yyyyyy
 * 
 * Case 3
 * right with self and right
 * xxxxxx
 *    yyyyyyyyy
 *   xxxxxx
 * 
 * Case 4
 * right with right and ?
 * xxxxxx
 *    yyyyyyyyy
 *   zzzzz
 * 
 * Case 5
 * left with left and self
 * yyyyyy
 *    xxxxxx
 *   xxxxxx
 *
 * Case 6
 * left with ? and left
 * yyyyyy
 *    xxxxxx
 *   zzzzz
 *
 * Case 7
 * ? with left and right - not needed
 * yyyyyyy
 *    zzzzzzzz
 *   xxxxxx
 * This is the "L-Set" case ! (The only case, so far,
 * polynomials from the L-Set could be deleted.) But all pairs
 * for deleting the pair from L must have been already filtered
 * in  Case 4 or Case 6. Thus Case 7 needn't (and musn't) be
 * considered anymore,
 * 
 * Case 8
 * ? with ? and ? - not needed
 * Another L-Set Case.
 * We do not need to consider that case, as this would have been
 * considered in earlier loops.
 *
 * This function has to be reviewed and tested thoroughly. */
void ShiftDVec::GebauerMoeller
  ( uint* selfOvls, uint sizeSelfOvls,
    uint** rightOvls, uint* sizesRightOvls,
    uint** leftOvls, uint* sizesLeftOvls, 
    LObject* H, int k, kStrategy strat      )
{
  //IDEA: Include a special Class for shifts!
  initDeBoGri
    ( ShiftDVec::indent, "Entering GebauerMoeller", 
      "Leaving GebauerMoeller", 128                 );
  deBoGriPrint
    ( "This is Gebauer-Moeller for filtering new Pairs.", 128 );
  deBoGriPrint
    ( selfOvls, sizeSelfOvls, "Shifts of self-overlaps.", 128 );

#if 1 //under construction, un-uncomment, if errors occeur

  // Case 1 & 2
  for( uint i = 0; i < sizeSelfOvls; ++i )
  {
    //self with self and self
    if( GMTest
        ( H, H, selfOvls[i], H, selfOvls, 
          sizeSelfOvls, selfOvls, sizeSelfOvls ) )
    {
      deBoGriPrint("Filtered self with self and self.", 128);
      selfOvls[i] = 0;
    }

    //self with right and left
    for(int l = 0; l <= k && selfOvls[i]; ++l)
    {
      TObject* Right = strat->S_2_T(l);
      if( GMTest
          ( H, H, selfOvls[i], Right, rightOvls[l],
            sizesRightOvls[l], leftOvls[l], sizesLeftOvls[l] ) )
      {
        deBoGriPrint("Filtered self with right and self.", 128);
        selfOvls[i] = 0;
      }
    }
  }

  // Case 3 & 5
  for(int l = 0; l <= k; ++l)
  {
    //right with self and right
    for(uint i = 0; i < sizesRightOvls[l]; ++i)
    {
      TObject* Right = strat->S_2_T(l);
      if ( GMTest
          ( H, Right, rightOvls[l][i], H, selfOvls,
            sizeSelfOvls, rightOvls[l], sizesRightOvls[l] ) )
      {
        deBoGriPrint("Filtered right with self and right.", 128);
        rightOvls[l][i] = 0;
      }
    }

    //left with left and self
    for(uint i = 0; i < sizesLeftOvls[l]; ++i)
    {
      TObject* Left = strat->S_2_T(l);
      if( GMTest
          ( Left, H, leftOvls[l][i], H, selfOvls,
            sizeSelfOvls, leftOvls[l], sizesLeftOvls[l] ) )
      {
        deBoGriPrint("Filtered left with left and self.", 2+128);
        leftOvls[l][i] = 0;
      }
    }
  }

 /* Case 4: right with right and ?
  * xxxxxx
  *    yyyyyyyyy   Right1
  *   zzzzz        Right2
  */
  for(int i = 0; i <= k; ++i)
  {
    for(uint j = 0; j < sizesRightOvls[i]; ++j)
    {
      for(int m = 0; m <= k; ++m)
      {
        for(uint l = 0; l < sizesRightOvls[m]; ++l)
        {
          uint shRight2 = rightOvls[m][l];
          if( !shRight2 ) continue;

          uint shRight1 = rightOvls[i][j];
          if( !shRight1 ) goto end_of_middle_loop_1;

          TObject* Right1 = strat->S_2_T(i);
          TObject* Right2 = strat->S_2_T(m);

          /*BOCO: 
           * TODO: Maybe it is more efficient not to
           * consider pairs with
           * shRight2 + Right2->GetDVsize() ==
           * shRight1 + Right1->GetDVsize()
           * Think about that!
           */
          if( shRight2 >= shRight1 ||
              shRight2 + Right2->GetDVsize() > 
              shRight1 + Right1->GetDVsize()   ) break;

          if( GMTestRight
                (H,Right1,Right2, shRight1,shRight2, strat) )
          {
            deBoGriPrint("Filtered right with right and ?.", 128);
            rightOvls[i][j] = 0; 
            goto end_of_middle_loop_1;
          }
        }
      }

      end_of_middle_loop_1:;
    }
  }

 /* Case 6: left with ? and left
  * yyyyyy     Left1
  *    xxxxxx
  *   zzzzz    Left2
  */
  for(int i = 0; i <= k; ++i)
  {
    for(uint j = 0; j < sizesLeftOvls[i]; ++j)
    {
      for(int m = 0; m <= k; ++m)
      {
        for(uint l = 0; l < sizesLeftOvls[m]; ++l)
        {
          uint shLeft1 = leftOvls[i][j];
          uint shLeft2 = leftOvls[m][l];
          if( !shLeft2 ) continue;
          if( !shLeft1 ) goto end_of_middle_loop_2;

          TObject* Left1 = strat->S_2_T(i);
          TObject* Left2 = strat->S_2_T(m);

          if( shLeft2 > shLeft1 ||
              shLeft1 - Left1->GetDVsize() <=
              shLeft2 - Left1->GetDVsize()    ) break;

          /*BOCO: 
           * TODO: Maybe it is more efficient not to
           * consider pairs with
           * shLeft1 == shLeft2
           * Think about that!
           */
          if( GMTestLeft
                (Left1,H,Left2, shLeft1,shLeft1-shLeft2, strat) )
          {
            deBoGriPrint("Filtered left with ? and left.", 128);
            leftOvls[i][j] = 0; 
            break;
          }
        }
      }

      end_of_middle_loop_2:;
    }
  }

#endif //if 1 - under construction
}


/* Enter the right overlaps of H with Elements from S */
void ShiftDVec::enterRightOverlaps
  ( LObject* H, kStrategy strat, int k, uint** rightOvls, 
    uint* sizesRightOvls, int isFromQ, int ecart, int atR )
{
  initDeBoGri
    ( ShiftDVec::indent, "Entering enterRightOverlaps", 
      "Leaving enterRightOverlaps", 4 + 1 + 128         );
  namespace SD = ShiftDVec;

  bool new_pair;
  for (int i = 0; i <= k; i++)
  {
    for (int j = 0; j < sizesRightOvls[i]; j++)
    {
      uint shiftSi = rightOvls[i][j];
      if( shiftSi &&
          !SD::createSPviolatesDeg
            ( strat->S[i], H->p, shiftSi, strat->uptodeg, 
              currRing, currRing, 
              strat->tailRing, strat->tailRing            ) &&
          !shiftViolatesDeg
            ( strat->S[i], shiftSi, strat->uptodeg, 
              currRing, strat->tailRing             )          )
        //grico: checks if degbound is violated
      {
        deBoGriPrint("No degree Violation!", 1);
        int isFromQi = 0; //BOCO: hack TODO: clear
        LObject* Pair = SD::enterOnePair
          ( H->p, atR, isFromQ, ecart, strat->S[i], shiftSi,
            strat->S_2_R[i], strat->ecartS[i], isFromQi, strat );

        //grico: sets new_pair to true, as in the old
        //version
        if(Pair != NULL) new_pair=true; 
      }
    }
  }

  //TODO: Gebauer Moeller on L, if new_pair
}


/* Enter the left overlaps of H with Elements from S */
void ShiftDVec::enterLeftOverlaps
  ( LObject* H, kStrategy strat, int k, uint** leftOvls, 
    uint* sizesLeftOvls, int isFromQ, int ecart, int atR )
{
  initDeBoGri
    ( ShiftDVec::indent, "Entering enterLeftOverlaps", 
      "Leaving enterLeftOverlaps", 4 + 1 + 128         );
  namespace SD = ShiftDVec;

  bool new_pair;
  for (int i = 0; i <= k; i++)
  {
    for (int j = 0; j < sizesLeftOvls[i]; j++)
    {
      uint shiftH = leftOvls[i][j];
      if( shiftH &&
          !SD::createSPviolatesDeg
            ( H->p, strat->S[i], shiftH, strat->uptodeg, 
              currRing, currRing, 
              strat->tailRing, strat->tailRing           ) && 
          !shiftViolatesDeg
            ( H->p, shiftH, strat->uptodeg,
              currRing, strat->tailRing     )                 )
        //grico: checks if degbound is violated
      {
        deBoGriPrint("No degree Violation!", 1);
        int isFromQi = 0; //BOCO: hack TODO: clear
        LObject* Pair = SD::enterOnePair
          ( strat->S[i], strat->S_2_R[i], isFromQi, 
            strat->ecartS[i], 
            H->p, shiftH, atR, isFromQ, ecart, strat );

        //grico: sets new_pair to true, as in the old
        //version
        if(Pair != NULL) new_pair=true; 
      }
    }
  }

  //TODO: Gebauer Moeller on L
  //No: I think we are finished with Gebauer Moeller already
}


/* Enter the self overlaps of H
 * We  will also apply the third criterion of
 * Gebauer-Moeller to L, during this process. REMARK: Be aware,
 * that in the normal bba, we have nothing to enter for the
 * first time, we invoke initenterpairs - There is no other
 * polynomial to pair the first entered polynomial with -,
 * however, in the letterplace variant, our first entered
 * polynomial might have self overlaps.                       */
void ShiftDVec::enterSelfOverlaps
  ( LObject* H, kStrategy strat, uint* selfOvls,
    uint numSelfOvls, int isFromQ, int ecart, int atR )
{
  initDeBoGri
    ( ShiftDVec::indent, "Entering enterSelfOverlaps", 
      "Leaving enterSelfOverlaps", 4 + 1 + 128         );

  namespace SD = ShiftDVec;

  LObject* Pair;

  bool new_pair = false;
  for (int j = 0; j < numSelfOvls; ++j)
  {
    uint shiftH = selfOvls[j];
    if( shiftH &&
        !SD::createSPviolatesDeg
          ( H->p, H->p, shiftH, strat->uptodeg, 
             currRing, currRing, 
             strat->tailRing, strat->tailRing   ) &&
        !shiftViolatesDeg
          ( H->p, shiftH, strat->uptodeg,
            currRing, strat->tailRing     )          )
      //grico: checks if degree bound is violated
    {
      deBoGriPrint("No degree Violation!", 1);

      Pair = SD::enterOnePair
        ( H->p, atR, isFromQ, ecart, 
          H->p, shiftH, atR, isFromQ, ecart, strat );

      if(Pair != NULL)
      {
        deBoGriPrint("entered new pair to B", 32);
        deBoGriPrint(Pair->p1, "Pair->p1", 32);
        deBoGriPrint(Pair->p2, "Pair->p2", 32);

        //grico: sets new_pair to true, as in the old version
        new_pair=true; 
      }
    }
  }

#if 0 
  //Work in Progress - 
  //No: This is not nescessary any long, i think, but i'm all
  //but sure; TODO: clear this!
 
  /* BOCO: This applies Gebauer-Moeller on strat->L .
   * Formerly, this was part of the chainCrit function.
   * However, we might have to move this later into the loop
   * above, and test it for every entered Pair. This depends on
   * the the validity of our GM3 function. See TODO inside of
   * the GM3 function.                                       */
  if(new_pair){
    for (int l=strat->Ll; l>=0; l--){
      //Gebauer-Moeller Kriterium 3
      if (ShiftDVec::GM3(H,&(strat->L[l]),strat))
        deleteInL(strat->L,&strat->Ll,l,strat);
    }
  }
#endif
}


#if 0 //No longer needed.
/* BOCO: This is the third Kriterion of Gebauer-Moeller.
 * It is known as pCompareChain for the commutative case.
 * It is used in enterSelfOverlaps. This is the Version for
 * filtering self-overlaps. For the other Version, filtering
 * Pairs from the L-Set, see below. 
 * Let h := H->p;  h1 := shift1*H->p;  h2 := shift2*H->p;
 * lcm := lcm(h, h1), then this function returns true, 
 * if h2 | lcm and fals otherwise. For the criterion to work
 * properly, shift1 > shift2 > 0 should hold (this will not be
 * tested, but is assumed) and the shifts h1 and h2 should
 * correspond to overlaps of h. The pairs which will then
 * actually be responsible for the cancellation of the pair 
 * (h, h1) will be (h, h2) and (h2, h1), where (h2, h1)
 * corresponds to a pair (h, h3), with h3 := (shift1 - shift2)*h
 * . Remark: 0 < shift1 - shift2 < shift1 . See commentary below
 * for further explanations.
 * This function has to be checked!                           */
bool ShiftDVec::GM3
  ( LObject* H, 
    uint shift1, uint shift2, kStrategy strat, ring r )
{
  /* We will do that by comparing the dvecs of h1 = s1*h and
   * h2 = s2 * h . We assume, that h1 and h2 have at least
   * overlap of degree one with h, and that the overlap of h2
   * with h is bigger than the overlap of h1 with h - This is
   * all due to our function prerequisites. We only need to find
   * out, if the part of h2, not overlapping h prefix of the the
   * part of h1, not overlapping h; consider the following
   * diagram with monospaced font as an example:
   * xyxyxyx     | h
   *     xyxyxyx | h1
   *   xyxyxyx   | h2
   * h has overlap with h1: xyx
   * h has overlap with h2: xyxyx
   * We need to proof:
   * yx prefixes yxyx
   * (In this case, it is true of course.) 
   * This is equivalent of showing, that the part of the DVec of
   * h2, corresponding to the non-overlapping part of h,
   * prefixes the part of the DVec of h2 corresponding to the
   * non-overlapping part of h.                               
   * If you have a more concise, or better way to express these
   * thoughts - feel free to contact the developers of this
   * piece of code.                                           */

  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering GM3 for self-overlaps.", 
      "Leaving GM3 for self-overlaps.", 128 );

  deBoGriPrint
    ("The following pair will be considered for cancellation:", 128);
  deBoGriPrint(H->p, "H->p: ", 128);
  deBoGriPrint(H->p, shift1, strat, "shift1*H->p: ", 128);
  deBoGriPrint
    ("The pair will be cancelled because of: ", 128);
  deBoGriPrint(H->p, shift2, strat, "shift2*H->p: ", 128);

  int degH = p_Totaldegree(H->p, r);
  int minCmpDV1 = degH - shift1;
  int minCmpDV2 = degH - shift2;

  if( H->cmpDVec(H, minCmpDV1, minCmpDV2, shift2) == 0 )
  {
    deBoGriPrint("GM3 will return true.", 128);
    return true;
  }

  deBoGriPrint("GM3 will return false.", 128);
  return false;
}
#endif




/* BOCO: This Version of the GM3 function is no longer needed.
 * BOCO: This is the third Kriterion of Gebauer-Moeller.
 * It is known as pCompareChain for the commutative case.
 * It is used in enterSelfOverlaps. 
 * Input is expected to be a TObject H, which corresponds to a
 * polynomial p1 and an LObject Pair, which corresponds to a pair
 * (p2, p3). We want to test, if (p2, p3) can be removed from
 * the Pairset. This is true, in our case, if p1 | lcm(p2, p3)
 * and p1 does not divide p2 or p3. In the implementation, we 
 * do not need to test the 2 latter conditions, if we restrict
 * our divisibility test to the apropriate part of lcm(p2, p3).
 * This is the general Pairtest (not specialized for
 * self-overlaps).
 * If the Kriterion applies, thus, (p2, p3) can be removed, true
 * will be returned, false otherwise.
 *
 * TODO:
 *  - This function has to be checked!
 *  - Improve the Explanation: Why do we not explicitly need the 
 *    pairs (p1, p2) and (p2, p3); etc. (if you have questions:
 *    ask the authors, who hopefully still know, what they did)*/
bool ShiftDVec::GM3
  ( LObject* H, LObject* Pair, kStrategy strat, ring r )
{
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering GM3 (General Version)", "Leaving GM3", 132 );

  deBoGriPrint
    ("The following pair will be considered for cancellation:", 128);
  deBoGriPrint(Pair->p1, "p2: ", 128);
  deBoGriPrint(Pair->p2, "p3: ", 128);
  deBoGriPrint
    ("The pair might be cancelled because of: ", 128);
  deBoGriPrint(H->p, "p1: ", 128);


  /* BOCO: Explanation/Idea/TODO
   * Pair->lcm==NULL probably means, that the pair is one of our
   * initial generators. The criterion would thus not be
   * applicable on it.                                        */
  if( Pair->lcm==NULL ) return false;
  deBoGriPrint("The lcm of the pair is not NULL", 4 + 128);

  uint dvSize1 = H->GetDVsize();
  uint dvSize2 = strat->R[Pair->i_r1]->GetDVsize();
  uint dvSize3 = strat->R[Pair->i_r2]->GetDVsize();
  uint dv3sh = 
    getShift(Pair->p2, strat->R[Pair->i_r2]->getDVecAtIndex(0));

  //BOCO: I hope, this will get me the length of the overlap 
  //of (p2, p3) .
  uint ovSize = dvSize3 - dv3sh;

  if( ovSize >= dvSize1 ) return false;
  deBoGriPrint("Size of Overlap < length(DVec(p1)).", 128);

  uint StartPos = dvSize2 >= dvSize1 ? dvSize2 - dvSize1 + 1 : 0;
  uint EndPos = dvSize2 - ovSize - 1;
  //if(EndPos + dvSize1 > degLcm) EndPos = degLcm - dvSize1;

  deBoGriSilenceIf
    (EndPos < 1000 && StartPos < 1000 && ovSize < 1000);
  deBoGriPrint(dvSize1, "dvSize1: ", 128);
  deBoGriPrint(dvSize2, "dvSize2: ", 128);
  deBoGriPrint(dvSize3, "dvSize3: ", 128);
  deBoGriPrint(dvSize3, "ovSize: ", 128);
  //deBoGriPrint(degLcm, "degLcm: ", 128);
  deBoGriPrint(StartPos, "StartPos: ", 128);
  deBoGriPrint(EndPos, "Endpos: ", 128);
  deBoGriSpeak();

  if( Pair->lcmDivisibleBy(H, StartPos, EndPos, strat->lV) )
  {
    deBoGriPrint("GM3 will return true.", 128);
    return true;
  }

  deBoGriPrint("GM3 will return false.", 128);
  return false;
}


#if 0 //BOCO: some older code, left here, because... I dont know
      //TODO: delete it some time
/* BOCO: This is the third Kriterion of Gebauer-Moeller.
 * It is known as pCompareChain for the commutative case.
 * It is used in enterSelfOverlaps. Input is expected to be an
 * LObject in form of (p, s*p), where p is an unshifted
 * polynomial and s*p a non-zero shift of p. L should be some 
 * pair from strat->L, which should be intrisically in the form
 * of (q, s'*r), where q is unshifted and s' a non-zero shift. 
 * We will test, if (p, L->p2) and (L->p1, s*p) are valid pairs
 * If they do not correspond to a pair in strat->B, they can
 * not be used for the criterion. To test that, we will test,
 * if the pair corresponds to a valid overlap. Examples:
 *  - x(1)y(2) and y(2)z(3) correspond to a valid overlap
 *  - x(1)y(2) and y(3)z(4) do not correspond to a valid 
 *    overlap
 *  - x(1)y(2)z(3) and y(2)x(3)z(4) do not correspond to a valid 
 *    overlap, though they have indeed gcd != 1 .
 * If one of the pairs is not valid, false will be returned,
 * otherwise, we will try to apply the third part of the
 * Gebauer-Moeller criterion to (p, L->p2), (p1, p2) and 
 * (L->p1, s*p) and return true, if it could be applied - in
 * this case one can get rid of the pair (p1, p2) -, otherwise,
 * false will be returned.
 * This function has to be checked!                           */
bool ShiftDVec::GM3(LObject* H, LObject* L, kStrategy strat)
{
  initDeBoGri
    ( ShiftDVec::indent, "Entering GM3", "Leaving GM3", 132 );

  deBoGriPrint(L->lcm, "L->lcm: ", 4 + 128);
  deBoGriPrint(L->p1, "L->p1: ", 4 + 128);
  deBoGriPrint(L->p2, "L->p2: ", 4 + 128);
  deBoGriPrint(H->p1, "H->p1: ", 4 + 128);
  deBoGriPrint(H->p2, "H->p2: ", 4 + 128);
  deBoGriPrint(strat->R[H->i_r1]->p, "strat->R[H->i_r1]->p", 4);
  deBoGriPrint(strat->R[H->i_r2]->p, "strat->R[H->i_r2]->p", 4);

  /* BOCO: Explanation/Idea/TODO
   * L->lcm==NULL probably means, that L is one of our initial
   * generators. The criterion would thus not be applicable on
   * it                                                      */
  if( L->lcm==NULL )
    return false;
  deBoGriPrint("The lcm of L is not NULL", 4 + 128);


  if(L->lcmDivisibleBy(strat->R[H->i_r1],strat->lV) > UINT_MAX-2)
    return false;
  deBoGriPrint("At GM3.", 4 + 128);
  deBoGriPrint("The lcm of L is divisible by a shift of H->p1", 4 + 128);

#if 0
  //BOCO: TODO
  //The kommutativ Gebauer-Moeller criterion part 3 says, that
  //we shall test, if lcm(H->p, p1) != lcm(L->p1, L->p2), 
  //and if lcm(H->p, p2) != lcm(L->p1, L->p2), but we that at
  //the moment for our non-commutative or letterplace case. We
  //neither sure, if we are allowed to do this, or if not, and
  //how we have to do it. We have yet to sort this out. The
  //implementation below is some sort of test like this, but it
  //is presumably very wrong.
 
  if( !L->gm3LcmUnEqualToLcm(H->p1, L->p2, strat->lV) ) 
    return false;
  deBoGriPrint("L->lcm != lcm(H->p1, L->p2)", 4);

  if( !L->gm3LcmUnEqualToLcm(L->p1, H->p2, strat->lV) ) 
    return false;
  deBoGriPrint("L->lcm != lcm(L->p1, H->p2)", 4);
#endif

  deBoGriPrint
    ("Because of Gebauer-Moeller following Pair is expected to be superflous:", 128);
  deBoGriPrint(L->p1, "p1: ", 128);
  deBoGriPrint(L->p2, "p2: ", 128);
  deBoGriPrint("Their lcm is:", 128);
  deBoGriPrint(L->lcm, "lcm: ", 128);
  deBoGriPrint("It can be left out, because of the following polynomial:", 128);
  deBoGriPrint(H->p1, "h: ", 128);

  deBoGriPrint("GM3 will return true.", 128);
  return true;
}
#endif


/* BOCO: 
 * This is an adapted version of enterOnePairNormal. It will
 * enter a pair corresponding to a self overlap into B and
 * return a pointer to the pair entered. The entered pair is
 * (H->p, shift*H->p).
 * BOCO IMPORTANT:
 * Unfortunatly, The shift of H is not in R; we will link to an
 * unshiftet version.                                        */
#if 0 //BOCO: original Header
void enterOnePairNormal
  ( int i,poly p,int ecart, 
    int isFromQ,kStrategy strat, int atR = -1 )
#else //BOCO: replacement
LObject* ShiftDVec::enterOnePair
  ( poly p1, int atR1, int isFromQ1, int ecart1,
    poly p2, uint shift, int atR2, int isFromQ2, int ecart2, 
    kStrategy strat                                          )
#endif
{
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering enterOnePair", "Leaving enterOnePair", 8 );

  namespace SD = ShiftDVec;

  if (strat->interred_flag) return NULL;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

#ifdef HAVE_SHIFTBBADVEC //BOCO: added
  p2 = p_LPshiftT
    ( p2, shift, strat->uptodeg, strat->lV, strat, currRing );
  loGriToFile("p_LPshiftT in enterOnePair ",0 ,1024, (void*)p2);
#endif

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

#if 0 //BOCO: replaced
#ifndef HAVE_RATGRING
  pLcm(p,strat->S[i],Lp.lcm);
#elif defined(HAVE_RATGRING)
  //  if (rIsRatGRing(currRing))
  pLcmRat(p,strat->S[i],Lp.lcm, currRing->real_var_start); // int rat_shift
#endif
#else //BOCO: replacement
#ifndef HAVE_RATGRING
  pLcm(p2,p1,Lp.lcm);
#elif defined(HAVE_RATGRING)
  //  if (rIsRatGRing(currRing))
  pLcmRat(p2,p1,Lp.lcm, currRing->real_var_start); // int rat_shift
#endif
#endif
  pSetm(Lp.lcm);

#if 0 //BOCO: deleted - we do not have the product criterion
  if (strat->sugarCrit && ALLOW_PROD_CRIT(strat))
  {
    if((!((strat->ecartS[i]>0)&&(ecart>0)))
    && pHasNotCF(p,strat->S[i]))
    {
    /*
    *the product criterion has applied for (s,p),
    *i.e. lcm(s,p)=product of the leading terms of s and p.
    *Suppose (s,r) is in L and the leading term
    *of p divides lcm(s,r)
    *(==> the leading term of p divides the leading term of r)
    *but the leading term of s does not divide the leading term of r
    *(notice that tis condition is automatically satisfied if r is still
    *in S), then (s,r) can be cancelled.
    *This should be done here because the
    *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
    *
    *Moreover, skipping (s,r) holds also for the noncommutative case.
    */
      strat->cp++;
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
    }
    else
      Lp.ecart = si_max(ecart,strat->ecartS[i]);
    if (strat->fromT && (strat->ecartS[i]>ecart))
    {
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
      /*the pair is (s[i],t[.]), discard it if the ecart is too big*/
    }
    /*
    *the set B collects the pairs of type (S[j],p)
    *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p)#lcm(r,p)
    *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
    *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
    */
    {
      j = strat->Bl;
      loop
      {
        if (j < 0)  break;
        compare=pDivComp(strat->B[j].lcm,Lp.lcm);
        if ((compare==1)
        &&(sugarDivisibleBy(strat->B[j].ecart,Lp.ecart)))
        {
          strat->c3++;
          if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
          {
            pLmFree(Lp.lcm);
            return;
          }
          break;
        }
        else
        if ((compare ==-1)
        && sugarDivisibleBy(Lp.ecart,strat->B[j].ecart))
        {
          deleteInL(strat->B,&strat->Bl,j,strat);
          strat->c3++;
        }
        j--;
      }
    }
  }
  else /*sugarcrit*/
  {
    if (ALLOW_PROD_CRIT(strat))
    {
      // if currRing->nc_type!=quasi (or skew)
      // TODO: enable productCrit for super commutative algebras...
      if(/*(strat->ak==0) && productCrit(p,strat->S[i])*/
      pHasNotCF(p,strat->S[i]))
      {
      /*
      *the product criterion has applied for (s,p),
      *i.e. lcm(s,p)=product of the leading terms of s and p.
      *Suppose (s,r) is in L and the leading term
      *of p devides lcm(s,r)
      *(==> the leading term of p devides the leading term of r)
      *but the leading term of s does not devide the leading term of r
      *(notice that tis condition is automatically satisfied if r is still
      *in S), then (s,r) can be canceled.
      *This should be done here because the
      *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
      */
          strat->cp++;
          pLmFree(Lp.lcm);
          Lp.lcm=NULL;
          return;
      }
      if (strat->fromT && (strat->ecartS[i]>ecart))
      {
        pLmFree(Lp.lcm);
        Lp.lcm=NULL;
        return;
        /*the pair is (s[i],t[.]), discard it if the ecart is too big*/
      }
      /*
      *the set B collects the pairs of type (S[j],p)
      *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p)#lcm(r,p)
      *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
      *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
      */
      for(j = strat->Bl;j>=0;j--)
      {
        compare=pDivComp(strat->B[j].lcm,Lp.lcm);
        if (compare==1)
        {
          strat->c3++;
          if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
          {
            pLmFree(Lp.lcm);
            return;
          }
          break;
        }
        else
        if (compare ==-1)
        {
          deleteInL(strat->B,&strat->Bl,j,strat);
          strat->c3++;
        }
      }
    }
  }
#endif

  /*
  *the pair (S[i],p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
#if 0 //BOCO: replaced
  if (strat->fromT && !TEST_OPT_INTSTRATEGY)
    pNorm(p);
#else //BOCO: replacement
  if (strat->fromT && !TEST_OPT_INTSTRATEGY)
    pNorm(p2);
#endif

#if 0 //BOCO: replaced
  if ((strat->S[i]==NULL) || (p==NULL))
    return;
#else //BOCO: replacement
  if ((p1==NULL) || (p2==NULL))
    {if(p2) {loGriToFile("pDelete in enteronepair ",0 ,1024, (void*)p2);pDelete(&p2);} return NULL;}
#endif

#if 0 //BOCO: I do not know, how to translate this.
  if ((strat->fromQ!=NULL) && (isFromQ!=0) && (strat->fromQ[i]!=0))
    Lp.p=NULL;
  else
#else //replacement
  if ((strat->fromQ!=NULL) && (isFromQ1!=0) && (isFromQ2!=0))
    Lp.p=NULL;
  else
#endif
  {
    #ifdef HAVE_PLURAL //BOCO: this should not be set
    assume(0) //BOCO: We don't HAVE_PLURAL
    if ( rIsPluralRing(currRing) )
    {
#if 0 //BOCO: replaced
      if(pHasNotCF(p, strat->S[i]))
#else //BOCO: replacement
      if(pHasNotCF(p2, p1))
#endif
      {
         if(ncRingType(currRing) == nc_lie)
         {
             // generalized prod-crit for lie-type
             strat->cp++;
#if 0 //BOCO: replaced
             Lp.p = nc_p_Bracket_qq(pCopy(p),strat->S[i]);
#else //BOCO: replacement
             Lp.p = nc_p_Bracket_qq(pCopy(p2),p1);
#endif
         }
         else
#if 0 //BOCO: We have no product criterion.
        if( ALLOW_PROD_CRIT(strat) )
        {
            // product criterion for homogeneous case in SCA
            strat->cp++;
            Lp.p = NULL;
        }
        else
#endif
        {
#if 0 //BOCO: replaced
          Lp.p = // nc_CreateSpoly(strat->S[i],p,currRing);
                nc_CreateShortSpoly(strat->S[i], p, currRing);
#else //BOCO: replacement
          Lp.p = nc_CreateShortSpoly(p1, p2, currRing);
#endif

          assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
          pNext(Lp.p) = strat->tail; // !!!
        }
      }
      else
      {
#if 0 //BOCO: replaced
        Lp.p = // nc_CreateSpoly(strat->S[i],p,currRing);
              nc_CreateShortSpoly(strat->S[i], p, currRing);
#else //BOCO: replacement
        Lp.p = nc_CreateShortSpoly(p1, p2, currRing);
#endif

        assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
        pNext(Lp.p) = strat->tail; // !!!

      }


#if 0 //BOCO: deleted
#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("enterOnePairNormal::\n strat->S[i]: "); pWrite(strat->S[i]);
        PrintS("p: "); pWrite(p);
        PrintS("SPoly: "); pWrite(Lp.p);
      }
#endif
#endif

    }
    else
    #endif //BOCO: HAVE_PLURAL
    {
      assume(!rIsPluralRing(currRing));

#if 0 //BOCO: replaced
      Lp.p = ksCreateShortSpoly(strat->S[i], p, strat->tailRing);
#else //BOCO: replacement
      Lp.p = ksCreateShortSpoly(p1, p2, strat->tailRing);
#endif

#if 0 //BOCO: deleted
#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("enterOnePairNormal::\n strat->S[i]: "); pWrite(strat->S[i]);
        PrintS("p: "); pWrite(p);
        PrintS("commutative SPoly: "); pWrite(Lp.p);
      }
#endif
#endif

    }
  }
  if (Lp.p == NULL)
  {
#if 0 //BOCO: TODO: review this better
    /*- the case that the s-poly is 0 -*/
    if (strat->pairtest==NULL) initPairtest(strat);
    strat->pairtest[i] = TRUE;/*- hint for spoly(S^[i],p)=0 -*/
    strat->pairtest[strat->sl+1] = TRUE;
    /*hint for spoly(S[i],p) == 0 for some i,0 <= i <= sl*/
    /*
    *suppose we have (s,r),(r,p),(s,p) and spoly(s,p) == 0 and (r,p) is
    *still in B (i.e. lcm(r,p) == lcm(s,p) or the leading term of s does not
    *devide lcm(r,p)). In the last case (s,r) can be canceled if the leading
    *term of p devides the lcm(s,r)
    *(this canceling should be done here because
    *the case lcm(s,p) == lcm(s,r) is not covered in chainCrit)
    *the first case is handeled in chainCrit
    */
#endif
    if (Lp.lcm!=NULL) pLmFree(Lp.lcm);
    loGriToFile("pDelete in enteronepair",0 ,1024, (void*)p2);
    pDelete(&p2);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
#if 0 //BOCO: replaced
    Lp.p1 = strat->S[i];
    Lp.p2 = p;
#else //BOCO: replacement
    Lp.p1 = p1;
    Lp.p2 = p2;
#endif

    if (
        (!rIsPluralRing(currRing))
//      ||  (rIsPluralRing(currRing) && (ncRingType(currRing) != nc_lie))
       )
    {
      assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
      pNext(Lp.p) = strat->tail; // !!!
    }

#if 0 //BOCO: replaced - needed work around, because of shifts
    if (atR >= 0)
    {
      Lp.i_r1 = strat->S_2_R[i];
      Lp.i_r2 = atR;
    }
    else
    {
      Lp.i_r1 = -1;
      Lp.i_r2 = -1;
    }
#else //BOCO: replacement
    Lp.i_r1 = atR1;
    Lp.i_r2 = atR2;
#endif

#if 0 //BOCO: replaced
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);
#else //BOCO: replacement
    strat->initEcartPair( &Lp, p1, p2, ecart1, ecart2 );
    //grico: still unsure about Ecart, since we do not use Mora, I think its not needed
#endif

    if (TEST_OPT_INTSTRATEGY)
    {
      if (!rIsPluralRing(currRing))
        nDelete(&(Lp.p->coef));
    }

    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    deBoGriPrint("Entering LObject Lp to B", 8);
    deBoGriPrint(Lp.lcm, "Lp.lcm: ", 8);
    deBoGriPrint(Lp.i_r1, "Lp.i_r1: ", 8);
    deBoGriPrint(Lp.i_r2, "Lp.i_r2: ", 8);
    deBoGriPrint(atR1, "atR1: ", 8);
    deBoGriPrint(atR2, "atR2: ", 8);
    assume(strat->R[Lp.i_r1]->p == Lp.p1);
    return SD::enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
    //grico: pair is first saved to B, will be merged into L at
    //the end
  }

  return NULL;
}


#if 0 //BOCO: former version, had to adapt that
/*
 * put the pair (q,p)  into the set B, ecart=ecart(p), 
 * q is the shift of some s[i]; BOCO: i do yet not know if
 * my approach of rewriting these functions is legal
 * grico: replaced the position i with qi, which is the position
 * of q (unshifted) in strat->S 
 * BOCO: 
 * returns a Pointer to the pair entered.
 * BOCO IMPORTANT:
 * Unfortunatly, The shift of H is not in R; we will link to an
 * unshiftet version.
 */
LObject* ShiftDVec::enterOnePair
  ( TObject* Q, TObject* H, int shift,int qi, int ecart, 
    int isFromQ, kStrategy strat, int atR                )
{
  namespace SD = ShiftDVec;

  poly p = p_LPshiftT
    (H->p, shift, strat->uptodeg, strat->lV, currRing);

  /* Format: q and p are like strat->P.p, so lm in CR, tail in TR */

  /* check this Formats: */
  assume(p_LmCheckIsFromRing(Q->p,currRing));
  assume(p_CheckIsFromRing(pNext(Q->p),strat->tailRing));
  assume(p_LmCheckIsFromRing(p,currRing));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));
  //grico: thought this are the assumption on our ring, so i left them here

  //grico to boco: till this point i took ur header, and left the assumption, the rest is new and imported from the enterOnePairNormal
  assume(qi<=strat->sl);
  if (strat->interred_flag) return NULL;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
    if (TEST_OPT_DEBUG)
    {
//       PrintS("enterOnePairShift(q,p) invoked with q = ");
//       wrp(q); //      wrp(pHead(q));
//       PrintS(", p = ");
//       wrp(p); //wrp(pHead(p));
//       PrintLn();
    }
#endif

  /*- computes the lcm(p,q) -*/
  //grico:old code, just changed to p and q
  Lp.lcm = pInit();
  // q replaces strat->S[i]

#ifndef HAVE_RATGRING
  pLcm(Q->p,p,Lp.lcm);
#elif defined(HAVE_RATGRING)
  //  if (rIsRatGRing(currRing))
  pLcmRat(Q->p,p,Lp.lcm, currRing->real_var_start); // int rat_shift
#endif
  pSetm(Lp.lcm);

  Lp.SetLcmDvecToNULL();

  //grico:
  //Product criterion not allowed, skipped some code, since our Overlap computation takes care of that
  //same for chain crit
  // the following is the computation of the s poly, which should be the same in our case and in the old case, since q is shifted
  // so these are basicly the old function, which should work (to boco: maybe a translation of the objects is needed?)
  // just the inputs may differ to the renaming to p and q
 /*
  *the pair (q,p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
  if (strat->fromT && !TEST_OPT_INTSTRATEGY)
    pNorm(p);

  if ((Q->p==NULL) || (p==NULL))
    return NULL;

  if ((strat->fromQ!=NULL) && (isFromQ!=0) && (strat->fromQ[qi]!=0))
    Lp.p=NULL;
  else
  {
    #ifdef HAVE_PLURAL
    if ( rIsPluralRing(currRing) )
    {
      if(pHasNotCF(p, Q->p))
      {
         if(ncRingType(currRing) == nc_lie)
         {
             // generalized prod-crit for lie-type
             strat->cp++;
             Lp.p = nc_p_Bracket_qq(pCopy(p),Q->p);
         }
         else
        if( ALLOW_PROD_CRIT(strat) )
        {
            // product criterion for homogeneous case in SCA
            strat->cp++;
            Lp.p = NULL;
        }
        else
        {
          Lp.p = // nc_CreateSpoly(q,p,currRing);
                nc_CreateShortSpoly(Q->p, p, currRing);

          assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
          pNext(Lp.p) = strat->tail; // !!!
        }
      }
      else
      {
        Lp.p = // nc_CreateSpoly(q,p,currRing);
              nc_CreateShortSpoly(Q->p, p, currRing);

        assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
        pNext(Lp.p) = strat->tail; // !!!

      }

#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("enterOnePairNormal::\n q: "); pWrite(Q->p);
        PrintS("p: "); pWrite(p);
        PrintS("SPoly: "); pWrite(Lp.p);
      }
#endif

    }
    else
    #endif
    {
      assume(!rIsPluralRing(currRing));
      Lp.p = ksCreateShortSpoly(Q->p, p, strat->tailRing);
#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("enterOnePairNormal::\n q: "); pWrite(Q->p);
        PrintS("p: "); pWrite(p);
        PrintS("commutative SPoly: "); pWrite(Lp.p);
      }
#endif

      }
  }
  if (Lp.p == NULL)
  {
    /*- the case that the s-poly is 0 -*/
    //if (strat->pairtest==NULL) initPairtest(strat);
    //strat->pairtest[qi] = TRUE;/*- hint for spoly(S^[qi],p)=0 -*/
    //strat->pairtest[strat->sl+1] = TRUE;
    //grico: pairtest not needed, should be done in dvec::chaincrit ****************************************

    /*hint for spoly(S[qi],p) == 0 for some qi,0 <= qi <= sl*/
    /*
    *suppose we have (s,r),(r,p),(s,p) and spoly(s,p) == 0 and (r,p) is
    *still in B (i.e. lcm(r,p) == lcm(s,p) or the leading term of s does not
    *devide lcm(r,p)). In the last case (s,r) can be canceled if the leading
    *term of p devides the lcm(s,r)
    *(this canceling should be done here because
    *the case lcm(s,p) == lcm(s,r) is not covered in chainCrit)
    *grico: this commentary isnt true any longer. all the cases are covered in the new chainCrit
    */
    if (Lp.lcm!=NULL) pLmFree(Lp.lcm);
  }
  else
  {
    /*- the pair (S[qi],p) enters B -*/
    Lp.p1 = Q->p;
    Lp.p2 = p;

    if (
        (!rIsPluralRing(currRing))
//      ||  (rIsPluralRing(currRing) && (ncRingType(currRing) != nc_lie))
       )
    {
      assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
      pNext(Lp.p) = strat->tail; // !!!
      // grico: i guess not needed, since we never have a PluralRing, but it doesnt harm anyone... 
    }

    if (atR >= 0)
    {
      Lp.i_r1 = strat->S_2_R[qi];
      Lp.i_r2 = atR;
    }
    else
    {
      Lp.i_r1 = -1;
      Lp.i_r2 = -1;
    }
    strat->initEcartPair(&Lp,Q->p,p,strat->ecartS[qi],ecart);
    //grico: still unsure about Ecart, since we do not use Mora, I think its not needed
    if (TEST_OPT_INTSTRATEGY)
    {
      if (!rIsPluralRing(currRing))
        nDelete(&(Lp.p->coef));
    }

    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    deBoGriPrint("Entering LObject Lp to B", 8);
    deBoGriPrint(Lp.lcm, "Lp.lcm: ", 8);
    deBoGriPrint(Lp.i_r1, "Lp.i_r1: ", 8);
    deBoGriPrint(Lp.i_r2, "Lp.i_r2: ", 8);
    deBoGriPrint(atR, "atR: ", 8);
    deBoGriPrint(qi, "qi: ", 8);
    return SD::enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
    //grico: pair is first saved to B, will be merged into L at
    //the end
  }
}
#endif


/* BOCO: This was heavily modified. The alternate
 * Gebauer-Moeller variant was cut out. Gebauer Moeller was
 * partly extracted and moved to the GM3 Function, which is part
 * of the enterSelfOverlaps function.                         */
void ShiftDVec::chainCrit
  (LObject* H, int ecart, kStrategy strat)
{
  /* grico: all new code, based on chaincritnormal
   * maybe shift must be added
   * ALLOW_PROD_CRIT(strat) = 0 grico: we do not want to allow
   * this product criterion, but this should be set on a
   * different procedure, like initBBA                         */

  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering chainCrit", "Leaving chainCrit", 64 );

namespace SD = ShiftDVec; 

#if 0 //BOCO: old code
  int i,j,l;
#else //replacement
int i,j;
#endif

  /*
  *pairtest[i] is TRUE if spoly(S[i],p) == 0.
  *In this case all elements in B such *that their lcm is
  divisible by the leading term of S[i] can be canceled
  */

  // BOCO:
  // The following pairtest could be adapted.
  if (strat->pairtest!=NULL)
  {
    {
      /*- i.e. there is an i with pairtest[i]==TRUE -*/
      for (j=0; j<=strat->sl; j++)
      {
        if (strat->pairtest[j])
        {
          for (i=strat->Bl; i>=0; i--)
          {
#if 0 //BOCO: old code - replaced
            if (pDivisibleBy(strat->S[j],strat->B[i].lcm)) old divisibility test
#else //replacement
            //new divisbility test
            if (strat->B[i].lcmDivisibleBy(strat->S_2_T(j),strat->lV))
#endif
            {
              deBoGriPrint("Pairtest removed pair from B.", 64);
              deBoGriPrint(strat->B[i].p1, "B[i]->p1: ", 64);
              deBoGriPrint(strat->B[i].p2, "B[i]->p2: ", 64);
              deBoGriPrint(strat->B[i].lcm, "B[i]->lcm: ", 64);
              deleteInL(strat->B,&strat->Bl,i,strat);
              strat->c3++;
            }
          }
        }
      }
    }
    omFreeSize(strat->pairtest,(strat->sl+2)*sizeof(BOOLEAN));
    strat->pairtest=NULL;
  }

//BOCO: old code, partially replaced (see remark below)
#if 0
  /* BOCO: remark
   * There were 2 versions of Gebauer-Moeller in the old code,
   * but we do yet only have one, which is somehow similar to
   * the following. The else clause is an alternative version,
   * we did not yet reproduce.                                 
   * Furthermore we do not have any surgar criterium.          */
  if (strat->Gebauer || strat->fromT)
  {
    if (strat->sugarCrit)
    {
    /*
    *suppose L[j] == (s,r) and p/lcm(s,r)
    *and lcm(s,r)#lcm(s,p) and lcm(s,r)#lcm(r,p)
    *and in case the sugar is o.k. then L[j] can be canceled
    */
      for (j=strat->Ll; j>=0; j--)
      {
        if (sugarDivisibleBy(ecart,strat->L[j].ecart)
        && ((pNext(strat->L[j].p) == strat->tail) || (pOrdSgn==1))
        && pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
        {
          if (strat->L[j].p == strat->tail)
          {
              deleteInL(strat->L,&strat->Ll,j,strat);
              strat->c3++;
          }
        }
      }
      /*
      *this is GEBAUER-MOELLER:
      *in B all elements with the same lcm except the "best"
      *(i.e. the last one in B with this property) will be canceled
      */
      j = strat->Bl;
      loop /*cannot be changed into a for !!! */
      {
        if (j <= 0) break;
        i = j-1;
        loop
        {
          if (i <  0) break;
          if (pLmEqual(strat->B[j].lcm,strat->B[i].lcm))
          {
            strat->c3++;
            if (sugarDivisibleBy(strat->B[j].ecart,strat->B[i].ecart))
            {
              deleteInL(strat->B,&strat->Bl,i,strat);
              j--;
            }
            else
            {
              deleteInL(strat->B,&strat->Bl,j,strat);
              break;
            }
          }
          i--;
        }
        j--;
      }
    }
    else /*sugarCrit*/
    {
      /*
      *suppose L[j] == (s,r) and p/lcm(s,r)
      *and lcm(s,r)#lcm(s,p) and lcm(s,r)#lcm(r,p)
      *and in case the sugar is o.k. then L[j] can be canceled
      */
      for (j=strat->Ll; j>=0; j--)
      {
        if (pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
        {
          if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
          {
            deleteInL(strat->L,&strat->Ll,j,strat);
            strat->c3++;
          }
        }
      }
      /*
      *this is GEBAUER-MOELLER:
      *in B all elements with the same lcm except the "best"
      *(i.e. the last one in B with this property) will be canceled
      */
      j = strat->Bl;
      loop   /*cannot be changed into a for !!! */
      {
        if (j <= 0) break;
        for(i=j-1; i>=0; i--)
        {
          if (pLmEqual(strat->B[j].lcm,strat->B[i].lcm))
          {
            strat->c3++;
            deleteInL(strat->B,&strat->Bl,i,strat);
            j--;
          }
        }
        j--;
      }
    }
    /*
    *the elements of B enter L
    */
    kMergeBintoL(strat);
  }
  else
  {
    for (j=strat->Ll; j>=0; j--)
    {
      if (pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
      {
        if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
        {
          deleteInL(strat->L,&strat->Ll,j,strat);
          strat->c3++;
        }
      }
    }
    /*
    *this is our MODIFICATION of GEBAUER-MOELLER:
    *First the elements of B enter L,
    *then we fix a lcm and the "best" element in L
    *(i.e the last in L with this lcm and of type (s,p))
    *and cancel all the other elements of type (r,p) with this lcm
    *except the case the element (s,r) has also the same lcm
    *and is on the worst position with respect to (s,p) and (r,p)
    */
    /*
    *B enters to L/their order with respect to B is permutated for elements
    *B[i].p with the same leading term
    */
    kMergeBintoL(strat);
    j = strat->Ll;
    loop  /*cannot be changed into a for !!! */
    {
      if (j <= 0)
      {
        /*now L[0] cannot be canceled any more and the tail can be removed*/
        if (strat->L[0].p2 == strat->tail) strat->L[0].p2 = p;
        break;
      }
      if (strat->L[j].p2 == p)
      {
        i = j-1;
        loop
        {
          if (i < 0)  break;
          if ((strat->L[i].p2 == p) && pLmEqual(strat->L[j].lcm,strat->L[i].lcm))
          {
            /*L[i] could be canceled but we search for a better one to cancel*/
            strat->c3++;
            if (isInPairsetL(i-1,strat->L[j].p1,strat->L[i].p1,&l,strat)
            && (pNext(strat->L[l].p) == strat->tail)
            && (!pLmEqual(strat->L[i].p,strat->L[l].p))
            && pDivisibleBy(p,strat->L[l].lcm))
            {
              /*
              *"NOT equal(...)" because in case of "equal" the element L[l]
              *is "older" and has to be from theoretical point of view behind
              *L[i], but we do not want to reorder L
              */
              strat->L[i].p2 = strat->tail;
              /*
              *L[l] will be canceled, we cannot cancel L[i] later on,
              *so we mark it with "tail"
              */
              deleteInL(strat->L,&strat->Ll,l,strat);
              i--;
            }
            else
            {
              deleteInL(strat->L,&strat->Ll,i,strat);
            }
            j--;
          }
          i--;
        }
      }
      else if (strat->L[j].p2 == strat->tail)
      {
        /*now L[j] cannot be canceled any more and the tail can be removed*/
        strat->L[j].p2 = p;
      }
      j--;
    }
  }
#else //BOCO: This is our implementation of Gebauer-Moeller:
{
#if 0 //BOCO: changed and moved to enterSelfOverlaps
  /* Grico: tried to copy this old code, but it seems not to
   * work properly                                        */
  for (j=strat->Ll; j>=0; j--)
  {
    if (ShiftDVec::compareChain(H,&(strat->L[j]),strat)) //renewed old procedure for new divisble tests
    {
      if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
      {
        //BOCO: debugging output
        deleteInL(strat->L,&strat->Ll,j,strat);
        strat->c3++;
      }
    }
  }
#endif
  /*
  *this is GEBAUER-MOELLER:
  *in B all elements with the same lcm except the "best"
  *(i.e. the last one in B with this property) will be canceled
  */
  j = strat->Bl;
  loop   /*cannot be changed into a for !!! */
  {
    if (j <= 0) break;
    for(i=j-1; i>=0; i--)
    {
#ifndef USE_DVEC_LCM //BOCO: original code
      if (pLmEqual(strat->B[j].lcm,strat->B[i].lcm)) //Grico: should still work with Letterplace setup
#else //BOCO: replacement (if USE_DVEC_LCM is used)
      if ( strat->B[j].compareLcmTo( &(strat->B[i]) ) )
#endif
      {
        deBoGriPrint("Gebauer-Moeller removed Pair from B.", 64);
        deBoGriPrint(strat->B[i].p1, "B[i].p1: ", 64);
        deBoGriPrint(strat->B[i].p2, "B[i].p2 ", 64);
        deBoGriPrint(strat->B[i].lcm, "B[i].lcm ", 64);
        strat->c3++;
        deleteInL(strat->B,&strat->Bl,i,strat);
        j--;
      }
    }
    j--;
  }
  SD::kMergeBintoL(strat); //merges the B set into the L set
}
#endif
}


/*2
* merge set B into L
*/
void ShiftDVec::kMergeBintoL(kStrategy strat)
{
  int j=strat->Ll+strat->Bl+1;
  if (j>strat->Lmax)
  {
    j=((j+setmaxLinc-1)/setmaxLinc)*setmaxLinc;
    strat->L = (LSet)omReallocSize(strat->L,strat->Lmax*sizeof(LObject),
                                 j*sizeof(LObject));
    strat->Lmax=j;
  }
  j = strat->Ll;
  int i;
  for (i=strat->Bl; i>=0; i--)
  {
    j = strat->posInL(strat->L,j,&(strat->B[i]),strat);
    ShiftDVec::enterL(&strat->L,&strat->Ll,&strat->Lmax,strat->B[i],j);
  }
  strat->Bl = -1;
}
