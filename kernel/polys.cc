#include "kernel/mod2.h"

#include "misc/options.h"

#include "polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/clapsing.h"
#include "polys/clapconv.h"

/// Widely used global variable which specifies the current polynomial ring for Singular interpreter and legacy implementations.
/// @Note: one should avoid using it in newer designs, for example due to possible problems in parallelization with threads.
VAR ring  currRing = NULL;
VAR coeffs coeffs_BIGINT;

void rChangeCurrRing(ring r)
{
  if (currRing!=NULL)
    currRing->options = si_opt_1 & TEST_RINGDEP_OPTS;
  //------------ set global ring vars --------------------------------
  currRing = r;
  if( r != NULL )
  {
    rTest(r);
    //------------ global variables related to coefficients ------------
    assume( r->cf!= NULL );
    nSetChar(r->cf);
    //------------ global variables related to polys
    p_SetGlobals(r); // also setting TEST_RINGDEP_OPTS
    //------------ global variables related to factory -----------------
  }
}

static long pPowerModMaxTotaldegree(poly p, const ring r)
{
  long degree=0;
  while (p!=NULL)
  {
    degree=si_max(degree,p_Totaldegree(p,r));
    pIter(p);
  }
  return degree;
}

static BOOLEAN pPowerModOverflow(poly left, poly right, const ring r)
{
  const long left_degree=pPowerModMaxTotaldegree(left,r);
  const long right_degree=pPowerModMaxTotaldegree(right,r);
  const long max_degree=si_max((long)rVar(r),(long)r->bitmask/2);
  BOOLEAN overflow=(left_degree>max_degree-right_degree);

  if (!overflow)
  {
    poly left_max=p_GetMaxExpP(left,r);
    poly right_max=p_GetMaxExpP(right,r);
    overflow=!p_LmExpVectorAddIsOk(left_max,right_max,r);
    p_LmFree(left_max,r);
    p_LmFree(right_max,r);
  }

  if (!overflow) return FALSE;

  Werror("OVERFLOW in quotient power(d=%ld, d=%ld, max=%ld)",
         left_degree,right_degree,max_degree);
  return TRUE;
}

static poly pPowerModMultiply(poly left, poly right, ideal G, const ring r)
{
  if ((left==NULL) || (right==NULL))
    return p_Mult_q(left,right,r);

  if (pPowerModOverflow(left,right,r))
  {
    p_Delete(&left,r);
    p_Delete(&right,r);
    return NULL;
  }

  poly product=p_Mult_q(left,right,r);
  poly result=kNF(G,NULL,product);
  p_Delete(&product,r);
  return result;
}

static poly pPowerModCurrent(poly base, int exp, ideal G, const ring r)
{
  if (exp==0)
  {
    p_Delete(&base,r);
    poly one=p_One(r);
    poly result=kNF(G,NULL,one);
    p_Delete(&one,r);
    return result;
  }
  if (base==NULL) return NULL;

  poly reduced=kNF(G,NULL,base);
  p_Delete(&base,r);
  base=reduced;

  if ((base==NULL) || (exp==1)) return base;

  poly result=NULL;
  while (exp>0)
  {
    if ((exp & 1)!=0)
    {
      poly factor;
      if (exp==1)
      {
        factor=base;
        base=NULL;
      }
      else
        factor=p_Copy(base,r);

      if (result==NULL)
        result=factor;
      else
      {
        result=pPowerModMultiply(result,factor,G,r);
        if (result==NULL)
        {
          p_Delete(&base,r);
          return NULL;
        }
      }
    }

    exp>>=1;
    if (exp!=0)
    {
      base=pPowerModMultiply(base,p_Copy(base,r),G,r);
      if (base==NULL)
      {
        p_Delete(&result,r);
        return NULL;
      }
    }
  }
  p_Delete(&base,r);
  return result;
}

poly p_PowerMod(poly base, int exp, ideal G, const ring r)
{
  if ((G==NULL) || idIs0(G) || (exp<0))
    return p_Power(base,exp,r);
  if (rIsNCRing(r))
  {
    WerrorS("quotient power is not implemented for non-commutative rings");
    p_Delete(&base,r);
    return NULL;
  }

  const ring save_ring=currRing;
  if (r!=currRing) rChangeCurrRing(r);
  poly result=pPowerModCurrent(base,exp,G,r);
  if (r!=save_ring) rChangeCurrRing(save_ring);
  return result;
}

poly p_Divide(poly p, poly q, const ring r)
{
  assume(q!=NULL);
  if (q==NULL)
  {
    WerrorS("div. by 0");
    return NULL;
  }
  if (p==NULL)
  {
    p_Delete(&q,r);
    return NULL;
  }
  if ((pNext(q)!=NULL)||rIsPluralRing(r))
  { /* This means that q != 0 consists of at least two terms*/
    if(p_GetComp(p,r)==0)
    {
      if (!rIsNCRing(r))
      {
        if ((rFieldType(r)==n_Q)
        ||(rFieldType(r)==n_Zp)
        ||(rFieldType(r)==n_algExt))
        {
          poly res=singclap_pdivide(p, q, r);
          p_Delete(&p,r);
          p_Delete(&q,r);
          return res;
        }
        if((rFieldType(r)==n_transExt)
        &&(convSingTrP(p,r))
        &&(convSingTrP(q,r)))
        {
          poly res=singclap_pdivide(p, q, r);
          p_Delete(&p,r);
          p_Delete(&q,r);
          return res;
        }
      }
      // generic division for poly
      {
        ideal vi=idInit(1,1); vi->m[0]=q;
        ideal ui=idInit(1,1); ui->m[0]=p;
        ideal R; matrix U;
        ring save_ring=currRing;
        if (r!=currRing) rChangeCurrRing(r);
        BITSET save_opt;
        SI_SAVE_OPT1(save_opt);
        si_opt_1 &= ~(Sy_bit(OPT_PROT));
        ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
        SI_RESTORE_OPT1(save_opt);
        if (r!=save_ring) rChangeCurrRing(save_ring);
        matrix T = id_Module2formatedMatrix(m,1,1,r);
        p=MATELEM(T,1,1); MATELEM(T,1,1)=NULL;
        id_Delete((ideal *)&T,r);
        id_Delete((ideal *)&U,r);
        id_Delete(&R,r);
        //vi->m[0]=NULL; ui->m[0]=NULL;
        id_Delete(&vi,r);
        id_Delete(&ui,r);
        return p;
      }
    }
    else
    {
      int comps=p_MaxComp(p,r);
      ideal I=idInit(comps,1);
      poly h;
      int i;
      // conversion to a list of polys:
      while (p!=NULL)
      {
        i=p_GetComp(p,r)-1;
        h=pNext(p);
        pNext(p)=NULL;
        p_SetComp(p,0,r);
        I->m[i]=p_Add_q(I->m[i],p,r);
        p=h;
      }
      // division and conversion to vector:
      h=NULL;
      p=NULL;
      for(i=comps-1;i>=0;i--)
      {
        if (I->m[i]!=NULL)
        {
          if((rFieldType(r)==n_transExt)
          &&(convSingTrP(I->m[i],r))
          &&(convSingTrP(q,r))
          &&(!rIsNCRing(r)))
          {
            h=singclap_pdivide(I->m[i],q,r);
          }
          else if (((rFieldType(r)==n_Q)
          ||(rFieldType(r)==n_Zp)
          ||(rFieldType(r)==n_algExt))
          &&(!rIsNCRing(r)))
            h=singclap_pdivide(I->m[i],q,r);
          else
          {
            ideal vi=idInit(1,1); vi->m[0]=q;
            ideal ui=idInit(1,1); ui->m[0]=I->m[i];
            ideal R; matrix U;
            ring save_ring=currRing;
            if (r!=currRing) rChangeCurrRing(r);
            BITSET save_opt;
            SI_SAVE_OPT1(save_opt);
            si_opt_1 &= ~(Sy_bit(OPT_PROT));
            ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
            SI_RESTORE_OPT1(save_opt);
            if (r!=save_ring) rChangeCurrRing(save_ring);
            if (idIs0(R))
            {
              matrix T = id_Module2formatedMatrix(m,1,1,r);
              p=MATELEM(T,1,1); MATELEM(T,1,1)=NULL;
              id_Delete((ideal *)&T,r);
            }
            else p=NULL;
            id_Delete((ideal*)&U,r);
            id_Delete(&R,r);
            vi->m[0]=NULL; ui->m[0]=NULL;
            id_Delete(&vi,r);
            id_Delete(&ui,r);
          }
          p_SetCompP(h,i+1,r);
          p=p_Add_q(p,h,r);
        }
      }
      id_Delete(&I,r);
      p_Delete(&q,r);
      return p;
    }
  }
  else
  { /* This means that q != 0 consists of just one term, or LetterPlace */
    if (pNext(q)!=NULL)
    {
      WerrorS("division over a coefficient domain only implemented for terms");
      return NULL;
    }
    return p_DivideM(p,q,r);
  }
  return NULL;
}

poly pp_Divide(poly p, poly q, const ring r)
{
  if (q==NULL)
  {
    WerrorS("div. by 0");
    return NULL;
  }
  if (p==NULL)
  {
    return NULL;
  }
  if ((pNext(q)!=NULL)||rIsPluralRing(r))
  { /* This means that q != 0 consists of at least two terms*/
    if(p_GetComp(p,r)==0)
    {
      if (!rIsNCRing(r))
      {
        if ((rFieldType(r)==n_Q)
        ||(rFieldType(r)==n_Zp)
        ||(rFieldType(r)==n_algExt))
        {
          poly res=singclap_pdivide(p, q, r);
          return res;
        }
        if((rFieldType(r)==n_transExt)
        &&(convSingTrP(p,r))
        &&(convSingTrP(q,r)))
        {
          poly res=singclap_pdivide(p, q, r);
          return res;
        }
      }
      // generic division for poly
      {
        ideal vi=idInit(1,1); vi->m[0]=p_Copy(q,r);
        ideal ui=idInit(1,1); ui->m[0]=p_Copy(p,r);
        ideal R; matrix U;
        ring save_ring=currRing;
        if (r!=currRing) rChangeCurrRing(r);
        BITSET save_opt;
        SI_SAVE_OPT1(save_opt);
        si_opt_1 &= ~(Sy_bit(OPT_PROT));
        ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
        SI_RESTORE_OPT1(save_opt);
        if (r!=save_ring) rChangeCurrRing(save_ring);
        matrix T = id_Module2formatedMatrix(m,1,1,r);
        p=MATELEM(T,1,1); MATELEM(T,1,1)=NULL;
        id_Delete((ideal *)&T,r);
        id_Delete((ideal *)&U,r);
        id_Delete(&R,r);
        //vi->m[0]=NULL; ui->m[0]=NULL;
        id_Delete(&vi,r);
        id_Delete(&ui,r);
        return p;
      }
    }
    else
    {
      p=p_Copy(p,r);
      int comps=p_MaxComp(p,r);
      ideal I=idInit(comps,1);
      poly h;
      int i;
      // conversion to a list of polys:
      while (p!=NULL)
      {
        i=p_GetComp(p,r)-1;
        h=pNext(p);
        pNext(p)=NULL;
        p_SetComp(p,0,r);
        I->m[i]=p_Add_q(I->m[i],p,r);
        p=h;
      }
      // division and conversion to vector:
      h=NULL;
      p=NULL;
      q=p_Copy(q,r);
      for(i=comps-1;i>=0;i--)
      {
        if (I->m[i]!=NULL)
        {
          if(!rIsNCRing(r))
          {
            if((rFieldType(r)==n_transExt)
            &&(convSingTrP(I->m[i],r))
            &&(convSingTrP(q,r)))
            {
              h=singclap_pdivide(I->m[i],q,r);
            }
            else if ((rFieldType(r)==n_Q)
            ||(rFieldType(r)==n_Zp))
              h=singclap_pdivide(I->m[i],q,r);
            else
            {
              ideal vi=idInit(1,1); vi->m[0]=q;
              ideal ui=idInit(1,1); ui->m[0]=I->m[i];
              ideal R; matrix U;
              ring save_ring=currRing;
              if (r!=currRing) rChangeCurrRing(r);
              BITSET save_opt;
              SI_SAVE_OPT1(save_opt);
              si_opt_1 &= ~(Sy_bit(OPT_PROT));
              ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
              SI_RESTORE_OPT1(save_opt);
              if (r!=save_ring) rChangeCurrRing(save_ring);
              if (idIs0(R))
              {
                matrix T = id_Module2formatedMatrix(m,1,1,r);
                p=MATELEM(T,1,1); MATELEM(T,1,1)=NULL;
                id_Delete((ideal *)&T,r);
              }
              id_Delete((ideal*)&U,r);
              id_Delete(&R,r);
              vi->m[0]=NULL; ui->m[0]=NULL;
              id_Delete(&vi,r);
              id_Delete(&ui,r);
            }
          }
          else
          {
            ideal vi=idInit(1,1); vi->m[0]=q;
            ideal ui=idInit(1,1); ui->m[0]=I->m[i];
            ideal R; matrix U;
            ring save_ring=currRing;
            if (r!=currRing) rChangeCurrRing(r);
            BITSET save_opt;
            SI_SAVE_OPT1(save_opt);
            si_opt_1 &= ~(Sy_bit(OPT_PROT));
            ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
            SI_RESTORE_OPT1(save_opt);
            if (r!=save_ring) rChangeCurrRing(save_ring);
            if (idIs0(R))
            {
              matrix T = id_Module2formatedMatrix(m,1,1,r);
              p=MATELEM(T,1,1); MATELEM(T,1,1)=NULL;
              id_Delete((ideal *)&T,r);
            }
            id_Delete((ideal*)&U,r);
            id_Delete(&R,r);
            vi->m[0]=NULL; ui->m[0]=NULL;
            id_Delete(&vi,r);
            id_Delete(&ui,r);
          }
          p_SetCompP(h,i+1,r);
          p=p_Add_q(p,h,r);
        }
      }
      id_Delete(&I,r);
      p_Delete(&q,r);
      return p;
    }
  }
  else
  { /* This means that q != 0 consists of just one term,
       or that r is over a coefficient ring. */
    if (pNext(q)!=NULL)
    {
      WerrorS("division over a coefficient domain only implemented for terms");
      return NULL;
    }
    return pp_DivideM(p,q,r);
  }
  return NULL;
}

poly p_DivRem(poly p, poly q, poly &rest, const ring r)
{
  assume(q!=NULL);
  rest=NULL;
  if (q==NULL)
  {
    WerrorS("div. by 0");
    return NULL;
  }
  if (p==NULL)
  {
    p_Delete(&q,r);
    return NULL;
  }
  if(p_GetComp(p,r)==0)
  {
    if((rFieldType(r)==n_transExt)
    &&(convSingTrP(p,r))
    &&(convSingTrP(q,r))
    &&(!rIsNCRing(r)))
    {
      poly res=singclap_pdivide(p, q, r);
      rest=singclap_pmod(p,q,r);
      p_Delete(&p,r);
      p_Delete(&q,r);
      return res;
    }
    else if ((r->cf->convSingNFactoryN!=ndConvSingNFactoryN)
    &&(!rField_is_Ring(r))
    &&(!rIsNCRing(r)))
    {
      poly res=singclap_pdivide(p, q, r);
      rest=singclap_pmod(p,q,r);
      p_Delete(&p,r);
      p_Delete(&q,r);
      return res;
    }
    else
    {
      ideal vi=idInit(1,1); vi->m[0]=q;
      ideal ui=idInit(1,1); ui->m[0]=p;
      ideal R; matrix U;
      ring save_ring=currRing;
      if (r!=currRing) rChangeCurrRing(r);
      BITSET save_opt;
      SI_SAVE_OPT1(save_opt);
      si_opt_1 &= ~(Sy_bit(OPT_PROT));
      ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
      SI_RESTORE_OPT1(save_opt);
      if (r!=save_ring) rChangeCurrRing(save_ring);
      p=m->m[0]; m->m[0]=NULL;
      id_Delete(&m,r);
      p_SetCompP(p,0,r);
      rest=R->m[0]; R->m[0]=NULL;
      id_Delete(&R,r);
      p_SetCompP(rest,0,r);
      id_Delete((ideal *)&U,r);
      //vi->m[0]=NULL; ui->m[0]=NULL;
      id_Delete(&vi,r);
      id_Delete(&ui,r);
      return p;
    }
  }
  return NULL;
}

poly singclap_gcd ( poly f, poly g, const ring r )
{
  poly res=NULL;

  if (f!=NULL)
  {
    //if (r->cf->has_simple_Inverse) p_Norm(f,r);
    if (rField_is_Zp(r))          p_Norm(f,r);
    else if (!rField_is_Ring(r))  p_Cleardenom(f, r);
  }
  if (g!=NULL)
  {
    //if (r->cf->has_simple_Inverse) p_Norm(g,r);
    if (rField_is_Zp(r))          p_Norm(g,r);
    else if (!rField_is_Ring(r))  p_Cleardenom(g, r);
  }
  else         return f; // g==0 => gcd=f (but do a p_Cleardenom/pNorm)
  if (f==NULL) return g; // f==0 => gcd=g (but do a p_Cleardenom/pNorm)
  if(!rField_is_Ring(r)
  && (p_IsConstant(f,r)
  ||p_IsConstant(g,r)))
  {
    res=p_One(r);
  }
  else if (r->cf->convSingNFactoryN!=ndConvSingNFactoryN)
  {
    res=singclap_gcd_r(f,g,r);
  }
  else
  {
    ideal I=idInit(2,1);
    I->m[0]=f;
    I->m[1]=p_Copy(g,r);
    intvec *w=NULL;
    ring save_ring=currRing;
    if (r!=currRing) rChangeCurrRing(r);
    BITSET save_opt;
    SI_SAVE_OPT1(save_opt);
    si_opt_1 &= ~(Sy_bit(OPT_PROT));
    ideal S1=idSyzygies(I,testHomog,&w);
    if (w!=NULL) delete w;
    // expect S1->m[0]=(-g/gcd,f/gcd)
    if (IDELEMS(S1)!=1) WarnS("error in syzygy computation for GCD");
    int lp;
    p_TakeOutComp(&S1->m[0],1,&res,&lp,r);
    p_Delete(&S1->m[0],r);
    // GCD is g divided iby (-g/gcd):
    res=p_Divide(g,res,r);
    // restore, r, opt:
    SI_RESTORE_OPT1(save_opt);
    if (r!=save_ring) rChangeCurrRing(save_ring);
    // clean the result
    res=p_Cleardenom(res,r);
    if (nCoeff_is_Ring(r->cf)) p_Content(res,r);
    return res;
  }
  p_Delete(&f, r);
  p_Delete(&g, r);
  return res;
}
