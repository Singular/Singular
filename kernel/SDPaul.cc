/* BOCO:
 * This is the initenterpairs for the case, that we want to
 * calculate a GB of a left-ideal in the factor algebra K<X>/I.
 */
void ShiftDVec::Paul::initenterpairs
  ( LObject* H, ideal I, int size_of_I,
    int k, int ecart, int isFromQ, kStrategy strat, int atR )
{
  assume(strat->R[atR]->p == H->p);

  namespace SD = ShiftDVec;
  BOOLEAN new_pair=FALSE;

  if ( (strat->syzComp==0) || (pGetComp(H->p)<=strat->syzComp) ) 
    //grico: this doesn't seem to be a wrong assumpiton, so I
    //kept it
  { 
    /* BOCO: 
     *  We will look for those overlaps, where a p_i in I is
     *  overlapped by a p_j in S from the right. Other overlaps
     *  do not exist due to theoretical reasons.
     */
 
    //BOCO: arrays for overlaps
    uint** overlaps = (uint**)omAlloc(size_of_I*sizeof(uint*));

    //Sizes for above arrays
    uint* ovl_sizes = (uint*)omAlloc(size_of_I*sizeof(uint));

    //BOCO: Find the overlaps
    for (int j = 0; j <= size_of_I; j++)
      ovl_sizes[j] = 
        SD::Paul::findOverlaps
          ( H, strat->S_2_T(j), 
            strat->lV, strat->uptodeg, &(overlaps[j]) );

    SD::Paul::GebauerMoeller
      ( overlaps, ovl_size, H, I, size_of_I, strat );

    SD::Paul::enterOverlaps
      ( H,I,size_of_I,strat,k,
        overlaps, ovl_sizes, isFromQ, ecart, atR );

    //BOCO: Free arrays with overlaps
    for (int j = 0; j <= size_of_I; j++)
      if( ovl_sizes[j] )
        omFreeSize
          ((ADDRESS)overlaps[j], sizeof(uint)*r_ovl_sizes[j]);

    omFreeSize( (ADDRESS)overlaps, sizeof(uint)*size_of_I );
  }


  //This was formerly done in the chainCrit.
  SD::kMergeBintoL(strat); //merges the B set into the L set
}

/* BOCO:
 *  Gebauer Moeller for GB of left ideals in factor algebra
 *  K<X>/I.
 *
 * Remark1:
 *  Case 1 of non-commutative Gebauer Moeller (see paper) can
 *  be dropped.
 *
 *  picture:
 *
 *  iiiiiii    | lm(p_i)
 *     JJJJJJJ | lm(p_j)
 *    IIIIII   | lm(p'i)
 *
 *  p'i,p_j can be cancelled
 *
 *  possible cases:
 *
 *  1) p_i,p_j and p_j,p'i are both candidates for new pairs
 *  2) lm(p_i),lm(p_j) are lms of a pair from L
 *  3) lm(p'i),lm(p_j) are lms of a pair from L
 *     -> cancellation in L
 *
 * Remark2:
 *  See todo list for an idea on how we could do this more
 *  efficient.
 */
void ShiftDVec::Paul::GebauerMoeller
  ( uint** overlaps,
    uint* sizesOvls, LObject* J, kStrategy strat )
{
  namespace SP = ShiftDVec::Paul;

  //BOCO IMPORTANT Assumption:
  //overlaps in overlaps are sorted: The biggesst are coming
  //first! (TODO: I hope, that is true.)

#if 1 //under construction, un-uncomment, if errors occeur
  for(int i = 0; i < strat->size_of_I; ++i)
  {
    //Case 1
    for(int j = 0; j < strat->size_of_I; ++j)
    {
      //filter from overlaps[i] with Gebauer Moeller
      SP::GMFilter( J, //TODO: write it!
                    strat->Get_I_at(i),
                    strat->Get_I_at(j),
                    overlaps[i], overlaps[j],
                    sizesOvls[i], sizesOvls[j] );
    }

    //Case 2 & 3
    for(int j = 0; j <= strat->Ll; ++i)
    {
      SP::GMFilter( strat->L[j], //TODO: write it!
                    J, strat->Get_I_at(i),
                    overlaps[i], sizesOvls[i] );
    }
  }
#endif //if 1 - under construction

}

/* BOCO: Stuff it! Bloody Bastard.
 *  iiiiiii    | lm(p_i)
 *     JJJJJJJ | lm(p_j)
 *    kkkkkk   | lm(p_k)
 * p_i and p_k from I. i_ovls_j and i_ovls_j contain all shifts
 * which correspond to an overlap of lm(p_i) with lm(p_j) and
 * lm(p_k) with lm(p_j) respectivly. J is always overlapped
 * with and i from I from the left. See the theory for left-GB
 * for explanations. We want to filter the p_i - p_j pairs.
 * We expect i_ovls_j and k_ovls_j to be sorted (biggest shifts,
 * and thus smallest overlaps first).
 * TODO: Let this function get checked by Grischa!
 */
void ShiftDVec::Paul::GMFilter
  ( LObject* J, LObject* p_i, LObject* p_k,
    uint* i_ovls_j, uint* k_ovls_j,
    uint size_i_ovls_j, uint size_i_ovls_k )
{
  uint tg_lm_pi = p_i->GetDVsize();
  uint tg_lm_pj = J->GetDVsize();
  uint tg_lm_pk = p_k->GetDVsize();

  for(int i = 0; i < size_i_ovls_j; ++i)
  {
    uint tg_lcm_i_j = i_ovls_j[i] + tg_lm_pj;
    for(int k = size_k_ovls_j-1; k >= 0; --k)
    {
      // Test if lcm_k_j divides lcm_i_j
      if( k_ovls_j[k] > i_ovls_j[i] ) break;
      uint shift_k = i_ovls_j[i] - k_ovls_j[k];
      if( shift_k + tg_lm_pj > tg_lcm_i_j ) break;
      if( p_i->cmpDVecProper(p_k, shift_k, 0, k_ovls_j[i]) )
    }
  }
}

/* vim: set foldmethod=syntax: */
