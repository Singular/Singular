/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz1.cc,v 1.13 1997-09-29 08:51:48 siebert Exp $ */
/*
* ABSTRACT: resolutions
*/


#include "mod2.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kstd1.h"
#include "kutil.h"
#include "spolys.h"
#include "spolys0.h"
#include "stairc.h"
#include "ipid.h"
#include "cntrlc.h"
#include "ipid.h"
#include "intvec.h"
#include "ipshell.h"
#include "limits.h"
#include "tok.h"
#include "numbers.h"
#include "modulop.h"
#include "ideals.h"
#include "intvec.h"
#include "ring.h"
#include "lists.h"
#include "syz.h"

/*--------------static variables------------------------*/
/*---contains the real components wrt. cdp--------------*/
//static int ** truecomponents=NULL;
//static int ** backcomponents=NULL;
//static int ** Howmuch=NULL;
//static int ** Firstelem=NULL;
/*---points to the real components of the actual module-*/
static int *  currcomponents=NULL;
/*---head-term-polynomials for the reduction------------*/
static poly redpol=NULL;
/*---counts number of applications of GM-criteria-------*/
//static int crit;
//static int zeroRed;
//static int dsim;
//static int simple;
static int euler;
/*---controls the ordering------------------------------*/
static intvec * orderedcomp;
static int *binomials;
static int highdeg;
static int highdeg_1;

/*3
* deletes all entres of a pair
*/
static void syDeletePair(SObject * so)
{
  pDelete(&(*so).p);
  pDelete(&(*so).lcm);
  pDelete(&(*so).syz);
  (*so).p1 = NULL;
  (*so).p2 = NULL;
  (*so).ind1 = 0;
  (*so).ind2 = 0;
  (*so).syzind = -1;
  (*so).order = 0;
  (*so).isNotMinimal = NULL;
}

/*3
* puts all entres of a pair to another
*/
static void syCopyPair(SObject * argso, SObject * imso)
{
  *imso=*argso;
  //(*imso).p = (*argso).p;
  //(*imso).p1 = (*argso).p1;
  //(*imso).p2 = (*argso).p2;
  //(*imso).lcm = (*argso).lcm;
  //(*imso).syz = (*argso).syz;
  //(*imso).ind1 = (*argso).ind1;
  //(*imso).ind2 = (*argso).ind2;
  //(*imso).syzind = (*argso).syzind;
  //(*imso).order = (*argso).order;
  //(*imso).isNotMinimal = (*argso).isNotMinimal;
  (*argso).p = NULL;
  (*argso).p1 = NULL;
  (*argso).p2 = NULL;
  (*argso).lcm = NULL;
  (*argso).syz = NULL;
  (*argso).ind1 = 0;
  (*argso).ind2 = 0;
  (*argso).syzind = -1;
  (*argso).order = 0;
  (*argso).isNotMinimal = NULL;
}

/*3
* deletes empty objects from a pair set beginning with
* pair first
* assumes a pair to be empty if .lcm does so
*/
static void syCompactifyPairSet(SSet sPairs, int sPlength, int first)
{
  int k=first,kk=0;

  while (k+kk<sPlength)
  {
    if (sPairs[k+kk].lcm!=NULL)
    {
      if (kk>0) syCopyPair(&sPairs[k+kk],&sPairs[k]);
      k++;
    }
    else
    {
      kk++;
    }
  }
}

/*3
* deletes empty objects from a pair set beginning with
* pair first
* assumes a pair to be empty if .lcm does so
*/
static void syCompactify1(SSet sPairs, int* sPlength, int first)
{
  int k=first,kk=0;

  while (k+kk<=*sPlength)
  {
    if (sPairs[k+kk].lcm!=NULL)
    {
      if (kk>0) syCopyPair(&sPairs[k+kk],&sPairs[k]);
      k++;
    }
    else
    {
      kk++;
    }
  }
  *sPlength -= kk;
}

/*3
* replaces comp1dpc during homogeneous syzygy-computations
* compares with components of currcomponents instead of the
* exp[0]
*/
static int syzcomp1dpc(poly p1, poly p2)
{
  /*4 compare monomials by order then revlex*/
    int i = pVariables;
    if ((p1->exp[i] == p2->exp[i]))
    {
      do
      {
        i--;
        if (i <= 1)
        {
          //(*orderingdepth)[pVariables-i]++;
           /*4 handle module case:*/
           if (p1->exp[0]==p2->exp[0]) return 0;
           else if 
              (currcomponents[p1->exp[0]]>currcomponents[p2->exp[0]]) 
                return 1;
           else return -1;
        }
      } while ((p1->exp[i] == p2->exp[i]));
    }
    //(*orderingdepth)[pVariables-i]++;
    if (p1->exp[i] < p2->exp[i]) return 1;
    return -1;
}

/*3
* replaces comp1dpc during homogeneous syzygy-computations
* compares with components of currcomponents instead of the
* exp[0]
*/
static int syzcomp2dpc(poly p1, poly p2)
{
  int o1=pGetOrder(p1), o2=pGetOrder(p2);
  if (o1 > o2) return 1;
  if (o1 < o2) return -1;

  if (o1>0)
  {
    int i = pVariables;
    while ((i>1) && (p1->exp[i]==p2->exp[i]))
      i--;
    //(*orderingdepth)[pVariables-i]++;
    if (i>1)
    {
      if (p1->exp[i] < p2->exp[i]) return 1;
      return -1;
    }
  }
  o1=p1->exp[0];
  o2=p2->exp[0];
  if (o1==o2/*p1->exp[0]==p2->exp[0]*/) return 0;
  if (currcomponents[o1]>currcomponents[o2]) return 1;
  return -1;
}

/*3
* compares only the monomial without component
*/
static int syzcompmonomdp(poly p1, poly p2)
{
  int i;

  /*4 compare monomials by order then revlex*/
  if (pGetOrder(p1) == pGetOrder(p2))
  {
    i = pVariables;
    if ((p1->exp[i] == p2->exp[i]))
    {
      do
      {
        i--;
        if (i <= 1)
          return 0;
      } while ((p1->exp[i] == p2->exp[i]));
    }
    if (p1->exp[i] < p2->exp[i]) return 1;
    return -1;
  }
  else if (pGetOrder(p1) > pGetOrder(p2)) return 1;
  return -1;
}

/*
* (i,j) in binomials[j-1,i-j+1]
* table size is: pVariables * (highdeg+1)
* highdeg_1==highdeg+1
*/
static void syBinomSet()
{
  highdeg_1=highdeg+1;
  for(int j=1;j<=highdeg;j++)
  {
    binomials[j/*0,j*/] = j;
    for (int i=1;i<pVariables;i++)
    {
      binomials[i*(highdeg_1)+j/*i,j*/]
      = binomials[(i-1)*(highdeg_1)+j/*i-1,j*/]*(j+i)/(i+1);
    }
  }
  for (int i=0;i<pVariables;i++)
  {  
    binomials[i*(highdeg_1)/*i,0*/]=0;
  }
}

static inline int syBinom(int i,int j)
{
  return binomials[(j-1)*(highdeg_1)+i-j+1/*j-1,i-j*/];
}

static void syzSetm(poly p)
{
  int i = 0,j;
  for (j=pVariables;j>0;j--)
    i += p->exp[j];

  if (i<=highdeg)
  {
    i=1;
    j = -INT_MAX;
    int *ip=binomials+pGetExp(p,1);
    loop
    {
      j += (*ip);
      if (i==pVariables) break;
      i++;
      ip+=highdeg_1+pGetExp(p,i);
    }
    pGetOrder(p) = j;
  }
  else
    pGetOrder(p)=i;
}

static inline poly syMultT(poly p,poly m)
{
  poly q,result=q=pNew();
  int j;
  
  if (pGetOrder(p)>0)
  {
    loop 
    {
      spMemcpy(q,p);
      for (j=pVariables;j>0;j--)
        pGetExp(q,j) += pGetExp(m,j);
      pSetCoeff0(q,nCopy(pGetCoeff(p)));
      syzSetm(q);
      pIter(p);
      if (p!=NULL)
      {
        pNext(q) = pNew();
        pIter(q);
      }
      else break;
    }
  }
  else
  {
    poly lastmon=NULL;
    int i=0;
    loop 
    {
      if (pGetOrder(p)!=i)
      {
        spMemcpy(q,p);
        for (j=pVariables;j>0;j--)
          pGetExp(q,j) += pGetExp(m,j);
        syzSetm(q);
        lastmon = q;
        i = pGetOrder(p);
      }
      else
      {
        spMemcpy(q,lastmon);
        pSetComp(q,pGetComp(p));
      }
      pSetCoeff0(q,nCopy(pGetCoeff(p)));
      pIter(p);
      if (p!=NULL)
      {
        pNext(q) = pNew();
        pIter(q);
      }
      else break;
    }
  }
  pNext(q) = NULL;
  return result;
}

static inline poly syMultTNeg(poly p,poly m)
{
  poly q,result=q=pNew();
  int j;
  
  if (pGetOrder(p)>0)
  {
    loop 
    {
      spMemcpy(q,p);
      for (j=pVariables;j>0;j--)
        pGetExp(q,j) += pGetExp(m,j);
      pSetCoeff0(q,nCopy(pGetCoeff(p)));
      pGetCoeff(q) = nNeg(pGetCoeff(q));
      syzSetm(q);
      pIter(p);
      if (p!=NULL)
      {
        pNext(q) = pNew();
        pIter(q);
      }
      else break;
    }
  }
  else
  {
    poly lastmon=NULL;
    int i=0;
    loop 
    {
      if (pGetOrder(p)!=i)
      {
        spMemcpy(q,p);
        for (j=pVariables;j>0;j--)
          pGetExp(q,j) += pGetExp(m,j);
        syzSetm(q);
        lastmon = q;
        i = pGetOrder(p);
      }
      else
      {
        spMemcpy(q,lastmon);
        pSetComp(q,pGetComp(p));
      }
      pSetCoeff0(q,nCopy(pGetCoeff(p)));
      pGetCoeff(q) = nNeg(pGetCoeff(q));
      pIter(p);
      if (p!=NULL)
      {
        pNext(q) = pNew();
        pIter(q);
      }
      else break;
    }
  }
  pNext(q) = NULL;
  return result;
}

static poly syMultT1(poly p,poly m)
{
  poly q,result=q=pNew();
  int j;
  
  if (pGetOrder(p)>0)
  {
    loop 
    {
      spMemcpy(q,p);
      for (j=pVariables;j>0;j--)
        pGetExp(q,j) += pGetExp(m,j);
      pSetCoeff0(q,nMult(pGetCoeff(p),pGetCoeff(m)));
      syzSetm(q);
      pIter(p);
      if (p!=NULL)
      {
        pNext(q) = pNew();
        pIter(q);
      }
      else break;
    }
  }
  else
  {
    poly lastmon=NULL;
    int i=0;
    loop 
    {
      if (pGetOrder(p)!=i)
      {
        spMemcpy(q,p);
        for (j=pVariables;j>0;j--)
          pGetExp(q,j) += pGetExp(m,j);
        syzSetm(q);
        lastmon = q;
        i = pGetOrder(p);
      }
      else
      {
        spMemcpy(q,lastmon);
        pSetComp(q,pGetComp(p));
      }
      pSetCoeff0(q,nMult(pGetCoeff(p),pGetCoeff(m)));
      pIter(p);
      if (p!=NULL)
      {
        pNext(q) = pNew();
        pIter(q);
      }
      else break;
    }
  }
  pNext(q) = NULL;
  return result;
}

static inline poly syAdd(poly m1,poly m2)
{
  if (m1==NULL)
    return m2;
  if (m2==NULL)
    return m1;
  if ((pGetOrder(m1)>0) && (pGetOrder(m2)>0))
  {
    return pAdd(m1,m2);
  }
  else
  {
    poly p,result=p=pNew();
    number n;
    loop
    {
      if (pGetOrder(m1)>pGetOrder(m2))
      {
        pNext(p) = m1;
        pIter(p);
        pIter(m1);
      }
      else if (pGetOrder(m1)<pGetOrder(m2))
      {
        pNext(p) = m2;
        pIter(p);
        pIter(m2);
      }
      else if (currcomponents[m1->exp[0]]>currcomponents[m2->exp[0]])
      {
        pNext(p) = m1;
        pIter(p);
        pIter(m1);
      }
      else if (currcomponents[m1->exp[0]]<currcomponents[m2->exp[0]])
      {
        pNext(p) = m2;
        pIter(p);
        pIter(m2);
      }
      else
      {
        n = nAdd(pGetCoeff(m1),pGetCoeff(m2));
        if (nIsZero(n))
        {
          pDelete1(&m1);
        }
        else
        {
          pNext(p) = m1;
          pIter(p);
          pIter(m1);
          pSetCoeff(p,n);
        }
        pDelete1(&m2);
      }
      if (m1==NULL)
      {
        pNext(p) = m2;
        break;
      }
      if (m2==NULL)
      {
        pNext(p) = m1;
        break;
      }
    }
    pDelete1(&result);
    return result;
  }
}

poly syOrdPolySchreyer(poly p)
{
  return pOrdPolySchreyer(p);
}

static poly sySPAdd(poly m1,poly m2,poly m)
{
  int i2=-INT_MAX;
  int i1;
  int j;
  //register poly m1=m11;
  poly p=redpol;
  poly tm2=pNew();
  number multwith=pGetCoeff(m);

  pDelete1(&m1);
  i1 = pGetOrder(m1);
  pIter(m2);
  spMemcpy(tm2,m2);
  j = 1;
  pGetExp(tm2,1)+=pGetExp(m,1);
  int *ip=binomials+pGetExp(tm2,1);
  loop
  {
    i2 += (*ip);
    if (j==pVariables) break;
    j++;
    pGetExp(tm2,j)+=pGetExp(m,j);
    ip+=highdeg_1+pGetExp(tm2,j);
  }
  pGetOrder(tm2) = i2;
  pSetCoeff0(tm2,nMult(pGetCoeff(m2),multwith));
  loop
  {
    j = i1-i2;
    if (j>0/*m1->order>tm2->order*/)
    {
      p = pNext(p) = m1;
      pIter(m1);
      if (m1!=NULL)
        i1 = pGetOrder(m1);
      else
        break;
    }
    else if (j<0/*m1->order<tm2->order*/)
    {
      p = pNext(p) = tm2;
      j = pGetOrder(m2);
      pIter(m2);
      if (m2!=NULL)
      {
        j -=pGetOrder(m2);
        tm2 = pNew();
        if (j!=0)
        {
          spMemcpy(tm2,m2);
          j = 1;
          pGetExp(tm2,1)+=pGetExp(m,1);
          int *ip=binomials+pGetExp(tm2,1);
          i2=-INT_MAX;
          loop
          {
            i2 += (*ip);
            if (j==pVariables) break;
            j++;
            pGetExp(tm2,j)+=pGetExp(m,j);
            ip+=highdeg_1+pGetExp(tm2,j);
          }
          pGetOrder(tm2) = i2;
          //simple++;
        }
        else
        {
          spMemcpy(tm2,p);
          pSetComp(tm2,pGetComp(m2));
          //dsim++;
        }
        //pNext(tm2) = NULL;
        pSetCoeff0(tm2,nMult(pGetCoeff(m2),multwith));
      }
      else
        break;
    }
    else 
    {
      j = currcomponents[m1->exp[0]]-currcomponents[m2->exp[0]];
      if (j>0/*currcomponents[m1->exp[0]]>currcomponents[m2->exp[0]]*/)
      {
        p = pNext(p) = m1;
        pIter(m1);
        if (m1!=NULL)
          i1 = pGetOrder(m1);
        else
          break;
      }
      else 
      {
        if (j<0/*currcomponents[m1->exp[0]]<currcomponents[m2->exp[0]]*/)
        {
          p = pNext(p) = tm2;
          j = pGetOrder(m2);
        }
        else
        {
          number n = nAdd(pGetCoeff(m1),pGetCoeff(tm2));
          if (nIsZero(n))
          {
            pDelete1(&tm2);
            nDelete(&n);
            j = 0;
          }
          else
          {
            p = pNext(p) = tm2;
            pSetCoeff(p,n);
            j = pGetOrder(m2);
          }
          pDelete1(&m1);
          if (m1==NULL)
          {
            pIter(m2);
            break;
          }
          else
            i1 = pGetOrder(m1);
        }
        pIter(m2);
        if (m2!=NULL)
        {
          j -=pGetOrder(m2);
          tm2 = pNew();
          if (j!=0)
          {
            spMemcpy(tm2,m2);
            j = 1;
            pGetExp(tm2,1)+=pGetExp(m,1);
            int *ip=binomials+pGetExp(tm2,1);
            i2=-INT_MAX;
            loop
            {
              i2 += (*ip);
              if (j==pVariables) break;
              j++;
              pGetExp(tm2,j)+=pGetExp(m,j);
              ip+=highdeg_1+pGetExp(tm2,j);
            }
            pGetOrder(tm2) = i2;
            //simple++;
          }
          else
          {
            spMemcpy(tm2,p);
            pSetComp(tm2,pGetComp(m2));
            //dsim++;
          }
          //pNext(tm2) = NULL;
          pSetCoeff0(tm2,nMult(pGetCoeff(m2),multwith));
        }
        else
          break;
      }
    }
  }
  if (m2==NULL)
  {
    pNext(p) = m1;
  }
  else
  {
    pNext(p) = syMultT1(m2,m);
  }
  return pNext(redpol);
}

static inline poly sySPoly(poly m1,poly m2,poly lcm)
{
  poly a=pDivide(lcm,m1);
  poly b1=NULL,b2=NULL;
  if (pNext(m1)!=NULL)
    b1 = syMultT(pNext(m1),a);
  pDelete(&a);
  a=pDivide(lcm,m2);
  if (pNext(m2)!=NULL)
    b2 = syMultTNeg(pNext(m2),a);
  pDelete(&a);
  return syAdd(b1,b2);
}

static poly sySPolyRed(poly m1,poly m2)
{
  if (pNext(m2)==NULL)
  {
    pDelete1(&m1);
    return m1;
  }
  poly a=pDivide(m1,m2);
  pSetCoeff0(a,nCopy(pGetCoeff(m1)));
  pGetCoeff(a) = nNeg(pGetCoeff(a));
  //return spSpolyRed(m2,m1,NULL);
  if (pNext(m2)==NULL)
  {
    pDelete1(&m1);
    return m1;
  }
  if (pNext(m1)==NULL)
    return syMultT1(pNext(m2),a);
  poly res;
  if (pGetOrder(m1)>0)
  {
    res = spSpolyRed(m2,m1,NULL);
  }
  else
  {
    res = sySPAdd(m1,m2,a);
  }
  pDelete(&a);
  return res;
}

BOOLEAN syDivisibleBy(poly a, poly b)
{
  if (a->exp[0]==b->exp[0])
  {
    int i=pVariables-1;
    short *e1=&(a->exp[1]);
    short *e2=&(b->exp[1]);
    if ((*e1) > (*e2)) return FALSE;
    do
    {
      i--;
      e1++;
      e2++;
      if ((*e1) > (*e2)) return FALSE;
    } while (i>0);
    return TRUE;
  }
  else 
    return FALSE;
}

BOOLEAN syDivisibleBy1(poly a, poly b)
{
  int i=pVariables-1;
  short *e1=&(a->exp[1]);
  short *e2=&(b->exp[1]);
  if ((*e1) > (*e2)) return FALSE;
  do
  {
    i--;
    e1++;
    e2++;
    if ((*e1) > (*e2)) return FALSE;
  } while (i>0);
  return TRUE;
}
/*
*int syDivisibleBy2(poly a, poly b)
*{
*  int i=pVariables-1;
*  short *e1=&(a->exp[1]);
*  short *e2=&(b->exp[1]);
*  int result=(*e2)-(*e1);
*  do
*  {
*    i--;
*    e1++;
*    e2++;
*  } while (i>0);
*  return 0;
*}
*/

static int syLength(poly p)
{
  int result=0;

  while (p!=NULL)
  {
    result++;
    pIter(p);
  }
  return result;
}

poly syRedtail (poly p, syStrategy syzstr, int index)
{
  poly h, hn;
  int j,pos;
  ideal redWith=syzstr->orderedRes[index];

  h = p;
  hn = pNext(h);
  while(hn != NULL)
  {
    j = syzstr->Firstelem[index-1][pGetComp(hn)]-1;
    if (j>=0)
    {
      pos = j+syzstr->Howmuch[index-1][pGetComp(hn)];
      while (j < pos)
      {
        if (syDivisibleBy1(redWith->m[j], hn))
        {
           //int syL=syLength(redWith->m[j]);
            //Print("r");
//for(int jj=j+1;jj<pos;jj++)
//{
  //if (syDivisibleBy1(redWith->m[jj],hn))
  //{
    //int syL1=syLength(redWith->m[jj]);
    //if (syL1<syL)
    //{
      //j = jj;
      //syL = syL1;
      //Print("take poly %d with %d monomials\n",j,syL);
      //Print("have poly %d with %d monomials\n",jj,syL1);
    //}
  //}
//}
          //if (pGetComp(redWith->m[j])!=pGetComp(hn))
          //{
            //Print("Hilfe!!!!!!!\n");
            //Print("Fehler in Modul %d bei Elem %d\n",index,j);
            //Print("Poly p: ");pWrite(hn);
            //Print("Poly redWith: ");pWrite(redWith->m[j]);
          //}
          hn = sySPolyRed(hn,redWith->m[j]);
          if (hn == NULL)
          {
            pNext(h) = NULL;
            return p;
          }
          j = syzstr->Firstelem[index-1][pGetComp(hn)]-1;
          pos = j+syzstr->Howmuch[index-1][pGetComp(hn)];
        }
        else
        {
          j++;
        }
      }
    }
    h = pNext(h) = hn;
    hn = pNext(h);
  }
  return p;
}

/*3
* local procedure for of syInitRes for the module case
*/
static int syChMin(intvec * iv)
{
  int i,j=-1,r=-1;

  for (i=iv->length()-1;i>=0;i--)
  {
    if ((*iv)[i]>=0)
    {
      if ((j<0) || ((*iv)[i]<j))
      {
        j = (*iv)[i];
        r = i;
      }
    }
  }
  return r;
}

/*3
* initialize the resolution and puts in the argument as
* zeroth entre, length must be > 0
* assumes that the basering is degree-compatible
*/
static SRes syInitRes(ideal arg,int * length, intvec * Tl, intvec * cw=NULL)
{
  if (idIs0(arg)) return NULL;
  SRes resPairs = (SRes)Alloc0(*length*sizeof(SSet));
  resPairs[0] = (SSet)Alloc0(IDELEMS(arg)*sizeof(SObject));
  intvec * iv=NULL;
  int i,j;

  if (idRankFreeModule(arg)==0)
  {
    iv = idSort(arg);
    for (i=0;i<IDELEMS(arg);i++)
    {
      (resPairs[0])[i].syz = /*pCopy*/(arg->m[(*iv)[i]-1]);
      arg->m[(*iv)[i]-1] = NULL;
      (resPairs[0])[i].order = pTotaldegree((resPairs[0])[i].syz);
    } 
  }
  else
  {
    iv = new intvec(IDELEMS(arg),1,-1);
    for (i=0;i<IDELEMS(arg);i++)
    {
      (*iv)[i] = pTotaldegree(arg->m[i])+(*cw)[pGetComp(arg->m[i])-1];
    }
    for (i=0;i<IDELEMS(arg);i++)
    {
      j = syChMin(iv);
      if (j<0) break;
      (resPairs[0])[i].syz = arg->m[j];
      arg->m[j] = NULL;
      (resPairs[0])[i].order = (*iv)[j];
      (*iv)[j] = -1;
    }
  }
  if (iv!=NULL)  delete iv;
  (*Tl)[0] = IDELEMS(arg);
  return resPairs;
}

/*3
* determines the place of a polynomial in the right ordered resolution
* set the vectors of truecomponents
*/
static void syOrder(poly p,syStrategy syzstr,int index,
                    int realcomp)
{
  int i=IDELEMS(syzstr->res[index-1])+1,j=0,k,tc,orc,ie=realcomp-1;
  int *trind1=syzstr->truecomponents[index-1];
  int *trind=syzstr->truecomponents[index];
  int *bc=syzstr->backcomponents[index];
  int *F1=syzstr->Firstelem[index-1];
  int *H1=syzstr->Howmuch[index-1];
  poly pp;
  polyset or=syzstr->orderedRes[index]->m;
  polyset or1=syzstr->orderedRes[index-1]->m;

  if (p==NULL) return;
  if (realcomp==0) realcomp=1;
 
  if (index>1)
    tc = trind1[pGetComp(p)]-1;
  else
    tc = pGetComp(p)-1;
  loop         //while ((j<ie) && (trind1[orc]<=tc+1))
  {
    if (j>=ie)
      break;
    else
    {
      orc = pGetComp(or[j]);
      if (trind1[orc]>tc+1) break;
      j += H1[orc];
    }
  }
  if (j>ie)
  {
    WerrorS("orderedRes to small");
    return;
  }
  ie++;
  if (or[j]!=NULL)
  {
    for (k=ie-1;k>j;k--)
    {
      or[k] = or[k-1];
      bc[k] = bc[k-1];
    }
  }
  or[j] = p;
  bc[j] = realcomp-1;
  (H1[pGetComp(p)])++;
  for (k=0;k<i;k++)
  {
    if (F1[k]>j)
      (F1[k])++;
  }
  if (F1[pGetComp(p)]==0)
    F1[pGetComp(p)]=j+1;
//Print("write in sort %d till %d\n",index-1,i-1);
//Print("poly: ");pWrite(p);
//Print("in module %d as %d -th element\n",index,j);
  for (k=0;k<IDELEMS((syzstr->res)[index]);k++)
  {
    if (trind[k]>j)
      trind[k] += 1;
  }
  for (k=IDELEMS((syzstr->res)[index])-1;k>realcomp;k--)
    trind[k] = trind[k-1];
  trind[realcomp] = j+1;
}

/*3
* reduces all pairs of degree deg in the module index
* put the reduced generators to the resolvente which contains
* the truncated std 
*/
static void syRedNextPairs(SSet nextPairs, syStrategy syzstr,
               int howmuch, int index)
{
  int i=howmuch-1,j,k=IDELEMS(syzstr->res[index]);
  int ks=IDELEMS(syzstr->res[index+1]),kk,l,ll;
  int ** Fin=syzstr->Firstelem;
  int ** Hin=syzstr->Howmuch;
  int ** bin=syzstr->backcomponents;
  number coefgcd,n;
  ideal or=syzstr->orderedRes[index+1];
  polyset redset=syzstr->orderedRes[index]->m;
  poly p=NULL,q;
  BOOLEAN isDivisible;
  SObject tso;

  if ((nextPairs==NULL) || (howmuch==0)) return;
  while ((k>0) && (syzstr->res[index]->m[k-1]==NULL)) k--;
  while ((ks>0) && (syzstr->res[index+1]->m[ks-1]==NULL)) ks--;
  while (i>=0)
  {
    tso = nextPairs[i];
    isDivisible = FALSE;
    if (syzstr->res[index+1]!=NULL)
    {
      l = Fin[index][pGetComp(tso.lcm)]-1;
      if (l>=0)
      {
        ll = l+Hin[index][pGetComp(tso.lcm)];
        while ((l<ll) && (!isDivisible))
        {
          if (or->m[l]!=NULL)
          {
            isDivisible = isDivisible || 
              syDivisibleBy1(or->m[l],tso.lcm);
          }
          l++;
        }
      }
    }
    if (!isDivisible)
    {
      tso.p = 
        sySPoly(tso.p1, tso.p2,tso.lcm);
      coefgcd = 
        nGcd(pGetCoeff(tso.p1),pGetCoeff(tso.p1));
      tso.syz = pHead(tso.lcm);
      pSetm(tso.syz);
      p = tso.syz;
      pSetCoeff(p,nDiv(pGetCoeff(tso.p1),coefgcd));
      pGetCoeff(p) = nNeg(pGetCoeff(p));
      pSetComp(p,tso.ind2+1);
      pNext(p) = pHead(tso.lcm);
      pIter(p);
      pSetm(p);
      pSetComp(p,tso.ind1+1);
      pSetCoeff(p,nDiv(pGetCoeff(tso.p2),coefgcd));
      nDelete(&coefgcd);
      if (tso.p != NULL)
      {
        q = tso.p;
        j = Fin[index-1][pGetComp(q)]-1;
        int pos = j+Hin[index-1][pGetComp(q)];
        loop
        {
          if (j<0) break;
          if (syDivisibleBy1(redset[j],q))
          {
            //int syL=syLength(redset[j]);
            //Print("r");
//for(int jj=j+1;jj<k;jj++)
//{
  //if (syDivisibleBy(redset[jj],q))
  //{
    //int syL1=syLength(redset[jj]);
    //if (syL1<syL)
    //{
      //j = jj;
      //syL = syL1;
      //Print("take poly %d with %d monomials\n",j,syL);
      //Print("have poly %d with %d monomials\n",jj,syL1);
    //}
  //}
//}
            pNext(p) = pHead(q);
            pIter(p);
            pSetComp(p,bin[index][j]+1);
            pGetCoeff(p) = nNeg(pGetCoeff(p));
            q = sySPolyRed(q,redset[j]);
            if (q==NULL) break;
            j = Fin[index-1][pGetComp(q)]-1;
            pos = j+Hin[index-1][pGetComp(q)];
          }
          else
          {
            j++;
            if (j==pos) break;
          }
        }
        tso.p = q;
      }
      if (tso.p != NULL)
      {
        if (TEST_OPT_PROT) Print("g");
        if (k==IDELEMS((syzstr->res)[index]))
        {
          pEnlargeSet(&(syzstr->res[index]->m),IDELEMS(syzstr->res[index]),16);
          syzstr->truecomponents[index]=(int*)ReAlloc((ADDRESS)syzstr->truecomponents[index],
                                       (IDELEMS(syzstr->res[index])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index])+17)*sizeof(int));
          syzstr->backcomponents[index]=(int*)ReAlloc((ADDRESS)syzstr->backcomponents[index],
                                       (IDELEMS(syzstr->res[index])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index])+17)*sizeof(int));
          syzstr->Howmuch[index]=(int*)ReAlloc((ADDRESS)syzstr->Howmuch[index],
                                       (IDELEMS(syzstr->res[index])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index])+17)*sizeof(int));
          syzstr->Firstelem[index]=(int*)ReAlloc((ADDRESS)syzstr->Firstelem[index],
                                       (IDELEMS(syzstr->res[index])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index])+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(syzstr->res[index])+1;mk<IDELEMS(syzstr->res[index])+17;mk++)
        {
          syzstr->Howmuch[index][mk] = 0;
          syzstr->Firstelem[index][mk] = 0;
        }
//Print("sort %d has now size %d\n",index,IDELEMS(res[index])+17);
          IDELEMS(syzstr->res[index]) += 16;
          pEnlargeSet(&(syzstr->orderedRes[index]->m),IDELEMS(syzstr->orderedRes[index]),16);
          IDELEMS(syzstr->orderedRes[index]) += 16;
          redset=syzstr->orderedRes[index]->m;
        }
        pNext(p) = pHead(tso.p);
        pIter(p);
        if (p!=NULL)
        {
          k++;
          pSetComp(p,k);
          pGetCoeff(p) = nNeg(pGetCoeff(p));
        }
        if (tso.p!=NULL)
        {
          syzstr->res[index]->m[k-1] = tso.p;
          pNorm(syzstr->res[index]->m[k-1]);
          syOrder(syzstr->res[index]->m[k-1],syzstr,index,k);
        }
        tso.isNotMinimal = p;
        tso.p = NULL;
      }
      else
      {
        if (TEST_OPT_PROT) Print(".");
        if (index % 2==0)
          euler++;
        else 
          euler--;
      }
      if (ks==IDELEMS(syzstr->res[index+1]))
      {
        pEnlargeSet(&(syzstr->res[index+1]->m),IDELEMS(syzstr->res[index+1]),16);
        syzstr->truecomponents[index+1]=(int*)ReAlloc((ADDRESS)syzstr->truecomponents[index+1],
                                       (IDELEMS(syzstr->res[index+1])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index+1])+17)*sizeof(int));
        syzstr->backcomponents[index+1]=(int*)ReAlloc((ADDRESS)syzstr->backcomponents[index+1],
                                       (IDELEMS(syzstr->res[index+1])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index+1])+17)*sizeof(int));
        syzstr->Howmuch[index+1]=(int*)ReAlloc((ADDRESS)syzstr->Howmuch[index+1],
                                       (IDELEMS(syzstr->res[index+1])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index+1])+17)*sizeof(int));
        syzstr->Firstelem[index+1]=(int*)ReAlloc((ADDRESS)syzstr->Firstelem[index+1],
                                       (IDELEMS(syzstr->res[index+1])+1)*sizeof(int),
                                       (IDELEMS(syzstr->res[index+1])+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(syzstr->res[index+1])+1;mk<IDELEMS(syzstr->res[index+1])+17;mk++)
        {
          syzstr->Howmuch[index+1][mk] = 0;
          syzstr->Firstelem[index+1][mk] = 0;
        }
//Print("sort %d has now size %d\n",index+1,IDELEMS(res[index+1])+17);
        IDELEMS(syzstr->res[index+1]) += 16;
        pEnlargeSet(&(syzstr->orderedRes[index+1]->m),IDELEMS(syzstr->orderedRes[index+1]),16);
        IDELEMS(syzstr->orderedRes[index+1]) += 16;
      }
      currcomponents = syzstr->truecomponents[index];
      syzstr->res[index+1]->m[ks] = syRedtail(tso.syz,syzstr,index+1);
      currcomponents = syzstr->truecomponents[index-1];
      syzstr->res[index+1]->m[ks] = tso.syz;
      pNorm(syzstr->res[index+1]->m[ks]);
      tso.syz =NULL;
      tso.syzind = ks;
      syOrder(syzstr->res[index+1]->m[ks],syzstr,index+1,ks+1);
      ks++;
      p = NULL;
      nextPairs[i] = tso;
    }
    else
    {
      syDeletePair(&nextPairs[i]);
      //crit++;
    }
    i--;
  }
} 

/*3
* reduces the generators of the module index in degree deg
* (which are actual syzygies of the module index-1)
* wrt. the ideal generated by elements of lower degrees
*/
static void syRedGenerOfCurrDeg(syStrategy syzstr, int deg, int index)
{
  ideal res=syzstr->res[index];
  int i=0,j,k=IDELEMS(res),kk;
  SSet sPairs=syzstr->resPairs[index-1];

  while ((k>0) && (res->m[k-1]==NULL)) k--;
  while ((i<(*syzstr->Tl)[index-1]) && (((sPairs)[i].syz==NULL) || 
          ((sPairs)[i].order<deg)))
    i++;
  if ((i>=(*syzstr->Tl)[index-1]) || ((sPairs)[i].order>deg)) return;
  while ((i<(*syzstr->Tl)[index-1]) && (((sPairs)[i].syz==NULL) ||
         ((sPairs)[i].order==deg))) 
  {
    if ((sPairs)[i].syz!=NULL)
    {
      j = k-1;
      while ((j>=0) && (res->m[j]!=NULL) && 
             ((sPairs)[i].syz!=NULL))
      {
        if (syDivisibleBy(res->m[j],(sPairs)[i].syz))
        {
          //Print("r");
          (sPairs)[i].syz = 
            //spSpolyRed(res->m[j],(sPairs)[i].syz,NULL);
            sySPolyRed((sPairs)[i].syz,res->m[j]);
          j = k-1;
        }
        else
        {
          j--;
        }
      }
      if ((sPairs)[i].syz != NULL)
      {
        if (k==IDELEMS(res))
        {
          pEnlargeSet(&(res->m),IDELEMS(res),16);
          syzstr->truecomponents[index]=(int*)ReAlloc((ADDRESS)syzstr->truecomponents[index],
                                       (IDELEMS(res)+1)*sizeof(int),
                                       (IDELEMS(res)+17)*sizeof(int));
          syzstr->backcomponents[index]=(int*)ReAlloc((ADDRESS)syzstr->backcomponents[index],
                                       (IDELEMS(res)+1)*sizeof(int),
                                       (IDELEMS(res)+17)*sizeof(int));
          syzstr->Howmuch[index]=(int*)ReAlloc((ADDRESS)syzstr->Howmuch[index],
                                       (IDELEMS(res)+1)*sizeof(int),
                                       (IDELEMS(res)+17)*sizeof(int));
          syzstr->Firstelem[index]=(int*)ReAlloc((ADDRESS)syzstr->Firstelem[index],
                                       (IDELEMS(res)+1)*sizeof(int),
                                       (IDELEMS(res)+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(res)+1;mk<IDELEMS(res)+17;mk++)
        {
          syzstr->Howmuch[index][mk] = 0;
          syzstr->Firstelem[index][mk] = 0;
        }
//Print("sort %d has now size %d\n",index,IDELEMS(res[index])+17);
          IDELEMS(res) += 16;
          pEnlargeSet(&(syzstr->orderedRes[index]->m),IDELEMS(syzstr->orderedRes[index]),16);
          IDELEMS(syzstr->orderedRes[index]) += 16;
        }
        if (BTEST1(6))
        {
          if ((sPairs)[i].isNotMinimal==NULL)
          {
            PrintLn();
            Print("minimal generator: ");pWrite((syzstr->resPairs[index-1])[i].syz);
            Print("comes from: ");pWrite((syzstr->resPairs[index-1])[i].p1);
            Print("and: ");pWrite((syzstr->resPairs[index-1])[i].p2);
          }
        }
        //res->m[k] = (sPairs)[i].syz;
        res->m[k] = syRedtail((sPairs)[i].syz,syzstr,index);
        (sPairs)[i].syzind = k;
        pNorm(res->m[k]);
  //      (sPairs)[i].syz = NULL;
        k++;
        syOrder(res->m[k-1],syzstr,index,k);
        euler++;
      }
      else
        (sPairs)[i].syzind = -1;
    }
    i++;
  }
}

/*3
* puts a pair into the right place in resPairs
*/
static void syEnterPair(SSet sPairs, SObject * so, int * sPlength,int index)
{
  int ll,k,no=(*so).order,sP=*sPlength,i;
  poly p=(*so).lcm;

  if ((sP==0) || (sPairs[sP-1].order<=no))
    ll = sP;
  else if (sP==1)
    ll = 0;
  else
  {
    int an=0,en=sP-1;
    loop 
    {
      if (an>=en-1)
      {
        if ((sPairs[an].order<=no) && (sPairs[an+1].order>no))
        {
          ll = an+1;
          break;
        }
        else if ((sPairs[en].order<=no) && (sPairs[en+1].order>no))
        {
          ll = en+1;
          break;
        }
        else if (sPairs[an].order>no)
        {
          ll = an;
          break;
        }
        else
        {
          Print("Hier ist was faul!\n");
          break;
        }
      }
      i=(an+en) / 2;
      if (sPairs[i].order <= no)
        an=i;
      else
        en=i;
    }
  }
  for (k=(*sPlength);k>ll;k--)
  {
    syCopyPair(&sPairs[k-1],&sPairs[k]);
  }
  syCopyPair(so,&sPairs[ll]);
  (*sPlength)++;
}

/*3
* computes pairs from the new elements (beginning with the element newEl)
* in the module index
*/
static void syCreateNewPairs(syStrategy syzstr, int index, int newEl)
{
  SSet temp;
  SObject tso;
  int i,ii,j,k=IDELEMS(syzstr->res[index]),l=(*syzstr->Tl)[index],ll;
  int qc,first,pos,jj,j1;
  int * bci=syzstr->backcomponents[index];
  poly p,q;
  polyset rs=syzstr->res[index]->m,nPm;

  while ((k>0) && (rs[k-1]==NULL)) k--;
  if (newEl>=k) return;
  ideal nP=idInit(k,syzstr->res[index]->rank);
  nPm=nP->m;
  while ((l>0) && ((syzstr->resPairs[index])[l-1].p1==NULL)) l--;
//Print("new pairs in module %d\n",index);
  for (j=newEl;j<k;j++)
  {
    q = rs[j];
    qc = pGetComp(q);
    first = syzstr->Firstelem[index-1][pGetComp(q)]-1;
    pos = first+syzstr->Howmuch[index-1][pGetComp(q)];
    for (i=first;i<pos;i++)
    {
      jj = bci[i];
      if (jj>=j) break;
      p = pOne();
      pLcm(rs[jj],q,p);
      pSetComp(p,j+1);
      ii = first;
      loop 
      {
        j1 = bci[ii];
        if (nPm[j1]!=NULL)
        {
          if (syDivisibleBy1(nPm[j1],p))
          {
            pDelete(&p);
            break;
          }
          else if (syDivisibleBy1(p,nPm[j1]))
          {
            pDelete(&(nPm[j1]));
            break;
          }
        }
        ii++;
        if (ii>=pos) break;
      }
      if (p!=NULL)
      {
        nPm[jj] = p;
      }
    }
    //for (i=0;i<j;i++)
    for (i=first;i<pos;i++)
    {
      ii = bci[i];
      if (nPm[ii]!=NULL)
      {
        if (l>=(*syzstr->Tl)[index])
        {
          temp = (SSet)Alloc0(((*syzstr->Tl)[index]+16)*sizeof(SObject));
          for (ll=0;ll<(*syzstr->Tl)[index];ll++)
          {
            temp[ll].p = (syzstr->resPairs[index])[ll].p;
            temp[ll].p1 = (syzstr->resPairs[index])[ll].p1;
            temp[ll].p2 = (syzstr->resPairs[index])[ll].p2;
            temp[ll].syz = (syzstr->resPairs[index])[ll].syz;
            temp[ll].lcm = (syzstr->resPairs[index])[ll].lcm;
            temp[ll].ind1 = (syzstr->resPairs[index])[ll].ind1;
            temp[ll].ind2 = (syzstr->resPairs[index])[ll].ind2;
            temp[ll].syzind = (syzstr->resPairs[index])[ll].syzind;
            temp[ll].order = (syzstr->resPairs[index])[ll].order;
            temp[ll].isNotMinimal = (syzstr->resPairs[index])[ll].isNotMinimal;
          }
          Free((ADDRESS)syzstr->resPairs[index],(*syzstr->Tl)[index]*sizeof(SObject));
          (*syzstr->Tl)[index] += 16;
          syzstr->resPairs[index] = temp;
        }
        tso.lcm = p = nPm[ii];
        nPm[ii] = NULL;
        tso.order = pGetOrder(p) = pTotaldegree(p);
        if ((syzstr->cw!=NULL) && (index>0) && (pGetComp(q)>0))
        {
          int ii=index-1,jj=pGetComp(q);
          while (ii>0) 
          {
            jj = pGetComp(syzstr->res[ii]->m[jj-1]);
            ii--;
          }
          tso.order += (*syzstr->cw)[jj-1];
        }
        tso.p1 = rs[ii];
        tso.p2 = q;
        tso.ind1 = ii;
        tso.ind2 = j;
        tso.syzind = -1;
        tso.isNotMinimal = NULL;
        tso.p = NULL;
        tso.syz = NULL;
        syEnterPair(syzstr->resPairs[index],&tso,&l,index);
      }
    }
  }
  idDelete(&nP);
}

static void sySyzTail(syStrategy syzstr, int index, int newEl)
{
  int j,ll,k=IDELEMS((syzstr->res)[index]);
  while ((k>0) && ((syzstr->res)[index]->m[k-1]==NULL)) k--;
  for (j=newEl;j<k;j++)
  {
    ll = 0;
    while ((ll<(*syzstr->Tl)[index]) && (syzstr->resPairs[index][ll].p1!=NULL) &&
           (syzstr->resPairs[index][ll].ind1!=j) && (syzstr->resPairs[index][ll].ind2!=j))
      ll++;
    if ((ll<(*syzstr->Tl)[index]) && (syzstr->resPairs[index][ll].p1!=NULL))
      syzstr->res[index]->m[j] = syRedtail(syzstr->res[index]->m[j],syzstr,index);
  }
}

static SSet syChosePairsPutIn(syStrategy syzstr, int *index,
               int *howmuch, int * actdeg, int an, int en)
{
  int newdeg=*actdeg,newindex=-1,i,t,sldeg;
  poly p;
  SSet result;
  SRes resPairs=syzstr->resPairs;
  
  if (an>syzstr->length) return NULL;
  if (en>syzstr->length) en=syzstr->length;
  while (*index<en)
  {
    if (resPairs[*index]!=NULL)
    {
      sldeg = (*actdeg)+*index;
      i = 0;
      if (*index!=0)
      {
        while ((i<(*syzstr->Tl)[*index]))
        {
          if ((resPairs[*index])[i].lcm!=NULL)
          {
            if ((resPairs[*index])[i].order == sldeg)
            {
              result = &(resPairs[*index])[i];
              *howmuch =1;
              i++;
              while ((i<(*syzstr->Tl)[*index]) && ((resPairs[*index])[i].lcm!=NULL)
                      && ((resPairs[*index])[i].order == sldeg))
              {
                i++;
                (*howmuch)++;
              }
              return result;
            }
          }
          i++;
        }
      }
      else
      {
        while ((i<(*syzstr->Tl)[*index]))
        {
          if ((resPairs[*index])[i].syz!=NULL)
          {
            if ((resPairs[*index])[i].order == sldeg)
            {
              result = &(resPairs[*index])[i];
              (*howmuch) =1;
              i++;
              while ((i<(*syzstr->Tl)[*index]) && ((resPairs[*index])[i].syz!=NULL)
                      && ((resPairs[*index])[i].order == *actdeg))
              {
                i++;
                (*howmuch)++;
              }
              return result;
            }
          }
          i++;
        }
      }
    }
    (*index)++;
  }
  *index = an;
  //if (TEST_OPT_PROT) Print("(Euler:%d)",euler);
  while (*index<en)
  {
    if (resPairs[*index]!=NULL)
    {
      i = 0;
      while ((i<(*syzstr->Tl)[*index]))
      {
        t = *actdeg+*index;
        if (((resPairs[*index])[i].lcm!=NULL) || 
              ((resPairs[*index])[i].syz!=NULL))
        {
          if ((resPairs[*index])[i].order > t)
            t = (resPairs[*index])[i].order;
        }
        if ((t>*actdeg+*index) && ((newdeg==*actdeg) || (t<newdeg+*index)))
        {
          newdeg = t-*index;
          newindex = *index;
          break;
        }
        i++;
      } 
    }
    (*index)++;
  }
  if (newdeg>*actdeg)
  {
    *actdeg = newdeg;
    *index = newindex;
    return syChosePairsPutIn(syzstr,index,howmuch,actdeg,an,en);
  }
  else return NULL;
}

/*3
* FOR THE HOMOGENEOUS CASE ONLY!
* looks through the pair set and the given module for
* remaining pairs or generators to consider
* returns a pointer to the first pair and the number of them in the given module
* works with slanted degree (i.e. deg=realdeg-index)
*/
static SSet syChosePairs(syStrategy syzstr, int *index,
               int *howmuch, int * actdeg)
{
  return syChosePairsPutIn(syzsts,index,howmuch,actdeg,0,syzstr->length);
}

/*3
* FOR THE INHOMOGENEOUS CASE ONLY!
* looks through the pair set and the given module for
* remaining pairs or generators to consider
* returns a pointer to the first pair and the number of them in the given module
* works with slanted degree (i.e. deg=realdeg-index)
* looks first through the 0 and 1 module then through the other
*/
static SSet syChosePairsIH(syStrategy syzstr, int *index,
               int *howmuch, int * actdeg, int mindeg)
{
  SSet result=NULL;

  result = syChosePairsPutIn(syzstr,index,howmuch,actdeg,0,2);
  if (result == NULL)
  {
    *actdeg = mindeg;
    result = syChosePairsPutIn(syzstr,index,howmuch,actdeg,2,syzstr->length);
  }
  return result;
}

/*3
* looks through the pair set and the given module for
* remaining pairs or generators to consider
* returns a pointer to the first pair and the number of them in the given module
* works deg by deg
*/
/*
*static SSet syChosePairs1(SRes resPairs,intvec * Tl, int *index, int *howmuch,
*                   int length,int * actdeg)
*{
*  int newdeg=*actdeg,newindex=-1,i,t;
*  SSet result;
*  
*  while (*index>=0)
*  {
*    if (resPairs[*index]!=NULL)
*    {
*      i = 0;
*      if (*index!=0)
*      {
*        while ((i<(*Tl)[*index]))
*        {
*          if ((resPairs[*index])[i].lcm!=NULL)
*          {
*            if (pGetOrder((resPairs[*index])[i].lcm) == *actdeg)
*            {
*              result = &(resPairs[*index])[i];
*              *howmuch =1;
*              i++;
*              while ((i<(*Tl)[*index]) && ((resPairs[*index])[i].lcm!=NULL)
*                      && (pGetOrder((resPairs[*index])[i].lcm) == *actdeg))
*              {
*                i++;
*                (*howmuch)++;
*              }
*              return result;
*            }
*          }
*          i++;
*        }
*      }
*      else
*      {
*        while ((i<(*Tl)[*index]))
*        {
*          if ((resPairs[*index])[i].syz!=NULL)
*          {
*            if ((resPairs[*index])[i].order == *actdeg)
*            {
*              result = &(resPairs[*index])[i];
*              (*howmuch) =1;
*              i++;
*              while ((i<(*Tl)[*index]) && ((resPairs[*index])[i].syz!=NULL)
*                      && ((resPairs[*index])[i].order == *actdeg))
*              {
*                i++;
*                (*howmuch)++;
*              }
*              return result;
*            }
*          }
*          i++;
*        }
*      }
*    }
*    (*index)--;
*  }
*  *index = length-1;
*  while (*index>=0)
*  {
*    if (resPairs[*index]!=NULL)
*    {
*      i = 0;
*      while ((i<(*Tl)[*index]))
*      {
*        t = *actdeg;
*        if ((resPairs[*index])[i].lcm!=NULL)
*        {
*          if (pGetOrder((resPairs[*index])[i].lcm) > *actdeg)
*            t = pGetOrder((resPairs[*index])[i].lcm);
*        }
*        else if ((resPairs[*index])[i].syz!=NULL)
*        {
*          if ((resPairs[*index])[i].order > *actdeg)
*            t = (resPairs[*index])[i].order;
*        }
*        if ((t>*actdeg) && ((newdeg==*actdeg) || (t<newdeg)))
*        {
*          newdeg = t;
*          newindex = *index;
*          break;
*        }
*        i++;
*      } 
*    }
*    (*index)--;
*  }
*  if (newdeg>*actdeg)
*  {
*    *actdeg = newdeg;
*    *index = newindex;
*    return syChosePairs1(resPairs,Tl,index,howmuch,length,actdeg);
*  }
*  else return NULL;
*}
*/
/*3
* statistics of the resolution
*/
static void syStatistics(resolvente res,int length)
{
  int i,j=1,k,deg=0;

  PrintLn();
  while ((j<length) && (res[j]!=NULL))
  {
    Print("In module %d: \n",j);
    k = 0;
    while ((k<IDELEMS(res[j])) && (res[j]->m[k]!=NULL))
    {
      i = 1;
      deg = pGetOrder(res[j]->m[k]);
      k++;
      while ((k<IDELEMS(res[j])) && (res[j]->m[k]!=NULL) &&
              (pGetOrder(res[j]->m[k])==deg))
      {
        i++;
        k++;
      }
      Print("%d elements of degree %d\n",i,deg);
    }
    j++;
  }
}

/*3
* initialize a module
*/
static int syInitSyzMod(syStrategy syzstr, int index, int init=17)
{
  int result;

  if (syzstr->res[index]==NULL) 
  {
    syzstr->res[index] = idInit(init-1,1);
    syzstr->truecomponents[index] = (int*)Alloc0(init*sizeof(int));
    if (index==0)
    {
      for (int i=0;i<init;i++)
        syzstr->truecomponents[0][i] = i;
    }
    syzstr->backcomponents[index] = (int*)Alloc0(init*sizeof(int));
    syzstr->Howmuch[index] = (int*)Alloc0(init*sizeof(int));
    syzstr->Firstelem[index] = (int*)Alloc0(init*sizeof(int));
//Print("sort %d has now size %d\n",index,init);
    syzstr->orderedRes[index] = idInit(init-1,1);
    result = 0;
  }
  else
  {
    result = IDELEMS(syzstr->res[index]);
    while ((result>0) && (syzstr->res[index]->m[result-1]==NULL)) result--;
  }
  return result;
}

void syKillComputation(syStrategy syzstr)
{
//Print("ref: %d\n",syzstr->references);
  if (syzstr->references>0)
  {
    (syzstr->references)--;
  }
  else
  {
    int i,j;
  
    if (syzstr->resPairs!=NULL)
    {
      for (i=0;i<syzstr->length;i++)
      {
        for (j=0;j<(*syzstr->Tl)[i];j++)
        {
          if ((syzstr->resPairs[i])[j].lcm!=NULL)
            pDelete(&((syzstr->resPairs[i])[j].lcm));
        }
        if (syzstr->orderedRes[i]!=NULL)
        {
          for (j=0;j<IDELEMS(syzstr->orderedRes[i]);j++)
          {
            syzstr->orderedRes[i]->m[j] = NULL;
          }
        }
        idDelete(&(syzstr->orderedRes[i]));
        if (syzstr->truecomponents[i]!=NULL)
        {
          Free((ADDRESS)syzstr->truecomponents[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
          syzstr->truecomponents[i]=NULL;
        }
        if (syzstr->backcomponents[i]!=NULL)
        {
          Free((ADDRESS)syzstr->backcomponents[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
          syzstr->backcomponents[i]=NULL;
        }
        if (syzstr->Howmuch[i]!=NULL)
        {
          Free((ADDRESS)syzstr->Howmuch[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
          syzstr->Howmuch[i]=NULL;
        }
        if (syzstr->Firstelem[i]!=NULL)
        {
          Free((ADDRESS)syzstr->Firstelem[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
          syzstr->Firstelem[i]=NULL;
        }
        if (syzstr->res[i]!=NULL)
        {
          for (j=0;j<IDELEMS(syzstr->res[i]);j++)
          {
            if (syzstr->res[i]->m[j]!=NULL)
              pDelete(&(syzstr->res[i]->m[j]));
          }
        }
        idDelete(&(syzstr->res[i]));
        Free((ADDRESS)syzstr->resPairs[i],(*syzstr->Tl)[i]*sizeof(SObject));
      }
      Free((ADDRESS)syzstr->resPairs,syzstr->length*sizeof(SObject*));
      Free((ADDRESS)syzstr->res,(syzstr->length+1)*sizeof(ideal));
      Free((ADDRESS)syzstr->orderedRes,(syzstr->length+1)*sizeof(ideal));
      Free((ADDRESS)syzstr->truecomponents,(syzstr->length+1)*sizeof(int*));
      Free((ADDRESS)syzstr->backcomponents,(syzstr->length+1)*sizeof(int*));
      Free((ADDRESS)syzstr->Howmuch,(syzstr->length+1)*sizeof(int*));
      Free((ADDRESS)syzstr->Firstelem,(syzstr->length+1)*sizeof(int*));
      Free((ADDRESS)syzstr->binom,pVariables*(syzstr->highdeg_1)*sizeof(int));
    }
    if (syzstr->minres!=NULL)
    {
      for (i=0;i<syzstr->length;i++)
      {
        if (syzstr->minres[i]!=NULL)
        {
          for (j=0;j<IDELEMS(syzstr->minres[i]);j++)
          {
            if (syzstr->minres[i]->m[j]!=NULL)
              pDelete(&(syzstr->minres[i]->m[j]));
          }
        }
        idDelete(&(syzstr->minres[i]));
      }
      Free((ADDRESS)syzstr->minres,(syzstr->length+1)*sizeof(ideal));
    }
    if (syzstr->fullres!=NULL)
    {
      for (i=0;i<syzstr->length;i++)
      {
        if (syzstr->fullres[i]!=NULL)
        {
          for (j=0;j<IDELEMS(syzstr->fullres[i]);j++)
          {
            if (syzstr->fullres[i]->m[j]!=NULL)
              pDelete(&(syzstr->fullres[i]->m[j]));
          }
        }
        idDelete(&(syzstr->fullres[i]));
      }
      Free((ADDRESS)syzstr->fullres,(syzstr->length+1)*sizeof(ideal));
    }
    if (syzstr->cw!=NULL)
      delete syzstr->cw;
    if (syzstr->resolution!=NULL)
      delete syzstr->resolution;
    Free((ADDRESS)syzstr,sizeof(ssyStrategy));
  }
}

intvec * syBettiOfComputation(syStrategy syzstr)
{
  int dummy;
  if (syzstr->resPairs!=NULL)
  {
    int i,j=-1,jj=-1,l;
    SRes rP=syzstr->resPairs;
   
    l = syzstr->length;
    while ((l>0) && (rP[l-1]==NULL)) l--;
    if (l==0) return NULL;
    l--;
    while (l>=0)
    {
      i = 0;
      while ((i<(*syzstr->Tl)[l]) && 
        ((rP[l][i].lcm!=NULL) || (rP[l][i].syz!=NULL)))
      {
        if (rP[l][i].isNotMinimal==NULL) 
        {
          if (j<rP[l][i].order-l)
            j = rP[l][i].order-l;
          if (jj<l)
            jj = l;
        }
        i++;
      }
      l--;
    }
    jj=jj+2;
    intvec *result=new intvec(j,jj,0);
    IMATELEM(*result,1,1) = max(1,idRankFreeModule(syzstr->res[1]));
    for (i=0;i<jj;i++)
    {
      j = 0;
      while ((j<(*syzstr->Tl)[i]) && 
        ((rP[i][j].lcm!=NULL) || (rP[i][j].syz!=NULL)))
      {
        if (rP[i][j].isNotMinimal==NULL)
          IMATELEM(*result,rP[i][j].order-i,i+2)++;
        j++;
      }
    }
    return result;
  }
  else if (syzstr->fullres!=NULL)
    return syBetti(syzstr->fullres,syzstr->length,&dummy);
  else
    return syBetti(syzstr->minres,syzstr->length,&dummy);
}

int syLength(syStrategy syzstr)
{
  return syzstr->length;
}

int sySize(syStrategy syzstr)
{
  resolvente r=syzstr->res;
  if (r==NULL)
    r = syzstr->fullres;
  if (r==NULL)
    r = syzstr->minres;
  if (r==NULL)
  {
    WerrorS("No resolution found");
    return 0;
  }
  int i=syzstr->length;
  while ((i>0) && (r[i-1]==NULL)) i--;
  return i;
}

int syDim(syStrategy syzstr)
{
  int i,j=-1,l;
  if (syzstr->resPairs!=NULL)
  {
    SRes rP=syzstr->resPairs;
   
    l = syzstr->length;
    while ((l>0) && (rP[l-1]==NULL)) l--;
    if (l==0) return -1;
    l--;
    while (l>=0)
    {
      i = 0;
      while ((i<(*syzstr->Tl)[l]) && 
        ((rP[l][i].lcm!=NULL) || (rP[l][i].syz!=NULL)) &&
        (rP[l][i].isNotMinimal!=NULL))
      {
        i++;
      }
      if ((i<(*syzstr->Tl)[l]) &&
        ((rP[l][i].lcm!=NULL) || (rP[l][i].syz!=NULL)) &&
        (rP[l][i].isNotMinimal==NULL))
        return l;
      l--;
    }
    return l;
  }
  else 
    return sySize(syzstr);
}

syStrategy syCopy(syStrategy syzstr)
{
  syStrategy result=syzstr;
  (result->references)++;
  return result;
}

static void syPrintEmptySpaces(int i)
{
  if (i!=0)
  {
    Print(" ");
    syPrintEmptySpaces(i/10);
  }
}

static void syPrintEmptySpaces1(int i)
{
  if (i!=0)
  {
    Print(" ");
    syPrintEmptySpaces1(i-1);
  }
}

static int syLengthInt(int i)
{
  int j=0;

  if (i==0) return 1;
  while (i!=0)
  {
    j++;
    i = i/10;
  }
  return j;
}

void syPrint(syStrategy syzstr)
{
  if ((syzstr->resPairs==NULL) && (syzstr->fullres==NULL) 
     && (syzstr->minres==NULL))
  {
    Print("No resolution defined\n");
    return;
  }
  int l=0;
  if (syzstr->resolution==NULL)
  {
    int j;
    if (syzstr->resPairs!=NULL)
    {
      syzstr->resolution = new intvec(syzstr->length+1);
      SRes rP=syzstr->resPairs;
      (*syzstr->resolution)[0] = max(1,idRankFreeModule(syzstr->res[1]));
      while ((l<syzstr->length) && (rP[l]!=NULL))
      {
        j=0;
        while ((j<(*syzstr->Tl)[l]) && 
          ((rP[l][j].lcm!=NULL) || (rP[l][j].syz!=NULL)))
        {
          if (rP[l][j].isNotMinimal==NULL)
            ((*syzstr->resolution)[l+1])++;
          j++;
        }
        l++;
      }
    }
    else
    {
      resolvente rr;
      syzstr->resolution = new intvec(syzstr->length+2);
      if (syzstr->minres!=NULL)
        rr = syzstr->minres;
      else
        rr = syzstr->fullres;
      (*syzstr->resolution)[0] = max(1,idRankFreeModule(rr[0]));
      while ((l<syzstr->length) && (rr[l]!=NULL))
      {
        j = IDELEMS(rr[l]);
        while ((l>0) && (rr[l]->m[j-1]==NULL)) j--;
        ((*syzstr->resolution)[l+1]) = j;
        l++;
      }
    }
  }
  char *sn=currRingHdl->id;
  int sl=strlen(sn);
  syPrintEmptySpaces1(sl);
  l = 0;
  loop
  {
    Print("%d",(*syzstr->resolution)[l]);
    if ((l>=syzstr->resolution->length()) || ((*syzstr->resolution)[l]==0))
      break;
    syPrintEmptySpaces1(sl+5);
    l++;
  }
  PrintLn();
  l = 0;
  loop
  {
    Print(sn);
    if ((l>=syzstr->resolution->length()) || ((*syzstr->resolution)[l]==0))
      break;
    syPrintEmptySpaces((*syzstr->resolution)[l]);
    Print(" <-- ");
    l++;
  }
  PrintLn();
  PrintLn();
  l = 0;
  loop
  {
    Print("%d",l);
    if ((l>=syzstr->resolution->length()) || ((*syzstr->resolution)[l]==0))
      break;
    syPrintEmptySpaces1(sl+5+syLengthInt((*syzstr->resolution)[l])-
                         syLengthInt(l));
    l++;
  }
  PrintLn();
  if (syzstr->minres==NULL)
  {
    Print("resolution not minimized yet");
    PrintLn();
  }
}

static resolvente syReorder(resolvente res,int length,
        syStrategy syzstr,BOOLEAN toCopy=TRUE,resolvente totake=NULL)
{
  int i,j,l;
  poly p,q,tq;
  polyset ri1;
  resolvente fullres;
  fullres = (resolvente)Alloc0((length+1)*sizeof(ideal));
  if (totake==NULL) 
    totake = res;
  for (i=length-1;i>0;i--)
  {
    if (res[i]!=NULL)
    {
      if (i>1)
      {
        j = IDELEMS(res[i-1]);
        while ((j>0) && (res[i-1]->m[j-1]==NULL)) j--;
        fullres[i-1] = idInit(IDELEMS(res[i]),j);
        ri1 = totake[i-1]->m;
        for (j=IDELEMS(res[i])-1;j>=0;j--)
        {
          p = res[i]->m[j];
          q = NULL;
          while (p!=NULL)
          {
            if (toCopy)
            {
              tq = pHead(p);
              pIter(p);
            }
            else
            {
              tq = p;
              pIter(p);
              pNext(tq) = NULL;
            }
            for (l=pVariables;l>0;l--)
              pGetExp(tq,l) -= pGetExp(ri1[pGetComp(tq)-1],l);
            pSetm(tq);
            q = pAdd(q,tq);
          }
          fullres[i-1]->m[j] = q;
        }
      }
      else
      {
        if (toCopy)
          fullres[i-1] = idCopy(res[i]);
        else
          fullres[i-1] = res[i];
        for (j=IDELEMS(res[i])-1;j>=0;j--)
          fullres[i-1]->m[j] = pOrdPoly(fullres[i-1]->m[j]);
      }
    }
  }
  if (!toCopy)
    Free((ADDRESS)res,(length+1)*sizeof(ideal));
  //syzstr->length = length;
  return fullres;
}

lists syConvRes(syStrategy syzstr)
{
  if ((syzstr->fullres==NULL) && (syzstr->minres==NULL))
  {
    syzstr->fullres = syReorder(syzstr->res,syzstr->length,syzstr);
  }
  resolvente tr;
  int typ0;
  if (syzstr->minres!=NULL)
    tr = syzstr->minres;
  else
    tr = syzstr->fullres;
  resolvente trueres=(resolvente)Alloc0((syzstr->length)*sizeof(ideal));
  for (int i=(syzstr->length)-1;i>=0;i--)
  {
    if (tr[i]!=NULL)
    {
      trueres[i] = idCopy(tr[i]);
    }
  }
  if (idRankFreeModule(trueres[0])==0)
    typ0 = IDEAL_CMD;
  else
    typ0 = MODUL_CMD;
  return liMakeResolv(trueres,syzstr->length,-1,typ0,NULL);
}

syStrategy syConvList(lists li)
{
  int typ0;
  syStrategy result=(syStrategy)Alloc0(sizeof(ssyStrategy));

  resolvente fr = liFindRes(li,&(result->length),&typ0);
  result->fullres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      result->fullres[i] = idCopy(fr[i]);
  }
  Free((ADDRESS)fr,(result->length)*sizeof(ideal));
  return result;
}

syStrategy syForceMin(lists li)
{
  int typ0;
  syStrategy result=(syStrategy)Alloc0(sizeof(ssyStrategy));

  resolvente fr = liFindRes(li,&(result->length),&typ0);
  result->minres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      result->minres[i] = idCopy(fr[i]);
  }
  Free((ADDRESS)fr,(result->length)*sizeof(ideal));
  return result;
}

static poly syStripOut(poly p,intvec * toStrip)
{
  if (toStrip==NULL) return p;
  poly pp=p;
  
  while ((pp!=NULL) && ((*toStrip)[pGetComp(pp)]!=0))
    pDelete1(&pp);
  p = pp;
  if (pp!=NULL) 
  {
    while (pNext(pp)!=NULL)
    {
      if ((*toStrip)[pGetComp(pNext(pp))]!=0)
        pDelete1(&pNext(pp));
      else
        pIter(pp);
    }
  }
  return p;
}

static poly syMinimizeP(poly toMin,syStrategy syzstr,int pNum,int index,
                        intvec * toStrip)
{
  int i,j,tc,lastin,newin=pNum-1;
  poly p,pp=pCopy(toMin),q=NULL,tq,pisN;
  SSet sPairs=syzstr->resPairs[index];

  pp = syStripOut(pp,toStrip);
  while (newin>=0)
  {
    lastin = newin;
    while ((newin>=0) && (sPairs[lastin].order==sPairs[newin].order))
      newin--;
//Print("Hier lastin:%d newin:%d\n",lastin,newin);
    for (i=newin+1;i<=lastin;i++)
    {
      if (sPairs[i].isNotMinimal!=NULL)
      {
        pisN = sPairs[i].isNotMinimal;
        tc = pGetComp(pisN);
        p = pp;
        while (p!=NULL)
        {
          if (pGetComp(p)==tc)
          {
            tq = pNew();
            for(j=pVariables; j>0; j--)
              tq->exp[j] = p->exp[j]-pisN->exp[j];
            tq->exp[0] = 0;
            pSetCoeff0(tq,nDiv(pGetCoeff(p),pGetCoeff(pisN)));
            pGetCoeff(tq) = nNeg(pGetCoeff(tq));
            q = syAdd(q,syStripOut(syMultT1(syzstr->res[index+1]->m[sPairs[i].syzind],tq),toStrip));
          }  
          pIter(p);
        }
        if (q!=NULL)
        {
          pp = syAdd(pp,q);
          q = NULL;
        }
      }
    }
  }
  return pp;
}

static void syKillEmptyEntres(resolvente res,int length)
{
  int i,j,jj,k,rj;
  intvec * changes;
  poly p;
  ideal ri;

  for (i=0;i<length;i++)
  {
    ri = res[i];
    if (ri!=NULL)
    {
      rj = IDELEMS(ri);
      changes = new intvec(rj+1,1,-1);
      while ((rj>0) && (ri->m[rj-1]==NULL)) rj--;
      j = k = 0;
      while (j+k<rj)
      {
        if (ri->m[j+k]!=NULL)
        {
          ri->m[j] = ri->m[j+k];
          (*changes)[j+k+1] = j+1; 
          j++;
        }
        else
        {
          k++;
        }
      }
      for (jj=j;jj<rj;jj++)
        ri->m[jj] = NULL;
      if (res[i+1]!=NULL)
      {
        ri = res[i+1];
        for (j=IDELEMS(ri)-1;j>=0;j--)
        {
          p = ri->m[j];
          while (p!=NULL)
          {
            pSetComp(p,(*changes)[pGetComp(p)]);
            pIter(p);
          }
        }
      }
    }
  }
}

static intvec * syToStrip(syStrategy syzstr, int index)
{
  intvec * result=NULL;

  if ((syzstr->resPairs[index-1]!=NULL) && (!idIs0(syzstr->res[index])))
  {
    result=new intvec(IDELEMS(syzstr->res[index])+1);
    for (int i=(*syzstr->Tl)[index-1]-1;i>=0;i--)
    {
      if (syzstr->resPairs[index-1][i].isNotMinimal!=NULL)
      {
        (*result)[syzstr->resPairs[index-1][i].syzind+1] = 1;
      }
    }
  }
  return result;
}

static resolvente syReadOutMinimalRes(syStrategy syzstr, 
           BOOLEAN computeStd=FALSE)
{
  intvec * Strip;
  resolvente tres=(resolvente)Alloc0((syzstr->length+1)*sizeof(ideal));
  if (computeStd)
  {
    tres[0] = syzstr->res[1];
    syzstr->res[1] = idInit(IDELEMS(tres[0]),tres[0]->rank);
    return tres;
  }
  int i,j,l,index,o1,ii,i1;
  poly p;
  ideal rs;
  SSet sPairs;
  int * ord,*b0,*b1;
  pSetmProc oldSetm=pSetm;
  pCompProc oldComp0=pComp0;
 
  if ((currRing->order[0]==ringorder_dp)
  &&  (currRing->order[1]==ringorder_C)
  &&  (currRing->order[2]==0))
  {
    ord=NULL;
  }
/*--- changes to a dpC-ring with special comp0------*/
  else
  {
    ord = (int*)Alloc0(3*sizeof(int));
    b0 = (int*)Alloc0(3*sizeof(int));
    b1 = (int*)Alloc0(3*sizeof(int));
    ord[0] = ringorder_dp;
    ord[1] = ringorder_C;
    b0[1] = 1;
    b1[0] = pVariables;
    pChangeRing(pVariables,1,ord,b0,b1,currRing->wvhdl);
  }
  pSetm =syzSetm;
  binomials = syzstr->binom;
  highdeg_1 = syzstr->highdeg_1;
  pComp0 = syzcomp2dpc;
  for (index=syzstr->length-1;index>0;index--)
  {
    if (syzstr->resPairs[index]!=NULL)
    {
      currcomponents = syzstr->truecomponents[index];
      sPairs = syzstr->resPairs[index];
      Strip = syToStrip(syzstr,index);
      tres[index+1] = idInit(IDELEMS(syzstr->res[index+1]),syzstr->res[index+1]->rank);
      i1 = (*syzstr->Tl)[index];
      for (i=0;i<i1;i++)
      {
        if ((sPairs[i].isNotMinimal==NULL) && (sPairs[i].lcm!=NULL))
        {
          o1 = sPairs[i].order;
          ii = i;
          while ((ii<i1) && (sPairs[ii].lcm!=NULL) && (sPairs[ii].order==o1))
            ii++;
          l = sPairs[i].syzind;
          tres[index+1]->m[l] =
            syMinimizeP(syzstr->res[index+1]->m[l],syzstr,ii,index,Strip);
        }
      }
      delete Strip;
      Strip = NULL;
    }
  }
  currcomponents = syzstr->truecomponents[0];
  tres[1] = idInit(IDELEMS(syzstr->res[1]),syzstr->res[1]->rank);
  sPairs = syzstr->resPairs[0];
  for (i=(*syzstr->Tl)[0]-1;i>=0;i--)
  {
    if (sPairs[i].syzind>=0)
    {
      tres[1]->m[sPairs[i].syzind] = pCopy(syzstr->res[1]->m[sPairs[i].syzind]);
    }
  }
/*--- changes to the original ring------------------*/
  if (ord!=NULL)
  {
    pChangeRing(pVariables,currRing->OrdSgn,currRing->order,
    currRing->block0,currRing->block1,currRing->wvhdl);
  }
  else
  {
    pSetm=oldSetm;
    pComp0=oldComp0;
  }
  tres = syReorder(tres,syzstr->length,syzstr,FALSE,syzstr->res);
  syKillEmptyEntres(tres,syzstr->length);
  idSkipZeroes(tres[0]);
  return tres;
}

syStrategy syMinimize(syStrategy syzstr)
{
  if (syzstr->minres==NULL)
  {
    if (syzstr->resPairs!=NULL)
    {
      syzstr->minres = syReadOutMinimalRes(syzstr);
    }
    else if (syzstr->fullres!=NULL)
    {
      syMinimizeResolvente(syzstr->fullres,syzstr->length,1);
      syzstr->minres = syzstr->fullres;
      syzstr->fullres = NULL;
    }
  }
  (syzstr->references)++;
  return syzstr;
}

static void sySetHighdeg()
{
  long long t=1, h_d=1;
  long long h_n=1+pVariables;
  while ((t=((t*h_n)/h_d))<INT_MAX)
  {
    h_d++;
    h_n++;
  }
  h_d--;
  highdeg = h_d;
  //Print("max deg=%d\n",highdeg);
}


/*2
* implementation of LaScala's algorithm
* assumes that the given module is homogeneous
* works with slanted degree, uses syChosePairs
*/
//syStrategy syLaScala1(ideal arg,int * length)
resolvente syLaScala1(ideal arg,int * length)
{
  BOOLEAN noPair=FALSE;
  int i,j,* ord,*b0,*b1,actdeg=32000,index=0,reg=-1;
  int startdeg,howmuch;
  poly p;
  ideal temp;
  SSet nextPairs;
  syStrategy syzstr=(syStrategy)Alloc0(sizeof(ssyStrategy));
  pSetmProc oldSetm=pSetm;
  pCompProc oldComp0=pComp0;

  //crit = 0;
  //zeroRed = 0;
  //simple = 0;
  //dsim = 0;
  euler = -1;
  redpol = pNew();
  //orderingdepth = new intvec(pVariables+1);
  if (*length<=0) *length = pVariables+2;
  syzstr->length = *length;
  if (idIs0(arg)) return NULL;
  if ((currRing->order[0]==ringorder_dp)
  &&  (currRing->order[1]==ringorder_C)
  &&  (currRing->order[2]==0))
  {
    ord=NULL;
  }  
/*--- changes to a dpC-ring with special comp0------*/
  else
  {
    ord = (int*)Alloc0(3*sizeof(int));
    b0 = (int*)Alloc0(3*sizeof(int));
    b1 = (int*)Alloc0(3*sizeof(int));
    ord[0] = ringorder_dp;
    ord[1] = ringorder_C;
    b0[1] = 1;
    b1[0] = pVariables;
    pChangeRing(pVariables,1,ord,b0,b1,currRing->wvhdl);
  }  
/*--- initializes the data structures---------------*/
  syzstr->Tl = new intvec(*length);
  temp = idInit(IDELEMS(arg),arg->rank);
  for (i=0;i<IDELEMS(arg);i++)
  {
    temp->m[i] = pOrdPoly(pCopy(arg->m[i]));
    if (temp->m[i]!=NULL) 
    {
      j = pTotaldegree(temp->m[i]);
      if (j<actdeg) actdeg = j;
    }
  }
  sySetHighdeg();
  binomials = (int*)Alloc(pVariables*(highdeg+1)*sizeof(int));
  syBinomSet();
  pSetm =syzSetm;
  for (i=0;i<IDELEMS(arg);i++)
  {
    p = temp->m[i];
    while (p!=NULL)
    {
      pSetm(p);
      pIter(p);
    }
  }
  pComp0 = syzcomp2dpc;
  syzstr->resPairs = syInitRes(temp,length,syzstr->Tl);
  syzstr->res = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  syzstr->orderedRes = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  syzstr->truecomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  syzstr->backcomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  syzstr->Howmuch = (int**)Alloc0((*length+1)*sizeof(int*));
  syzstr->Firstelem = (int**)Alloc0((*length+1)*sizeof(int*));
  int len0=idRankFreeModule(arg)+1;
  syzstr->truecomponents[0] = (int*)Alloc(len0*sizeof(int));
  syzstr->backcomponents[0] = (int*)Alloc(len0*sizeof(int));
  syzstr->Howmuch[0] = (int*)Alloc(len0*sizeof(int));
  syzstr->Firstelem[0] = (int*)Alloc(len0*sizeof(int));
//Print("sort %d has now size %d\n",0,len0);
  for (i=0;i<len0;i++)
    syzstr->truecomponents[0][i] = i;
  startdeg = actdeg;
  nextPairs = syChosePairs(syzstr,&index,&howmuch,&actdeg);
  //if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
/*--- computes the resolution ----------------------*/ 
  while (nextPairs!=NULL)
  {
    if (TEST_OPT_PROT) Print("%d",actdeg);
    if (TEST_OPT_PROT) Print("(m%d)",index);
    currcomponents = syzstr->truecomponents[max(index-1,0)];
    i = syInitSyzMod(syzstr,index);
    j = syInitSyzMod(syzstr,index+1);
    if (index>0)
    {
      syRedNextPairs(nextPairs,syzstr,howmuch,index);
      syCompactifyPairSet(syzstr->resPairs[index],(*syzstr->Tl)[index],0);
    }
    else
      syRedGenerOfCurrDeg(syzstr,actdeg,index+1);
/*--- creates new pairs -----------------------------*/      
    syCreateNewPairs(syzstr,index,i);
    if (index<(*length)-1) 
    {
      syCreateNewPairs(syzstr,index+1,j);
      //currcomponents = syzstr->truecomponents[index];
      //sySyzTail(syzstr,index+1,j);
      //currcomponents = syzstr->truecomponents[index-1];
    }
    index++;
    nextPairs = syChosePairs(syzstr,&index,&howmuch,&actdeg);
    //if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
  }
  //return syzstr;
//}
/*--- deletes temporary data structures-------------*/
  idDelete(&temp);
  for (i=0;i<*length;i++)
  {
    for (j=0;j<(*syzstr->Tl)[i];j++)
    {
      if ((syzstr->resPairs[i])[j].lcm!=NULL)
        pDelete(&((syzstr->resPairs[i])[j].lcm));
    }
    if (syzstr->orderedRes[i]!=NULL)
    {
      for (j=0;j<IDELEMS(syzstr->orderedRes[i]);j++)
        syzstr->orderedRes[i]->m[j] = NULL;
    }
    idDelete(&(syzstr->orderedRes[i]));
    if (syzstr->truecomponents[i]!=NULL)
    {
      Free((ADDRESS)syzstr->truecomponents[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
      syzstr->truecomponents[i]=NULL;
    }
    if (syzstr->backcomponents[i]!=NULL)
    {
      Free((ADDRESS)syzstr->backcomponents[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
      syzstr->backcomponents[i]=NULL;
    }
    if (syzstr->Howmuch[i]!=NULL)
    {
      Free((ADDRESS)syzstr->Howmuch[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
      syzstr->Howmuch[i]=NULL;
    }
    if (syzstr->Firstelem[i]!=NULL)
    {
      Free((ADDRESS)syzstr->Firstelem[i],(IDELEMS(syzstr->res[i])+1)*sizeof(int));
      syzstr->Firstelem[i]=NULL;
    }
    Free((ADDRESS)syzstr->resPairs[i],(*syzstr->Tl)[i]*sizeof(SObject));
  }
  Free((ADDRESS)syzstr->resPairs,*length*sizeof(SObject*));
  Free((ADDRESS)syzstr->orderedRes,(*length+1)*sizeof(ideal));
  Free((ADDRESS)syzstr->truecomponents,(*length+1)*sizeof(int*));
  Free((ADDRESS)syzstr->backcomponents,(*length+1)*sizeof(int*));
  Free((ADDRESS)syzstr->Howmuch,(*length+1)*sizeof(int*));
  Free((ADDRESS)syzstr->Firstelem,(*length+1)*sizeof(int*));
  syzstr->truecomponents = NULL;
  syzstr->backcomponents = NULL;
  syzstr->Howmuch = NULL;
  syzstr->Firstelem = NULL;
  if (BTEST1(6)) syStatistics(syzstr->res,(*length+1));
  (*length)++;
/*--- changes to the original ring------------------*/
  if (ord!=NULL)
  {
    pChangeRing(pVariables,currRing->OrdSgn,currRing->order,
    currRing->block0,currRing->block1,currRing->wvhdl);
  }
  else
  {
    pSetm=oldSetm;
    pComp0=oldComp0;
  }
  syReOrderResolventFB(syzstr->res,*length,2);
  for (i=0;i<*length-1;i++)
  {
    syzstr->res[i] = syzstr->res[i+1];
    if (syzstr->res[i]!=NULL)
    {
      polyset rsi = syzstr->res[i]->m;
      if (i>0)
      {
        for (j=IDELEMS(syzstr->res[i])-1;j>=0;j--)
        {
          rsi[j] = syOrdPolySchreyer(rsi[j]);
        }
      }
      else
      {
        if (ord!=NULL)
        {
          for (j=IDELEMS(syzstr->res[i])-1;j>=0;j--)
          {
            rsi[j] = pOrdPoly(rsi[j]);
          }
        }
        else
        {
          for (j=IDELEMS(syzstr->res[i])-1;j>=0;j--)
          {
            p = rsi[j];
            while (p!=NULL)
            {
              pSetm(p);
              pIter(p);
            }
          }
        }
      }
      idSkipZeroes(syzstr->res[i]);
    }
  }
  syzstr->res[*length-1] = NULL;
  if (ord!=NULL)
  {
    Free((ADDRESS)ord,3*sizeof(int));
    Free((ADDRESS)b0,3*sizeof(int));
    Free((ADDRESS)b1,3*sizeof(int));
  }  
  Free((ADDRESS)binomials,pVariables*(highdeg_1)*sizeof(int));
  pDelete1(&redpol);
  if (TEST_OPT_PROT) 
  {
    //Print("simple: %d\n",simple);
    //Print("dsim: %d\n",dsim);
    //Print("crit %d-times used \n",crit); 
    //Print("%d reductions to zero \n",zeroRed);
  }
  //delete orderingdepth;
  if (TEST_OPT_PROT) PrintLn();
  return syzstr->res;
}

/*2
* implementation of LaScala's algorithm
* assumes that the given module is homogeneous
* works with slanted degree, uses syChosePairs
*/
syStrategy syLaScala3(ideal arg,int * length)
{
  BOOLEAN noPair=FALSE;
  int i,j,* ord,*b0,*b1,actdeg=32000,index=0,reg=-1;
  int startdeg,howmuch;
  poly p;
  ideal temp;
  SSet nextPairs;
  syStrategy syzstr=(syStrategy)Alloc0(sizeof(ssyStrategy));
  if ((idIs0(arg)) || 
      ((idRankFreeModule(arg)>0) && (!idHomModule(arg,NULL,&(syzstr->cw)))))
  {
    syzstr->minres = (resolvente)Alloc0(sizeof(ideal));
    syzstr->length = 1;
    syzstr->minres[0] = idInit(1,arg->rank);
    return syzstr;
  }
  pSetmProc oldSetm=pSetm;
  pCompProc oldComp0=pComp0;

  //crit = 0;
  //zeroRed = 0;
  //simple = 0;
  //dsim = 0;
  euler = -1;
  redpol = pNew();
  //orderingdepth = new intvec(pVariables+1);
  syzstr->length = *length = pVariables+2;
  if (idIs0(arg)) return NULL;
  if ((currRing->order[0]==ringorder_dp)
  &&  (currRing->order[1]==ringorder_C)
  &&  (currRing->order[2]==0))
  {
    ord=NULL;
  }  
/*--- changes to a dpC-ring with special comp0------*/
  else
  {
    ord = (int*)Alloc0(3*sizeof(int));
    b0 = (int*)Alloc0(3*sizeof(int));
    b1 = (int*)Alloc0(3*sizeof(int));
    ord[0] = ringorder_dp;
    ord[1] = ringorder_C;
    b0[1] = 1;
    b1[0] = pVariables;
    pChangeRing(pVariables,1,ord,b0,b1,currRing->wvhdl);
  }  
/*--- initializes the data structures---------------*/
  syzstr->Tl = new intvec(*length);
  temp = idInit(IDELEMS(arg),arg->rank);
  for (i=0;i<IDELEMS(arg);i++)
  {
    temp->m[i] = pOrdPoly(pCopy(arg->m[i]));
    if (temp->m[i]!=NULL) 
    {
      j = pTotaldegree(temp->m[i]);
      if (j<actdeg) actdeg = j;
    }
  }
  sySetHighdeg();
  binomials = (int*)Alloc(pVariables*(highdeg+1)*sizeof(int));
  syBinomSet();
  pSetm =syzSetm;
  for (i=0;i<IDELEMS(arg);i++)
  {
    p = temp->m[i];
    while (p!=NULL)
    {
      pSetm(p);
      pIter(p);
    }
  }
  pComp0 = syzcomp2dpc;
  currcomponents = (int*)Alloc0((arg->rank+1)*sizeof(int));
  for (i=0;i<=arg->rank;i++)
    currcomponents[i] = i;
  syzstr->resPairs = syInitRes(temp,length,syzstr->Tl,syzstr->cw);
  Free((ADDRESS)currcomponents,(arg->rank+1)*sizeof(int));
  syzstr->res = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  syzstr->orderedRes = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  syzstr->truecomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  syzstr->backcomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  syzstr->Howmuch = (int**)Alloc0((*length+1)*sizeof(int*));
  syzstr->Firstelem = (int**)Alloc0((*length+1)*sizeof(int*));
  int len0=idRankFreeModule(arg)+1;
  //syzstr->truecomponents[0] = (int*)Alloc(len0*sizeof(int));
  //syzstr->backcomponents[0] = (int*)Alloc(len0*sizeof(int));
  //syzstr->Howmuch[0] = (int*)Alloc(len0*sizeof(int));
  //syzstr->Firstelem[0] = (int*)Alloc(len0*sizeof(int));
//Print("sort %d has now size %d\n",0,len0);
  startdeg = actdeg;
  nextPairs = syChosePairs(syzstr,&index,&howmuch,&actdeg);
  //if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
/*--- computes the resolution ----------------------*/ 
  while (nextPairs!=NULL)
  {
    if (TEST_OPT_PROT) Print("%d",actdeg);
    if (TEST_OPT_PROT) Print("(m%d)",index);
    if (index==0)
      i = syInitSyzMod(syzstr,index,len0);
    else
      i = syInitSyzMod(syzstr,index);
    currcomponents = syzstr->truecomponents[max(index-1,0)];
    j = syInitSyzMod(syzstr,index+1);
    if (index>0)
    {
      syRedNextPairs(nextPairs,syzstr,howmuch,index);
      syCompactifyPairSet(syzstr->resPairs[index],(*syzstr->Tl)[index],0);
    }
    else
      syRedGenerOfCurrDeg(syzstr,actdeg,index+1);
/*--- creates new pairs -----------------------------*/      
    syCreateNewPairs(syzstr,index,i);
    if (index<(*length)-1) 
    {
      syCreateNewPairs(syzstr,index+1,j);
      //currcomponents = syzstr->truecomponents[index];
      //sySyzTail(syzstr,index+1,j);
      //currcomponents = syzstr->truecomponents[index-1];
    }
    index++;
    nextPairs = syChosePairs(syzstr,&index,&howmuch,&actdeg);
    //if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
  }
  if (temp!=NULL) idDelete(&temp);
  if (ord!=NULL)
  {
    pChangeRing(pVariables,currRing->OrdSgn,currRing->order,
    currRing->block0,currRing->block1,currRing->wvhdl);
  }
  else
  {
    pSetm=oldSetm;
    pComp0=oldComp0;
  }
  if (ord!=NULL)
  {
    Free((ADDRESS)ord,3*sizeof(int));
    Free((ADDRESS)b0,3*sizeof(int));
    Free((ADDRESS)b1,3*sizeof(int));
  }  
  syzstr->binom = binomials;
  syzstr->highdeg_1 = highdeg_1;
  pDelete1(&redpol);
  if (TEST_OPT_PROT) PrintLn();
  return syzstr;
}
/*2
* second implementation of LaScala's algorithm
* assumes that the given module is homogeneous
* works deg by deg, uses syChosePairs1
*/
/*
*resolvente syLaScala2(ideal arg,int * length)
*{
*  BOOLEAN noPair=FALSE;
*  int i,j,* ord,*b0,*b1,actdeg=32000,index=1,reg=-1;
*  int startdeg,howmuch;
*  intvec * Tl;
*  poly p;
*  ideal temp;
*  resolvente res,orderedRes;
*  SSet nextPairs;
*  SRes resPairs;
*
*  //crit = 0;
*  //zeroRed = 0;
*  //simple = 0;
*  //dsim = 0;
*  redpol = pNew();
*  //orderingdepth = new intvec(pVariables+1);
*  if (*length<=0) *length = pVariables+2;
*  if (idIs0(arg)) return NULL;
//--- changes to a dpC-ring with special comp0------
*  ord = (int*)Alloc0(3*sizeof(int));
*  b0 = (int*)Alloc0(3*sizeof(int));
*  b1 = (int*)Alloc0(3*sizeof(int));
*  ord[0] = ringorder_dp;
*  ord[1] = ringorder_C;
*  b0[1] = 1;
*  b1[0] = pVariables;
*  pChangeRing(pVariables,1,ord,b0,b1,currRing->wvhdl);
//--- initializes the data structures---------------
*  Tl = new intvec(*length);
*  temp = idInit(IDELEMS(arg),arg->rank);
*  for (i=0;i<IDELEMS(arg);i++)
*  {
*    temp->m[i] = pOrdPoly(pCopy(arg->m[i]));
*    if (temp->m[i]!=NULL) 
*    {
*      j = pTotaldegree(temp->m[i]);
*      if (j<actdeg) actdeg = j;
*    }
*  }
*  {
*    highdeg=1;
*    long long t=1;
*    long long h_n=1+pVariables;
*    while ((t=(((long long)t*(long long)h_n)/(long long)highdeg))<INT_MAX)
*    {
*      highdeg++;
*      h_n++;
*    }
*    highdeg--;
*    Print("max deg=%d\n",highdeg);
*  }  
*  binomials = (int*)Alloc(pVariables*(highdeg+1)*sizeof(int));
*  syBinomSet();
*  pSetm =syzSetm;
*  for (i=0;i<IDELEMS(arg);i++)
*  {
*    p = temp->m[i];
*    while (p!=NULL)
*    {
*      pSetm(p);
*      pIter(p);
*    }
*  }
*  pComp0 = syzcomp2dpc;
*  resPairs = syInitRes(temp,length,Tl);
*  res = (resolvente)Alloc0((*length+1)*sizeof(ideal));
*  orderedRes = (resolvente)Alloc0((*length+1)*sizeof(ideal));
*  truecomponents = (int**)Alloc0((*length+1)*sizeof(int*));
*  backcomponents = (int**)Alloc0((*length+1)*sizeof(int*));
*  Howmuch = (int**)Alloc0((*length+1)*sizeof(int*));
*  Firstelem = (int**)Alloc0((*length+1)*sizeof(int*));
*  int len0=idRankFreeModule(arg)+1;
*  truecomponents[0] = (int*)Alloc(len0*sizeof(int));
*  backcomponents[0] = (int*)Alloc(len0*sizeof(int));
*  Howmuch[0] = (int*)Alloc(len0*sizeof(int));
*  Firstelem[0] = (int*)Alloc(len0*sizeof(int));
//Print("sort %d has now size %d\n",0,len0);
*  for (i=0;i<len0;i++)
*    truecomponents[0][i] = i;
*  startdeg = actdeg;
*  nextPairs = syChosePairs1(resPairs,Tl,&index,&howmuch,*length,&actdeg);
*  if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
//--- computes the resolution ---------------------- 
*  while (nextPairs!=NULL)
*  {
*    if (TEST_OPT_PROT) Print("%d",actdeg);
*    if (TEST_OPT_PROT) Print("(m%d)",index);
*    currcomponents = truecomponents[max(index-1,0)];
*    i = syInitSyzMod(res,orderedRes,index);
*    j = syInitSyzMod(res,orderedRes,index+1);
*    if (index>0)
*    {
*      syRedNextPairs(nextPairs,res,orderedRes,howmuch,index);
*      syCompactifyPairSet(resPairs[index],(*Tl)[index],0);
*    }
*    else
*      syRedGenerOfCurrDeg(resPairs,res,orderedRes,actdeg,index+1,Tl);
//--- creates new pairs -----------------------------      
*    syCreateNewPairs(resPairs,Tl,res,index,i);
*    if (index<(*length)-1) 
*    {
*      syCreateNewPairs(resPairs,Tl,res,index+1,j);
*      currcomponents = truecomponents[index];
*      sySyzTail(resPairs,Tl,orderedRes,res,index+1,j);
*      currcomponents = truecomponents[index-1];
*    }
*    index--;
*    nextPairs = syChosePairs1(resPairs,Tl,&index,&howmuch,*length,&actdeg);
*    if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
*  }
//--- deletes temporary data structures-------------
*  idDelete(&temp);
*  for (i=0;i<*length;i++)
*  {
*    for (j=0;j<(*Tl)[i];j++)
*    {
*      if ((resPairs[i])[j].lcm!=NULL)
*        pDelete(&(resPairs[i])[j].lcm);
*    }
*    if (orderedRes[i]!=NULL)
*    {
*      for (j=0;j<IDELEMS(orderedRes[i]);j++)
*        orderedRes[i]->m[j] = NULL;
*    }
*    idDelete(&orderedRes[i]);
*    if (truecomponents[i]!=NULL)
*    {
*      Free((ADDRESS)truecomponents[i],(IDELEMS(res[i])+1)*sizeof(int));
*      truecomponents[i]=NULL;
*    }
*    if (backcomponents[i]!=NULL)
*    {
*      Free((ADDRESS)backcomponents[i],(IDELEMS(res[i])+1)*sizeof(int));
*      backcomponents[i]=NULL;
*    }
*    if (Howmuch[i]!=NULL)
*    {
*      Free((ADDRESS)Howmuch[i],(IDELEMS(res[i])+1)*sizeof(int));
*      Howmuch[i]=NULL;
*    }
*    if (Firstelem[i]!=NULL)
*    {
*      Free((ADDRESS)Firstelem[i],(IDELEMS(res[i])+1)*sizeof(int));
*      Firstelem[i]=NULL;
*    }
*    Free((ADDRESS)resPairs[i],(*Tl)[i]*sizeof(SObject));
*  }
*  Free((ADDRESS)resPairs,*length*sizeof(SObject*));
*  Free((ADDRESS)orderedRes,(*length+1)*sizeof(ideal));
*  Free((ADDRESS)truecomponents,(*length+1)*sizeof(int*));
*  Free((ADDRESS)backcomponents,(*length+1)*sizeof(int*));
*  Free((ADDRESS)Howmuch,(*length+1)*sizeof(int*));
*  Free((ADDRESS)Firstelem,(*length+1)*sizeof(int*));
*  truecomponents = NULL;
*  backcomponents = NULL;
*  Howmuch = NULL;
*  Firstelem = NULL;
*  if (BTEST1(6)) syStatistics(res,(*length+1));
*  (*length)++;
//--- changes to the original ring------------------
*  pChangeRing(pVariables,currRing->OrdSgn,currRing->order,
*    currRing->block0,currRing->block1,currRing->wvhdl);
*  syReOrderResolventFB(res,*length,2);
*  for (i=0;i<*length-1;i++)
*  {
*    res[i] = res[i+1];
*    if (res[i]!=NULL)
*    {
*      if (i>0)
*      {
*        for (j=0;j<IDELEMS(res[i]);j++)
*        {
*          res[i]->m[j] = syOrdPolySchreyer(res[i]->m[j]);
*        }
*        idSkipZeroes(res[i]);
*      }
*      else
*      {
*        for (j=0;j<IDELEMS(res[i]);j++)
*        {
*          p = res[i]->m[j];
*          while (p!=NULL)
*          {
*            pSetm(p);
*            pIter(p);
*          }
*        }
*      }
*    }
*  }
*  res[*length-1] = NULL;
*  Free((ADDRESS)ord,3*sizeof(int));
*  Free((ADDRESS)b0,3*sizeof(int));
*  Free((ADDRESS)b1,3*sizeof(int));
*  Free((ADDRESS)binomials,pVariables*(highdeg_1)*sizeof(int));
*  pDelete1(&redpol);
*  if (TEST_OPT_PROT) 
*  {
*    //Print("simple: %d\n",simple);
*    //Print("dsim: %d\n",dsim);
*    //Print("crit %d-times used \n",crit); 
*    //Print("%d reductions to zero \n",zeroRed);
*  }
*  //delete orderingdepth;
*  return res;
*}
*/
