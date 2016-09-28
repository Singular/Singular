#include <Singular/libsingular.h>
#include <vector>
#include <iostream>

// global variable potentially storing output
ideal idealCache=NULL;

std::vector<int> customstd_satstdSaturatingVariables;

// //------------------------------------------------------------------------
// // example of a routine which changes nothing
// static BOOLEAN display_sp(kStrategy strat)
// {
//   // will be call each time a new s-poly is computed (strat->P)
//   // the algorithm assures that strat->P.p!=NULL, in currRing
//   // if strat->P.t_p==NULL: strat->P.p->next is in currRing
//   // otherwise: strat->P.t_p->next==strat->P.p->next, in strat->tailRing
//   // must return TRUE, if strat->P is changed, FALSE otherwise
//   PrintS("a new s-poly found: ");
//   p_Write(strat->P.p,currRing,strat->tailRing);
//   return FALSE;
// }
// static BOOLEAN std_with_display(leftv res, leftv args)
// {
//   if (args!=NULL)
//   {
//     if (args->Typ()==IDEAL_CMD)
//     {
//       ideal I=(ideal)args->Data();
//       I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,display_sp);
//       idSkipZeroes(I);
//       res->data=(char*)I;
//       res->rtyp=IDEAL_CMD;
//       return FALSE;
//     }
//   }
//   WerrorS("expected: std_with_display(`idea;`)");
//   return TRUE;
// }

//------------------------------------------------------------------------
// routine that simplifies the new element by dividing it with the maximal possible
// partially saturating the ideal with respect to all variables doing so
static BOOLEAN sat_vars_sp(kStrategy strat)
{
  BOOLEAN b = FALSE; // set b to TRUE, if spoly was changed,
                     // let it remain FALSE otherwise
  if (strat->P.t_p==NULL)
  {
    poly p=strat->P.p;

    // iterate over all terms of p and
    // compute the minimum mm of all exponent vectors
    int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    p_GetExpV(p,mm,currRing);
    bool nonTrivialSaturationToBeDone=true;
    for (p=pNext(p); p!=NULL; pIter(p))
    {
      nonTrivialSaturationToBeDone=false;
      p_GetExpV(p,m0,currRing);
      for (int i=customstd_satstdSaturatingVariables.size()-1; i>=0; i--)
      {
        int li = customstd_satstdSaturatingVariables[i];
        mm[li]=si_min(mm[li],m0[li]);
        if (mm[li]>0) nonTrivialSaturationToBeDone=true;
      }
      // abort if the minimum is zero in each component
      if (nonTrivialSaturationToBeDone==false) break;
    }
    if (nonTrivialSaturationToBeDone==true)
    {
      // std::cout << "simplifying!" << std::endl;
      p=p_Copy(strat->P.p,currRing);
      memset(&strat->P,0,sizeof(strat->P));
      strat->P.tailRing = strat->tailRing;
      strat->P.p=p;
      while(p!=NULL)
      {
        for (int i=customstd_satstdSaturatingVariables.size()-1; i>=0; i--)
        {
          int li = customstd_satstdSaturatingVariables[i];
          p_SubExp(p,li,mm[li],currRing);
        }
        p_Setm(p,currRing);
        pIter(p);
      }
      b = TRUE;
    }
    omFree(mm);
    omFree(m0);
  }
  else
  {
    poly p=strat->P.t_p;

    // iterate over all terms of p and
    // compute the minimum mm of all exponent vectors
    int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    p_GetExpV(p,mm,strat->tailRing);
    bool nonTrivialSaturationToBeDone=true;
    for (p = pNext(p); p!=NULL; pIter(p))
    {
      nonTrivialSaturationToBeDone=false;
      p_GetExpV(p,m0,strat->tailRing);
      for(int i=customstd_satstdSaturatingVariables.size()-1; i>=0; i--)
      {
        int li = customstd_satstdSaturatingVariables[i];
        mm[li]=si_min(mm[li],m0[li]);
        if (mm[li]>0) nonTrivialSaturationToBeDone = true;
      }
      // abort if the minimum is zero in each component
      if (!nonTrivialSaturationToBeDone) break;
    }
    if (nonTrivialSaturationToBeDone)
    {
      p=p_Copy(strat->P.t_p,strat->tailRing);
      memset(&strat->P,0,sizeof(strat->P));
      strat->P.tailRing = strat->tailRing;
      strat->P.t_p=p;
      while(p!=NULL)
      {
        for(int i=customstd_satstdSaturatingVariables.size()-1; i>=0; i--)
        {
          int li = customstd_satstdSaturatingVariables[i];
          p_SubExp(p,li,mm[li],strat->tailRing);
        }
        p_Setm(p,strat->tailRing);
        pIter(p);
      }
      strat->P.GetP();
      b = TRUE;
    }
    omFree(mm);
    omFree(m0);
  }
  return b; // return TRUE if sp was changed, FALSE if not
}

static BOOLEAN satstd(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    leftv v = u->next;

    if (v==NULL)
    {
      int n = rVar(currRing);
      customstd_satstdSaturatingVariables = std::vector<int>(n);
      for (int i=n-1; i>=0; i--)
        customstd_satstdSaturatingVariables[i] = i+1;
    }
    else
    {
      if (v->Typ()==IDEAL_CMD)
      {
        ideal J = (ideal) v->Data();

        int k = IDELEMS(J);
        customstd_satstdSaturatingVariables = std::vector<int>(k);
        for (int i=0; i<k; i++)
        {
          poly x = J->m[i];
          int li = p_Var(x,currRing);
          if (li>0)
            customstd_satstdSaturatingVariables[i]=li;
          else
          {
            WerrorS("satstd: second argument only ideals generated by variables supported for now");
            return TRUE;
          }
        }
      }
      else
      {
        WerrorS("satstd: unexpected parameters");
        return TRUE;
      }
    }

    ideal I = (ideal) u->Data();

    idealCache = NULL;
    I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,sat_vars_sp);
    customstd_satstdSaturatingVariables = std::vector<int>();

    res->rtyp=IDEAL_CMD;
    if (idealCache)
    {
      id_Delete(&I,currRing);
      res->data = (char*) idealCache;
      idealCache = NULL;
    }
    else
    {
      idSkipZeroes(I);
      res->data=(char*)I;
    }
    return FALSE;
  }
  WerrorS("satstd: unexpected parameters");
  return TRUE;
}

static BOOLEAN abort_if_monomial_sp(kStrategy strat)
{
  BOOLEAN b = FALSE; // set b to TRUE, if spoly was changed,
                     // let it remain FALSE otherwise
  if (strat->P.t_p==NULL)
  {
    poly p=strat->P.p;
    if (pNext(p)==NULL)
    {
      while ((strat->Ll >= 0))
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      return FALSE;
    }
  }
  else
  {
    poly p=strat->P.t_p;
    if (pNext(p)==NULL)
    {
      while ((strat->Ll >= 0))
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      return FALSE;
    }
  }
  return b; // return TRUE if sp was changed, FALSE if not
}

BOOLEAN monomialabortstd(leftv res, leftv args)
{
  if (args!=NULL)
  {
    if ((args->Typ()==IDEAL_CMD) && (args->next==NULL))
    {
      ideal I=(ideal)args->Data();
      I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,abort_if_monomial_sp);
      idSkipZeroes(I);
      res->rtyp=IDEAL_CMD;
      res->data=(char*)I;
      return FALSE;
    }
  }
  WerrorS("monomialabortstd: unexpected parameters");
  return TRUE;
}

// static long wDeg(const poly p, const ring r)
// {
//   if (r->order[0] == ringorder_lp)
//     return p_GetExp(p,1,currRing);
//   if (r->order[0] == ringorder_ls)
//     return -p_GetExp(p,1,currRing);

//   if (r->order[0] == ringorder_dp)
//   {
//     long d = 0;
//     for (int i=1; i<=rVar(r); i++)
//       d = d + p_GetExp(p,i,r);
//     return d;
//   }
//   if (r->order[0] == ringorder_wp || r->order[0] == ringorder_a)
//   {
//     long d = 0;
//     for (int i=r->block0[0]; i<=r->block1[0]; i++)
//       d = d + p_GetExp(p,i,r)*r->wvhdl[0][i-1];
//     return d;
//   }
//   if (r->order[0] == ringorder_ws)
//   {
//     long d = 0;
//     for (int i=r->block0[0]; i<=r->block1[0]; i++)
//       d = d - p_GetExp(p,i,r)*r->wvhdl[0][i-1];
//     return d;
//   }
// }

// static bool isInitialFormMonomial(const poly g, const ring r)
// {
//   if (g->next==NULL)
//     return true;
//   return wDeg(g,r)!=wDeg(g->next,r);
// }

// //------------------------------------------------------------------------
// // routine that checks whether the initial form is a monomial,
// // breaks computation if it finds one, writing the element into idealCache
// static BOOLEAN sat_sp_initial(kStrategy strat)
// {
//   BOOLEAN b = FALSE; // set b to TRUE, if spoly was changed,
//                      // let it remain FALSE otherwise
//   if (strat->P.t_p==NULL)
//   {
//     poly p=strat->P.p;
//     if (pNext(p)==NULL)
//     {
//       // if a term is contained in the ideal, abort std computation
//       // and store the output in idealCache to be returned
//       while ((strat->Ll >= 0))
//         deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
//       idealCache = idInit(1);
//       idealCache->m[0] = p_One(currRing);
//       return FALSE;
//     }
//     if (isInitialFormMonomial(p,currRing))
//     {
//       while ((strat->Ll >= 0))
//         deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
//       idealCache = idInit(1);
//       idealCache->m[0] = p_Copy(p,currRing);
//     }
//     int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
//     int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
//     p_GetExpV(p,mm,currRing);
//     int m_null=0;
//     while(p!=NULL)
//     {
//       m_null=0;
//       p_GetExpV(p,m0,currRing);
//       for(int i=1;i<=rVar(currRing);i++)
//       {
//         mm[i]=si_min(mm[i],m0[i]);
//         if (mm[i]>0) m_null++;
//       }
//       if (m_null==0) break;
//       pIter(p);
//     }
//     if (m_null>0)
//     {
//       std::cout << "simplifying!" << std::endl;
//       p=p_Copy(strat->P.p,currRing);
//       strat->P.p=p;
//       while(p!=NULL)
//       {
//         for(int i=1;i<=rVar(currRing);i++)
//           p_SubExp(p,i,mm[i],currRing);
//         p_Setm(p,currRing);
//         pIter(p);
//       }
//       b = TRUE;
//     }
//     omFree(mm);
//     omFree(m0);
//   }
//   else
//   {
//     poly p=strat->P.t_p;
//     if (pNext(p)==NULL)
//     {
//       // if a term is contained in the ideal, abort std computation
//       // and store the output in idealCache to be returned
//       while ((strat->Ll >= 0))
//         deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
//       idealCache = idInit(1);
//       idealCache->m[0] = p_One(currRing);
//       return FALSE;
//     }
//     if (isInitialFormMonomial(p,strat->tailRing))
//     {
//       while ((strat->Ll >= 0))
//         deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
//       nMapFunc identity = n_SetMap(strat->tailRing,currRing);
//       idealCache = idInit(1);
//       idealCache->m[0] = p_PermPoly(p,NULL,strat->tailRing,currRing,identity,NULL,0);
//     }
//     int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
//     int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
//     p_GetExpV(p,mm,strat->tailRing);
//     int m_null=0;
//     while(p!=NULL)
//     {
//       m_null=0;
//       p_GetExpV(p,m0,strat->tailRing);
//       for(int i=1;i<=rVar(currRing);i++)
//       {
//         mm[i]=si_min(mm[i],m0[i]);
//         if (mm[i]>0) m_null++;
//       }
//       if (m_null==0) break;
//       pIter(p);
//     }
//     if (m_null>0)
//     {
//       std::cout << "simplifying!" << std::endl;
//       p=p_Copy(strat->P.t_p,strat->tailRing);
//       strat->P.t_p=p;
//       strat->P.p=NULL;
//       while(p!=NULL)
//       {
//         for(int i=1;i<=rVar(currRing);i++)
//           p_SubExp(p,i,mm[i],strat->tailRing);
//         p_Setm(p,strat->tailRing);
//         pIter(p);
//       }
//       strat->P.GetP();
//       b = TRUE;
//     }
//     omFree(mm);
//     omFree(m0);
//   }
//   return b; // return TRUE if sp was changed, FALSE if not
// }
// static BOOLEAN satstdWithInitialCheck(leftv res, leftv args)
// {
//   if (args!=NULL)
//   {
//     if ((args->Typ()==IDEAL_CMD) && (args->next==NULL))
//     {
//       ideal I=(ideal)args->Data();
//       idealCache = NULL;
//       I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,sat_sp_initial);
//       res->rtyp=IDEAL_CMD;
//       if (idealCache)
//         res->data=(char*)idealCache;
//       else
//         res->data=(char*)I;
//       return FALSE;
//     }
//   }
//   WerrorS("satstdWithInitialCheck: unexpected parameters");
//   return TRUE;
// }



//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(customstd)(SModulFunctions* p)
{
  // p->iiAddCproc("std_demo","std_with_display",FALSE,std_with_display);
  p->iiAddCproc("customstd.lib","satstd",FALSE,satstd);
  // p->iiAddCproc("std_demo","satstdWithInitialCheck",FALSE,satstdWithInitialCheck);
  p->iiAddCproc("customstd.lib","monomialabortstd",FALSE,monomialabortstd);
  // PrintS("init of std_demo - type `listvar(Std_demo);` to its contents\n");
  return (MAX_TOK);
}
