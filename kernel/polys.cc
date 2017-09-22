#include "kernel/mod2.h"

#include "omalloc/omalloc.h"
#include "misc/options.h"

#include "polys.h"
#include "kernel/ideals.h"
#include "kernel/ideals.h"
#include "polys/clapsing.h"

/// Widely used global variable which specifies the current polynomial ring for Singular interpreter and legacy implementatins.
/// @Note: one should avoid using it in newer designs, for example due to possible problems in parallelization with threads.
ring  currRing = NULL;

void rChangeCurrRing(ring r)
{
  #if 0
  if ((currRing!=NULL)&&(currRing!=r))
  {
    currRing->options=si_opt_1 & TEST_RINGDEP_OPTS;
  }
  #endif
  //------------ set global ring vars --------------------------------
  currRing = r;
  if( r != NULL )
  {
    rTest(r);
    //------------ global variables related to coefficients ------------
    assume( r->cf!= NULL );
    nSetChar(r->cf);
    //------------ global variables related to polys
    p_SetGlobals(r);
    //------------ global variables related to factory -----------------
  }
}

poly p_Divide(poly p, poly q, const ring r)
{
  assume(q!=NULL);
  if (p==NULL)
  {
    p_Delete(&q,r);
    return NULL;
  }
  if (q==NULL)
  {
    WerrorS("div. by 0");
    return NULL;
  }
  if (pNext(q)!=NULL)
  { /* This means that q != 0 consists of at least two terms*/
    if(p_GetComp(p,r)==0)
    {
      if ((r->cf->convSingNFactoryN!=ndConvSingNFactoryN)
      &&(!rField_is_Ring(r)))
      {
        poly res=singclap_pdivide(p, q, r);
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
        int save_opt;
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
          if ((r->cf->convSingNFactoryN!=ndConvSingNFactoryN)
          &&(!rField_is_Ring(r)))
            h=singclap_pdivide(I->m[i],q,r);
          else
          {
            ideal vi=idInit(1,1); vi->m[0]=q;
            ideal ui=idInit(1,1); ui->m[0]=I->m[i];
            ideal R; matrix U;
            ring save_ring=currRing;
            if (r!=currRing) rChangeCurrRing(r);
            int save_opt;
            SI_SAVE_OPT1(save_opt);
            si_opt_1 &= ~(Sy_bit(OPT_PROT));
            ideal m = idLift(vi,ui,&R, FALSE,TRUE,TRUE,&U);
            SI_RESTORE_OPT1(save_opt);
            if (r!=save_ring) rChangeCurrRing(save_ring);
            matrix T = id_Module2formatedMatrix(m,1,1,r);
            h=MATELEM(T,1,1); MATELEM(T,1,1)=NULL;
            id_Delete((ideal*)&T,r);
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
       or that currRing is over a coefficient ring. */
#ifdef HAVE_RINGS
    if (!rField_is_Domain(currRing))
    {
      WerrorS("division only defined over coefficient domains");
      return NULL;
    }
    if (pNext(q)!=NULL)
    {
      WerrorS("division over a coefficient domain only implemented for terms");
      return NULL;
    }
#endif
    return p_DivideM(p,q,r);
  }
  return FALSE;
}
