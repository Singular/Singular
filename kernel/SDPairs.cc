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
 *
 * \todo Maybe we should introduce a namespace 'Pair'?
 * \todo Test and debug procedures from this file
 * \todo Find out, which files we have to include
 * \todo The following procedures must be compared to their
 *       spielwiese version (and to the ShiftDVec version):
 *       - initenterpairs
 *       - kMergeBintoL
 *       - pLcm
 *       - those I have forgotten to list here
 */

#include <misc/options.h>

#include <kernel/SDBase.h>
#include <kernel/SDkutil.h>
#include <kernel/SDDebug.h>
#include <kernel/SDPairs.h>
#include <kernel/SDMultiplication.h>

/** Find and enter pairs (see ShiftDVed::initenterpairs)
 *
 * additional comment:
 * pos seems to be the position of h in S (see bba function)
 *
 * \todo write a bit about it, tell why it is efficient and valid
 *       to use clearS in that way
 * \todo improve doxygen documentation
 */
void ShiftDVec::enterpairs( LObject* H, int k,
                            int ecart, int pos,
                            SD::kStrategy strat, int atR )
{
  namespace SD = ShiftDVec;
  int j=pos;

#ifdef HAVE_RINGS
  assume (!rField_is_Ring(currRing));
#endif

  SD::initenterpairs(H,k,ecart,0,strat, atR);

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
      SD::clearS(H,h_sev, &j,&k,strat);
      j++;
    }
  }
}

/** Seems to be some heuristic for direct reduction after
 *  initenterpairs
 *
 *  I'm not really sure, what it does, I may have know once, but
 *  now I will not care.
 *
 *  Cares for shifts of p, as the divisibleBy functions do care
 *  for shifts.
 *
 *  \todo explanation of procedure and its parameters.
 */
void ShiftDVec::clearS( LObject* H,
                        unsigned long p_sev, int* at,
                        int* k, ShiftDVec::kStrategy strat )
{
  namespace SD = ShiftDVec;

  assume(p_sev == pGetShortExpVector(H->p));
  if (strat->noClearS) return;

  TObject T(strat->S[*at], currRing, strat->tailRing);  
  /* BOCO: maybe dvec and TObject already exist
   * -> we should improve that!!!! */

  uint shift = SD::p_LmShortDivisibleBy
            (&T,p_sev, H, ~ strat->sevS[*at], currRing);

  //BOCO: if T.dvec does not exist, then T.p ist a constant
  //      monomial, in which case shift will be UINT_MAX-1
  assume(!H->SD_Ext()->dvec || shift != UINT_MAX-1);

  //BOCO: if T.dvec == NULL, then T.p is a constant, which
  //TODO: implies R = 1 and we could stop here
  if ( H->SD_Ext()->dvec && shift > UINT_MAX-2 ) return;

  deleteInS((*at),strat);
  (*at)--;
  (*k)--;
}

/** Initialize everything, then enter Pairs into L
 *
 *  Allocate memory then find overlapping pairs, i.e. pairs, of
 *  the Form (S, s*H) or (H, s*S), where S and H have an overlap,
 *  such that S and s*H, where s*H is the Letterplace polynomial
 *  shifted by s, or s*S and H, respectively, have a common
 *  polynomial (actual polynomial, not a field element!) divisor.
 *  Then apply Gebauer Moeller criterions to filter out some
 *  pairs and enter the rest of the pairs into strat->L - they
 *  will eventually become new S-polynomials.
 *
 *  \remark
 *    We do no longer have the chainCrit() function from the
 *    original bba, instead we do the greatest filtration of
 *    pairs, after having the overlaps (better: the shifts
 *    defining the overlaps) and some additional filtration may
 *    be done afterwards in the enter[...]Overlaps procedures
 *    (this is yet a TODO). There is not much similiarity to the
 *    original chainCrit() procedure.
 *
 *  \todo Gebauer Moeller int the enter[...]Overlaps procedures
 *  \todo doxygen parameter definitions here
 *  \todo this procedure was rewritten and has to be tested again
 */
void ShiftDVec::initenterpairs
  ( LObject* H, int k, int ecart, 
    int isFromQ, SD::kStrategy strat, int atR )
{
  namespace SD = ShiftDVec;

  assume(strat->R[atR]->p == H->p);

  //grico: this doesn't seem to be a wrong assumpiton, so kept!
  if ( (strat->syzComp!=0) && (pGetComp(H->p)>strat->syzComp) ) 
    return;

  BOOLEAN new_pair=FALSE;

  // First we will find all overlaps of H with each
  // element of S. We use very large arrays of arrays. This
  // could be partially improved, by applying the
  // Gebauer-Moeller criterion as soon as possible. Maybe, we 
  // could change the findRightOverlaps function accordingly.
 
  // get us the overlaps!
  uint *s_overlaps, s_ovl_size;
  uint **r_overlaps, *r_ovl_sizes;
  uint **l_overlaps, *l_ovl_sizes;
  SD::AllocOvls( &l_overlaps, &l_ovl_sizes, k );
  SD::AllocOvls( &r_overlaps, &r_ovl_sizes, k );
  SD::FindOvls( H, &s_overlaps, &s_ovl_size, strat );     //self
  SD::FindOvls( H, 0, k, l_overlaps, l_ovl_sizes, strat );//left
  SD::FindOvls( H, 1, k, r_overlaps, r_ovl_sizes, strat );//right

  //Before entering new pairs, we want to use the Gebauer-Moeller
  //criterion, to filter some pairs, we do not need to enter.
  //Why does that not work in debug Version???
  if( TEST_OPT_LPDVGEBMOELL )
    SD::GebauerMoeller
      ( s_overlaps, s_ovl_size, 
        r_overlaps, r_ovl_sizes, 
        l_overlaps, l_ovl_sizes, H, k, strat );

  // enter left and right overlaps
  enterOverlaps( H, 1, strat, k,
                 r_overlaps, r_ovl_sizes, isFromQ, ecart, atR );
  enterOverlaps( H, 0, strat, k,
                 l_overlaps, l_ovl_sizes, isFromQ, ecart, atR );

  // enter self overlaps
  enterSelfOverlaps( H, strat, s_overlaps,
                     s_ovl_size, isFromQ, ecart, atR );

  // Free Arrays for Left and Right Overlaps
  FreeOvls( r_overlaps, r_ovl_sizes, k  );
  FreeOvls( l_overlaps, l_ovl_sizes, k  );

  // Free Array for Self Overlaps
  FreeOvlsPair( s_overlaps, s_ovl_size );

  //This was formerly done in the chainCrit:
  SD::kMergeBintoL(strat); //merges the B set into the L set
}

/** Allocate Memory for Left or Right Overlaps
 *
 *  The overlaps are defined through the corresponding pairs of
 *  polynomials and the shift applied to one of these
 *  polynomials.
 *
 *  In this function we allocate memory for the shifts which must
 *  be applied to H or the elements from strat->S
 *  ( see ShiftDVec::initenterpairs() ) when H is right
 *  overlapped by an element from strat->S or an element from
 *  strat->S is overlapped by H, respectively.
 *
 *  k+1 is the number of elements in strat->S.
 *
 *  \todo parameter defintions for doxygen
 */
void ShiftDVec::AllocOvls(uint*** overlaps, uint** sizes, int k)
{
  *sizes = NULL;
  *overlaps = NULL;

  if( k < 0 ) return; // no elements in the S Set yet

  *overlaps = (uint**) OMALLOC( k+1, uint* );
  *sizes    = (uint*)  OMALLOC( k+1, uint );
}

/** Free memory allocated for overlaps 
 *
 *  For more explanations see ShiftDVec::AllocOvls().
 */
void ShiftDVec::FreeOvls( uint** overlaps, uint* sizes, int k  )
{
  if( k < 0 ) return;

  for( int i = 0; i <= k; i++ )
    if( sizes[i] ) FreeOvlsPair( overlaps[i], sizes[i] );

  OMFREES( overlaps, k+1, uint* );
  OMFREES( sizes, k+1, uint );
}

/** Free memory allocated for overlaps (pair version)
 *
 *  This is only for one pair of non-commutative polynomials!
 *  (Which may result in several pairs of Letterplace
 *   polynomials.)
 */
void ShiftDVec::FreeOvlsPair( uint* overlaps, uint size  )
{
  if( !size ) return;

  OMFREES( overlaps, size, uint );
}

/** Find Overlaps of H with Elements from strat->S
 *
 * @param[in]   right   if true find overlaps, where H is
 *                      overlapped from the right by an element
 *                      of strat->S
 * @param[out]  ovls    array of arrays where the shifts
 *                      corresponding to the overlaps are stored
 *                      each subarray corresponds to a
 *                      non-commutative pair H, strat->S[i] for
 *                      an 0 <= i <= k (and thus several overlaps
 *                      may be found)
 * @param[out]  sizes   sizes[i] is the number of overlaps found
 *                      for the pair H, strat->S[i]
 *
 * \todo doxygen: explain parameters
 */
void ShiftDVec::FindOvls( LObject* H,
                          BOOLEAN right,
                          int k, uint** ovls,
                          uint* sizes, kStrategy strat )
{
  for (int i = 0; i <= k; i++)
  {
    TObject* R = right ? strat->S_2_T(i) : H;
    TObject* L = right ? H : strat->S_2_T(i);
    sizes[i] = 
      SD::findRightOverlaps( L, R, strat->get_lV(),
                             strat->get_uptodeg(), &(ovls[i]) );
  }
}

/** Find Overlaps of H with itself
 *
 * \todo doxygen: explain parameters
 */
void ShiftDVec::FindOvls( LObject* H,
                          uint** ovls,
                          uint* size, kStrategy strat )
{
  *size = SD::findRightOverlaps( H, H, strat->get_lV(),
                                 strat->get_uptodeg(), ovls );
}

/** Find right overlaps (t1 right overlapped by t2)
 *
 *  Finds all shifts sh, where the letterplace objects t1 and
 *  st2 := sh * t2 have a common divisor.
 *  into shifts. Any previous contents of shifts will be 
 *  deleted. Central Overlaps, i.e. shifts where st2 divides t1,
 *  are not considered. The found shifts are stored in uint **
 *  overlaps. The number of the found shifts, thus the size of
 *  uint ** overlaps is returned.
 *  Do we only get overlaps where sh * t2 is not violating the
 *  maxDeg? How do we test that?
 *  The current implementation does not yet care for maxDeg.
 *  We should have to change maxElCompared.
 *  overlaps have to be freed manually (with omFreeSize)!
 *
 *  \todo better doxygen docu ?
 *  \todo docu of parameters
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
  if(t1->SD_Ext()->dvSize == 0 || t2->SD_Ext()->dvSize == 0)
    return 0;
  *overlaps = (uint*) OMALLOC_0( t1->SD_Ext()->dvSize+1, uint );

  memcpy( (void*)(*overlaps),
          (void*)t1->SD_Ext()->dvec,
          t1->SD_Ext()->dvSize*sizeof(uint) );

  //uint maxElCompared = min(t1.dvSize, maxDeg); //not correct
  uint maxElCompared = t1->SD_Ext()->dvSize; //eventuell zuviele shifts

  /* We want only to find right overlappings, not central
   * overlappings.                                        */
  int shift = t1->SD_Ext()->dvSize - t2->SD_Ext()->dvSize + 1;
  if( t2->SD_Ext()->dvSize > t1->SD_Ext()->dvSize ) shift = 1;

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
    uint size_old = t1->SD_Ext()->dvSize+1;
    uint size_new = numberOfShifts;
    (*overlaps) =
      (uint*) OMREALLOCS( *overlaps, size_old, size_new, uint );
  }else{
    OMFREES( *overlaps, t1->SD_Ext()->dvSize+1, uint );
  }

  return numberOfShifts;
}

/** Gebauer Moeller for Letterplace (at least the main part of it)
 *
 *  Hints:
\verbatim
(xxxxxx is our H)

Case 1
self with self and self
xxxxxx
   xxxxxx
 xxxxxx

Case 2
self with right and left
xxxxxx
   xxxxxx
 yyyyyy

Case 3
right with self and right
xxxxxx
   yyyyyyyyy
  xxxxxx

Case 4
right with right and ?
xxxxxx
   yyyyyyyyy
  zzzzz

Case 5
left with left and self
yyyyyy
   xxxxxx
  xxxxxx

Case 6
left with ? and left
yyyyyy
   xxxxxx
  zzzzz

Case 7
? with left and right - not needed
yyyyyyy
   zzzzzzzz
  xxxxxx
This is the "L-Set" case ! (The only case, so far,
polynomials from the L-Set could be deleted.) But all pairs
for deleting the pair from L must have been already filtered
in  Case 4 or Case 6. Thus Case 7 needn't (and musn't) be
considered anymore,

Case 8
? with ? and ? - not needed
Another L-Set Case.
We do not need to consider that case, as this would have been
considered in earlier loops.
\endverbatim
 *
 *  \todo This function has to be reviewed and tested thoroughly.
 *  \todo explain this $?&X/§! function! Properly!
 *  \todo reflect once more, if we really can leave out those
 *        cases discussed above
 *  \todo split this procedure into several procedures
 */
void ShiftDVec::GebauerMoeller
  ( uint* selfOvls, uint sizeSelfOvls,
    uint** rightOvls, uint* sizesRightOvls,
    uint** leftOvls, uint* sizesLeftOvls, 
    LObject* H, int k, SD::kStrategy strat )
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

/** Used for some of the Gebauer Moeller Cases
 *
 * Suppose, we have three polynomials - h1, h2, h3 and that
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
 *
 * \todo Special cases: h1 | h3 and h2 | h3
 * \todo documentation
 */
bool ShiftDVec::GMTest
  ( TObject* H1,
    TObject* H2, uint sH2,
    TObject* H3, 
    uint* ovlH1H3, uint numOvlH1H3, 
    uint* ovlH3H2, uint numOvlH3H2  )
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

/** Used for one of the Gebauer-Moeller Cases 
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
 * \todo This function has to be checked!
 * \todo improve doxygen Documentation
 */
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

/** Used for one of the Gebauer-Moeller Cases
 *
 * This function is somehow similar to the GMTestRight function,
 * but somehow different. (I know, this is a great comment, so
 * please don't chop of my head.)
 *
 * \todo This function has to be checked!
 * \todo doxygen documentation
 */
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

/** @brief function to enter left or right overlaps into L
 *  
 *  This function will enter overlaps from H with elements from S
 *  into our Pair set L. This function can be invoked for right
 *  overlaps (H is overlapped from right) and left overlaps by
 *  setting from_right according to the following mapping:
 *  \code
    if    from_right  H is overlapped from the right
    else              H is overlapped from the left  \endcode
    \todo Do we have to apply Gebauer Moeller on L? Check out
          if and when to do it (for left or for right ovls?)
          Do also consider the notes to
          ShiftDVec::enterSelfOverlaps() and ShiftDVec::GM3().
    \todo Description of parameters.
    \todo This function was originally split into two functions
          but having one function is clearer. But it is yet not
          tested!
 */
void ShiftDVec::enterOverlaps
  ( LObject* H, BOOLEAN from_right,
    SD::kStrategy strat, int k, uint** ovls,
    uint* sizes_ovls, int isFromQ, int ecart, int atR )
{
  namespace SD = ShiftDVec;

  bool new_pair;
  poly pr, pl;
  int isFromQr, isFromQl, atRr, atRl, ecartr, ecartl;
  ring tRing = strat->tailRing; ring cRing = currRing;
  int uptodeg = strat->get_uptodeg();
  for (int i = 0; i <= k; i++)
  {
    // here we handle difference between left/right overlap case
    if( from_right ){
      pr = strat->S[i];
      pl = H->p;
      isFromQr = strat->fromQ ? strat->fromQ[i] : NULL;
      isFromQl = isFromQ;
      atRr     = strat->S_2_R[i];     atRl   = atR;
      ecartr   = strat->ecartS[i];    ecartl = ecart;
    }
    else{
      pr = H->p;
      pl = strat->S[i];
      isFromQr = isFromQ;
      isFromQl = strat->fromQ ? strat->fromQ[i] : NULL;
      atRr     = atR;                 atRl   = strat->S_2_R[i];
      ecartr   = ecart;               ecartl = strat->ecartS[i];
    }

    for (int j = 0; j < sizes_ovls[i]; j++)
    {
      uint shift = ovls[i][j];

      if( !shift ) continue; // pair was filtered by GM

      // Creation may violate the degree bound
      // If this funcion returns true, degree bound violation is
      // very likely, but not necessarily given, yet we will not
      // consider the pair (continue with next Iteration)
      if(
        SD::createSPviolatesDeg( pr, pl, shift, uptodeg,
                                 cRing, cRing, tRing, tRing ) ||
        SD::shiftViolatesDeg( pr, shift, uptodeg, cRing, tRing )
      ) continue;

      // In a former version we tested additionally, if the shift
      // of pr would violate the degree bound. However, that
      // would be inappropriate, especially, since we do not
      // store shifts any longer.

      deBoGriPrint("No degree Violation!", 1);

      //grico: sets new_pair to true, as in the old version
      if(
      SD::enterOnePair( pl, atRl, isFromQl, ecartl,
                        pr, shift, atRr, ecartr,isFromQr, strat )
      ) new_pair=true;
    }
  }

  //TODO: Gebauer Moeller on L, if new_pair (only if from_right,
  //      because otherwise I think we already considered all
  //      possible pairs, but check that out!)
}

/** Enter the self overlaps of H
 *
 *  We  will also apply the third criterion of
 *  Gebauer-Moeller to L, during this process.
 *  \remark
 *    Be aware, that in the normal bba, we have nothing to enter
 *    for the first time, we invoke initenterpairs. - There is no
 *    other polynomial to pair the first entered polynomial with.
 *    However, in the non-commutative case, our first entered
 *    polynomial might have self overlaps.
 *  \todo Clear TODO from the source code of this function.
 *  \todo Add doxygen parameter description
 */
void ShiftDVec::enterSelfOverlaps
  ( LObject* H,
    SD::kStrategy strat, uint* selfOvls,
    uint numSelfOvls, int isFromQ, int ecart, int atR )
{
  initDeBoGri
    ( ShiftDVec::indent, "Entering enterSelfOverlaps", 
      "Leaving enterSelfOverlaps", 4 + 1 + 128         );

  namespace SD = ShiftDVec;

  LObject* Pair;

  bool new_pair = false;
  int uptodeg = strat->get_uptodeg();
  ring tRing = strat->tailRing; ring cRing = currRing;
  for (int j = 0; j < numSelfOvls; ++j)
  {
    uint shiftH = selfOvls[j];
    if( !shiftH ) continue;

    // Creation may violate the degree bound
    // If this funcion returns true, degree bound violation is
    // very likely, but not necessarily given, yet we will not
    // consider the pair (continue with next Iteration)
    if(
      SD::createSPviolatesDeg( H->p, H->p, shiftH, uptodeg,
                               cRing, cRing, tRing, tRing ) ||
      SD::shiftViolatesDeg( H->p, shiftH, uptodeg, cRing, tRing )
    ) continue;

    // In a former version we tested additionally, if the shift
    // of pr would violate the degree bound. However, that
    // would be inappropriate, especially, since we do not
    // store shifts any longer.

    //grico: sets new_pair to true, as in the old version
    if(
    SD::enterOnePair( H->p, atR, isFromQ, ecart, 
                      H->p, shiftH, atR, isFromQ, ecart, strat )
    ) new_pair=true;
  }

  if(0) //TODO !!!
  {
    //TODO: This is a case we had excluded - until now
    //      But now I suppose, that we should consider it,
    //      if we want to get the most out of Gebauer-Moeller
   
    /* BOCO: This applies Gebauer-Moeller on strat->L .
     * Formerly, this was part of the chainCrit function.
     * However, we might have to move this later into the loop
     * above, and test it for every entered Pair. This depends on
     * the the validity of our GM3 function. See TODO inside of
     * the GM3 function.
     */ 
    if(new_pair){
      for (int l=strat->Ll; l>=0; l--){
        //Gebauer-Moeller Kriterium 3
        if (SD::GM3(H,&(strat->L[l]),strat))
          deleteInL(strat->L,&strat->Ll,l,strat);
      }
    }
  }
}

/** Gebauer Moeller Criterion Number 3
 *
 *  This is the third Kriterion of Gebauer-Moeller.  It is known
 *  as pCompareChain for the commutative case.  It is used in
 *  enterSelfOverlaps. 
 *
 *  Input is expected to be a TObject H, which corresponds to a
 *  polynomial p1 and an LObject Pair, which corresponds to a
 *  pair (p2, p3). We want to test, if (p2, p3) can be removed
 *  from the Pairset. This is true, in our case, if p1 | lcm(p2,
 *  p3) and p1 does not divide p2 or p3. In the implementation,
 *  we do not need to test the 2 latter conditions, if we
 *  restrict our divisibility test to the appropriate part of
 *  lcm(p2, p3).  This is the general Pairtest (not specialized
 *  for self-overlaps).  If the Kriterion applies, thus, (p2, p3)
 *  can be removed, true will be returned, false otherwise.
 *
 *  \todo The original comment stated, that this function is not
 *        needed any longer, whilst a comment in
 *        ShiftDVec::enterSelfOverlaps() tells a different story.
 *  \todo This function was never tested \todo Improve the
 *        Explanation: Why do we not explicitly need the pairs
 *        (p1, p2) and (p2, p3); etc.
 *  \todo Is the implementation of lcmDivisibleBy used in this
 *        function correct?
 *  \todo Clear TODO from the function body.
 *  \todo Create doxygen parameter Explanation.
 */
bool ShiftDVec::GM3
  ( LObject* H,
    LObject* Pair,SD::kStrategy strat, ring r )
{
  /* Explanation/Idea/TODO
   *
   * Pair->lcm==NULL probably means, that the pair is one of
   * our initial generators. The criterion would thus not be
   * applicable on it.
   */
  if( Pair->lcm==NULL ) return false;
  deBoGriPrint("The lcm of the pair is not NULL", 4 + 128);

  uint dvSize1 = H->SD_Ext()->GetDVsize();
  uint dvSize2 = strat->R[Pair->i_r1]->SD_Ext()->GetDVsize();
  uint dvSize3 = strat->R[Pair->i_r2]->SD_Ext()->GetDVsize();
  uint dv3sh = getShift( Pair->p2,
           strat->R[Pair->i_r2]->SD_Ext()->getDVecAtIndex(0) );

  //I hope, this will get me the length of the overlap of (p2,p3)
  uint ovSize = dvSize3 - dv3sh;

  if( ovSize >= dvSize1 ) return false;
  deBoGriPrint("Size of Overlap < length(DVec(p1)).", 128);

  uint StartPos = dvSize2 >= dvSize1 ? dvSize2-dvSize1+1 : 0;
  uint EndPos = dvSize2 - ovSize - 1;
  //if(EndPos + dvSize1 > degLcm) EndPos = degLcm - dvSize1;

  if( Pair->SD_LExt()->lcmDivisibleBy
        ( H, StartPos, EndPos, strat->get_lV() ) ) return true;

  return false;
}

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
 *  \code
 void enterOnePairNormal( int i,poly p,
                          int ecart, int isFromQ,
                          kStrategy strat, int atR = -1) \endcode
 *  ( Note to myself:
 *    No change to it in current Singular spielwiese since
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
 *  \todo Clear TODO from parameter description.
 *  \todo There is TODO left from the original implementation
 *  \todo Create a doxygen Link to enterOnePairNormal
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
  Lp.lcm = P_INIT( currRing );
  SD::p_Lcm( p1, p2, shift, Lp.lcm, strat, currRing );
  pSetm(Lp.lcm);

  /*
   * the pair (S[i],p) enters B if the spoly != 0
   */

  /*-  compute the short s-polynomial -*/

  if (strat->fromT && !TEST_OPT_INTSTRATEGY) pNorm(p2);

  if ((p1==NULL) || (p2==NULL))
    { if(p2){P_DELETE(&p2, currRing);} return NULL; }

  if ( (isFromQ1!=0) && (isFromQ2!=0) ) Lp.p=NULL;
  else{
    assume(!rIsPluralRing(currRing));
    Lp.p = SD::ksCreateShortSpoly( p1, p2, shift,
                                   strat->get_lV(),
                                   strat->tailRing );
  }

  if (Lp.p == NULL){if(Lp.lcm!=NULL) P_LMFREE(Lp.lcm, currRing);}
  else{
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = p1;
    Lp.p2 = p2;
    Lp.SD_LExt_Init()->shift_p2 = shift;

    assume(!rIsPluralRing(currRing));
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
    return SD::enterL(&strat->B,&strat->Bl,&strat->Bmax,&Lp,l);
    //grico: pair is first saved to B, will be merged into L at
    //the end
  }

  return NULL;
}

/** Calculate the Lcm of p1 and shift*p2
 *  \todo Add doxygen parameter description for this function.
 *  \todo For what do we need to actually compute the lcm of the
 *        leading monomial of the S-Poly? Do we really need it?
 *        Otherwise it would be better not to compute it, since
 *        it could violate the degree bound, where the computed
 *        S-poly doesn't.
 */
void ShiftDVec::p_Lcm( poly p1, poly p2,
                       uint shift_lm_p2,
                       poly return_lcm,
                       SD::kStrategy strat, ring r )
{
  assume( r == currRing );
  poly p2s = PMLP_SHIFT( p2, shift_lm_p2, strat, r );
#ifdef HAVE_RATGRING
  // Warning, not tested; will for sure crash for most examples,
  // since shift can get us out of uptodeg !!!
  pLcmRat( p2s, p1, return_lcm, r->real_var_start );
#else
  pLcm( p1, p2s, return_lcm );
#endif
  if(p2s) P_DELETE( &p2s, r );
}

/** Merge the temporary pair set B into the pair set L
 *
 *  This procedure is used at the end of initenterpairs, when the
 *  new pairs were entered into strat->B . strat->B will be
 *  merged into strat->L . The position of the new pairs in L
 *  will be determined by strat->posInL .
 * \todo document this function
 */
void ShiftDVec::kMergeBintoL( SD::kStrategy strat )
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

  ExtensionTTest(strat->L, strat->Ll);
}

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
