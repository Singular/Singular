#include <Singular/libsingular.h>

// example of a routine which changes nothing
static BOOLEAN display_sp(kStrategy strat)
{
  // will be call each time a new s-poly is computed (strat->P)
  // the algorithm assures that strat->P.p!=NULL, in currRing
  // if strat->P.t_p==NULL: strat->P.p->next is in currRing
  // otherwise: strat->P.t_p->next==strat->P.p->next, in strat->tailRing
  // must return TRUE, if strat->P is changed, FALSE otherwise
  PrintS("a new s-poly found: ");
  p_Write(strat->P.p,currRing,strat->tailRing);
  return FALSE;
}
static BOOLEAN std_with_display(leftv res, leftv args)
{
  if (args!=NULL)
  {
    if (args->Typ()==IDEAL_CMD)
    {
      ideal I=(ideal)args->Data();
      I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,display_sp);
      idSkipZeroes(I);
      res->data=(char*)I;
      res->rtyp=IDEAL_CMD;
      return FALSE;
    }
  }
  WerrorS("expected: std_with_display(`idea;`)");
  return TRUE;
}
//------------------------------------------------------------------------
// example of a routine which divides by the i-th variable
static BOOLEAN divide_sp(kStrategy strat)
{
  bool b = false;
  int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
  int *m0=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
  if (strat->P.t_p==NULL)
  {
    poly p=strat->P.p;
    p_GetExpV(p,mm,currRing);
    int m_null=0;
    while(p!=NULL)
    {
      m_null=0;
      p_GetExpV(p,m0,currRing);
      for(int i=1;i<=rVar(currRing);i++)
      {
        mm[i]=si_min(mm[i],m0[i]);
        if (mm[i]>0) m_null++;
      }
      if (m_null==0) break;
      pIter(p);
    }
    if (m_null>0)
    {
      //Print("dividing the following with respect to variable %d and power %ld: ",var_to_divide,m);
      p_Write(strat->P.p,currRing,strat->tailRing);
      PrintS(" -> ");
      p=p_Copy(strat->P.p,currRing);
      strat->P.p=p;
      while(p!=NULL)
      {
        for(int i=1;i<=rVar(currRing);i++)
          p_SubExp(p,i,mm[i],currRing);
        p_Setm(p,currRing);
        pIter(p);
      }
      p_Write(strat->P.p,currRing,strat->tailRing);
      b = true;
    }
  }
  else
  {
    poly p=strat->P.t_p;
    p_GetExpV(p,mm,strat->tailRing);
    int m_null=0;
    while(p!=NULL)
    {
      m_null=0;
      p_GetExpV(p,m0,strat->tailRing);
      for(int i=1;i<=rVar(currRing);i++)
      {
        mm[i]=si_min(mm[i],m0[i]);
        if (mm[i]>0) m_null++;
      }
      if (m_null==0) break;
      pIter(p);
    }
    if (m_null>0)
    {
      //Print("dividing the following with respect to variable %d and power %ld: ",var_to_divide,m);
      p_Write(strat->P.t_p,strat->tailRing,strat->tailRing);
      PrintS(" -> ");
      p=p_Copy(strat->P.t_p,strat->tailRing);
      strat->P.t_p=p;
      strat->P.p=NULL;
      while(p!=NULL)
      {
        for(int i=1;i<=rVar(currRing);i++)
          p_SubExp(p,i,mm[i],strat->tailRing);
        p_Setm(p,strat->tailRing);
        pIter(p);
      }
      strat->P.GetP();
      Print("result: ");
      p_Write(strat->P.p,currRing,strat->tailRing);
      b = true;
    }
  }
  omFree(mm);
  omFree(m0);
  return b;// return TRUE if sp was changed, FALSE if not
}
static BOOLEAN std_with_divide(leftv res, leftv args)
{
  if (args!=NULL)
  {
    if ((args->Typ()==IDEAL_CMD) && (args->next==NULL))
    {
      ideal I=(ideal)args->Data();
      I=kStd(I,currRing->qideal,testHomog,NULL,NULL,0,0,NULL,divide_sp);
      idSkipZeroes(I);
      res->data=(char*)I;
      res->rtyp=IDEAL_CMD;
      return FALSE;
    }
  }
  WerrorS("expected: std_with_divide(`idea;`,`int`)");
  return TRUE;
}
//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(std_demo)(SModulFunctions* p)
{
  p->iiAddCproc("std_demo","std_with_display",FALSE,std_with_display);
  p->iiAddCproc("std_demo","std_with_divide",FALSE,std_with_divide);
  PrintS("init of std_demo - type `listvar(Std_demo);` to its contents\n");
  return (MAX_TOK);
}

