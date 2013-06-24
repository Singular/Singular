#include <kutil.h>
#include <SDBase.h>
#include <SDReduce.h>

/* reduction procedure for the homogeneous case
 * and the case of a degree-ordering
 * original resides in kstd2.cc
 */
int ShiftDVec::redHomog( LObject* h, ::kStrategy strategy )
{
  namespace SD = ShiftDVec;
  namespace RD = ShiftDVec::Reduce;

  SD::kStrategy strat = static_cast<SD::kStrategy>(strategy);

  if (strat->tl<0) return 1;
  assume(h->FDeg == h->pFDeg());

  int i,j, pass, ii;
  unsigned long not_sev;
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg();
  h->SetShortExpVector();
  h->GetLmTailRing();
  int li;
  not_sev = ~ h->sev;
  loop
  {
    uint shift = 0;
    j = SD::kFindDivisibleByInT
          ( strat->T, strat->sevT, h, shift, strat );
    if (j < 0) return 1;

    li = strat->T[j].pLength;
    ii = j;
    i = j;

    //the polynomial to reduce with (up to the moment) is;
    //pi with length li

    if( TEST_OPT_LENGTH )
      ii = RD::red_get_opt_len( &shift,strat, i,li,not_sev, h );

    // end of search: have to reduce with pi
    RD::red_reduce_with( h, strat->T+ii, shift, strat );

    not_sev = RD::after_red( h );

    int ret;
    if( (ret = RD::red_with_S(h, strat, &pass)) ) return ret;
  }
}

/*- search the shortest possible with respect to length -*/
int ShiftDVec::Reduce::red_get_opt_len
  ( uint* shift, SD::kStrategy strat,
    int i, int li, unsigned long not_sev, LObject* h )
{
  int ii;
  poly h_p;
  ring r;
  h->GetLm(h_p, r);

  loop
  {
    i++;
    if (i > strat->tl)
      break;
    if (li<=1)
      break;
    /*BOCO:
     * The original p_LmShortDivisibleBy checks if second arg
     * divides first arg, we check if first arg is divisibly
     * by second arg! */
    if(strat->T[i].pLength < li){
      TObject t_h(h_p);
      TObject t_i(strat->T[i].GetLmTailRing());
      t_h.SD_Ext_Init();
      t_i.SD_Ext_Init();
      *shift = SD::p_LmShortDivisibleBy
        (&t_h, strat->sevT[i], &t_i, not_sev, strat->tailRing);
      /*
       * the polynomial to reduce with is now;
       */
      if(*shift < UINT_MAX)
      {
        li = strat->T[i].pLength;
        ii = i;
      }
    }
  }

  return ii;
}

void ShiftDVec::Reduce::debug_red_with_print( TObject* red,
                                              TObject* with )
{
#ifdef KDEBUG
  PrintS("red:");
  red->wrp();
  PrintS(" with ");
  with->wrp();
#endif
}

void ShiftDVec::Reduce::debug_red_to_print( LObject* to )
{
#ifdef KDEBUG
  PrintS("\nto ");
  h->wrp();
  PrintLn();
#endif
}

void
ShiftDVec::Reduce::red_reduce_with(LObject* red,
                                   TObject* with,
                                   uint shift, kStrategy strat)
{
  if (TEST_OPT_DEBUG) debug_red_with_print( red, with );
  assume(strat->fromT == FALSE);

  TObject tmp; //save the unshifted poly
  tmp.p = with->p;
  tmp.t_p = with->t_p;
  if(shift > 0)
  {
    with->t_p = p_LPshiftT
      ( tmp.t_p, shift, 
        strat->get_uptodeg(),
        strat->get_lV(), strat, strat->tailRing );
    with->p = p_LPshiftT
      ( tmp.p, shift, 
        strat->get_uptodeg(),
        strat->get_lV(), strat, currRing );
  }
  tmp.tailRing = with->tailRing; //BOCO: added...
  SD::ksReducePoly( red, &tmp, with );
  red->SD_Ext()->freeDVec();

  if(shift > 0)
  {
    p_Delete(&(with->t_p), strat->tailRing); 
    if(with->p)
    {
      with->p->next = NULL;
      pDelete(&(with->p));
    }

    with->p = tmp.p;
    with->t_p = tmp.t_p;
  }

#ifdef KDEBUG
  if (TEST_OPT_DEBUG) debug_red_to_print( h );
#endif
}

unsigned long ShiftDVec::Reduce::after_red( LObject* h )
{
  poly h_p = h->GetLmTailRing();

  if (h_p == NULL)
  {
    if (h->lcm!=NULL) pLmFree(h->lcm);

    h->SD_Ext()->freeDVec(); //BOCO TODO: is this necessary ?

#ifdef KDEBUG
      h->lcm=NULL;
#endif
    return 0;
  }

  h->SetShortExpVector();
  return ~ h->sev;
}

/*
 * try to reduce the s-polynomial h
 *test first whether h should go to the lazyset L
 *-if the degree jumps
 *-if the number of pre-defined reductions jumps
 */
int ShiftDVec::Reduce::red_with_S
  ( LObject* h, SD::kStrategy strat, int* pass )
{
  (*pass)++;
  if ( !TEST_OPT_REDTHROUGH &&
       (strat->Ll >= 0)     && (*pass > strat->LazyPass) )
  {
    h->SetLmCurrRing();
    int at = strat->posInL(strat->L,strat->Ll,h,strat);
    if (at <= strat->Ll)
    {
      int dummy=strat->sl; int shift_dummy;
      int j = SD::kFindDivisibleByInS( strat, &dummy,
                                       h, shift_dummy );
      if (j < 0) return 1;

      SD::enterL( &strat->L,&strat->Ll,&strat->Lmax,h,at );
#ifdef KDEBUG
      if (TEST_OPT_DEBUG) Print(" lazy: -> L%d\n",at);
#endif
      h->Clear();
      return -1;
    }
  }

  return 0;
}
