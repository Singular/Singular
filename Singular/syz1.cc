/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz1.cc,v 1.2 1997-04-04 12:32:53 Singular Exp $ */
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
#include "ideals.h"
#include "intvec.h"
#include "ring.h"
#include "syz.h"

/*--------------static variables------------------------*/
/*---contains the real components wrt. cdp--------------*/
static int ** truecomponents=NULL;
static int ** backcomponents=NULL;
static int ** Howmuch=NULL;
static int ** Firstelem=NULL;
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
  (*so).order = 0;
  (*so).isNotMinimal = 0;
}

/*3
* puts all entres of a pair to another
*/
static void syCopyPair(SObject * argso, SObject * imso)
{
  (*imso).p = (*argso).p;
  (*imso).p1 = (*argso).p1;
  (*imso).p2 = (*argso).p2;
  (*imso).lcm = (*argso).lcm;
  (*imso).syz = (*argso).syz;
  (*imso).ind1 = (*argso).ind1;
  (*imso).ind2 = (*argso).ind2;
  (*imso).order = (*argso).order;
  (*imso).isNotMinimal = (*argso).isNotMinimal;
  (*argso).p = NULL;
  (*argso).p1 = NULL;
  (*argso).p2 = NULL;
  (*argso).lcm = NULL;
  (*argso).syz = NULL;
  (*argso).ind1 = 0;
  (*argso).ind2 = 0;
  (*argso).isNotMinimal = 0;
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

poly syRedtail (poly p, ideal redWith, int index)
{
  poly h, hn;
  int j,pos;

  h = p;
  hn = pNext(h);
  while(hn != NULL)
  {
    j = Firstelem[index-1][pGetComp(hn)]-1;
    if (j>=0)
    {
      pos = j+Howmuch[index-1][pGetComp(hn)];
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
          j = Firstelem[index-1][pGetComp(hn)]-1;
          pos = j+Howmuch[index-1][pGetComp(hn)];
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
* initialize the resolution and puts in the argument as
* zeroth entre, length must be > 0
* assumes that the basering is degree-compatible
*/
static SRes syInitRes(ideal arg,int * length, intvec * Tl)
{
  if (idIs0(arg)) return NULL;
  SRes resPairs = (SRes)Alloc0(*length*sizeof(SSet));
  resPairs[0] = (SSet)Alloc0(IDELEMS(arg)*sizeof(SObject));
  intvec * iv = idSort(arg);
  int i;

  for (i=0;i<IDELEMS(arg);i++)
  {
    (resPairs[0])[i].syz = pCopy(arg->m[(*iv)[i]-1]);
  } 
  delete iv;
  (*Tl)[0] = IDELEMS(arg);
  return resPairs;
}

/*3
* determines the place of a polynomial in the right ordered resolution
* set the vectors of truecomponents
*/
static void syOrder(poly p,resolvente res,resolvente orderedRes,int index,
                    int realcomp)
{
  int i=IDELEMS(res[index-1])+1,j=0,k,tc,orc,ie=realcomp-1;
  int *trind1=truecomponents[index-1],*trind=truecomponents[index];
  int *bc=backcomponents[index],*F1=Firstelem[index-1];
  int *H1=Howmuch[index-1];
  poly pp;
  polyset or=orderedRes[index]->m,or1=orderedRes[index-1]->m;

  if (p==NULL) return;
  if (realcomp==0) realcomp=1;
 
  if (index>1)
    tc = trind1[pGetComp(p)]-1;
  else
    tc = pGetComp(p);
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
    Werror("orderedRes to small");
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
  for (k=0;k<IDELEMS(res[index]);k++)
  {
    if (trind[k]>j)
      trind[k] += 1;
  }
  for (k=IDELEMS(res[index])-1;k>realcomp;k--)
    trind[k] = trind[k-1];
  trind[realcomp] = j+1;
}

/*3
* reduces all pairs of degree deg in the module index
* put the reduced generators to the resolvente which contains
* the truncated std 
*/
static void syRedPairsOfCurrDeg(SRes resPairs, resolvente res, resolvente orderedRes,
               int deg, int index, intvec * Tl, intvec ** minGen)
{
  int i=0,j,k=IDELEMS(res[index]),kk;
  number coefgcd,n;
  poly p=NULL;

  if (resPairs[index]==NULL) return;
  while ((k>0) && (res[index]->m[k-1]==NULL)) k--;
  while ((i<(*Tl)[index]) && ((resPairs[index])[i].lcm!=NULL)
          && (pGetOrder((resPairs[index])[i].lcm)<deg)) i++;
  if ((i>=(*Tl)[index]) || ((resPairs[index])[i].lcm==NULL)
          || (pGetOrder((resPairs[index])[i].lcm)>deg)) return;
  while ((i<(*Tl)[index]) && ((resPairs[index])[i].lcm!=NULL)
          && (pGetOrder((resPairs[index])[i].lcm)==deg)) 
  {
    (resPairs[index])[i].p = 
      spSpolyCreate((resPairs[index])[i].p2, (resPairs[index])[i].p1,NULL);
    coefgcd = 
      nGcd(pGetCoeff((resPairs[index])[i].p1),pGetCoeff((resPairs[index])[i].p1));
    if ((*minGen[index])[(resPairs[index])[i].ind2]==0)
    {
      (resPairs[index])[i].syz = pHead((resPairs[index])[i].lcm);
      p = (resPairs[index])[i].syz;
      pSetCoeff(p,nDiv(pGetCoeff((resPairs[index])[i].p1),coefgcd));
      pGetCoeff(p) = nNeg(pGetCoeff(p));
      pSetComp(p,(resPairs[index])[i].ind2+1);
    }
    if ((*minGen[index])[(resPairs[index])[i].ind1]==0)
    {
      if ((resPairs[index])[i].syz==NULL)
      {
        (resPairs[index])[i].syz = pHead((resPairs[index])[i].lcm);
        p = (resPairs[index])[i].syz;
      }
      else
      {
        pNext(p) = pHead((resPairs[index])[i].lcm);
        pIter(p);
      }
      pSetComp(p,(resPairs[index])[i].ind1+1);
      pSetCoeff(p,nDiv(pGetCoeff((resPairs[index])[i].p2),coefgcd));
    }
    nDelete(&coefgcd);
    j = k-1;
    while ((j>=0) && (res[index]->m[j]!=NULL) 
           && ((resPairs[index])[i].p!=NULL))
    {
      if (syDivisibleBy(res[index]->m[j],(resPairs[index])[i].p))
      {
        if ((*minGen[index])[j]==0)
        {
          if ((resPairs[index])[i].syz==NULL)
          {
            (resPairs[index])[i].syz = pHead((resPairs[index])[i].p);
            p = (resPairs[index])[i].syz;
          }
          else
          {
            pNext(p) = pHead((resPairs[index])[i].p);
            pIter(p);
          }
          pSetComp(p,j+1);
          pGetCoeff(p) = nNeg(pGetCoeff(p));
        }
        (resPairs[index])[i].p = 
          spSpolyRed(res[index]->m[j],(resPairs[index])[i].p,NULL);
        j = k-1;
      }
      else
      {
        j--;
      }
    }
    if ((resPairs[index])[i].p != NULL)
    {
      if (TEST_OPT_PROT) Print("g");
      if (k==IDELEMS(res[index]))
      {
        pEnlargeSet(&(res[index]->m),IDELEMS(res[index]),16);
        truecomponents[index]=(int*)ReAlloc((ADDRESS)truecomponents[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
        backcomponents[index]=(int*)ReAlloc((ADDRESS)backcomponents[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
        Howmuch[index]=(int*)ReAlloc((ADDRESS)Howmuch[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
        Firstelem[index]=(int*)ReAlloc((ADDRESS)Firstelem[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(res[index])+1;mk<IDELEMS(res[index])+17;mk++)
        {
          Howmuch[index][mk] = 0;
          Firstelem[index][mk] = 0;
        }
//Print("sort %d has now size %d\n",index,IDELEMS(res[index])+17);
        IDELEMS(res[index]) += 16;
        pEnlargeSet(&(orderedRes[index]->m),IDELEMS(orderedRes[index]),16);
        IDELEMS(orderedRes[index]) += 16;
        intvec * temp = new intvec(minGen[index]->length()+16);
        for (kk=minGen[index]->length()-1;kk>=0;kk--)
          (*temp)[kk] = (*minGen[index])[kk];
        delete minGen[index];
        minGen[index] = temp;
      }
      if ((resPairs[index])[i].syz==NULL)
      {
        (resPairs[index])[i].syz = pHead((resPairs[index])[i].p);
        p = (resPairs[index])[i].syz;
      }
      else
      {
        pNext(p) = pHead(resPairs[index][i].p);
        pIter(p);
      }
      if (p!=NULL)
      {
        k++;
        pSetComp(p,k);
        pGetCoeff(p) = nNeg(pGetCoeff(p));
      }
      if (resPairs[index][i].p!=NULL)
      {
        res[index]->m[k] = resPairs[index][i].p;
        pNorm(res[index]->m[k]);
        syOrder(res[index]->m[k-1],res,orderedRes,index,k);
      }
      resPairs[index][i].isNotMinimal = 1;
      resPairs[index][i].p =NULL;
    }
    else
      if (TEST_OPT_PROT) Print(".");
    p = NULL;
    i++;
  }
} 

/*3
* reduces all pairs of degree deg in the module index
* put the reduced generators to the resolvente which contains
* the truncated std 
*/
static void syRedNextPairs(SSet nextPairs, resolvente res, resolvente orderedRes,
               int howmuch, int index)
{
  int i=howmuch-1,j,k=IDELEMS(res[index]),ks=IDELEMS(res[index+1]),kk,l,ll;
  number coefgcd,n;
  polyset redset=orderedRes[index]->m;
  poly p=NULL,q;
  BOOLEAN isDivisible;

  if ((nextPairs==NULL) || (howmuch==0)) return;
  while ((k>0) && (res[index]->m[k-1]==NULL)) k--;
  while ((ks>0) && (res[index+1]->m[ks-1]==NULL)) ks--;
  while (i>=0)
  {
    isDivisible = FALSE;
    if (res[index+1]!=NULL)
    {
      l = Firstelem[index][pGetComp(nextPairs[i].lcm)]-1;
      if (l>=0)
      {
        ll = l+Howmuch[index][pGetComp(nextPairs[i].lcm)];
        while ((l<ll) && (!isDivisible))
        {
          if (res[index+1]->m[l]!=NULL)
          {
            isDivisible = isDivisible || 
              syDivisibleBy1(orderedRes[index+1]->m[l],nextPairs[i].lcm);
          }
          l++;
        }
      }
    }
    if (!isDivisible)
    {
      nextPairs[i].p = 
        sySPoly(nextPairs[i].p1, nextPairs[i].p2,nextPairs[i].lcm);
      coefgcd = 
        nGcd(pGetCoeff(nextPairs[i].p1),pGetCoeff(nextPairs[i].p1));
      nextPairs[i].syz = pHead(nextPairs[i].lcm);
      pSetm(nextPairs[i].syz);
      p = nextPairs[i].syz;
      pSetCoeff(p,nDiv(pGetCoeff(nextPairs[i].p1),coefgcd));
      pGetCoeff(p) = nNeg(pGetCoeff(p));
      pSetComp(p,nextPairs[i].ind2+1);
      pNext(p) = pHead(nextPairs[i].lcm);
      pIter(p);
      pSetm(p);
      pSetComp(p,nextPairs[i].ind1+1);
      pSetCoeff(p,nDiv(pGetCoeff(nextPairs[i].p2),coefgcd));
      nDelete(&coefgcd);
      if (nextPairs[i].p != NULL)
      {
        q = nextPairs[i].p;
        j = Firstelem[index-1][pGetComp(q)]-1;
        int pos = j+Howmuch[index-1][pGetComp(q)];
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
            pSetComp(p,backcomponents[index][j]+1);
            pGetCoeff(p) = nNeg(pGetCoeff(p));
            q = sySPolyRed(q,redset[j]);
            if (q==NULL) break;
            j = Firstelem[index-1][pGetComp(q)]-1;
            pos = j+Howmuch[index-1][pGetComp(q)];
          }
          else
          {
            j++;
            if (j==pos) break;
          }
        }
        nextPairs[i].p = q;
      }
      if (nextPairs[i].p != NULL)
      {
        if (TEST_OPT_PROT) Print("g");
        if (k==IDELEMS(res[index]))
        {
          pEnlargeSet(&(res[index]->m),IDELEMS(res[index]),16);
          truecomponents[index]=(int*)ReAlloc((ADDRESS)truecomponents[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
          backcomponents[index]=(int*)ReAlloc((ADDRESS)backcomponents[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
          Howmuch[index]=(int*)ReAlloc((ADDRESS)Howmuch[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
          Firstelem[index]=(int*)ReAlloc((ADDRESS)Firstelem[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(res[index])+1;mk<IDELEMS(res[index])+17;mk++)
        {
          Howmuch[index][mk] = 0;
          Firstelem[index][mk] = 0;
        }
//Print("sort %d has now size %d\n",index,IDELEMS(res[index])+17);
          IDELEMS(res[index]) += 16;
          pEnlargeSet(&(orderedRes[index]->m),IDELEMS(orderedRes[index]),16);
          IDELEMS(orderedRes[index]) += 16;
          redset=orderedRes[index]->m;
        }
        pNext(p) = pHead(nextPairs[i].p);
        pIter(p);
        if (p!=NULL)
        {
          k++;
          pSetComp(p,k);
          pGetCoeff(p) = nNeg(pGetCoeff(p));
        }
        if (nextPairs[i].p!=NULL)
        {
          res[index]->m[k-1] = nextPairs[i].p;
          pNorm(res[index]->m[k-1]);
          syOrder(res[index]->m[k-1],res,orderedRes,index,k);
        }
        nextPairs[i].isNotMinimal = 1;
        nextPairs[i].p =NULL;
      }
      else
      {
        if (TEST_OPT_PROT) Print(".");
        if (index % 2==0)
          euler++;
        else 
          euler--;
      }
      if (ks==IDELEMS(res[index+1]))
      {
        pEnlargeSet(&(res[index+1]->m),IDELEMS(res[index+1]),16);
        truecomponents[index+1]=(int*)ReAlloc((ADDRESS)truecomponents[index+1],
                                       (IDELEMS(res[index+1])+1)*sizeof(int),
                                       (IDELEMS(res[index+1])+17)*sizeof(int));
        backcomponents[index+1]=(int*)ReAlloc((ADDRESS)backcomponents[index+1],
                                       (IDELEMS(res[index+1])+1)*sizeof(int),
                                       (IDELEMS(res[index+1])+17)*sizeof(int));
        Howmuch[index+1]=(int*)ReAlloc((ADDRESS)Howmuch[index+1],
                                       (IDELEMS(res[index+1])+1)*sizeof(int),
                                       (IDELEMS(res[index+1])+17)*sizeof(int));
        Firstelem[index+1]=(int*)ReAlloc((ADDRESS)Firstelem[index+1],
                                       (IDELEMS(res[index+1])+1)*sizeof(int),
                                       (IDELEMS(res[index+1])+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(res[index+1])+1;mk<IDELEMS(res[index+1])+17;mk++)
        {
          Howmuch[index+1][mk] = 0;
          Firstelem[index+1][mk] = 0;
        }
//Print("sort %d has now size %d\n",index+1,IDELEMS(res[index+1])+17);
        IDELEMS(res[index+1]) += 16;
        pEnlargeSet(&(orderedRes[index+1]->m),IDELEMS(orderedRes[index+1]),16);
        IDELEMS(orderedRes[index+1]) += 16;
      }
      currcomponents = truecomponents[index];
      res[index+1]->m[ks] = syRedtail(nextPairs[i].syz,orderedRes[index+1],index+1);
      currcomponents = truecomponents[index-1];
      //res[index+1]->m[ks] = nextPairs[i].syz;
      pNorm(res[index+1]->m[ks]);
      nextPairs[i].syz =NULL;
      syOrder(res[index+1]->m[ks],res,orderedRes,index+1,ks+1);
      ks++;
      p = NULL;
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
static void syRedGenerOfCurrDeg(SRes resPairs,resolvente res,resolvente orderedRes,
         int deg, int index,intvec * Tl)
{
  int i=0,j,k=IDELEMS(res[index]),kk;

  while ((k>0) && (res[index]->m[k-1]==NULL)) k--;
  while ((i<(*Tl)[index-1]) && (((resPairs[index-1])[i].syz==NULL) || 
          (pTotaldegree((resPairs[index-1])[i].syz)<deg)))
    i++;
  if ((i>=(*Tl)[index-1]) || (pTotaldegree((resPairs[index-1])[i].syz)>deg)) return;
  while ((i<(*Tl)[index-1]) && (((resPairs[index-1])[i].syz==NULL) ||
         (pTotaldegree((resPairs[index-1])[i].syz)==deg))) 
  {
    if ((resPairs[index-1])[i].syz!=NULL)
    {
      j = k-1;
      while ((j>=0) && (res[index]->m[j]!=NULL) && 
             ((resPairs[index-1])[i].syz!=NULL))
      {
        if (syDivisibleBy(res[index]->m[j],(resPairs[index-1])[i].syz))
        {
          //Print("r");
          (resPairs[index-1])[i].syz = 
            spSpolyRed(res[index]->m[j],(resPairs[index-1])[i].syz,NULL);
          j = k-1;
        }
        else
        {
          j--;
        }
      }
      if ((resPairs[index-1])[i].syz != NULL)
      {
        if (k==IDELEMS(res[index]))
        {
          pEnlargeSet(&(res[index]->m),IDELEMS(res[index]),16);
          truecomponents[index]=(int*)ReAlloc((ADDRESS)truecomponents[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
          backcomponents[index]=(int*)ReAlloc((ADDRESS)backcomponents[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
          Howmuch[index]=(int*)ReAlloc((ADDRESS)Howmuch[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
          Firstelem[index]=(int*)ReAlloc((ADDRESS)Firstelem[index],
                                       (IDELEMS(res[index])+1)*sizeof(int),
                                       (IDELEMS(res[index])+17)*sizeof(int));
        int mk;
        for (mk=IDELEMS(res[index])+1;mk<IDELEMS(res[index])+17;mk++)
        {
          Howmuch[index][mk] = 0;
          Firstelem[index][mk] = 0;
        }
//Print("sort %d has now size %d\n",index,IDELEMS(res[index])+17);
          IDELEMS(res[index]) += 16;
          pEnlargeSet(&(orderedRes[index]->m),IDELEMS(orderedRes[index]),16);
          IDELEMS(orderedRes[index]) += 16;
        }
        if (BTEST1(6))
        {
          if ((resPairs[index-1])[i].isNotMinimal==0)
          {
            PrintLn();
            Print("minimal generator: ");pWrite((resPairs[index-1])[i].syz);
            Print("comes from: ");pWrite((resPairs[index-1])[i].p1);
            Print("and: ");pWrite((resPairs[index-1])[i].p2);
          }
        }
        //res[index]->m[k] = (resPairs[index-1])[i].syz;
        res[index]->m[k] = syRedtail((resPairs[index-1])[i].syz,orderedRes[index],index);
        pNorm(res[index]->m[k]);
  //      (resPairs[index-1])[i].syz = NULL;
        k++;
        syOrder(res[index]->m[k-1],res,orderedRes,index,k);
        euler++;
      }
      //else
      //{
        //zeroRed++;
      //}
    }
    i++;
  }
}

/*3
* puts a pair into the right place in resPairs
*/
static void syEnterPair(SSet sPairs, SObject * so, int * sPlength,int index)
{
  int ll,k,no=pGetOrder((*so).lcm),sP=*sPlength,i;
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
* applies GM-criteria to triples (i,j,k) where j is fixed
*/
static void syCrit(SRes resPairs, intvec * Tl, int index, int j)
{
  int i,k,l,ll,ini=max(1,(*Tl)[index]);
  ideal temp=idInit(ini,1);
   
  for (l=(*Tl)[index]-1;l>=0;l--)
  {
    if ((resPairs[index][l].lcm!=NULL) && 
        (pGetComp(resPairs[index][l].lcm)==j))
    {
      temp->m[l] = pDivide(resPairs[index][l].p1,resPairs[index][l].lcm);
      pSetComp(temp->m[l],resPairs[index][l].ind1);
    }
  }
  idSkipZeroes(temp);
  k = IDELEMS(temp)-1;
  while ((k>=0) && (temp->m[k]==NULL)) k--;
  while (k>0)
  {
    for (i=0;i<k;i++)
    {
      l = pVariables;
      while ((l>0) && (pGetExp(temp->m[i],l)*pGetExp(temp->m[k],l)==0))
        l--;
      if (l==0)
      {
        ll = 0;
        while ((ll<(*Tl)[index]) && 
          (((pGetComp(temp->m[i])!=resPairs[index][ll].ind1) ||
          (pGetComp(temp->m[k])!=resPairs[index][ll].ind2)) &&
          ((pGetComp(temp->m[k])!=resPairs[index][ll].ind1) ||
          (pGetComp(temp->m[i])!=resPairs[index][ll].ind2))))
          ll++;
        if ((ll<(*Tl)[index]) && (resPairs[index][ll].lcm!=NULL))
        {
          syDeletePair(&resPairs[index][ll]);
          //crit++;
        }
      }
    }
    k--;
  }
  syCompactifyPairSet(resPairs[index],(*Tl)[index],0);
  idDelete(&temp);
}

/*3
* computes pairs from the new elements (beginning with the element newEl)
* in the module index
*/
static void syCreateNewPairs(SRes resPairs, intvec * Tl, resolvente res,
                int index, int newEl)
{
  SSet temp;
  SObject tso;
  int i,ii,j,k=IDELEMS(res[index]),l=(*Tl)[index],ll;
  int qc,first,pos,jj,j1;
  poly p,q;
  polyset rs=res[index]->m,nPm;

  while ((k>0) && (res[index]->m[k-1]==NULL)) k--;
  if (newEl>=k) return;
  ideal nP=idInit(k,res[index]->rank);
  nPm=nP->m;
  while ((l>0) && ((resPairs[index])[l-1].p1==NULL)) l--;
//Print("new pairs in module %d\n",index);
  for (j=newEl;j<k;j++)
  {
    q = rs[j];
    qc = pGetComp(q);
    first = Firstelem[index-1][pGetComp(q)]-1;
    pos = first+Howmuch[index-1][pGetComp(q)];
    for (i=first;i<pos;i++)
    {
      jj = backcomponents[index][i];
      if (jj>=j) break;
      p = pOne();
      pLcm(rs[jj],q,p);
      pSetComp(p,j+1);
      ii = first;
      loop 
      {
        j1 = backcomponents[index][ii];
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
    for (i=0;i<j;i++)
    {
      if (nPm[i]!=NULL)
      {
        if (l>=(*Tl)[index])
        {
          temp = (SSet)Alloc0(((*Tl)[index]+16)*sizeof(SObject));
          for (ll=0;ll<(*Tl)[index];ll++)
          {
            temp[ll].p = (resPairs[index])[ll].p;
            temp[ll].p1 = (resPairs[index])[ll].p1;
            temp[ll].p2 = (resPairs[index])[ll].p2;
            temp[ll].syz = (resPairs[index])[ll].syz;
            temp[ll].lcm = (resPairs[index])[ll].lcm;
            temp[ll].ind1 = (resPairs[index])[ll].ind1;
            temp[ll].ind2 = (resPairs[index])[ll].ind2;
            temp[ll].order = (resPairs[index])[ll].order;
            temp[ll].isNotMinimal = (resPairs[index])[ll].isNotMinimal;
          }
          Free((ADDRESS)resPairs[index],(*Tl)[index]*sizeof(SObject));
          (*Tl)[index] += 16;
          resPairs[index] = temp;
        }
        tso.lcm = p = nPm[i];
        nPm[i] = NULL;
        tso.order = pGetOrder(p) = pTotaldegree(p);
        tso.p1 = rs[i];
        tso.p2 = q;
        tso.ind1 = i;
        tso.ind2 = j;
        tso.isNotMinimal = 0;
        tso.p = NULL;
        tso.syz = NULL;
        syEnterPair(resPairs[index],&tso,&l,index);
      }
    }
  }
  idDelete(&nP);
}

static void sySyzTail(SRes resPairs, intvec * Tl, resolvente orderedRes,
                resolvente res, int index, int newEl)
{
  int j,ll,k=IDELEMS(res[index]);
  while ((k>0) && (res[index]->m[k-1]==NULL)) k--;
  for (j=newEl;j<k;j++)
  {
    ll = 0;
    while ((ll<(*Tl)[index]) && (resPairs[index][ll].p1!=NULL) &&
           (resPairs[index][ll].ind1!=j) && (resPairs[index][ll].ind2!=j))
      ll++;
    if ((ll<(*Tl)[index]) && (resPairs[index][ll].p1!=NULL))
      res[index]->m[j] = syRedtail(res[index]->m[j],orderedRes[index],index);
  }
}

/*3
* looks through the pair set and the given module for
* remaining pairs or generators to consider
*/
static BOOLEAN syCheckPairs(SRes resPairs,intvec * Tl,
                   int length,int * actdeg)
{
  int newdeg=*actdeg,i,index=0,t;
  
  while ((index<length) && (resPairs[index]!=NULL))
  {
    i = 0;
    while ((i<(*Tl)[index]))
    {
      t = *actdeg;
      if ((resPairs[index])[i].lcm!=NULL)
      {
        if (pGetOrder((resPairs[index])[i].lcm) > *actdeg)
          t = pGetOrder((resPairs[index])[i].lcm);
      }
      else if ((resPairs[index])[i].syz!=NULL)
      {
        if (pGetOrder((resPairs[index])[i].syz) > *actdeg)
          t = pGetOrder((resPairs[index])[i].syz);
      }
      if ((t>*actdeg) && ((newdeg==*actdeg) || (t<newdeg)))
      {
        newdeg = t;
        break;
      }
      i++;
    } 
    index++;
  }
  if (newdeg>*actdeg)
  {
    *actdeg = newdeg;
    return TRUE;
  }
  else return FALSE;
}

/*3
* looks through the pair set and the given module for
* remaining pairs or generators to consider
* returns a pointer to the first pair and the number of them in the given module
* works with slanted degree (i.e. deg=realdeg-index)
*/
static SSet syChosePairs(SRes resPairs,intvec * Tl, int *index, int *howmuch,
                   int length,int * actdeg)
{
  int newdeg=*actdeg,newindex=-1,i,t,sldeg;
  SSet result;
  
  while (*index<length)
  {
    if (resPairs[*index]!=NULL)
    {
      sldeg = (*actdeg)+*index;
      i = 0;
      if (*index!=0)
      {
        while ((i<(*Tl)[*index]))
        {
          if ((resPairs[*index])[i].lcm!=NULL)
          {
            if (pGetOrder((resPairs[*index])[i].lcm) == sldeg)
            {
              result = &(resPairs[*index])[i];
              *howmuch =1;
              i++;
              while ((i<(*Tl)[*index]) && ((resPairs[*index])[i].lcm!=NULL)
                      && (pGetOrder((resPairs[*index])[i].lcm) == sldeg))
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
        while ((i<(*Tl)[*index]))
        {
          if ((resPairs[*index])[i].syz!=NULL)
          {
            if (pTotaldegree((resPairs[*index])[i].syz) == sldeg)
            {
              result = &(resPairs[*index])[i];
              (*howmuch) =1;
              i++;
              while ((i<(*Tl)[*index]) && ((resPairs[*index])[i].syz!=NULL)
                      && (pTotaldegree((resPairs[*index])[i].syz) == *actdeg))
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
  *index = 0;
  //if (TEST_OPT_PROT) Print("(Euler:%d)",euler);
  while (*index<length)
  {
    if (resPairs[*index]!=NULL)
    {
      i = 0;
      while ((i<(*Tl)[*index]))
      {
        t = *actdeg+*index;
        if ((resPairs[*index])[i].lcm!=NULL)
        {
          if (pGetOrder((resPairs[*index])[i].lcm) > *actdeg+*index)
            t = pGetOrder((resPairs[*index])[i].lcm);
        }
        else if ((resPairs[*index])[i].syz!=NULL)
        {
          if (pTotaldegree((resPairs[*index])[i].syz) > *actdeg+*index)
            t = pTotaldegree((resPairs[*index])[i].syz);
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
    return syChosePairs(resPairs,Tl,index,howmuch,length,actdeg);
  }
  else return NULL;
}

/*3
* looks through the pair set and the given module for
* remaining pairs or generators to consider
* returns a pointer to the first pair and the number of them in the given module
* works deg by deg
*/
static SSet syChosePairs1(SRes resPairs,intvec * Tl, int *index, int *howmuch,
                   int length,int * actdeg)
{
  int newdeg=*actdeg,newindex=-1,i,t;
  SSet result;
  
  while (*index>=0)
  {
    if (resPairs[*index]!=NULL)
    {
      i = 0;
      if (*index!=0)
      {
        while ((i<(*Tl)[*index]))
        {
          if ((resPairs[*index])[i].lcm!=NULL)
          {
            if (pGetOrder((resPairs[*index])[i].lcm) == *actdeg)
            {
              result = &(resPairs[*index])[i];
              *howmuch =1;
              i++;
              while ((i<(*Tl)[*index]) && ((resPairs[*index])[i].lcm!=NULL)
                      && (pGetOrder((resPairs[*index])[i].lcm) == *actdeg))
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
        while ((i<(*Tl)[*index]))
        {
          if ((resPairs[*index])[i].syz!=NULL)
          {
            if (pTotaldegree((resPairs[*index])[i].syz) == *actdeg)
            {
              result = &(resPairs[*index])[i];
              (*howmuch) =1;
              i++;
              while ((i<(*Tl)[*index]) && ((resPairs[*index])[i].syz!=NULL)
                      && (pTotaldegree((resPairs[*index])[i].syz) == *actdeg))
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
    (*index)--;
  }
  *index = length-1;
  while (*index>=0)
  {
    if (resPairs[*index]!=NULL)
    {
      i = 0;
      while ((i<(*Tl)[*index]))
      {
        t = *actdeg;
        if ((resPairs[*index])[i].lcm!=NULL)
        {
          if (pGetOrder((resPairs[*index])[i].lcm) > *actdeg)
            t = pGetOrder((resPairs[*index])[i].lcm);
        }
        else if ((resPairs[*index])[i].syz!=NULL)
        {
          if (pTotaldegree((resPairs[*index])[i].syz) > *actdeg)
            t = pTotaldegree((resPairs[*index])[i].syz);
        }
        if ((t>*actdeg) && ((newdeg==*actdeg) || (t<newdeg)))
        {
          newdeg = t;
          newindex = *index;
          break;
        }
        i++;
      } 
    }
    (*index)--;
  }
  if (newdeg>*actdeg)
  {
    *actdeg = newdeg;
    *index = newindex;
    return syChosePairs1(resPairs,Tl,index,howmuch,length,actdeg);
  }
  else return NULL;
}

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
* sets regularity computed from the leading terms of arg
*/
static int sySetRegularity(ideal arg)
{
  if (idIs0(arg)) return -1;
  ideal temp=idInit(IDELEMS(arg),arg->rank);
  int i,len,reg=-1;
//  intvec * w=new intvec(2);

  for (i=IDELEMS(arg)-1;i>=0;i--)
  {
    if (arg->m[i]!=NULL)
      temp->m[i] = pHead(arg->m[i]);
  }
  idSkipZeroes(temp);
  resolvente res = sySchreyerResolvente(temp,-1,&len,TRUE,TRUE);
  intvec * dummy = syBetti(res,len,&reg, NULL);
  delete dummy;
  for (i=0;i<len;i++)
  {
    if (res[i]!=NULL) idDelete(&(res[i]));
  }
  Free((ADDRESS)res,len*sizeof(ideal));
  idDelete(&temp);
  return reg;
}

/*3
* initialize a module
*/
static int syInitSyzMod(resolvente res,resolvente orderedRes,
                         int index)
{
  int result;

  if (res[index]==NULL) 
  {
    res[index] = idInit(16,1);
    truecomponents[index] = (int*)Alloc0(17*sizeof(int));
    backcomponents[index] = (int*)Alloc0(17*sizeof(int));
    Howmuch[index] = (int*)Alloc0(17*sizeof(int));
    Firstelem[index] = (int*)Alloc0(17*sizeof(int));
//Print("sort %d has now size %d\n",index,17);
    orderedRes[index] = idInit(16,1);
    result = 0;
  }
  else
  {
    result = IDELEMS(res[index]);
    while ((result>0) && (res[index]->m[result-1]==NULL)) result--;
  }
  return result;
}

/*2
* implementation of LaScala's algorithm
* assumes that the given module is homogeneous
* works with slanted degree, uses syChosePairs
*/
resolvente syLaScala1(ideal arg,int * length)
{
  BOOLEAN noPair=FALSE;
  int i,j,* ord,*b0,*b1,actdeg=32000,index=0,reg=-1;
  int startdeg,howmuch;
  intvec * Tl;
  poly p;
  ideal temp;
  resolvente res,orderedRes;
  SSet nextPairs;
  SRes resPairs;
  pSetmProc oldSetm=pSetm;

  //crit = 0;
  //zeroRed = 0;
  //simple = 0;
  //dsim = 0;
  euler = -1;
  redpol = pNew();
  //orderingdepth = new intvec(pVariables+1);
  if (*length<=0) *length = pVariables+2;
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
  Tl = new intvec(*length);
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
  {
    highdeg=1;
    long long t=1;
    long long h_n=1+pVariables;
    while ((t=(((long long)t*(long long)h_n)/(long long)highdeg))<INT_MAX)
    {
      highdeg++;
      h_n++;
    }
    highdeg--;
    Print("max deg=%d\n",highdeg);
  }  
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
  resPairs = syInitRes(temp,length,Tl);
  res = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  orderedRes = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  truecomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  backcomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  Howmuch = (int**)Alloc0((*length+1)*sizeof(int*));
  Firstelem = (int**)Alloc0((*length+1)*sizeof(int*));
  int len0=idRankFreeModule(arg)+1;
  truecomponents[0] = (int*)Alloc(len0*sizeof(int));
  backcomponents[0] = (int*)Alloc(len0*sizeof(int));
  Howmuch[0] = (int*)Alloc(len0*sizeof(int));
  Firstelem[0] = (int*)Alloc(len0*sizeof(int));
//Print("sort %d has now size %d\n",0,len0);
  for (i=0;i<len0;i++)
    truecomponents[0][i] = i;
  startdeg = actdeg;
  nextPairs = syChosePairs(resPairs,Tl,&index,&howmuch,*length,&actdeg);
  //if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
/*--- computes the resolution ----------------------*/ 
  while (nextPairs!=NULL)
  {
    if (TEST_OPT_PROT) Print("%d",actdeg);
    if (TEST_OPT_PROT) Print("(m%d)",index);
    currcomponents = truecomponents[max(index-1,0)];
    i = syInitSyzMod(res,orderedRes,index);
    j = syInitSyzMod(res,orderedRes,index+1);
    if (index>0)
    {
      syRedNextPairs(nextPairs,res,orderedRes,howmuch,index);
      syCompactifyPairSet(resPairs[index],(*Tl)[index],0);
    }
    else
      syRedGenerOfCurrDeg(resPairs,res,orderedRes,actdeg,index+1,Tl);
/*--- creates new pairs -----------------------------*/      
    syCreateNewPairs(resPairs,Tl,res,index,i);
    if (index<(*length)-1) 
    {
      syCreateNewPairs(resPairs,Tl,res,index+1,j);
      //currcomponents = truecomponents[index];
      //sySyzTail(resPairs,Tl,orderedRes,res,index+1,j);
      //currcomponents = truecomponents[index-1];
    }
    index++;
    nextPairs = syChosePairs(resPairs,Tl,&index,&howmuch,*length,&actdeg);
    //if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
  }
/*--- deletes temporary data structures-------------*/
  idDelete(&temp);
  for (i=0;i<*length;i++)
  {
    for (j=0;j<(*Tl)[i];j++)
    {
      if ((resPairs[i])[j].lcm!=NULL)
        pDelete(&(resPairs[i])[j].lcm);
    }
    if (orderedRes[i]!=NULL)
    {
      for (j=0;j<IDELEMS(orderedRes[i]);j++)
        orderedRes[i]->m[j] = NULL;
    }
    idDelete(&orderedRes[i]);
    if (truecomponents[i]!=NULL)
    {
      Free((ADDRESS)truecomponents[i],(IDELEMS(res[i])+1)*sizeof(int));
      truecomponents[i]=NULL;
    }
    if (backcomponents[i]!=NULL)
    {
      Free((ADDRESS)backcomponents[i],(IDELEMS(res[i])+1)*sizeof(int));
      backcomponents[i]=NULL;
    }
    if (Howmuch[i]!=NULL)
    {
      Free((ADDRESS)Howmuch[i],(IDELEMS(res[i])+1)*sizeof(int));
      Howmuch[i]=NULL;
    }
    if (Firstelem[i]!=NULL)
    {
      Free((ADDRESS)Firstelem[i],(IDELEMS(res[i])+1)*sizeof(int));
      Firstelem[i]=NULL;
    }
    Free((ADDRESS)resPairs[i],(*Tl)[i]*sizeof(SObject));
  }
  Free((ADDRESS)resPairs,*length*sizeof(SObject*));
  Free((ADDRESS)orderedRes,(*length+1)*sizeof(ideal));
  Free((ADDRESS)truecomponents,(*length+1)*sizeof(int*));
  Free((ADDRESS)backcomponents,(*length+1)*sizeof(int*));
  Free((ADDRESS)Howmuch,(*length+1)*sizeof(int*));
  Free((ADDRESS)Firstelem,(*length+1)*sizeof(int*));
  truecomponents = NULL;
  backcomponents = NULL;
  Howmuch = NULL;
  Firstelem = NULL;
  if (BTEST1(6)) syStatistics(res,(*length+1));
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
  }
  syReOrderResolventFB(res,*length,2);
  for (i=0;i<*length-1;i++)
  {
    res[i] = res[i+1];
    if (res[i]!=NULL)
    {
      if (i>0)
      {
        for (j=0;j<IDELEMS(res[i]);j++)
        {
          res[i]->m[j] = syOrdPolySchreyer(res[i]->m[j]);
        }
        idSkipZeroes(res[i]);
      }
      else
      {
        for (j=0;j<IDELEMS(res[i]);j++)
        {
          res[i]->m[j] = pOrdPoly(res[i]->m[j]);
        }
      }
    }
  }
  res[*length-1] = NULL;
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
  return res;
}

/*2
* second implementation of LaScala's algorithm
* assumes that the given module is homogeneous
* works deg by deg, uses syChosePairs1
*/
resolvente syLaScala2(ideal arg,int * length)
{
  BOOLEAN noPair=FALSE;
  int i,j,* ord,*b0,*b1,actdeg=32000,index=1,reg=-1;
  int startdeg,howmuch;
  intvec * Tl;
  poly p;
  ideal temp;
  resolvente res,orderedRes;
  SSet nextPairs;
  SRes resPairs;

  //crit = 0;
  //zeroRed = 0;
  //simple = 0;
  //dsim = 0;
  redpol = pNew();
  //orderingdepth = new intvec(pVariables+1);
  if (*length<=0) *length = pVariables+2;
  if (idIs0(arg)) return NULL;
/*--- changes to a dpc-ring with special comp0------*/
  ord = (int*)Alloc0(3*sizeof(int));
  b0 = (int*)Alloc0(3*sizeof(int));
  b1 = (int*)Alloc0(3*sizeof(int));
  ord[0] = ringorder_dp;
  ord[1] = ringorder_C;
  b0[1] = 1;
  b1[0] = pVariables;
  pChangeRing(pVariables,1,ord,b0,b1,currRing->wvhdl);
/*--- initializes the data structures---------------*/
  Tl = new intvec(*length);
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
  {
    highdeg=1;
    long long t=1;
    long long h_n=1+pVariables;
    while ((t=(((long long)t*(long long)h_n)/(long long)highdeg))<INT_MAX)
    {
      highdeg++;
      h_n++;
    }
    highdeg--;
    Print("max deg=%d\n",highdeg);
  }  
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
  resPairs = syInitRes(temp,length,Tl);
  res = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  orderedRes = (resolvente)Alloc0((*length+1)*sizeof(ideal));
  truecomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  backcomponents = (int**)Alloc0((*length+1)*sizeof(int*));
  Howmuch = (int**)Alloc0((*length+1)*sizeof(int*));
  Firstelem = (int**)Alloc0((*length+1)*sizeof(int*));
  int len0=idRankFreeModule(arg)+1;
  truecomponents[0] = (int*)Alloc(len0*sizeof(int));
  backcomponents[0] = (int*)Alloc(len0*sizeof(int));
  Howmuch[0] = (int*)Alloc(len0*sizeof(int));
  Firstelem[0] = (int*)Alloc(len0*sizeof(int));
//Print("sort %d has now size %d\n",0,len0);
  for (i=0;i<len0;i++)
    truecomponents[0][i] = i;
  startdeg = actdeg;
  nextPairs = syChosePairs1(resPairs,Tl,&index,&howmuch,*length,&actdeg);
  if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
/*--- computes the resolution ----------------------*/ 
  while (nextPairs!=NULL)
  {
    if (TEST_OPT_PROT) Print("%d",actdeg);
    if (TEST_OPT_PROT) Print("(m%d)",index);
    currcomponents = truecomponents[max(index-1,0)];
    i = syInitSyzMod(res,orderedRes,index);
    j = syInitSyzMod(res,orderedRes,index+1);
    if (index>0)
    {
      syRedNextPairs(nextPairs,res,orderedRes,howmuch,index);
      syCompactifyPairSet(resPairs[index],(*Tl)[index],0);
    }
    else
      syRedGenerOfCurrDeg(resPairs,res,orderedRes,actdeg,index+1,Tl);
/*--- creates new pairs -----------------------------*/      
    syCreateNewPairs(resPairs,Tl,res,index,i);
    if (index<(*length)-1) 
    {
      syCreateNewPairs(resPairs,Tl,res,index+1,j);
      currcomponents = truecomponents[index];
      sySyzTail(resPairs,Tl,orderedRes,res,index+1,j);
      currcomponents = truecomponents[index-1];
    }
    index--;
    nextPairs = syChosePairs1(resPairs,Tl,&index,&howmuch,*length,&actdeg);
    if (TEST_OPT_PROT) Print("(%d,%d)",howmuch,index);
  }
/*--- deletes temporary data structures-------------*/
  idDelete(&temp);
  for (i=0;i<*length;i++)
  {
    for (j=0;j<(*Tl)[i];j++)
    {
      if ((resPairs[i])[j].lcm!=NULL)
        pDelete(&(resPairs[i])[j].lcm);
    }
    if (orderedRes[i]!=NULL)
    {
      for (j=0;j<IDELEMS(orderedRes[i]);j++)
        orderedRes[i]->m[j] = NULL;
    }
    idDelete(&orderedRes[i]);
    if (truecomponents[i]!=NULL)
    {
      Free((ADDRESS)truecomponents[i],(IDELEMS(res[i])+1)*sizeof(int));
      truecomponents[i]=NULL;
    }
    if (backcomponents[i]!=NULL)
    {
      Free((ADDRESS)backcomponents[i],(IDELEMS(res[i])+1)*sizeof(int));
      backcomponents[i]=NULL;
    }
    if (Howmuch[i]!=NULL)
    {
      Free((ADDRESS)Howmuch[i],(IDELEMS(res[i])+1)*sizeof(int));
      Howmuch[i]=NULL;
    }
    if (Firstelem[i]!=NULL)
    {
      Free((ADDRESS)Firstelem[i],(IDELEMS(res[i])+1)*sizeof(int));
      Firstelem[i]=NULL;
    }
    Free((ADDRESS)resPairs[i],(*Tl)[i]*sizeof(SObject));
  }
  Free((ADDRESS)resPairs,*length*sizeof(SObject*));
  Free((ADDRESS)orderedRes,(*length+1)*sizeof(ideal));
  Free((ADDRESS)truecomponents,(*length+1)*sizeof(int*));
  Free((ADDRESS)backcomponents,(*length+1)*sizeof(int*));
  Free((ADDRESS)Howmuch,(*length+1)*sizeof(int*));
  Free((ADDRESS)Firstelem,(*length+1)*sizeof(int*));
  truecomponents = NULL;
  backcomponents = NULL;
  Howmuch = NULL;
  Firstelem = NULL;
  if (BTEST1(6)) syStatistics(res,(*length+1));
  (*length)++;
/*--- changes to the original ring------------------*/
  pChangeRing(pVariables,currRing->OrdSgn,currRing->order,
    currRing->block0,currRing->block1,currRing->wvhdl);
  syReOrderResolventFB(res,*length,2);
  for (i=0;i<*length-1;i++)
  {
    res[i] = res[i+1];
    if (res[i]!=NULL)
    {
      if (i>0)
      {
        for (j=0;j<IDELEMS(res[i]);j++)
        {
          res[i]->m[j] = syOrdPolySchreyer(res[i]->m[j]);
        }
        idSkipZeroes(res[i]);
      }
      else
      {
        for (j=0;j<IDELEMS(res[i]);j++)
        {
          p = res[i]->m[j];
          while (p!=NULL)
          {
            pSetm(p);
            pIter(p);
          }
        }
      }
    }
  }
  res[*length-1] = NULL;
  Free((ADDRESS)ord,3*sizeof(int));
  Free((ADDRESS)b0,3*sizeof(int));
  Free((ADDRESS)b1,3*sizeof(int));
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
  return res;
}
