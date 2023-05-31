#include "Singular/libsingular.h"
#include "libpolys/polys/prCopy.h"
//#include <vector>
//#include <iostream>

// global variable potentially storing output
//ideal idealCache=NULL;

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

static BOOLEAN satstd(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL)
  && ((u->Typ()==IDEAL_CMD)||(u->Typ()==MODUL_CMD)))
  {
    ideal I=(ideal)u->Data();
    leftv v = u->next;

    res->rtyp=IDEAL_CMD;
    ideal J;
    if (v==NULL)
    {
      J=id_MaxIdeal(1,currRing);
    }
    else
    {
      if (v->Typ()==IDEAL_CMD)
      {
        J = (ideal) v->Data();
      }
      else
      {
        args->CleanUp();
        WerrorS("satstd: unexpected parameters");
        return TRUE;
      }
    }
    I=id_Satstd(I,J,currRing);

#if 0 /* unused */
    if (idealCache)
    {
      id_Delete(&I,currRing);
      res->data = (char*) idealCache;
      idealCache = NULL;
    }
    else
#endif
    {
      idSkipZeroes(I);
      res->data=(char*)I;
    }
    if (v==NULL) id_Delete(&J,currRing);
    args->CleanUp();
    setFlag(res,FLAG_STD);
    return (res->data==NULL);
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

STATIC_VAR char* si_filename;
static BOOLEAN print_spoly(kStrategy strat)
{
  char *s;
  if (strat->P.t_p==NULL)
  {
    s=pString(strat->P.p);
  }
  else
  {
    poly p=strat->P.t_p;
    s=p_String(strat->P.t_p,strat->tailRing);
  }
  FILE *f=fopen(si_filename,"a");
  fwrite(s,strlen(s),1,f);
  fwrite("\n",1,1,f);
  fflush(f);
  fclose(f);
  return FALSE; // return TRUE if sp was changed, FALSE if not
}

STATIC_VAR int si_filenr;
static BOOLEAN print_syz(kStrategy strat)
{
  char *s=NULL;
  if (strat->P.t_p==NULL)
  {
    if (pGetComp(strat->P.p)>=strat->syzComp)
      s=pString(strat->P.p);
  }
  else
  {
    if (p_GetComp(strat->P.t_p,strat->tailRing)>=strat->syzComp)
      s=p_String(strat->P.t_p,strat->tailRing);
  }
  if (s!=NULL)
  {
    char *fn=(char*)malloc(strlen(si_filename)+12);
    sprintf(fn,"%s.%d",si_filename,si_filenr);
    si_filenr++;
    FILE *f=fopen(fn,"w");
    fwrite(s,strlen(s),1,f);
    fwrite("\n",1,1,f);
    fclose(f);
    free(fn);
  }
  return FALSE; // return TRUE if sp was changed, FALSE if not
}

static BOOLEAN std_print_spoly(leftv res, leftv args)
{
  if (args!=NULL)
  {
    if ((args->Typ()==IDEAL_CMD) && (args->next!=NULL)
     && (args->next->Typ()==STRING_CMD))
    {
      si_filename=(char*)args->next->Data();
      ideal I=(ideal)args->Data();
      I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,print_spoly);
      idSkipZeroes(I);
      res->rtyp=IDEAL_CMD;
      res->data=(char*)I;
      return FALSE;
    }
  }
  WerrorS("std_print_spoly: unexpected parameters");
  return TRUE;
}

static ideal idGroebner_print(ideal temp,int syzComp, intvec* w=NULL, tHomog hom=testHomog)
{
  ideal temp1;
  if (w==NULL)
  {
    if (hom==testHomog)
      hom=(tHomog)idHomModule(temp,currRing->qideal,&w); //sets w to weight vector or NULL
  }
  else
  {
    w=ivCopy(w);
    hom=isHomog;
  }
  temp1 = kStd(temp,currRing->qideal,hom,&w,NULL,syzComp,0,NULL,print_syz);
  idDelete(&temp);
  if (w!=NULL) delete w;
  return temp1;
}

static ideal idPrepare_print (ideal  h1, ideal h11, tHomog hom, int syzcomp, intvec **w)
{
  ideal   h2,h22;
  int     j,k;
  poly    p,q;

  assume(!idIs0(h1));
  k = id_RankFreeModule(h1,currRing);
  if (h11!=NULL)
  {
    k = si_max(k,(int)id_RankFreeModule(h11,currRing));
    h22=idCopy(h11);
  }
  h2=idCopy(h1);
  int i = IDELEMS(h2);
  if (h11!=NULL) i+=IDELEMS(h22);
  if (k == 0)
  {
    id_Shift(h2,1,currRing);
    if (h11!=NULL) id_Shift(h22,1,currRing);
    k = 1;
  }
  if (syzcomp<k)
  {
    Warn("syzcomp too low, should be %d instead of %d",k,syzcomp);
    syzcomp = k;
    rSetSyzComp(k,currRing);
  }
  h2->rank = syzcomp+i;

  for (j=0; j<IDELEMS(h2); j++)
  {
    p = h2->m[j];
    q = pOne();
    pSetComp(q,syzcomp+1+j);
    pSetmComp(q);
    if (p!=NULL)
    {
      {
        while (pNext(p)) pIter(p);
        p->next = q;
      }
    }
    else
      h2->m[j]=q;
  }
  if (h11!=NULL)
  {
    ideal h=id_SimpleAdd(h2,h22,currRing);
    id_Delete(&h2,currRing);
    id_Delete(&h22,currRing);
    h2=h;
  }

  idTest(h2);
  #if 0
  matrix TT=id_Module2Matrix(idCopy(h2),currRing);
  PrintS(" --------------before std------------------------\n");
  ipPrint_MA0(TT,"T");
  PrintLn();
  idDelete((ideal*)&TT);
  #endif

  ideal h3;
  if (w!=NULL) h3=idGroebner_print(h2,syzcomp,*w,hom);
  else         h3=idGroebner_print(h2,syzcomp,NULL,hom);
  return h3;
}

static ideal idSyzygies_print (ideal  h1, tHomog h,intvec **w)
{
  ideal s_h1;
  int   j, k, length=0,reg;
  BOOLEAN isMonomial=TRUE;
  int ii, idElemens_h1;

  assume(h1 != NULL);

  idElemens_h1=IDELEMS(h1);
#ifdef PDEBUG
  for(ii=0;ii<idElemens_h1 /*IDELEMS(h1)*/;ii++) pTest(h1->m[ii]);
#endif
  if (idIs0(h1))
  {
    ideal result=idFreeModule(idElemens_h1/*IDELEMS(h1)*/);
    return result;
  }
  int slength=(int)id_RankFreeModule(h1,currRing);
  k=si_max(1,slength /*id_RankFreeModule(h1)*/);

  assume(currRing != NULL);
  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rSetSyzComp(k,syz_ring);

  if (orig_ring != syz_ring)
  {
    rChangeCurrRing(syz_ring);
    s_h1=idrCopyR_NoSort(h1,orig_ring,syz_ring);
  }
  else
  {
    s_h1 = h1;
  }

  idTest(s_h1);

  BITSET save_opt;
  SI_SAVE_OPT1(save_opt);
  si_opt_1|=Sy_bit(OPT_REDTAIL_SYZ);

  ideal s_h3=idPrepare_print(s_h1,NULL,h,k,w); // main (syz) GB computation

  SI_RESTORE_OPT1(save_opt);

  if (orig_ring != syz_ring)
  {
    idDelete(&s_h1);
    for (j=0; j<IDELEMS(s_h3); j++)
    {
      if (s_h3->m[j] != NULL)
      {
        if (p_MinComp(s_h3->m[j],syz_ring) > k)
          p_Shift(&s_h3->m[j], -k,syz_ring);
        else
          p_Delete(&s_h3->m[j],syz_ring);
      }
    }
    idSkipZeroes(s_h3);
    s_h3->rank -= k;
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rDelete(syz_ring);
    #ifdef HAVE_PLURAL
    if (rIsPluralRing(orig_ring))
    {
      id_DelMultiples(s_h3,orig_ring);
      idSkipZeroes(s_h3);
    }
    #endif
    idTest(s_h3);
    return s_h3;
  }

  ideal e = idInit(IDELEMS(s_h3), s_h3->rank);

  for (j=IDELEMS(s_h3)-1; j>=0; j--)
  {
    if (s_h3->m[j] != NULL)
    {
      if (p_MinComp(s_h3->m[j],syz_ring) <= k)
      {
        e->m[j] = s_h3->m[j];
        isMonomial=isMonomial && (pNext(s_h3->m[j])==NULL);
        p_Delete(&pNext(s_h3->m[j]),syz_ring);
        s_h3->m[j] = NULL;
      }
    }
  }

  idSkipZeroes(s_h3);

  idDelete(&e);
  assume(orig_ring==currRing);
  idTest(s_h3);
  if (currRing->qideal != NULL)
  {
    ideal ts_h3=kStd(s_h3,currRing->qideal,h,w);
    idDelete(&s_h3);
    s_h3 = ts_h3;
  }
  return s_h3;
}

static BOOLEAN syz_print_spoly(leftv res, leftv args)
{
  if (args!=NULL)
  {
    if (((args->Typ()==IDEAL_CMD)||(args->Typ()==MODUL_CMD)) && (args->next!=NULL)
     && (args->next->Typ()==STRING_CMD))
    {
      si_filename=(char*)args->next->Data();
      si_filenr=0;
      ideal v_id=(ideal)args->Data();
      intvec *ww=(intvec *)atGet(args,"isHomog",INTVEC_CMD);
      intvec *w=NULL;
      tHomog hom=testHomog;
      if (ww!=NULL)
      {
        if (idTestHomModule(v_id,currRing->qideal,ww))
        {
          w=ivCopy(ww);
          int add_row_shift=w->min_in();
          (*w)-=add_row_shift;
          hom=isHomog;
        }
        else
        {
          //WarnS("wrong weights");
          delete ww; ww=NULL;
          hom=testHomog;
        }
      }
      else
      {
        if (args->Typ()==IDEAL_CMD)
          if (idHomIdeal(v_id,currRing->qideal))
            hom=isHomog;
      }
      ideal S=idSyzygies_print(v_id,hom,&w);
      res->data = (char *)S;
      res->rtyp=args->Typ();
      if (hom==isHomog)
      {
        int vl=S->rank;
        intvec *vv=new intvec(vl);
        if ((args->Typ()==IDEAL_CMD)||(ww==NULL))
        {
          for(int i=0;i<vl;i++)
          {
            if (v_id->m[i]!=NULL)
              (*vv)[i]=p_Deg(v_id->m[i],currRing);
          }
        }
        else
        {
          p_SetModDeg(ww, currRing);
          for(int i=0;i<vl;i++)
          {
            if (v_id->m[i]!=NULL)
              (*vv)[i]=currRing->pFDeg(v_id->m[i],currRing);
          }
          p_SetModDeg(NULL, currRing);
       }
       if (idTestHomModule(S,currRing->qideal,vv))
         atSet(res,omStrDup("isHomog"),vv,INTVEC_CMD);
       else
         delete vv;
      }
      if (w!=NULL) delete w;
      return FALSE;
    }
  }
  WerrorS("syz_print_spoly: unexpected parameters");
  return TRUE;
}

static BOOLEAN monomialabortstd(leftv res, leftv args)
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
  p->iiAddCproc("customstd.lib","std_print_spoly",FALSE,std_print_spoly);
  p->iiAddCproc("customstd.lib","syz_print_spoly",FALSE,std_print_spoly);

  return (MAX_TOK);
}
