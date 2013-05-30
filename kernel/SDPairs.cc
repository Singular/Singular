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
#include <kernel/SDkutil.h>
#include <kernel/SDDebug.h>
#include <kernel/SDBase.h>

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

typedef skStrategy* kStrategy;


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
  (ShiftDVec::LObject* H,int k,int ecart,int pos,ShiftDVec::kStrategy strat, int atR)
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
    //Print
    //("start clearS k=%d, pos=%d, sl=%d\n",k,pos,strat->sl);
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
  (ShiftDVec::LObject* H, unsigned long p_sev, int* at, int* k, ShiftDVec::kStrategy strat)
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
  assume(!H->SD_Ext()->dvec || shift != UINT_MAX-1);

  //BOCO: if T.dvec == NULL, then T.p is a constant, which
  //TODO: implies R = 1 and we could stop here
  if ( H->SD_Ext()->dvec && shift > UINT_MAX-2 ) return;
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
  ( ShiftDVec::LObject* H, int k, int ecart, 
    int isFromQ, ShiftDVec::kStrategy strat, int atR )
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
          ( H, strat->S_2_T(i), strat->get_lV(),
            strat->get_uptodeg(), &(r_overlaps[i]) );

      l_ovl_sizes[i] = 
        SD::findRightOverlaps
          ( strat->S_2_T(i), H, strat->get_lV(),
            strat->get_uptodeg(), &(l_overlaps[i]) );
    }

    //BOCO: We are also interested in overlaps of H with itself.
    uint* s_overlaps;
    uint  s_ovl_size = 
      SD::findRightOverlaps
        ( H, H,
          strat->get_lV(), strat->get_uptodeg(), &s_overlaps );

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
  if( SD::debogri == 1024 )
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

/* Finds all shifts sh, where the letterplace objects t1 and
 * st2 := sh * t2 have a common divisor.
 * into shifts. Any previous contents of shifts will be 
 * deleted. Central Overlaps, i.e. shifts where st2 divides t1,
 * are not considered. The found shifts are stored in uint **
 * overlaps. The number of the found shifts, thus the size of
 * uint ** overlaps is returned.
 * Do we only get overlaps where sh * t2 is not violating the
 * maxDeg? How do we test that?
 * The current implementation does not yet care for maxDeg.
 * We should have to change maxElCompared.
 * overlaps have to be freed manually (with omFreeSize)!
 */
uint ShiftDVec::findRightOverlaps
  ( sTObject * t1, sTObject * t2, 
    int numVars, int maxDeg, uint ** overlaps )
{
  t1->SD_Ext()->SetDVecIfNULL(); t2->SD_Ext()->SetDVecIfNULL();
  //assume(t1->dvSize > 0 && t2->dvSize > 0);
  //TODO:
  //  t1->dvSize-1 should be sufficient because we dont want
  //  central overlaps
  *overlaps = (uint*)omAlloc0((t1->SD_Ext()->dvSize+1)*sizeof(uint));
  if(t1->SD_Ext()->dvSize == 0 || t2->SD_Ext()->dvSize == 0) return 0;

  memcpy
    ((void*)(*overlaps), (void*)t1->SD_Ext()->dvec, t1->SD_Ext()->dvSize*sizeof(uint));



  //uint maxElCompared = min(t1.dvSize, maxDeg); //not correct
  uint maxElCompared = t1->SD_Ext()->dvSize; //eventuell zuviele shifts

  /* We want only to find right overlappings, not central
   * overlappings.                                        */
  int shift =
    t2->SD_Ext()->dvSize > t1->SD_Ext()->dvSize ? 1 : t1->SD_Ext()->dvSize - t2->SD_Ext()->dvSize + 1;

  int cmpLen = (t1->SD_Ext()->dvSize - shift) * sizeof(uint);
  uint numberOfShifts = 0;
  for(uint i = 0; i < shift; ++i)
    (*overlaps)[shift] += (*overlaps)[i];
  (*overlaps)[shift] -= (shift * numVars);
  for( uint i = 0;
       shift < maxElCompared;
       ++shift, cmpLen -= sizeof(uint) )
  {
    if( !memcmp( (void*)((*overlaps)+shift),
                 (void*)t2->SD_Ext()->dvec,cmpLen) )
    {
      (*overlaps)[shift+1] = 
        (*overlaps)[shift] + (*overlaps)[shift+1] - numVars;
      (*overlaps)[i] = shift; i = ++numberOfShifts;
    } else {
      (*overlaps)[shift+1] = 
        (*overlaps)[shift] + (*overlaps)[shift+1] - numVars;
    }
  }

  if( numberOfShifts > 0 ){
    (*overlaps) = (uint*) omrealloc
      ( (*overlaps), numberOfShifts*sizeof(uint) );
    loGriToFile
      ( "Reallocation for overlaps in findRightOverlaps ",
        numberOfShifts*sizeof(uint), 1024, (*overlaps)     );
  }else{
    loGriToFile
      ( "omFreeSize for overlaps in findRightOverlaps ",
        (t1->SD_Ext()->dvSize+1)*sizeof(uint), 1024, (void*) *overlaps );
    omFreeSize
      ((ADDRESS)(*overlaps), sizeof(uint)*(t1->SD_Ext()->dvSize+1));
  }

  return numberOfShifts;
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
    uint shift2, uint shift3, SD::kStrategy strat, ring r )
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
  assume(shift2 > shift3 && shift3 > 0);

  int degH1 = H1->SD_Ext()->GetDVsize(); 
  int degH3 = H3->SD_Ext()->GetDVsize(); 

  //assume(degH3 + shift3 < H2->SD_Ext()->GetDVsize() + shift2);
  assume(degH3 + shift3 <= H2->SD_Ext()->GetDVsize() + shift2);

  int minCmpDV2 = degH1 - shift2;
  int minCmpDV3 = degH1 - shift3;
  int cmpLength = degH3 + shift3 - degH1;

  int dv1_eq_dv3 = H2->SD_Ext()->cmpDVecProper
    ( H3, minCmpDV2, minCmpDV3, cmpLength, strat->get_lV() );
  if( dv1_eq_dv3 == 0 ) return true;

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
    uint shift2, uint shift3, SD::kStrategy strat, ring r )
{
  //assume(shift2 > shift3 && shift3 > 0);
  assume(shift2 > shift3);
  assume( H1->SD_Ext()->GetDVsize() <
                          H2->SD_Ext()->GetDVsize() + shift2 );
  assume( H3->SD_Ext()->GetDVsize() + shift3 <
                          H2->SD_Ext()->GetDVsize() + shift2 );

  int minCmpDV1 = shift3;
  int minCmpDV3 = 0;
  int cmpLength = shift2 - shift3;

  int H1_eq_H3 = 
    H1->SD_Ext()->cmpDVecProper
      ( H3, minCmpDV1, minCmpDV3, cmpLength, strat->get_lV() );
  if( H1_eq_H3 == 0 ) return true;

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
#if DEBOGRI > 0
  static int counter;
  ++counter;
#endif

  if( sH2 == 0 ) return false;

  assume( H1->SD_Ext()->GetDVsize() > 0 );
  assume( H2->SD_Ext()->GetDVsize() > 0 );
  assume( H3->SD_Ext()->GetDVsize() > 0 );

  //maxShiftOfH3 = min(sizeOfLcmDVec - sizeOfH3DVec, sH2 - 1)
  //sizeOfLcmDVec = sizeOfH2DVec + sH2 - 1
  uint maxShiftOfH3;
  uint minShiftOfH3;
  if( H3->SD_Ext()->GetDVsize() > H2->SD_Ext()->GetDVsize() )
      //TODO: case h2 | h3
  {
    maxShiftOfH3 = H2->SD_Ext()->GetDVsize() +
                   sH2 - H3->SD_Ext()->GetDVsize();
  }
  else
    maxShiftOfH3 = sH2 - 1;

  int i = 0;
  if( H3->SD_Ext()->GetDVsize() > H1->SD_Ext()->GetDVsize() )
      //TODO: case h1 | h3
  {
    minShiftOfH3 = 1;
  }
  else
  {
    minShiftOfH3 = H1->SD_Ext()->GetDVsize() - H3->SD_Ext()->GetDVsize() + 1;
    for(; ovlH1H3[i] && minShiftOfH3 > ovlH1H3[i]; ++i)
      if(i >= numOvlH1H3)
      {
        loGriToFile("GMTest return 2 ", counter, 4, NULL);
        return false;
      }
  }

  int j;
  for( j = numOvlH3H2-1; j >= 0; --j )
    if( ovlH3H2[j] && ovlH3H2[j] + minShiftOfH3 <= sH2 ) break;
  if(j < 0) return false;


  if(ovlH1H3[i] > maxShiftOfH3) return false;

  uint sH3;
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
    ShiftDVec::LObject* H, int k, ShiftDVec::kStrategy strat      )
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
              shRight2 + Right2->SD_Ext()->GetDVsize() > 
              shRight1 + Right1->SD_Ext()->GetDVsize()   ) break;

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
              shLeft1 - Left1->SD_Ext()->GetDVsize() <=
              shLeft2 - Left1->SD_Ext()->GetDVsize()    ) break;

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
  ( ShiftDVec::LObject* H, ShiftDVec::kStrategy strat, int k, uint** rightOvls, 
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
      bool creation_of_spoly_violates_degree =
        SD::createSPviolatesDeg
          ( strat->S[i], H->p, shiftSi,
            strat->get_uptodeg(), currRing,
            currRing, strat->tailRing, strat->tailRing );
      bool shift_of_spoly_violates_degree =
        SD::shiftViolatesDeg
            ( strat->S[i], shiftSi,
              strat->get_uptodeg(), 
              currRing, strat->tailRing );
      if( shiftSi &&
          !shift_of_spoly_violates_degree &&
          !creation_of_spoly_violates_degree )
        //grico: checks if degbound is violated
      {
        deBoGriPrint("No degree Violation!", 1);
        int isFromQi = 0; //BOCO: hack TODO: clear
        LObject* Pair = SD::enterOnePair
          ( H->p, atR, isFromQ,
            ecart, strat->S[i],
            shiftSi, strat->S_2_R[i],
            strat->ecartS[i], isFromQi, strat );

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
  ( ShiftDVec::LObject* H,
    ShiftDVec::kStrategy strat, int k, uint** leftOvls,
    uint* sizesLeftOvls, int isFromQ, int ecart, int atR )
{
  namespace SD = ShiftDVec;

  bool new_pair;
  for (int i = 0; i <= k; i++)
  {
    for (int j = 0; j < sizesLeftOvls[i]; j++)
    {
      uint shiftH = leftOvls[i][j];
      bool creation_of_spoly_violates_degree =
        SD::createSPviolatesDeg
          ( H->p, strat->S[i],
            shiftH, strat->get_uptodeg(), currRing,
            currRing, strat->tailRing, strat->tailRing );
      bool shift_of_spoly_violates_degree =
        SD::shiftViolatesDeg
          ( H->p, shiftH,
            strat->get_uptodeg(), currRing, strat->tailRing );
      if( shiftH &&
          !shift_of_spoly_violates_degree &&
          !creation_of_spoly_violates_degree )
        //grico: checks if degbound is violated
      {
        int isFromQi = 0; //BOCO: hack TODO: clear
        SD::LObject* Pair = SD::enterOnePair
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
  ( ShiftDVec::LObject* H,
    ShiftDVec::kStrategy strat, uint* selfOvls,
    uint numSelfOvls, int isFromQ, int ecart, int atR )
{
  initDeBoGri
    ( ShiftDVec::indent, "Entering enterSelfOverlaps", 
      "Leaving enterSelfOverlaps", 4 + 1 + 128         );

  namespace SD = ShiftDVec;

  SD::LObject* Pair;

  bool new_pair = false;
  for (int j = 0; j < numSelfOvls; ++j)
  {
    uint shiftH = selfOvls[j];
    bool creation_of_spoly_violates_degree =
      SD::createSPviolatesDeg
        ( H->p, H->p, shiftH, strat->get_uptodeg(),
          currRing,currRing,strat->tailRing,strat->tailRing );
    bool shift_of_spoly_violates_degree =
      SD::shiftViolatesDeg
        ( H->p, shiftH,
          strat->get_uptodeg(), currRing, strat->tailRing );
    if( shiftH &&
        !shift_of_spoly_violates_degree &&
        !creation_of_spoly_violates_degree )
    {
      Pair = SD::enterOnePair
        ( H->p, atR, isFromQ, ecart, 
          H->p, shiftH, atR, isFromQ, ecart, strat );

      if(Pair != NULL)
      {
        //grico: sets new_pair to true, as in the old version
        new_pair=true; 
      }
    }
  }

#if 0 
  //Work in Progress - 
  //No: This is not nescessary any long, i think, but i'm all
  //but sure; TODO: clear this! Update: I think we have to
  //care about this case!!!
 
  /* BOCO: This applies Gebauer-Moeller on strat->L .
   * Formerly, this was part of the chainCrit function.
   * However, we might have to move this later into the loop
   * above, and test it for every entered Pair. This depends on
   * the the validity of our GM3 function. See TODO inside of
   * the GM3 function.                                       */
  if(new_pair){
    for (int l=strat->Ll; l>=0; l--){
      //Gebauer-Moeller Kriterium 3
      if (SD::GM3(H,&(strat->L[l]),strat))
        deleteInL(strat->L,&strat->Ll,l,strat);
    }
  }
#endif
}


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
  ( SD::LObject* H,
    SD::LObject* Pair,SD::kStrategy strat, ring r )
{
  /* BOCO: Explanation/Idea/TODO
   * Pair->lcm==NULL probably means, that the pair is one of
   * our initial generators. The criterion would thus not be
   * applicable on it.                                       */
  if( Pair->lcm==NULL ) return false;
  deBoGriPrint("The lcm of the pair is not NULL", 4 + 128);

  uint dvSize1 = H->SD_Ext()->GetDVsize();
  uint dvSize2 = strat->R[Pair->i_r1]->SD_Ext()->GetDVsize();
  uint dvSize3 = strat->R[Pair->i_r2]->SD_Ext()->GetDVsize();
  uint dv3sh = getShift( Pair->p2,
           strat->R[Pair->i_r2]->SD_Ext()->getDVecAtIndex(0) );

  //BOCO: I hope, this will get me the length of the overlap 
  //of (p2, p3) .
  uint ovSize = dvSize3 - dv3sh;

  if( ovSize >= dvSize1 ) return false;
  deBoGriPrint("Size of Overlap < length(DVec(p1)).", 128);

  uint StartPos = dvSize2 >= dvSize1 ? dvSize2-dvSize1+1 : 0;
  uint EndPos = dvSize2 - ovSize - 1;
  //if(EndPos + dvSize1 > degLcm) EndPos = degLcm - dvSize1;

  if( Pair->lcmDivisibleBy
        ( H, StartPos, EndPos, strat->get_lV() ) ) return true;

  return false;
}

/* BOCO: 
 * This is an adapted version of enterOnePairNormal. It will
 * enter a pair corresponding to a self overlap into B and
 * return a pointer to the pair entered. The entered pair is
 * (H->p, shift*H->p).
 * BOCO IMPORTANT:
 * Unfortunatly, The shift of H is not in R; we will link to an
 * unshiftet version.                                        */
ShiftDVec::LObject* ShiftDVec::enterOnePair
  ( poly p1, int atR1,
    int isFromQ1, int ecart1,
    poly p2, uint shift, int atR2,
    int isFromQ2, int ecart2, SD::kStrategy strat )
{
  if (strat->interred_flag) return NULL;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

  p2 = p_LPshiftT
    ( p2, shift, strat->get_uptodeg(),
      strat->get_lV(), strat, currRing );

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

#ifndef HAVE_RATGRING
  pLcm(p2,p1,Lp.lcm);
#elif defined(HAVE_RATGRING)
  // if (rIsRatGRing(currRing))
  pLcmRat(p2,p1,Lp.lcm, currRing->real_var_start);
#endif
  pSetm(Lp.lcm);

  /*
   * the pair (S[i],p) enters B if the spoly != 0
   */

  /*-  compute the short s-polynomial -*/

  if (strat->fromT && !TEST_OPT_INTSTRATEGY) pNorm(p2);

  if ((p1==NULL) || (p2==NULL))
    { if(p2){pDelete(&p2);} return NULL; }

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
    assume(strat->R[Lp.i_r1]->p == Lp.p1);
    Lp.SD_Object_Extension = NULL;
    return SD::enterL(&strat->B,&strat->Bl,&strat->Bmax,&Lp,l);
    //grico: pair is first saved to B, will be merged into L at
    //the end
  }

  return NULL;
}


/* BOCO: This was heavily modified. The alternate
 * Gebauer-Moeller variant was cut out. Gebauer Moeller was
 * partly extracted and moved to the GM3 Function, which is part
 * of the enterSelfOverlaps function.                         */
void ShiftDVec::chainCrit
  (ShiftDVec::LObject* H, int ecart, ShiftDVec::kStrategy strat)
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
            if ( strat->B[i].lcmDivisibleBy
                   (strat->S_2_T(j),strat->get_lV()) )
            {
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
    if (SD::compareChain(H,&(strat->L[j]),strat)) //renewed old procedure for new divisble tests
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
void ShiftDVec::kMergeBintoL(ShiftDVec::kStrategy strat)
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
    ShiftDVec::enterL
      ( &strat->L,&strat->Ll,&strat->Lmax,&(strat->B[i]),j );
  }
  strat->Bl = -1;
}
