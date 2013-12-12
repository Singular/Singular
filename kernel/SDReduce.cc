/*! @file
 *
 * @brief Reduction of Letterplace Polynomials
 *
 * This file is part of the Letterplace ShiftDVec Project
 *
 * @author Grischa Studzinski
 * @author Benjamin Schnitzler benjaminschnitzler@googlemail.com
 *
 * @copyright see main copyright notice for Singular
 *
 * \todo Which procedures have to be adapted for spielwiese?
 * \todo Test and debug procedures from this file
 * \todo Find out, which files we have to include
 * \todo add documentation, where missing
 */

#include <kutil.h>
#include <SDDebug.h>
#include <SDBase.h>
#include <SDReduce.h>
#include <SDMultiplication.h>

#include <misc/options.h> // for TEST_OPT_LENGTH macro

/** @brief Reduction Procedure for the Homogeneous case
 *         and the case of a Degree-Ordering for Letterplace
 *
 *  This function was derived from redHomog.
 *
 *  We want to reduce h with elements from strat->T or strat->U
 *
 *  \todo explanation (of parameters, too)
 *  \todo explanation of strat->T and strat->U
 */
int ShiftDVec::redLP( LObject* h, ::kStrategy strategy )
{
  namespace SD = ShiftDVec;
  namespace RD = ShiftDVec::Reduce;

  ExtensionTTest(h, -2);

  SD::kStrategy strat = static_cast<SD::kStrategy>(strategy);

  if (strat->tl<0) return 1;
  assume(h->FDeg == h->pFDeg());

  int i,j, pass, ii;
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg();
  h->SetShortExpVector();
  h->GetLmTailRing();
  int li;
  loop
  {
    uint shift = 0;
    j = RD::kFindDivisibleByInUT( h, shift, strat );
    if (j < 0) return 1;

    li = strat->T[j].pLength;
    ii = j;
    i = j;

    //the polynomial to reduce with (up to the moment) is;
    //pi with length li

    if( TEST_OPT_LENGTH )
      ii = RD::red_get_opt_len( &shift,strat, i,li, h );

    // end of search: have to reduce with pi
    RD::red_reduce_with( h, strat->T+ii, shift, strat );

    if( !RD::after_red( h ) ) return 0;

    int ret;
    if( (ret = RD::red_with_US(h, strat, &pass)) ) return ret;
  }
}

/** @brief Search the shortest possible Divisor with respect to
 *         length in strat->T
 *
 *  \todo review and explain this procedure and its parameters
 */ 
int ShiftDVec::Reduce::red_get_opt_len
  (uint* shift, SD::kStrategy strat, int i, int li, LObject* h)
{
  unsigned long not_sev = ~ h->sev;
  int ii;
  poly h_p;
  ring r;
  h_p = h->GetLmTailRing();

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
  to->wrp();
  PrintLn();
#endif
}

/* Reduction Procedure for ShiftDVec Letterplace
 *
 * \todo explain function and parameters in detail
 */
void
ShiftDVec::Reduce::red_reduce_with( LObject* red,
                                    TObject* with,
                                    uint sh, kStrategy strat )
{
  if (TEST_OPT_DEBUG) debug_red_with_print( red, with );
  assume(strat->fromT == FALSE);

  TObject tmp; //save the unshifted poly
  tmp.p = with->p;
  tmp.t_p = with->t_p;
  if(sh > 0)
  {
    with->t_p = PLP_SHIFT_T(tmp.t_p, sh, strat, strat->tailRing);
    with->p   = PLP_SHIFT_T( tmp.p, sh, strat, currRing );
  }
  tmp.tailRing = with->tailRing; //BOCO: added...
  SD::ksReducePoly( red, &tmp, with, NULL, NULL, strat );
  red->SD_Ext()->freeDVec();

  if(sh > 0)
  {
    P_DELETE(&(with->t_p), strat->tailRing); 
    if(with->p)
    {
      with->p->next = NULL;
      P_DELETE(&(with->p), currRing);
    }

    with->p = tmp.p;
    with->t_p = tmp.t_p;
  }

#ifdef KDEBUG
  if (TEST_OPT_DEBUG) debug_red_to_print( red );
#endif
}

unsigned long ShiftDVec::Reduce::after_red( LObject* h )
{
  poly h_p = h->GetLmTailRing();

  if (h_p == NULL)
  {
    if (h->lcm!=NULL) P_LMFREE(h->lcm, currRing);

    h->SD_Ext()->freeDVec(); //BOCO TODO: is this necessary ?

#ifdef KDEBUG
      h->lcm=NULL;
#endif
    return 0;
  }

  h->SetShortExpVector();
  return 1;
}

/*
 * try to reduce the s-polynomial h
 *test first whether h should go to the lazyset L
 *-if the degree jumps
 *-if the number of pre-defined reductions jumps
 */
int ShiftDVec::Reduce::red_with_US
  ( LObject* h, SD::kStrategy strat, int* pass )
{
  namespace RD = ShiftDVec::Reduce;

  (*pass)++;
  if ( !TEST_OPT_REDTHROUGH &&
       (strat->Ll >= 0)     && (*pass > strat->LazyPass) )
  {
    h->SetLmCurrRing();
    int at = strat->posInL(strat->L,strat->Ll,h,strat);
    if (at <= strat->Ll)
    {
      uint shift_dummy;
      int j = RD::kFindDivisibleByInUS(h, shift_dummy, strat );
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


/* BOCO: 
 * This version of the function is used in redtailBba.
 * It overloads its non-dvec version.
 *
 * WARNING:
 * The Returned object may have to be shifted, to devide L,
 * therefor shift will be set.
 * If uptodeg == 0, we will not care, if the reduction of L->p
 * with a found poly p in S violates the degree bound,
 * otherwise we do (by not returning p, if the reduction
 * violates the degree bound).
 */
TObject * ShiftDVec::Reduce::kFindDivisibleByInS
  ( SD::kStrategy strat, int pos,
    LObject* L, TObject* T, 
    uint & shift, int lV, int uptodeg, long ecart )
{
  namespace SD = ShiftDVec;

  int j = 0;

  const unsigned long not_sev = ~L->sev;
  const unsigned long* sev = strat->sevS;

  poly p;
  ring r;
  L->GetLm(p, r);
  L->SD_Ext()->SetDVecIfNULL(p, r);

  if (r == currRing)
  {
    loop
    {
      if (j > pos) return NULL;
      if ( strat->tl < 0 || strat->S_2_R[j] == -1 )
      {
        /*BOCO:
         * The original p_LmDivisibleBy checks if second arg
         * divides first arg, we check if first arg is divisibly
         * by second arg! */
        sTObject t(strat->S[j]);
        t.SD_Ext_Init()->SetDVec();
        shift = p_LmDivisibleBy( L, &t, r, lV,
                                 strat->is_lgb_case() );
      }
      else
      {
        strat->S_2_T(j)->SD_Ext_Init()->SetDVecIfNULL();
        shift = p_LmDivisibleBy( L, strat->S_2_T(j),
                                 r, lV, strat->is_lgb_case() );
      }
      if ( shift < UINT_MAX && 
           ( strat->homog ||
             !redViolatesDeg
               ( L->p, strat->S[j], uptodeg,
                 currRing, currRing, strat->tailRing ) ) )
      { break; }
      j++;
    }
    // if called from NF, T objects do not exist:
    if (strat->tl < 0 || strat->S_2_R[j] == -1)
    {
      T->Set(strat->S[j], r, strat->tailRing);
      return T;
    }
    else return strat->S_2_T(j);
  }
  else
  {
    TObject* t;
    loop
    {
      if (j > pos) return NULL;
      assume(strat->S_2_R[j] != -1);
      /*BOCO: TODO: Why do we have Problems with this?
      if ( !(ecart== LONG_MAX || ecart>= strat->ecartS[j]) )
      */
      if(true)
      {
        t = strat->S_2_T(j);
        assume( t != NULL && t->t_p != NULL &&
                t->tailRing == r && t->p == strat->S[j] );
        strat->S_2_T(j)->SD_Ext_Init()
                       ->SetDVecIfNULL(t->t_p, r);
        /*BOCO:
         * The original p_LmDivisibleBy checks if second arg
         * divides first arg, we check if first arg is divisibly
         * by second arg! */
        shift = p_LmDivisibleBy( L, t, r, lV,
                                 strat->is_lgb_case() );
        if ( shift < UINT_MAX && 
             ( strat->homog || 
               !redViolatesDeg(p, t->t_p, uptodeg, r, r, r) ) )
        { return t; }
      }
      j++;
    }
  }

/*BOCO: - undefined return value? -
 *  I think it may happen, that the return Value of this
 *  function is undefined. Maybe it would be clearer to return
 *  something
 return NULL;
 */
}

int ShiftDVec::Reduce::kFindDivisibleByInT
  ( const TSet &T, int tlen,
    const unsigned long* sevT, LObject * L, 
    uint & shift, SD::kStrategy strat, const int start )
{
  namespace SD = ShiftDVec;

  ExtensionTTest(T, tlen);
  ExtensionTTest(L, -2);

  unsigned long not_sev = ~L->sev;

  int j = start;
  int ret;  //BOCO: added

  poly p=L->p;
  ring r=currRing;

  //BOCO: TODO: 
  //I think the following line is souperflous -> remove it!
  if (p==NULL)  { r=L->tailRing; p=L->t_p; }

  L->GetLm(p, r);

  //BOCO: added following line
  L->SD_Ext_Init_If_NULL();

  poly TObject::*pp;
  pp = (r == currRing ? &TObject::p : &TObject::t_p);

  int lV = strat->get_lV();
  int uptodeg = strat->get_uptodeg();
  int get_right_divisor = strat->is_lgb_case() && T==strat->T;
  loop
  {
    if (j > tlen) {ret = -1; break;};
    shift = L->SD_Ext()->divisibleBy_Comp( &T[j], pp, r, lV,
                                           get_right_divisor );
    int rvd = redViolatesDeg(L, &T[j], uptodeg, currRing);
    if ( shift < UINT_MAX && (strat->homog || !rvd) )
    { ret = j; break; }
    j++;
  }

  return ret;  //BOCO: added
}

/** @brief find a divisor in strat->T or strat->U
 *
 *  \todo I have forgotten, what strat->U is. An additional set
 *        of divisors?
 *  \todo explain this function and its parameters (or let it be,
 *        since an explanation for kFindDivisibleByInT would be
 *        sufficient).
 */
int ShiftDVec::Reduce::kFindDivisibleByInUT
  ( LObject* h, uint& shift, SD::kStrategy strat )
{
  namespace RD = ShiftDVec::Reduce;

  int j = RD::kFindDivisibleByInT
           (strat->T, strat->tl, strat->sevT, h, shift, strat);

  if( j >= 0 || !strat->is_lgb_case() ) return j;

  return
    RD::kFindDivisibleByInT( strat->U,
                             strat->get_size_of_U()-1,
                             strat->sevT, h, shift, strat );
}

int ShiftDVec::Reduce::kFindDivisibleByInUS
  ( LObject* h, uint& shift, SD::kStrategy strat )
{
  namespace RD = ShiftDVec::Reduce;

  int j = SD::kFindDivisibleByInS( strat,
                                   &(strat->sl), h, shift );

  if( j >= 0 || !strat->is_lgb_case() ) return j;

  return
    RD::kFindDivisibleByInT( strat->U,
                             strat->get_size_of_U()-1,
                             strat->sevT, h, shift, strat );
}

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
