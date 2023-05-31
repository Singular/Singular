/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Stainly
*/

#include "kernel/mod2.h"

#if !defined(__CYGWIN__) || defined(STATIC_VERSION)
// acces from a module to routines from the main program
// does not work on windows (restrict of the dynamic linker),
// a static version is required:
// ./configure --with-builtinmodules=cohomo,...


#include "omalloc/omalloc.h"
#include "misc/mylimits.h"
#include "libpolys/misc/intvec.h"
#include <assert.h>
#include <unistd.h>

#include "kernel/combinatorics/hilb.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/combinatorics/hutil.h"
#include "cohomo.h"//for my thing
#include "kernel/GBEngine/tgb.h"//
#include "Singular/ipid.h"//for ggetid
#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/simpleideals.h"
#include "Singular/lists.h"
#include "kernel/linear_algebra/linearAlgebra.h"//for printNumber
#include "kernel/GBEngine/kstd1.h"//for gb
#include <kernel/ideals.h>
#if 1

#include<libpolys/polys/ext_fields/transext.h>
#include<libpolys/coeffs/coeffs.h>
#include<kernel/linear_algebra/linearAlgebra.h>
#include <coeffs/numbers.h>
#include <vector>
#include <Singular/ipshell.h>
#include <Singular/libsingular.h>
#include <time.h>

/***************************print(only for debugging)***********************************************/
//print vector of integers.
static void listprint(std::vector<int> vec)
{
  unsigned i;
  for(i=0;i<vec.size();i++)
  {
    Print("   _[%d]=%d\n",i+1,vec[i]);
    PrintLn();
  }
  if(vec.size()==0)
  {
    PrintS("   _[1]= \n");
    PrintLn();
  }
}

//print vector of vectors of integers.
static void listsprint(std::vector<std::vector<int> > posMat)
{
  unsigned i;
  for(i=0;i<posMat.size();i++)
  {
    Print("[%d]:\n",i+1);
    listprint(posMat[i]);
    Print("\n");
    PrintLn();
  }
  if(posMat.size()==0)
  {
    PrintS("[1]:\n");
    PrintLn();
  }
}


//print ideal.
static void id_print(ideal h)
{
  int i;
  for(i=0;i<IDELEMS(h);i++)
  {
    Print(" [%d]\n",i+1);
    pWrite(h->m[i]);
    PrintLn();
  }
}

//only for T^2,
//print vector of polynomials.
static void lpprint( std::vector<poly> pv)
{
  for(unsigned i=0;i<pv.size();i++)
  {
    Print("   _[%d]=",i+1);
    pWrite(pv[i]);
  }
  if(pv.size()==0)
  {
    PrintS("   _[1]= \n");
    PrintLn();
  }
}

//print vector of vectors of polynomials.
static void lpsprint(std::vector<std::vector<poly> > pvs)
{
  for(unsigned i=0;i<pvs.size();i++)
  {
    Print("[%d]:\n",i+1);
    lpprint(pvs[i]);
    Print("\n");
    PrintLn();
  }
  if(pvs.size()==0)
  {
    PrintS("[1]:\n");
    PrintLn();
  }
}

/*************operations for vectors (regard vectors as sets)*********/

//returns true if integer n is in vector vec,
//otherwise, returns false
static bool IsinL(int a, std::vector<int> vec)
{
  unsigned i;
  for(i=0;i<vec.size();i++)
  {
    if(a==vec[i])
    {
      return true;
    }
  }
  return false;
}

//returns intersection of vectors p and q,
//returns empty if they are disjoint
static std::vector<int> vecIntersection(std::vector<int> p, std::vector<int> q)
{
  unsigned i;
  std::vector<int> inte;
  for(i=0;i<p.size();i++)
  {
    if(IsinL(p[i],q))
      inte.push_back(p[i]);
  }
  return inte;
}

//returns true if vec1 is contained in vec2
static bool vsubset(std::vector<int> vec1, std::vector<int> vec2)
{
  int i;
  if(vec1.size()>vec2.size())
    return false;
  for(i=0;i<vec1.size();i++)
  {
    if(!IsinL(vec1[i],vec2))
      return false;
  }
  return true;
}

//not strictly equal(order doesn't matter)
static bool vEvl(std::vector<int> vec1, std::vector<int> vec2)
{
  if(vec1.size()==0 && vec2.size()==0)
    return true;
  if(vsubset(vec1,vec2)&&vsubset(vec2,vec1))
    return true;
  return false;
}

//the length of vec must be same to it of the elements of vecs
//returns true if vec is as same as some element of vecs ((not strictly same))
//returns false if vec is not in vecs
static bool vInvsl(std::vector<int> vec, std::vector<std::vector<int> > vecs)
{
  int i;
  for(i=0;i<vecs.size();i++)
  {
    if(vEvl(vec,vecs[i]))
    {
      return true;
    }
  }
  return false;
}

//returns the union of two vectors(as the union of sets)
static std::vector<int> vecUnion(std::vector<int> vec1, std::vector<int> vec2)
{
  std::vector<int> vec=vec1;
  unsigned i;
  for(i=0;i<vec2.size();i++)
  {
    if(!IsinL(vec2[i],vec))
      vec.push_back(vec2[i]);
  }
  return vec;
}

static std::vector<int> vecMinus(std::vector<int> vec1,std::vector<int> vec2)
{
  std::vector<int> vec;
  for(unsigned i=0;i<vec1.size();i++)
  {
    if(!IsinL(vec1[i],vec2))
    {
      vec.push_back(vec1[i]);
    }
  }
  return vec;
}

static std::vector<std::vector<int> > vsMinusv(std::vector<std::vector<int> > vecs, std::vector<int> vec)
{
  int i;
  std::vector<std::vector<int> > rem;
  for(i=0;i<vecs.size();i++)
  {
    if(!vEvl(vecs[i],vec))
    {
      rem.push_back(vecs[i]);
    }
  }
  return (rem);
}

static std::vector<std::vector<int> > vsUnion(std::vector<std::vector<int> > vs1, std::vector<std::vector<int> > vs2)
{
  int i;
  std::vector<std::vector<int> > vs=vs1;
  for(i=0;i<vs2.size();i++)
  {
    if(!vInvsl(vs2[i],vs))
    {
      vs.push_back(vs2[i]);
    }
  }
  return vs;
}

static std::vector<std::vector<int> > vsIntersection(std::vector<std::vector<int> > vs1, std::vector<std::vector<int> > vs2)
{
  int i;
  std::vector<std::vector<int> > vs;
  for(i=0;i<vs2.size();i++)
  {
    if(vInvsl(vs2[i],vs1))
    {
      vs.push_back(vs2[i]);
    }
  }
  return vs;
}

/*************************************for transition between ideal and vectors******************************************/

//P should be monomial,
// vector version of poly support(poly p)
static std::vector<int> support1(poly p)
{
  int j;
  std::vector<int> supset;
  if(p==0) return supset;
  for(j=1;j<=rVar(currRing);j++)
  {
    if(pGetExp(p,j)>0)
    {
      supset.push_back(j);
    }
  }
  return (supset);
}

//simplicial complex(the faces set is ideal h)
static std::vector<std::vector<int> >  supports(ideal h)
{
  std::vector<std::vector<int> > vecs;
  std::vector<int> vec;
  if(!idIs0(h))
  {
    for(int s=0;s<IDELEMS(h);s++)
    {
      vec=support1(h->m[s]);
      vecs.push_back(vec);
    }
  }
  return vecs;
}

// only for eqsolve1
// p could be any polynomial
static std::vector<int> support2(poly p)
{
  int j;
  poly q;
  std::vector<int> supset;
  for(j=1;j<=rVar(currRing);j++)
  {
    q=pCopy(p);
    while (q!=NULL)
    {
      if(p_GetExp(q,j,currRing)!=0)
      {
        supset.push_back(j);
        break;
      }
      q=pNext(q);
    }
  }
  return (supset);
}

//the supports of ideal
static std::vector<std::vector<int> >  supports2(ideal h)
{
  std::vector<std::vector<int> > vecs;
  std::vector<int> vec;
  if(!idIs0(h))
  {
    for(int s=0;s<IDELEMS(h);s++)
    {
      vec=support2(h->m[s]);
      vecs.push_back(vec);
    }
  }
  return vecs;
}

//convert the vector(vbase[i] are the coefficients of x_{i+1}) to a polynomial w.r.t. current ring
//vector vbase has length of currRing->N.
static poly pMake(std::vector<int> vbase)
{
  int n=vbase.size(); poly p,q=0;
  for(int i=0;i<n;i++)
  {
    if(vbase[i]!=0)
    {
      p = pOne();pSetExp(p, i+1, 1);pSetm(p);pSetCoeff(p, nInit(vbase[i]));
      q = pAdd(q, p);
    }
  }
  return q;
}

//convert the vectors to a ideal(for T^1)
static ideal idMake(std::vector<std::vector<int> > vecs)
{
  int lv=vecs.size(), i;
  poly p;
  ideal id_re=idInit(1,1);
  for(i=0;i<lv;i++)
  {
    p=pMake(vecs[i]);
    idInsertPoly(id_re, p);
  }
  idSkipZeroes(id_re);
  return id_re;
}

/*****************************quotient ring of two ideals*********************/

//the quotient ring of h1 respect to h2
static ideal idmodulo(ideal h1,ideal h2)
{
  ideal gb=kStd(h2,NULL,testHomog,NULL,NULL,0,0,NULL);
  idSkipZeroes(gb);
  ideal idq=kNF(gb,NULL,h1);
  idSkipZeroes(idq);
  return idq;
}

//returns the coeff of the monomial of polynomial p which involves the mth varialbe
//assume the polynomial p has form of y1+y2+...
static int pcoef(poly p, int m)
{
  int i,co; poly q=pCopy(p);
  for(i=1;i<=currRing->N;i++)
  {
    if(p_GetExp(q,m,currRing)!=0)
    {
      co=n_Int(pGetCoeff(q),currRing->cf);
      return co;
    }
    else
      q=pNext(q);
  }
  if(q!=NULL)
    co=0;
  return co;
}

//returns true if p involves the mth variable of the current ring
static bool vInp(int m,poly p)
{
  poly q=pCopy(p);
  while (q!=NULL)
  {
    if(p_GetExp(q,m,currRing)!=0)
    {
      return true;
    }
    q=pNext(q);
  }
  return false;
}

//returns the vector w.r.t. polynomial p
static std::vector<int> vMake(poly p)
{
  int i;
  std::vector<int> vbase;
  for(i=1;i<=currRing->N;i++)
  {
    if(vInp(i,p))
    {
      vbase.push_back(pcoef(p,i));
    }
    else
    {
      vbase.push_back(0);
    }
  }
  return (vbase);
}

//returns the vectors w.r.t. ideal h
static std::vector<std::vector<int> > vsMake(ideal h)
{
  std::vector<int> vec;
  std::vector<std::vector<int> > vecs;
  int i;
  for(i=0;i<IDELEMS(h);i++)
  {
    vec=vMake(h->m[i]);
    vecs.push_back(vec);
  }
  return vecs;
}

//the quotient ring of two ideals which are represented by vectors,
//the result is also represented by vector.
static std::vector<std::vector<int> > vecqring(std::vector<std::vector<int> > vec1, std::vector<std::vector<int> > vec2)
{
  ideal h1=idMake(vec1), h2=idMake(vec2);
  ideal h=idmodulo(h1,h2);
  std::vector<std::vector<int> > vecs= vsMake(h);
  return vecs;
}

/****************************************************************/
//construct a monomial based on the support of it
//returns a squarefree monomial
static poly pMaken(std::vector<int> vbase)
{
  int n=vbase.size();
  poly p,q=pOne();
  for(int i=0;i<n;i++)
  {
    p = pOne();pSetExp(p, vbase[i], 1);pSetm(p);pSetCoeff(p, nInit(1));
    //pWrite(p);
    q=pp_Mult_mm(q,p,currRing);
  }
  return q;
}

// returns a ideal according to a set of supports
static ideal idMaken(std::vector<std::vector<int> > vecs)
{
  ideal id_re=idInit(1,1);
  poly p;
  int i,lv=vecs.size();
  for(i=0;i<lv;i++)
  {
    p=pMaken(vecs[i]);
    idInsertPoly(id_re, p);
  }
  idSkipZeroes(id_re);
  //id_print(id_re);
  return id_re;
}

/********************************new version for stanley reisner ideal ***********************************************/

static std::vector<std::vector<int> > b_subsets(std::vector<int> vec)
{
  int i,j;
  std::vector<int> bv;
  std::vector<std::vector<int> > vecs;
  for(i=0;i<vec.size();i++)
  {
    bv.push_back(vec[i]);
    vecs.push_back(bv);
    bv.clear();
  }
  //listsprint(vecs);
  for(i=0;i<vecs.size();i++)
  {
    for(j=i+1;j<vecs.size();j++)
    {
      bv=vecUnion(vecs[i], vecs[j]);
      if(!vInvsl(bv,vecs))
        vecs.push_back(bv);
    }
  }
  //listsprint(vecs);
  return(vecs);
}

//the number of the variables
static int idvert(ideal h)
{
  int i, j, vert=0;
  if(idIs0(h))
    return vert;
  for(i=currRing->N;i>0;i--)
  {
    for(j=0;j<IDELEMS(h);j++)
    {
      if(pGetExp(h->m[j],i)>0)
      {
        vert=i;
        return vert;
      }
    }
  }
  return vert;
}

static int pvert(poly p)
{
  int i, vert=0;
  for(i=currRing->N;i>0;i--)
  {
      if(pGetExp(p,i)>0)
      {
        vert=i;
        return vert;
      }
  }
  return vert;
}

/*
//full complex
static std::vector<std::vector<int> > fullcomplex(ideal h)
{
  int vert=vertnum(h), i, j;
  //Print("there are %d vertices\n", vert);
  std::vector<std::vector<int> > fmons;
  std::vector<int> pre;
  for(i=1;i<=vert;i++)
  {
    pre.push_back(i);
  }
  fmons=b_subsets(pre);
  return fmons;
}*/

/*
//all the squarefree monomials whose degree is less or equal to n
static std::vector<std::vector<int> > sfrmons(ideal h, int n)
{
  int vert=vertnum(h), i, j, time=0;
  std::vector<std::vector<int> > fmons, pres, pres0, pres1;
  std::vector<int> pre;
  for(i=1;i<=vert;i++)
  {
    pre.push_back(i);
    pres0.push_back(pre);
  }
  pres=pres0;
  for(i=0;i<size(pres),time<=n;i++)
  {
    time++;
    pre=pres[i];
    for(j=0;j<size(pres0);j++)
    {
      pre=vecUnion(pre, pres0[j]);
      if(pre.)
    }
  }
  return fmons;
}
*/

/*
static ideal id_complement(ideal h)
{
  int i,j;
  std::vector<std::vector<int> > full=fullcomplex(h), hvs=supports(h), res;
  for(i=0;i<full.size();i++)
  {
    if(!vInvsl(full[i], hvs))
    {
      res.push_back(full[i]);
    }
  }
  return idMaken(res);
}*/


/*****************About simplicial complex and stanley-reisner ideal and ring **************************/

//h1 minus h2
static ideal idMinus(ideal h1,ideal h2)
{
  ideal h=idInit(1,1);
  int i,j,eq=0;
  for(i=0;i<IDELEMS(h1);i++)
  {
    eq=0;
    for(j=0;j<IDELEMS(h2);j++)
    {
      if(p_EqualPolys(pCopy(h1->m[i]),pCopy(h2->m[j]), currRing))
      {
        eq=1;
        break;
      }
    }
    if(eq==0)
    {
      idInsertPoly(h, pCopy(h1->m[i]));
    }
  }
  idSkipZeroes(h);
  return h;
}

//If poly P is squarefree, returns 1
//returns 0 otherwise,
static bool p_Ifsfree(poly P)
{
  int i,sf=1;
  for(i=1;i<=rVar(currRing);i++)
  {
    if (pGetExp(P,i)>1)
    {
      sf=0;
      break;
    }
  }
  return sf;
}

//returns the set of all squarefree monomials of degree deg in ideal h
static ideal sfreemon(ideal h,int deg)
{
  int j;
  ideal temp;
  temp=idInit(1,1);
  if(!idIs0(h))
  {
    for(j=0;j<IDELEMS(h);j++)
    {
      if((p_Ifsfree(h->m[j]))&&(pTotaldegree(h->m[j])==deg))
      {
        idInsertPoly(temp, h->m[j]);
      }
    }
    idSkipZeroes(temp);
  }
  return temp;
}

//full simplex represented by ideal.
//(all the squarefree monomials over the polynomial ring)
static ideal id_sfmon(ideal h)
{
  ideal asfmons,sfmons,mons;
  int j, vert=idvert(h);
  mons=id_MaxIdeal(1, currRing);
  asfmons=sfreemon(mons,1);
  for(j=2;j<=vert;j++)
  {
    mons=id_MaxIdeal(j, currRing);
    sfmons=sfreemon(mons,j);
    asfmons=id_Add(asfmons,sfmons,currRing);
  }
  return asfmons;
}

//if the input ideal is simplicial complex, returns the stanley-reisner ideal,
//if the input ideal is stanley-reisner ideal, returns the monomial ideal according to simplicial complex.
//(nonfaces and faces).
//returns the complement of the ideal h (consisting of only squarefree polynomials)
static ideal id_complement(ideal h)
{
  int j, vert=idvert(h);
  ideal i1=id_sfmon(h);
  ideal i3=idInit(1,1);
  poly p;
  for(j=0;j<IDELEMS(i1);j++)
  {
    p=pCopy(i1->m[j]);
    if(pvert(p)<=vert)
    {
      idInsertPoly(i3, p);
    }
  }
  ideal i2=idMinus(i3,h);
  idSkipZeroes(i2);
  return (i2);
}

//Returns true if p is one of the generators of ideal X
//returns false otherwise
static bool IsInX(poly p,ideal X)
{
  int i;
  for(i=0;i<IDELEMS(X);i++)
  {
    if(pEqualPolys(p,X->m[i]))
    {
      //PrintS("yes\n");
      return(true);
    }
  }
  //PrintS("no\n");
  return(false);
}

//returns the monomials in the quotient ring R/(h1+h2) which have degree deg.
static ideal qringadd(ideal h1, ideal h2, int deg)
{
  ideal h,qrh;
  h=idAdd(h1,h2);
  qrh=scKBase(deg,h);
  return qrh;
}

//returns the maximal degree of the monomials in ideal h
static int id_maxdeg(ideal h)
{
  int i,max;
  max=pTotaldegree(h->m[0]);
  for(i=1;i<IDELEMS(h);i++)
  {
    if(pTotaldegree(h->m[i]) > max)
      max=pTotaldegree(h->m[i]);
  }
  return (max);
}

//input ideal h (a squarefree monomial ideal) is the ideal associated to simplicial complex,
//and returns the Stanley-Reisner ideal(minimal generators)
static ideal idsrRing(ideal h)
{
  int i,n;
  ideal pp,qq,rsr,ppp,hc=idCopy(h);
  for(i=1;i<=rVar(currRing);i++)
  {
    pp=sfreemon(hc,i);
    pp=scKBase(i,pp);//quotient ring (R/I_i)_i
    if(!idIs0(pp))
    {
      pp=sfreemon(pp,i);
      rsr=pp;
      //Print("This is the first quotient generators %d:\n",i);
      //id_print(rsr);
      break;
    }
  }
  for(n=i+1;n<=rVar(currRing);n++)
  {
    qq=sfreemon(hc,n);
    pp=qringadd(qq,rsr,n);
    ppp=sfreemon(pp,n);
    rsr=idAdd(rsr,ppp);
  }
  idSkipZeroes(rsr);
  return rsr;
}

//returns the set of all the polynomials could divide p
static ideal SimFacset(poly p)
{
  int i,j,max=pTotaldegree(p);
  ideal h1,mons,id_re=idInit(1,1);
  for(i=1;i<max;i++)
  {
    mons=id_MaxIdeal(i, currRing);
    h1=sfreemon(mons,i);

    for(j=0;j<IDELEMS(h1);j++)
    {
      if(p_DivisibleBy(h1->m[j],p,currRing))
      {
        idInsertPoly(id_re, h1->m[j]);
      }
    }
  }
  idSkipZeroes(id_re);
  return id_re;
}

static ideal idadda(ideal h1, ideal h2)
{
  ideal h=idInit(1,1);
  for(int i=0;i<IDELEMS(h1);i++)
  {
    if(!IsInX(h1->m[i],h))
    {
      idInsertPoly(h, h1->m[i]);
    }
  }
  for(int i=0;i<IDELEMS(h2);i++)
  {
    if(!IsInX(h2->m[i],h))
    {
      idInsertPoly(h, h2->m[i]);
    }
  }
  idSkipZeroes(h);
  return h;
}

//complicated version
//(returns false if it is not a simplicial complex and print the simplex)
//input h is need to be at least part of faces
static ideal IsSimplex(ideal h)
{
  int i,max=id_maxdeg(h);
  poly e=pOne();
  ideal id_re, id_so=idCopy(h);
  for(i=0;i<IDELEMS(h);i++)
  {
    id_re=SimFacset(h->m[i]);
    if(!idIs0(id_re))
    {
      id_so=idadda(id_so, id_re);//idAdd(id_so,id_re);
    }
  }
  idInsertPoly(id_so,e);
  idSkipZeroes(id_so);
  return (idMinus(id_so,h));
}

//input is the subset of the Stainley-Reisner ideal
//returns the faces
//is not used
static ideal complementsimplex(ideal h)
{
  int i,j;poly p,e=pOne();
  ideal h1=idInit(1,1), pp, h3;
  for(i=1;i<=rVar(currRing);i++)
  {
    p = pOne(); pSetExp(p, i, 2); pSetm(p); pSetCoeff(p, nInit(1));
    idInsertPoly(h1, p);
  }
  idSkipZeroes(h1);
  ideal h2=idAdd(h,h1);
  pp=scKBase(1,h2);
  h3=idCopy(pp);
  for(j=2;j<=rVar(currRing);j++)
  {
    pp=scKBase(j,h2);
    h3=idAdd(h3,pp);
  }
  idInsertPoly(h3, e);
  idSkipZeroes(h3);
  return (h3);
}

static int dim_sim(ideal h)
{
  int dim=pTotaldegree(h->m[0]), i;
  for(i=1; i<IDELEMS(h);i++)
  {
    if(dim<pTotaldegree(h->m[i]))
    {
      dim=pTotaldegree(h->m[i]);
    }
  }
  return dim;
}

static int num4dim(ideal h, int n)
{
  int num=0;
  for(int i=0; i<IDELEMS(h); i++)
  {
    if(pTotaldegree(h->m[i])==n)
    {
      num++;
    }
  }
  return num;
}

/********************Procedures for T1(M method and N method) ***********/

//h is ideal( monomial ideal) associated to simplicial complex
//returns the all the monomials x^b (x^b must be able to divide
//at least one monomial in Stanley-Reisner ring)
//not so efficient
static ideal findb(ideal h)
{
  ideal ib=id_sfmon(h), nonf=id_complement(h), bset=idInit(1,1);
  poly e=pOne();
  int i,j;
  for(i=0;i<IDELEMS(ib);i++)
  {
    for(j=0;j<IDELEMS(nonf);j++)
    {
      if(p_DivisibleBy(ib->m[i],nonf->m[j],currRing))
      {
        idInsertPoly(bset, ib->m[i]);
        break;
      }
    }
  }
  idInsertPoly(bset,e);
  idSkipZeroes(bset);
  return bset;
}

//h is ideal(monomial ideal associated to simplicial complex
//1.poly S is x^b
//2.and deg(x^a)=deg(x^b)
//3.x^a and x^a have disjoint supports
//returns all the possible x^a according conditions 1. 2. 3.
static ideal finda(ideal h,poly S,int ddeg)
{
  poly e=pOne();
  ideal h2=id_complement(h), aset=idInit(1,1);
  int i,deg1=pTotaldegree(S);
  int tdeg=deg1+ddeg;
  if(tdeg!=0)
  {
    std::vector<int> v,bv=support1(S),in;
    std::vector<std::vector<int> > hvs=supports(h);
    ideal ia=id_MaxIdeal(tdeg, currRing);
    for(i=0;i<IDELEMS(ia);i++)
    {
      v=support1(ia->m[i]);
      in=vecIntersection(v,bv);
      if(vInvsl(v,hvs)&&in.size()==0)
      {
        idInsertPoly(aset, ia->m[i]);
      }
    }
    idSkipZeroes(aset);
  }
  else idInsertPoly(aset,e);
  return(aset);
}

//returns true if support(p) union support(a) minus support(b) is face,
//otherwise returns false
//(the vector version of mabcondition)
static bool mabconditionv(std::vector<std::vector<int> > hvs,std::vector<int> pv,std::vector<int> av,std::vector<int> bv)
{
  std::vector<int> uv=vecUnion(pv,av);
  uv=vecMinus(uv,bv);
  if(vInvsl(uv,hvs))
  {
    return(true);
  }
  return(false);
}

// returns the set of nonfaces p where mabconditionv(h, p, a, b) is true
static std::vector<std::vector<int> > Mabv(ideal h,poly a,poly b)
{
  std::vector<int> av=support1(a), bv=support1(b), pv, vec;
  ideal h2=id_complement(h);
  std::vector<std::vector<int> > hvs=supports(h), h2v=supports(h2), vecs;
  for(unsigned i=0;i<h2v.size();i++)
  {
    pv=h2v[i];
    if(mabconditionv(hvs,pv,av,bv))
    {
      vecs.push_back(pv);
    }
  }
  return vecs;
}

/***************************************************************************/
//For solving the equations which has form of x_i-x_j.(equations got from T_1)
/***************************************************************************/

//subroutine for soleli1
static std::vector<int> eli1(std::vector<int> eq1,std::vector<int> eq2)
{
  int i,j;
  std::vector<int> eq;
  if(eq1[0]==eq2[0])
  {
    i=eq1[1];j=eq2[1];
    eq.push_back(i);
    eq.push_back(j);
  }
  else
  {
    eq=eq2;
  }
  return(eq);
}

/*
//get triangular form(eqs.size()>0)
static std::vector<std::vector<int> > soleli1( std::vector<std::vector<int> > eqs)
{
  int i,j;
  std::vector<int> yaya;
  std::vector<std::vector<int> >  pre=eqs, ppre, re;
  if(eqs.size()>0)
  {
    re.push_back(eqs[0]);
    pre.erase(pre.begin());
  }
  for(i=0;i<re.size(),pre.size()>0;i++)
  {
    yaya=eli1(re[i],pre[0]);
    re.push_back(yaya);
    for(j=1;j<pre.size();j++)
    {
      ppre.push_back(eli1(re[i],pre[j]));
    }
    pre=ppre;
    ppre.resize(0);
  }
  return re;
}*/
//make sure the first element is smaller that the second one
static std::vector<int> keeporder(  std::vector<int> vec)
{
  std::vector<int> yaya;
  int n;
  if(vec[0]>vec[1])
  {
    n=vec[0];
    vec[0]=vec[1];
    vec[1]=n;
  }
  return vec;
}

static std::vector<std::vector<int> > soleli1( std::vector<std::vector<int> > eqs)
{
  int i;
  std::vector<int> yaya;
  std::vector<std::vector<int> >  pre=eqs, ppre, re;
  if(eqs.size()>0)
  {
    re.push_back(eqs[0]);
    pre.erase(pre.begin());
  }
  while(pre.size()>0)
  {
    yaya=keeporder(eli1(re[0],pre[0]));
    for(i=1;i<re.size();i++)
    {
      if(!vInvsl(yaya, re))
      {
        yaya=eli1(re[i],yaya);
        yaya=keeporder(yaya);
      }
    }
    if(!vInvsl(yaya, re))
    {
      re.push_back(yaya);
    }
    pre.erase(pre.begin());
  }
  return re;
}

// input is a set of equations who is of triangular form(every equations has a form of x_i-x_j)
// n is the number of variables
//get the free variables and the dimension
static std::vector<int> freevars(int n,  std::vector<int> bset, std::vector<std::vector<int> > gset)
{
  int ql=gset.size(), bl=bset.size(), i;
  std::vector<int> mvar, fvar;
  for(i=0;i<bl;i++)
  {
    mvar.push_back(bset[i]);
  }
  for(i=0;i<ql;i++)
  {
    mvar.push_back(gset[i][0]);
  }
  for(i=1;i<=n;i++)
  {
    if(!IsinL(i,mvar))
    {
      fvar.push_back(i);
    }
  }
  return fvar;
}

//return the set of free variables except the vnum one
static std::vector<int> fvarsvalue(int vnum, std::vector<int> fvars)
{
  int i;
  std::vector<int> fset=fvars;
  for(i=0;i<fset.size();i++)
  {
    if(fset[i]==vnum)
    {
      fset.erase(fset.begin()+i);
      break;
    }
  }
  return fset;
}

//returns the simplified bset and gset
//enlarge bset, simplify gset
static std::vector<std::vector<int> > vAbsorb( std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  std::vector<int> badset=bset;
  int i,j,m, bl=bset.size(), gl=gset.size();
  for(i=0;i<bl;i++)
  {
    m=badset[i];
    for(j=0;j<gl;j++)
    {
      if(gset[j][0]==m && !IsinL(gset[j][1],badset))
      {
        badset.push_back(gset[j][1]);
        gset.erase(gset.begin()+j);
        j--;
        gl--;
        bl++;
      }
      else if(!IsinL(gset[j][0],badset) && gset[j][1]==m)
      {
        badset.push_back(gset[j][0]);
        gset.erase(gset.begin()+j);
        j--;
        gl--;
        bl++;
      }
      else if(IsinL(gset[j][0],badset) && IsinL(gset[j][1],badset))
      {
        gset.erase(gset.begin()+j);
        j--;
        gl--;
      }
      else
      {
        ;
      }
    }
  }
  if(badset.size()==0) badset.push_back(0);
  gset.push_back(badset);
  return gset;
}

//the labels of new variables are started with 1
//returns a vector of solution space according to index
static std::vector<int> vecbase1(int num, std::vector<int> oset)
{
  int i;
  std::vector<int> base;
  for(i=0;i<num;i++)
  {
    if(IsinL(i+1,oset))
      base.push_back(1);
    else
      base.push_back(0);
  }
  return base;
}

//returns a vector which has length of n,
//and all the entries are 0.
static std::vector<int> make0(int n)
{
  int i;
  std::vector<int> vec;
  for(i=0;i<n;i++)
  {
    vec.push_back(0);
  }
  return vec;
}

//returns a vector which has length of n,
//and all the entries are 1.
static std::vector<int> make1(int n)
{
  int i;
  std::vector<int> vec;
  for(i=0;i<n;i++)
  {
    vec.push_back(1);
  }
  return vec;
}

//input gset must be the triangular form after zero absorbing according to the badset,
//bset must be the zero set after absorbing.
static std::vector<int> ofindbases1(int num, int vnum, std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  std::vector<std::vector<int> > goodset;
  std::vector<int> fvars=freevars(num,   bset,  gset), oset, base;
  std::vector<int> zset=fvarsvalue(vnum, fvars);
  zset=vecUnion(zset,bset);
  oset.push_back(vnum);
  goodset=vAbsorb(oset, gset);
  oset=goodset[goodset.size()-1];
  goodset.erase(goodset.end());
  base= vecbase1(num,  oset);
  return base;
}

//input gset must be the triangular form after zero absorbing according to the badset
//bset must be the zero set after absorbing
static std::vector<std::vector<int> > ofindbases(int num,  std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  int i,m;
  std::vector<std::vector<int> > bases;
  std::vector<int> fvars=freevars(num,   bset,  gset), base1;
  if (fvars.size()==0)
  {
    base1=make0(num);
    bases.push_back(base1);
  }
  else
  {
    for(i=0;i<fvars.size();i++)
    {
      m=fvars[i];
      base1=ofindbases1(num, m, bset, gset);
      bases.push_back(base1);
    }
  }
  //PrintS("They are the bases for the solution space:\n");
  //listsprint(bases);
  return bases;
}

//gset is a set of equations which have forms of x_i-x_j
//num is the number of varialbes also the length of the set which we need to consider
//output is trigular form of gset and badset where x_i=0
static std::vector<std::vector<int> > eli2(int num, std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  std::vector<int> badset;
  std::vector<std::vector<int> > goodset, solve;
//PrintS("This is the input bset\n");listprint(bset);
//PrintS("This is the input gset\n");listsprint(gset);
  if(gset.size()!=0)//gset is not empty
  {
   //find all the variables which are zeroes

    if(bset.size()!=0)//bset is not empty
    {
      goodset=vAbsorb(bset, gset);//e.g. x_1=0, put x_i into the badset if x_i-x_1=0 or x_1-x_i=0
      int m=goodset.size();
      badset=goodset[m-1];
      goodset.erase(goodset.end());
    }
    else //bset is empty
    {
      goodset=gset;//badset is empty
    }//goodset is already the set which doesn't contain zero variables
//PrintS("This is the badset after absorb \n");listprint(badset);
//PrintS("This is the goodset after absorb \n");listsprint(goodset);
    goodset=soleli1(goodset);//get the triangular form of goodset
//PrintS("This is the goodset after triangulization \n");listsprint(goodset);
    solve=ofindbases(num,badset,goodset);
  }
  else
  {
    solve=ofindbases(num,bset,gset);
  }
//PrintS("This is the solution\n");listsprint(solve);
  return solve;
}

/********************************************************************/
/************************links***********************************/

//returns the links of face a in simplicial complex X
static std::vector<std::vector<int> > links(poly a, ideal h)
{
  int i;
  std::vector<std::vector<int> > lk,X=supports(h);
  std::vector<int> U,In,av=support1(a);
  for(i=0;i<X.size();i++)
  {
    U=vecUnion(av,X[i]);
    In=vecIntersection(av,X[i]);
    if( In.size()==0 && vInvsl(U,X))
    {
      //PrintS("The union of them is FACE and intersection is EMPTY!\n");
      lk.push_back(X[i]);
    }
    else
    {
      ;
    }
  }
  return lk;
}

static int redefinedeg(poly p, int  num)
{
  int deg=0, deg0;
  for(int i=1;i<=currRing->N;i++)
  {
    deg0=pGetExp(p, i);
    if(i>num)
    {
      deg= deg+2*deg0;
    }
    else
    {
      deg=deg+deg0;
    }
  }
  //Print("the new degree is: %d\n", deg);
  return (deg);
}

// the degree of variables should be same
static ideal p_a(ideal h)
{
  poly p;
  int i,j,deg=0,deg0;
  ideal aset=idCopy(h),ia,h1=idsrRing(h);
//PrintS("idsrRing is:\n");id_print(h1);
  std::vector<int> as;
  std::vector<std::vector<int> > hvs=supports(h);
  for(i=0;i<IDELEMS(h1);i++)
  {
    deg0=pTotaldegree(h1->m[i]);
    if(deg < deg0)
      deg=deg0;
  }
  for(i=2;i<=deg;i++)
  {
    ia=id_MaxIdeal(i, currRing);
    for(j=0;j<IDELEMS(ia);j++)
    {
      p=pCopy(ia->m[j]);
      if(!IsInX(p,h))
      {
        as=support1(p);
        if(vInvsl(as,hvs))
        {
          idInsertPoly(aset, p);
        }
      }
    }
  }
  idSkipZeroes(aset);
  return(aset);
}

/*only for the exampels whose variables has degree more than 1*/
/*ideal p_a(ideal h)
{
  poly e=pOne(), p;
  int i,j,deg=0,deg0, ord=4;
  ideal aset=idCopy(h),ia,h1=idsrRing(h);
//PrintS("idsrRing is:\n");id_print(h1);
  std::vector<int> as;
  std::vector<std::vector<int> > hvs=supports(h);
  for(i=0;i<IDELEMS(h1);i++)
  {
    deg0=redefinedeg(h1->m[i],ord);
    if(deg < deg0)
      deg=deg0;
  }
  for(i=2;i<=deg;i++)
  {
    ia=id_MaxIdeal(i, currRing);
    for(j=0;j<IDELEMS(ia);j++)
    {
      p=pCopy(ia->m[j]);
      if(!IsInX(p,h))
      {
        as=support1(p);
        if(vInvsl(as,hvs))
        {
          idInsertPoly(aset, p);
        }
      }
    }
  }
  idSkipZeroes(aset);
  return(aset);
}*/

static std::vector<int> vertset(std::vector<std::vector<int> > vecs)
{
  int i,j;
  std::vector<int> vert;
  std::vector<std::vector<int> > vvs;
  for(i=1;i<=currRing->N;i++)
  {
    for(j=0;j<vecs.size();j++)
    {
      if(IsinL(i, vecs[j]))
      {
        if(!IsinL(i , vert))
        {
          vert.push_back(i);
        }
        break;
      }
    }
  }
  return (vert);
}

//smarter way
static ideal p_b(ideal h, poly a)
{
  std::vector<std::vector<int> > pbv,lk=links(a,h), res;
  std::vector<int> vert=vertset(lk), bv;
  res=b_subsets(vert);
  int i, adg=pTotaldegree(a);
  poly e=pOne();
  ideal idd=idInit(1,1);
  for(i=0;i<res.size();i++)
  {
    if(res[i].size()==adg)
      pbv.push_back(res[i]);
  }
  if(pEqualPolys(a,e))
  {
    idInsertPoly(idd, e);
    idSkipZeroes(idd);
    return (idd);
  }
  idd=idMaken(pbv);
  return(idd);
}

/*//dump way to get pb
// the degree of variables should be same
static ideal p_b(ideal h, poly a)
{
  std::vector<std::vector<int> > pbv,lk=links(a,h),res;
// PrintS("Its links are :\n");id_print(idMaken(lk));
  res=id_subsets(lk);
  //PrintS("res is :\n");listsprint(res);
  std::vector<int> bv;
  ideal bset=findb(h);
  int i,j,nu=res.size(),adg=pTotaldegree(a);
  poly e=pOne();ideal idd=idInit(1,1);
  for(i=0;i<res.size();i++)
  {
    if(res[i].size()==adg)
      pbv.push_back(res[i]);
  }
  if(pEqualPolys(a,e)){idInsertPoly(idd, e); idSkipZeroes(idd); return (idd);}
  for(i=0;i<nu;i++)
  {
    for(j=i+1;j<nu;j++)
    {
      if(res[i].size()!=0 && res[j].size()!=0)
      {
        bv = vecUnion(res[i], res[j]);
        if(IsInX(pMaken(bv),bset)  && bv.size()==adg && !vInvsl(bv,pbv))
          {pbv.push_back(bv);}
      }
    }
  }
  idd=idMaken(pbv);
  //id_print(idd);
  return(idd);
}*/

// also only for the examples whose variables have degree more than 1(ndegreeb and p_b)
/*int ndegreeb(std::vector<int> vec, int num)
{
  int deg, deg0=0;
  for(int i=0;i<vec.size();i++)
  {
    if(vec[i]>num)
    {
      deg0++;
    }
  }
  deg=vec.size()+deg0;
  return(deg);
}

static ideal p_b(ideal h, poly a)
{
  std::vector<std::vector<int> > pbv,lk=links(a,h),res;
// PrintS("Its links are :\n");id_print(idMaken(lk));
  res=id_subsets(lk);
  //PrintS("res is :\n");listsprint(res);
  std::vector<int> bv;
  ideal bset=findb(h);
  int i,j,nu=res.size(),ord=4,adg=redefinedeg(a, ord);
  poly e=pOne();ideal idd=idInit(1,1);
  for(i=0;i<res.size();i++)
  {
    if(ndegreeb(res[i],ord)==adg)
      pbv.push_back(res[i]);
  }
  if(pEqualPolys(a,e)){idInsertPoly(idd, e); idSkipZeroes(idd); return (idd);}
  for(i=0;i<nu;i++)
  {
    for(j=i+1;j<nu;j++)
    {
      if(res[i].size()!=0 && res[j].size()!=0)
      {
        bv = vecUnion(res[i], res[j]);
  //PrintS("bv is :\n");listprint(bv);
 //Print("bv's degree is : %d\n", ndegreeb(bv,ord));
        if(IsInX(pMaken(bv),bset)  && ndegreeb(bv,ord)==adg && !vInvsl(bv,pbv))
        {
          pbv.push_back(bv);
        }
      }
    }
  }
  idd=idMaken(pbv);
  //id_print(idd);
  return(idd);
}*/

//input is a squarefree monomial p
//output is all the squarefree monomials which could divid p(including p itself?)
static ideal psubset(poly p)
{
  int i,j,max=pTotaldegree(p);
  ideal h1,mons, id_re=idInit(1,1);
  for(i=1;i<max;i++)
  {
    mons=id_MaxIdeal(i, currRing);
    h1=sfreemon(mons,i);
    for(j=0;j<IDELEMS(h1);j++)
    {
      if(p_DivisibleBy(h1->m[j],p,currRing))
        idInsertPoly(id_re, h1->m[j]);
    }
  }
  idSkipZeroes(id_re);
  //PrintS("This is the facset\n");
  //id_print(id_re);
  return id_re;
}

//inserts a new vector which has two elements a and b into vector gset (which is a vector of vectors)
//(especially for gradedpiece1 and gradedpiece1n)
static std::vector<std::vector<int> > listsinsertlist(std::vector<std::vector<int> > gset, int a, int b)
{
  std::vector<int> eq;
  eq.push_back(a);
  eq.push_back(b);
  gset.push_back(eq);
  return gset;
}

static std::vector<int> makeequation(int i,int j, int t)
{
  std::vector<int> equation;
  equation.push_back(i);
  equation.push_back(j);
  equation.push_back(t);
  //listprint(equation);
  return equation;
}

/****************************************************************/
//only for solving the equations obtained from T^2
//input should be a vector which has only 3 entries
static poly pMake3(std::vector<int> vbase)
{
  int co=1;
  poly p,q=0;
  for(int i=0;i<3;i++)
  {
    if(vbase[i]!=0)
    {
      if(i==1) co=-1;
      p = pOne();pSetExp(p, vbase[i], 1);pSetm(p);pSetCoeff(p, nInit(co));
    }
    else p=0;
      q = pAdd(q, p);
    co=1;
  }
  return q;
}

static ideal idMake3(std::vector<std::vector<int> > vecs)
{
  ideal id_re=idInit(1,1);
  poly p;
  int i,lv=vecs.size();
  for(i=0;i<lv;i++)
  {
    p=pMake3(vecs[i]);
    idInsertPoly(id_re, p);
  }
  idSkipZeroes(id_re);
  return id_re;
}

/****************************************************************/

//change the current ring to a new ring which is in num new variables
static void equmab(int num)
{
  int i;
  //Print("There are %d new variables for equations solving.\n",num);
  ring r=currRing;
  char** tt;
  coeffs cf=nCopyCoeff(r->cf);
  tt=(char**)omAlloc(num*sizeof(char *));
  for(i=0; i <num; i++)
  {
    tt[i] = (char*)omalloc(10); //if required enlarge it later
    sprintf (tt[i], "t(%d)", i+1);
    tt[i]=omStrDup(tt[i]);
  }
  ring R=rDefault(cf,num,tt,ringorder_lp);
  idhdl h=enterid(omStrDup("Re"),0,RING_CMD,&IDROOT,FALSE);
  IDRING(h)=rCopy(R);
  rSetHdl(h);
}

//returns the trivial case of T^1
//b must only contain one variable
static std::vector<int> subspace1(std::vector<std::vector<int> > mv, std::vector<int> bv)
{
  int i, num=mv.size();
  std::vector<int> base;
  for(i=0;i<num;i++)
  {
    if(IsinL(bv[0],mv[i]))
      base.push_back(1);
    else
      base.push_back(0);
  }
  return base;
}

/***************************only for T^2*************************************/
//vbase only has two elements which records the position of the monomials in mv

static std::vector<poly> pMakei(std::vector<std::vector<int> > mv,std::vector<int> vbase)
{
  poly p;
  std::vector<poly> h1;
  int n=vbase.size();
  for(int i=0;i<n;i++)
  {
    p=pMaken(mv[vbase[i]]);
    h1.push_back(p);
  }
  return h1;
}

// returns a ideal according to a set of supports
static std::vector<std::vector<poly> > idMakei(std::vector<std::vector<int> > mv,std::vector<std::vector<int> > vecs)
{
  int i,lv=vecs.size();
  std::vector<std::vector<poly> > re;
  std::vector<poly> h;
  for(i=0;i<lv;i++)
  {
    h=pMakei(mv,vecs[i]);
    re.push_back(h);
  }
  //PrintS("This is the metrix M:\n");
  //listsprint(vecs);
  //PrintS("the ideal according to metrix M is:\n");
  return re;
}

/****************************************************************/

//return the graded pieces of cohomology T^1 according to a,b
//original method (only for debugging)
static void gradedpiece1(ideal h,poly a,poly b)
{
  int i,j,m;
  ideal sub=psubset(b);
  std::vector<int> av=support1(a), bv=support1(b), bad, vv;
  std::vector<std::vector<int> > hvs=supports(h), sbv=supports(sub), mv=Mabv(h,a,b),good;
  m=mv.size();
  ring r=currRing;
  if( m > 0 )
  {
    for(i=0;i<m;i++)
    {
      if(!vsubset(bv,mv[i]))
      {
        bad.push_back(i+1);
      }
    }
    for(i=0;i<m;i++)
    {
      for(j=i+1;j<m;j++)
      {
        vv=vecUnion(mv[i],mv[j]);
        if(mabconditionv(hvs,vv,av,bv))
        {
          good=listsinsertlist(good,i+1,j+1);
        }
        else
        {
          //PrintS("They are not in Mabt!\n");
          ;
        }
      }
    }
    std::vector<std::vector<int> > solve=eli2(m,bad,good);
    if(bv.size()!=1)
    {
      //PrintS("This is the solution of coefficients:\n");
      listsprint(solve);
    }
     else
    {
      std::vector<int> su=subspace1(mv,bv);
      //PrintS("This is the solution of subspace:\n");
      //listprint(su);
      std::vector<std::vector<int> > suu;
      suu.push_back(su);
      equmab(solve[0].size());
      std::vector<std::vector<int> > solves=vecqring(solve,suu);
      //PrintS("This is the solution of coefficients:\n");
      listsprint(solves);
      rChangeCurrRing(r);
    }
  }
  else
  {
    PrintS("No element considered!\n");
  }
}

//Returns true if b can divide p*q
static bool condition1for2(std::vector<int > pv,std::vector<int > qv,std::vector<int > bv)
{
  std::vector<int > vec=vecUnion(pv,qv);
  if(vsubset(bv,vec))
  {
    //PrintS("condition1for2 yes\n");
    return true;
  }
  //PrintS("condition1for2 no\n");
  return false;
}

//Returns true if support(p) union support(q) union support(s) union support(a) minus support(b) is face
static bool condition2for2(std::vector<std::vector<int> > hvs, std::vector<int> pv,  std::vector<int> qv, std::vector<int> sv, std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> vec=vecUnion(pv,qv);
  vec=vecUnion(vec,sv);
  if(mabconditionv(hvs,vec,av,bv))
  {
    //PrintS("condition2for2 yes\n");
    return (true);
  }
  //PrintS("condition2for2 no\n");
  return (false);
}

static bool condition3for2(std::vector<std::vector<int> > hvs, std::vector<int> pv,  std::vector<int> qv,  std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> v1,v2,v3;
  v1=vecIntersection(pv,qv);//intersection
  v2=vecUnion(pv,qv);
  v2=vecUnion(v2,av);
  v2=vecMinus(v2,bv);
  v3=vecUnion(v1,v2);
  if(vInvsl(v3,hvs))
  {
    //PrintS("condition3for2 yes\n");
    return(true);
  }
  //PrintS("condition3for2 no\n");
  return(false);
}

/****************solve the equations got from T^2*********************/

static ideal getpresolve(ideal h)
{
  //ring r=currRing;
  //assume (LIB "presolve.lib");
  sleftv a;a.Init();
  a.rtyp=IDEAL_CMD;a.data=(void*)h;
  idhdl solve=ggetid("elimlinearpart");
  if(solve==NULL)
  {
    WerrorS("presolve.lib are not loaded!");
    return NULL;
  }
  BOOLEAN sl=iiMake_proc(solve,NULL,&a);
  //PrintS("no errors here\n");
  if(sl)
  {
    WerrorS("error in solve!");
  }
  lists L=(lists) iiRETURNEXPR.Data();
  ideal re=(ideal)L->m[4].CopyD();
  //iiRETURNEXPR.CleanUp();
  iiRETURNEXPR.Init();
  //PrintS("no errors here\n");
  //idSkipZeroes(re);
  //id_print(re);
  return re;
}

static std::vector<int> numfree(ideal h)
{
  int i,j;
  std::vector<int> fvar;
  for(j=1;j<=currRing->N;j++)
  {
    for(i=0;i<IDELEMS(h);i++)
    {
      if(vInp(j,h->m[i]))
      {
        fvar.push_back(j);
        break;
      }
    }
  }
  //Print("There are %d free variables in total\n",num);
  return fvar;
}

static std::vector<std::vector<int> > canonicalbase(int n)
{
  std::vector<std::vector<int> > vecs;
  std::vector<int> vec;
  int i,j;
  for(i=0;i<n;i++)
  {
    for(j=0;j<n;j++)
    {
      if(i==j)
        vec.push_back(1);
      else
        vec.push_back(0);
    }
    vecs.push_back(vec);
    vec.clear();
  }
  return vecs;
}

static std::vector<std::vector<int> > getvector(ideal h,int n)
{
  std::vector<int> vec;
  std::vector<std::vector<int> > vecs;
  ideal h2=idCopy(h);
  if(!idIs0(h))
  {
    ideal h1=getpresolve(h2);
    poly q,e=pOne();
    int lg=IDELEMS(h1),n,i,j,t;
    std::vector<int> fvar=numfree(h1);
    n=fvar.size();
    if(n==0)
    {
      vec=make0(IDELEMS(h1));vecs.push_back(vec);//listsprint(vecs);
    }
    else
    {
      for(t=0;t<n;t++)
      {
        vec.clear();
        for(i=0;i<lg;i++)
        {
          q=pCopy(h1->m[i]);
          //pWrite(q);
          if(q==0)
          {
            vec.push_back(0);
          }
          else
          {
            q=p_Subst(q, fvar[t], e,currRing);
            //Print("the %dth variable was substituted by 1:\n",fvar[t]);
            //pWrite(q);
            for(j=0;j<n;j++)
            {
              //Print("the %dth variable was substituted by 0:\n",fvar[j]);
              q=p_Subst(q, fvar[j],0,currRing);
              //pWrite(q);
            }
            if(q==0)
            {
              vec.push_back(0);
            }
            else
            {
              vec.push_back(n_Int(pGetCoeff(q),currRing->cf));
            }
          }
        }
        //listprint(vec);
        vecs.push_back(vec);
      }
    }
  }
  else
  {vecs=canonicalbase(n);}
  //listsprint(vecs);
  return vecs;
}

/**************************************************************************/

//subspace of T2(find all the possible values of alpha)
static std::vector<int> findalpha(std::vector<std::vector<int> > mv, std::vector<int> bv)
{
  std::vector<int> alset;
  for(unsigned i=0;i<mv.size();i++)
  {
    if(vsubset(bv,mv[i]))
    {
      alset.push_back(i);
    }
  }
  //Print("This is the alpha set, and the subspace is dim-%ld\n",alset.size());
  //listprint(alset);
  return alset;
}

static std::vector<int> subspacet1(int num, std::vector<std::vector<int> > ntvs)
{
  int i, j, t, n=ntvs.size();
  std::vector<int> subase;
  for(t=0;t<n;t++)
  {
    i=ntvs[t][0];
    j=ntvs[t][1];
    if(i==(num))
    {
      subase.push_back(1);
    }
    else if(j==num)
    {
      subase.push_back(-1);
    }
    else
    {
      subase.push_back(0);
    }
  }
  //Print("This is the basis w.r.t. %dth polynomial in alpha set\n",num);
  //listprint(subase);
  return subase;
}

//subspace for T^2(mab method)
static std::vector<std::vector<int> > subspacet(std::vector<std::vector<int> > mv, std::vector<int> bv,std::vector<std::vector<int> > ntvs)
{
  std::vector<int> alset=findalpha(mv,bv), subase;
  std::vector<std::vector<int> > subases;
  for(unsigned i=0;i<alset.size();i++)
  {
    subase=subspacet1(alset[i],ntvs);
    subases.push_back(subase);
  }
  //PrintS("These are the bases for the subspace:\n");
  //listsprint(subases);
  return subases;
}

static std::vector<std::vector<int> > mabtv(std::vector<std::vector<int> > hvs,  std::vector<std::vector<int> > Mv,   std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> v1,var;
  std::vector<std::vector<int> > vars;
  for(unsigned i=0;i<Mv.size();i++)
  {
    for(unsigned j=i+1;j<Mv.size();j++)
    {
      var.clear();
      v1=vecUnion(Mv[i],Mv[j]);
      if(mabconditionv(hvs, v1, av, bv))
      {
        var.push_back(i);
        var.push_back(j);
        vars.push_back(var);
      }
    }
  }
  return vars;
}

//fix the problem of the number of the new variables
//original method for T^2(only for debugging)
static void gradedpiece2(ideal h,poly a,poly b)
{
  int t0,t1,t2,i,j,t,m;
  ideal sub=psubset(b);
  ring r=rCopy(currRing);
  std::vector<std::vector<int> > hvs=supports(h), mv=Mabv(h,a,b), mts, vecs,vars;
  std::vector<int> av=support1(a), bv=support1(b), vec,var;
  mts=mabtv(hvs,mv,av,bv);
  PrintS("The homomorphism should map onto:\n");
  lpsprint(idMakei(mv,mts));
  m=mv.size();
  if(m > 0)
  {
    vars=mabtv(hvs,mv,av,bv);
    int vn=vars.size();
    for(t0=0;t0<vars.size();t0++)
    {
      i=vars[t0][0];
      j=vars[t0][1];
      if(!condition1for2(mv[i],mv[j],bv))//condition 1
      {
        //PrintS("And they satisfy the condition 1.\n");
        vec=makeequation(t0+1,0,0);
        //PrintS("So the equation:\n");
        //pWrite(p);
        //PrintS("holds.\n");
        vecs.push_back(vec);
        vec.clear();
      }
      if(condition3for2(hvs,mv[i],mv[j],av,bv))//condition 3
      {
        //PrintS("And they satisfy the condition 3.\n");
        vec=makeequation(t0+1,0,0);
        //PrintS("So the equation: \n");
        //pWrite(p);
        //PrintS("holds.\n");
        vecs.push_back(vec);
        vec.clear();
      }
      for(t1=t0+1;t1<vars.size();t1++)
      {
        for(t2=t1+1;t2<vars.size();t2++)
        {
          if(vars[t0][0]==vars[t1][0]&&vars[t1][1]==vars[t2][1]&&vars[t0][1]==vars[t2][0])
          {
            i=vars[t0][0];
            j=vars[t0][1];
            t=vars[t1][1];
            if(condition2for2(hvs,mv[i],mv[j],mv[t],av,bv))//condition 2
            {
              vec=makeequation(t0+1,t1+1,t2+1);
              vecs.push_back(vec);
              vec.clear();
            }
          }
        }
      }
    }
    //PrintS("this is EQUATIONS:\n");
    //listsprint(vecs);
    equmab(vn);
    ideal id_re=idMake3(vecs);
    //id_print(id_re);
    std::vector<std::vector<int> > re=getvector(id_re,vn);
    PrintS("this is the solution for ideal :\n");
    listsprint(re);
    rChangeCurrRing(r);
    std::vector<std::vector<int> > sub=subspacet(mv, bv,vars);
    PrintS("this is the solution for subspace:\n");
    listsprint(sub);
    equmab(vn);
    std::vector<std::vector<int> > solve=vecqring(re, sub);
    PrintS("This is the solution of coefficients:\n");
    listsprint(solve);
    rChangeCurrRing(r);
  }
  else
  {
    PrintS("No element considered!");
  }
}

/**********************************************************************/
//For the method of N_{a-b}

//returns true if pv(support of monomial) satisfies pv union av minus bv is in hvs
static bool nabconditionv(std::vector<std::vector<int> > hvs, std::vector<int> pv,  std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> vec1=vecIntersection(pv,bv), vec2=vecUnion(pv,bv);
  int s1=vec1.size();
  if(!vInvsl(vec2,hvs) && s1==0 && vsubset(av,pv))
  {
    //PrintS("nab condition satisfied\n");
    return(true);
  }
  //PrintS("nab condition not satisfied\n");
  return(false);
}

//returns N_{a-b}
static std::vector<std::vector<int> > Nabv(std::vector<std::vector<int> > hvs,  std::vector<int> av,  std::vector<int> bv)
{
  std::vector<std::vector<int> > vecs;
  int num=hvs.size();
  for(int i=0;i<num;i++)
  {
    if(nabconditionv(hvs,hvs[i],av,bv))
    {
      //PrintS("satisfy:\n");
      vecs.push_back(hvs[i]);
    }
  }
  return vecs;
}

//returns true if pv union qv union av minus bv is in hvs
//hvs is simplicial complex
static bool nabtconditionv(std::vector<std::vector<int> > hvs,  std::vector<int> pv, std::vector<int> qv)
{
  std::vector<int> v1;
  v1=vecUnion(pv,qv);
  if(vInvsl(v1,hvs))
  {
    return (true);
  }
  return (false);
}

//returns N_{a-b}^(2)
static std::vector<std::vector<int> > nabtv(std::vector<std::vector<int> > hvs,    std::vector<std::vector<int> > Nv,   std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> v1,var;
  std::vector<std::vector<int> > vars;
  for(unsigned i=0;i<Nv.size();i++)
  {
    for(unsigned j=i+1;j<Nv.size();j++)
    {
      var.clear();
      if(nabtconditionv(hvs, Nv[i], Nv[j]))
      {
        var.push_back(i);
        var.push_back(j);
        vars.push_back(var);
      }
    }
  }
  return vars;
}

//p must be the monomial which is a face
//  ideal sub=psubset(b); bvs=supports(sub);
static bool tNab(std::vector<std::vector<int> > hvs, std::vector<int> pv, std::vector<std::vector<int> > bvs)
{
  std::vector<int> sv;
  if(bvs.size()<=1) return false;
  for(unsigned i=0;i<bvs.size();i++)
  {
    sv=vecUnion(pv,bvs[i]);
    if(!vInvsl(sv,hvs))
    {
      return true;
    }
  }
  return false;
}

static std::vector<int>  tnab(std::vector<std::vector<int> > hvs,std::vector<std::vector<int> > nvs,std::vector<std::vector<int> > bvs)
{
  std::vector<int> pv, vec;
  for(unsigned j=0;j<nvs.size();j++)
  {
    pv=nvs[j];
    if(tNab(hvs, pv, bvs))
    {
      vec.push_back(j);
    }
  }
  return vec;
}

//the image phi(pv)=pv union av minus bv
static std::vector<int> phimage(std::vector<int> pv,  std::vector<int> av, std::vector<int> bv)
{
  std::vector<int> qv=vecUnion(pv,av);
  qv=vecMinus(qv,bv);
  return qv;
}

//mvs and nvs are the supports of ideal Mab and Nab
//vecs is the solution of nab
static std::vector<std::vector<int> > value1(std::vector<std::vector<int> > mvs, std::vector<std::vector<int> > nvs, std::vector<std::vector<int> > vecs,std::vector<int> av, std::vector<int> bv)
{
  int j;
  std::vector<int> pv, base;
  std::vector<std::vector<int> > bases;
  for(unsigned t=0;t<vecs.size();t++)
  {
    for(unsigned i=0;i<mvs.size();i++)
    {
      pv=phimage(mvs[i],av,bv);
      for( j=0;j<nvs.size();j++)
      {
        if(vEvl(pv,nvs[j]))
        {
          base.push_back(vecs[t][j]);
          break;
        }
      }
      if(j==nvs.size())
      {
        base.push_back(0);
      }
    }
    if(base.size()!=mvs.size())
    {
      //WerrorS("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");
      WerrorS("Errors in Equations solving (Values Finding)!");
      usleep(1000000);
      assert(false);

    }
    bases.push_back(base);
    base.clear();
  }
  return bases;
}

static intvec *Tmat(std::vector<std::vector<int> > vecs)
{
    //std::vector<std::vector<int> > solve=gradedpiece1n(h,a,b);
   //Print("the size of solve is: %ld\n",solve.size());
 //vtm(solve);
  intvec *m;
  int i,j, a=vecs.size();
  if(a==0)
  {
    m=new intvec(1,1,10);
  }
  else
  {
    int b=vecs[0].size();
    m=new intvec(a,b,0);
    for(i=1;i<=a;i++)
    {
      for(j=1;j<=b;j++)
      {
        IMATELEM(*m,i,j)=vecs[i-1][j-1];
      }
    }
  }
  return (m);
}

//returns the set of position number of minimal gens in M
static std::vector<int> gensindex(ideal M, ideal ids)
{
  int i;
  std::vector<int> vec,index;
  if(!idIs0(M))
  {
    std::vector<std::vector<int> > vecs=supports(ids);
    for(i=0;i<IDELEMS(M);i++)
    {
      vec=support1(M->m[i]);
      if(vInvsl(vec,vecs))
        index.push_back(i);
    }
  }
  return (index);
}

static ideal mingens(ideal h, poly a, poly b)
{
  int i;
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  ideal M=idMaken(mv), hi=idInit(1,1);
  std::vector<int> index = gensindex(M, idsrRing(h));
  for(i=0;i<index.size();i++)
  {
    idInsertPoly(hi,M->m[index[i]]);
  }
  idSkipZeroes(hi);
  return (hi);
}

static std::vector<std::vector<int> >  minisolve(std::vector<std::vector<int> > solve,  std::vector<int> index)
{
  int i,j;
  std::vector<int> vec,solm;
  std::vector<std::vector<int> > solsm;
  for(i=0;i<solve.size();i++)
  {
    vec=solve[i];
    for(j=0;j<vec.size();j++)
    {
      if(IsinL(j,index))
        solm.push_back(vec[j]);
    }
    solsm.push_back(solm);
    solm.clear();
  }
  return (solsm);
}

//T_1 graded piece(N method)
//frame of the most efficient version
//regardless of links
static intvec * gradedpiece1n(ideal h,poly a,poly b)
{
  int i,j,co,n;
  std::vector<std::vector<int> > hvs=supports(h),mv=Mabv(h,a,b),sbv,nv,good,solve;
  std::vector<int> av=support1(a), bv=support1(b), bad, tnv, index;
  ideal sub=psubset(b),M;
  sbv=supports(sub);
  nv=Nabv(hvs,av,bv);
  M=idMaken(mv);
  index = gensindex(M, idsrRing(h));
  n=nv.size();
  ring r=currRing;
  if(n > 0)
  {
    tnv=tnab(hvs,nv,sbv);
    for(i=0;i<tnv.size();i++)
    {
      co=tnv[i];
      bad.push_back(co+1);
    }
    for(i=0;i<n;i++)
    {
      for(j=i+1;j<n;j++)
      {
        if(nabtconditionv(hvs,nv[i],nv[j]))
        {
          good=listsinsertlist(good,i+1,j+1);
        }
        else
        {
          ;
        }
      }
    }
    solve=eli2(n,bad,good);
    if(bv.size()!=1)
    {;
      //PrintS("This is the solution of coefficients:\n");
      //listsprint(solve);
    }
    else
    {
      std::vector<int> su=make1(n);
      std::vector<std::vector<int> > suu;
      suu.push_back(su);
      equmab(n);
      solve=vecqring(solve,suu);
      //PrintS("This is the solution of coefficients:\n");
      //listsprint(solve);
      rChangeCurrRing(r);
    }
    solve=value1(mv,nv,solve,av,bv);
  }
  else
  {
    //PrintS("No element considered here!\n");
    solve.clear();
  }
  //PrintS("This is the solution of final coefficients:\n");
  //listsprint(solve);
  solve=minisolve(solve,index);
  intvec *sl=Tmat(solve);
  //sl->show(0,0);
  return sl;
}

//for debugging
static void T1(ideal h)
{
  ideal bi=findb(h),ai;
  int mm=0;
  id_print(bi);
  poly a,b;
  std::vector<std::vector<int> > solve;
  for(int i=0;i<IDELEMS(bi);i++)
  {
    //PrintS("This is aset according to:");
    b=pCopy(bi->m[i]);
    pWrite(b);
    ai=finda(h,b,0);
    if(!idIs0(ai))
    {
      id_print(ai);
      for(int j=0;j<IDELEMS(ai);j++)
      {
        //PrintS("This is a:");
        a=pCopy(ai->m[j]);
        //pWrite(a);
        intvec * solve=gradedpiece1n(h, a, b);
        if (IMATELEM(*solve,1,1)!=10)
           mm++;
      }
    }
  }
  Print("Finished %d!\n",mm);
}

static bool condition2for2nv(std::vector<std::vector<int> > hvs, std::vector<int> pv, std::vector<int> qv,  std::vector<int> fv)
{
  std::vector<int> vec=vecUnion(pv,qv);
  vec=vecUnion(vec,fv);
  if(vInvsl(vec,hvs))
  {
    //PrintS("condition2for2 yes\n");
    return (true);
  }
  //PrintS("condition2for2 no\n");
  return (false);
}

//for subspace of T2(find all the possible values of alpha)
static std::vector<int> findalphan(std::vector<std::vector<int> >  N, std::vector<int>  tN)
{
  int i;std::vector<int> alset,vec;
  for(i=0;i<N.size();i++)
  {
   // vec=N[i];
    if(!IsinL(i,tN))
    {
      alset.push_back(i);
    }
  }
  //listprint(alset);
  return alset;
}

//subspace of T^2 (nab method)
static std::vector<std::vector<int> > subspacetn(std::vector<std::vector<int> >  N, std::vector<int>   tN, std::vector<std::vector<int> > ntvs)
{
  int i;
  std::vector<int> alset=findalphan(N,tN), subase;
  std::vector<std::vector<int> > subases;
  for(i=0;i<alset.size();i++)
  {
    subase=subspacet1(alset[i],ntvs);
    subases.push_back(subase);
  }
  //PrintS("These are the bases for the subspace:\n");
  //listsprint(subases);
  return subases;
}

//mts  Mabt
//nts  Nabt
//mvs Mab
//nvs Nab
static std::vector<std::vector<int> > value2(std::vector<std::vector<int> > mvs, std::vector<std::vector<int> > nvs, std::vector<std::vector<int> > mts, std::vector<std::vector<int> > nts, std::vector<std::vector<int> > vecs,std::vector<int> av,   std::vector<int> bv)
{
  int row,col,j;
  std::vector<int> pv,qv, base;
  std::vector<std::vector<int> > bases;
  //PrintS("This is the nabt:\n");
  //listsprint(nts);
  //PrintS("nabt ends:\n");
  //PrintS("This is the mabt:\n");
  //listsprint(mts);
  //PrintS("mabt ends:\n");
  for(unsigned t=0;t<vecs.size();t++)
  {
    for(unsigned i=0;i<mts.size();i++)
    {
      row=mts[i][0];
      col=mts[i][1];
      pv=phimage(mvs[row],av,bv);
      qv=phimage(mvs[col],av,bv);
      if(vEvl(pv,qv))
        base.push_back(0);
      else
      {
        for(j=0;j<nts.size();j++)
        {
          row=nts[j][0];
          col=nts[j][1];
          if(vEvl(pv,nvs[row])&&vEvl(qv,nvs[col]))
          {
            base.push_back(vecs[t][j]);break;
          }
          else if(vEvl(pv,nvs[col])&&vEvl(qv,nvs[row]))
          {
            base.push_back(-vecs[t][j]);break;
          }
        }
        if(j==nts.size()) {base.push_back(0);}
      }
    }
    if(base.size()!=mts.size())
    {
      WerrorS("Errors in Values Finding(value2)!");
       //WerrorS("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");
      usleep(1000000);
      assert(false);
    }
    bases.push_back(base);
    base.clear();
  }
  return bases;
}

static ideal genst(ideal h, poly a, poly b)
{
  std::vector<std::vector<int> > hvs=supports(h),mv,mts;
  std::vector<int> av=support1(a), bv=support1(b);
  mv=Mabv(h,a,b);
  mts=mabtv(hvs,mv,av,bv);
  std::vector<std::vector<poly> > pvs=idMakei(mv,mts);
  ideal gens=idInit(1,1);
  for(unsigned i=0;i<pvs.size();i++)
  {
    idInsertPoly(gens,pvs[i][0]);
    idInsertPoly(gens,pvs[i][1]);
  }
  idSkipZeroes(gens);
  return (gens);
}

static intvec * gradedpiece2n(ideal h,poly a,poly b)
{
  int i,j,t,n;
  std::vector<std::vector<int> > hvs=supports(h),nv,mv,mts,sbv,vecs,vars,ntvs,solve;
  std::vector<int> av=support1(a), bv=support1(b),tnv,vec,var;
  ideal sub=psubset(b);
  sbv=supports(sub);
  nv=Nabv(hvs,av,bv);
  n=nv.size();
  tnv=tnab(hvs,nv,sbv);
  ring r=currRing;
  mv=Mabv(h,a,b);
  mts=mabtv(hvs,mv,av,bv);
  //PrintS("The relations are:\n");
  //listsprint(mts);
  //PrintS("The homomorphism should map onto:\n");
  //lpsprint(idMakei(mv,mts));
  if(n>0)
  {
    ntvs=nabtv( hvs, nv, av, bv);
  //PrintS("The current homomorphism map onto###:\n");
  //lpsprint(idMakei(nv,ntvs));
    int l=ntvs.size();
    for(int t0=0;t0<l;t0++)
    {
      i=ntvs[t0][0];
      j=ntvs[t0][1];
      if(tNab(hvs,nv[i],sbv)&&tNab(hvs,nv[j],sbv))//condition 1
      {
        vec=makeequation(t0+1,0,0);
        vecs.push_back(vec);
        vec.clear();
      }
      for(int t1=t0+1;t1<ntvs.size();t1++)
      {
        for(int t2=t1+1;t2<ntvs.size();t2++)
        {
          if(ntvs[t0][0]==ntvs[t1][0]&&ntvs[t1][1]==ntvs[t2][1]&&ntvs[t0][1]==ntvs[t2][0])
          {
            i=ntvs[t0][0];
            j=ntvs[t0][1];
            t=ntvs[t1][1];
            if(condition2for2nv(hvs,nv[i],nv[j],nv[t]))
            {
              vec=makeequation(t0+1,t1+1,t2+1);
              vecs.push_back(vec);
              vec.clear();
            }
          }
        }
      }
    }
    //PrintS("this is EQUATIONS:\n");
    //listsprint(vecs);
    if(n==1) l=1;
    equmab(l);
    ideal id_re=idMake3(vecs);
    //id_print(id_re);
    std::vector<std::vector<int> > re=getvector(id_re,l);
    //PrintS("this is the solution for ideal :\n");
    //listsprint(re);
    rChangeCurrRing(r);
    std::vector<std::vector<int> > sub=subspacetn(nv, tnv,ntvs);
     //PrintS("this is the solution for subspace:\n");
    //listsprint(sub);
    equmab(l);
    solve=vecqring(re, sub);
    //PrintS("This is the solution of coefficients:\n");
    //listsprint(solve);
    rChangeCurrRing(r);
    solve=value2(mv,nv,mts,ntvs,solve,av,bv);
  }
  else
    solve.clear();
  intvec *sl=Tmat(solve);
  return sl;
}

//for debugging
static void T2(ideal h)
{
  ideal bi=findb(h),ai;
  id_print(bi);
  poly a,b;
  int mm=0,gp=0;
  std::vector<int> bv,av;
  std::vector<std::vector<int> > solve;
  for(int i=0;i<IDELEMS(bi);i++)
  {
    b=pCopy(bi->m[i]);
    //bv=support1(b);
    //PrintS("This is aset according to:");
    pWrite(b);
//if(bv.size()==2)
  //{
    ai=finda(h,b,0);
    if(!idIs0(ai))
    {
      PrintS("This is a set according to current b:\n");
      id_print(ai);
      for(int j=0;j<IDELEMS(ai);j++)
      {
        PrintS("This is a:");
        a=pCopy(ai->m[j]);
        pWrite(a);
        PrintS("This is b:");
        pWrite(b);
        intvec *solve=gradedpiece2n(h, a, b);
        delete solve;
        gp++;
      }
    }
    mm=mm+1;
  }
  if(mm==IDELEMS(bi))
      PrintS("Finished!\n");
  Print("There are %d graded pieces in total.\n",gp);
}

/*****************************for links*******************************************/
//the image phi(pv)=pv minus av minus bv
static std::vector<int> phimagel(std::vector<int> fv,  std::vector<int> av, std::vector<int> bv)
{
  std::vector<int> nv;
  nv=vecMinus(fv,bv);
  nv=vecMinus(nv,av);
  return nv;
}

//mvs and nvs are the supports of ideal Mab and Nab
//vecs is the solution of nab
static std::vector<std::vector<int> > value1l(std::vector<std::vector<int> > mvs, std::vector<std::vector<int> > lks, std::vector<std::vector<int> > vecs,std::vector<int> av, std::vector<int> bv)
{
  int j;
  std::vector<int> pv;
  std::vector<int> base;
  std::vector<std::vector<int> > bases;
  for(unsigned t=0;t<vecs.size();t++)
  {
    for(unsigned i=0;i<mvs.size();i++)
    {
      pv=phimagel(mvs[i], av, bv);
      for(j=0;j<lks.size();j++)
      {
        if(vEvl(pv,lks[j]))
        {
          base.push_back(vecs[t][j]);break;
        }
      }
      //if(j==lks.size()) {base.push_back(0);}
    }
    if(base.size()!=mvs.size())
    {
      WerrorS("Errors in Values Finding(value1l)!");
      usleep(1000000);
      assert(false);
    }
    bases.push_back(base);
    base.clear();
  }
  return bases;
}

/***************************************************/
VAR clock_t t_begin, t_mark, t_start, t_construct=0, t_solve=0, t_value=0, t_total=0;
/**************************************************/

static void TimeShow(clock_t t_construct, clock_t t_solve, clock_t t_value ,clock_t t_total)
{
  Print("The time of value matching for first order deformation:   %.2f sec ;\n", ((double) t_value)/CLOCKS_PER_SEC);
  Print("The total time of fpiece:  %.2f sec ;\n", ((double) t_total)/CLOCKS_PER_SEC);
  Print("The time of equations construction for fpiece:   %.2f sec ;\n", ((double) t_construct)/CLOCKS_PER_SEC);
  Print("The total time of equations solving for fpiece:  %.2f sec ;\n", ((double) t_solve)/CLOCKS_PER_SEC);
  PrintS("__________________________________________________________\n");
}

static std::vector<std::vector<int> > gpl(ideal h,poly a,poly b)
{
  int i,j,co;
  std::vector<std::vector<int> > hvs=supports(h),sbv,nv,mv,good,solve;
  std::vector<int> av=support1(a), bv=support1(b),index,bad,tnv;
  ideal sub=psubset(b);
  sbv=supports(sub);
  nv=Nabv(hvs,av,bv);
  mv=Mabv(h,a,b);
  ideal M=idMaken(mv);
  index = gensindex(M, idsrRing(h));
  int n=nv.size();
  ring r=currRing;
  t_begin=clock();
  if(n > 0)
  {
    tnv=tnab(hvs,nv,sbv);
    for(i=0;i<tnv.size();i++)
    {
      co=tnv[i];
      bad.push_back(co+1);
    }
    for(i=0;i<n;i++)
    {
      for(j=i+1;j<n;j++)
      {
        if(nabtconditionv(hvs,nv[i],nv[j]))
        {
          good=listsinsertlist(good,i+1,j+1);
        }
        else
        {
          ;
        }
      }
    }
    t_construct=t_construct+clock()-t_begin;
    t_begin=clock();
    solve=eli2(n,bad,good);
    t_solve=t_solve+clock()-t_begin;
    if(bv.size()!=1)
    {;
    }
    else
    {
      std::vector<int> su=make1(n);
      std::vector<std::vector<int> > suu;
      suu.push_back(su);
      equmab(n);
      solve=vecqring(solve,suu);
      rChangeCurrRing(r);
    }
  }
  else
  {
    solve.clear();
  }
  //listsprint(solve);
  //sl->show(0,0);
  return solve;
}

//T^1
//only need to consider the links of a, and reduce a to empty set
static intvec * gradedpiece1nl(ideal h,poly a,poly b, int set)
{
  t_start=clock();
  poly e=pOne();
  std::vector<int> av=support1(a),bv=support1(b),index, em;
  std::vector<std::vector<int> > solve, hvs=supports(h), lks=links(a,h),  mv=Mabv(h,a,b), nvl;
  ideal id_links=idMaken(lks);
  ideal M=idMaken(mv);
  index = gensindex(M, idsrRing(h));
  solve=gpl(id_links,e,b);
  t_mark=clock();
  nvl=Nabv(lks,em,bv);
  solve=value1l(mv, nvl , solve, av, bv);
  if(set==1)
  {
    solve=minisolve(solve,index);
  }
  intvec *sl=Tmat(solve);
  t_value=t_value+clock()-t_mark;
  t_total=t_total+clock()-t_start;
  return sl;
}

//for finding values of T^2
static std::vector<std::vector<int> > value2l(std::vector<std::vector<int> > mvs, std::vector<std::vector<int> > lks, std::vector<std::vector<int> > mts, std::vector<std::vector<int> > lkts, std::vector<std::vector<int> > vecs,std::vector<int> av,   std::vector<int> bv)
{
  std::vector<int> pv,qv,base;
  int row,col,j;
  std::vector<std::vector<int> > bases;
  if(vecs.size()==0)
  {

  }
  for(unsigned t=0;t<vecs.size();t++)
  {
    for(unsigned i=0;i<mts.size();i++)
    {
      row=mts[i][0];
      col=mts[i][1];
      pv=phimagel(mvs[row],av,bv);
      qv=phimagel(mvs[col],av,bv);
      if(vEvl(pv,qv))
        base.push_back(0);
      else
      {
        for(j=0;j<lkts.size();j++)
        {
          row=lkts[j][0];
          col=lkts[j][1];
          if(vEvl(pv,lks[row])&&vEvl(qv,lks[col]))
          {
            base.push_back(vecs[t][j]);break;
          }
          else if(vEvl(qv,lks[row])&&vEvl(pv,lks[col]))
          {
            base.push_back(-vecs[t][j]);break;
          }
        }
        //if(j==lkts.size())
        //{
          //base.push_back(0);
        //}
      }
    }
    if(base.size()!=mts.size())
    {
       WerrorS("Errors in Values Finding!");
        //WerrorS("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");
      usleep(1000000);
      assert(false);
    }
    bases.push_back(base);
    base.clear();
  }
  return bases;
}

static std::vector<std::vector<int> > gpl2(ideal h,poly a,poly b)
{
  int i,j,t,n;
  std::vector<std::vector<int> > hvs=supports(h),sbv,nv,mv,mts,vecs,vars,ntvs,solve;
  std::vector<int> av=support1(a), bv=support1(b),vec,var,tnv;
  ideal sub=psubset(b);
  sbv=supports(sub);
  nv=Nabv(hvs,av,bv);
  n=nv.size();
  tnv=tnab(hvs,nv,sbv);
  ring r=currRing;
  mv=Mabv(h,a,b);
  mts=mabtv(hvs,mv,av,bv);
  if(n>0)
  {
    ntvs=nabtv( hvs, nv, av, bv);
    int l=ntvs.size();
    if(l>0)
    {
      for(int t0=0;t0<l;t0++)
      {
        i=ntvs[t0][0];
        j=ntvs[t0][1];
        if(tNab(hvs,nv[i],sbv)&&tNab(hvs,nv[j],sbv))//condition 1
        {
          vec=makeequation(t0+1,0,0);
          vecs.push_back(vec);
          vec.clear();
        }
        for(int t1=t0+1;t1<ntvs.size();t1++)
        {
          for(int t2=t1+1;t2<ntvs.size();t2++)
          {
            if(ntvs[t0][0]==ntvs[t1][0]&&ntvs[t1][1]==ntvs[t2][1]&&ntvs[t0][1]==ntvs[t2][0])
            {
              i=ntvs[t0][0];
              j=ntvs[t0][1];
              t=ntvs[t1][1];
              if(condition2for2nv(hvs,nv[i],nv[j],nv[t]))
              {
                vec=makeequation(t0+1,t1+1,t2+1);
                vecs.push_back(vec);
                vec.clear();
              }
            }
          }
        }
      }
      if(n==1) {l=1;}
      equmab(l);
      ideal id_re=idMake3(vecs);
      std::vector<std::vector<int> > re=getvector(id_re,l);
      rChangeCurrRing(r);
      std::vector<std::vector<int> > sub=subspacetn(nv, tnv,ntvs);
      equmab(l);
      solve=vecqring(re, sub);
      rChangeCurrRing(r);
    }
    else
    {
      solve.clear();
    }
  }
  else
    solve.clear();
  return solve;
}

static intvec * gradedpiece2nl(ideal h,poly a,poly b)
{
  poly e=pOne();
  std::vector<int> av=support1(a), bv=support1(b), em;
  std::vector<std::vector<int> > hvs=supports(h), mv=Mabv(h,a,b),mts,solve,lks,nvl,ntsl;
  mts=mabtv(hvs,mv,av,bv);
  lks=links(a,h);
  ideal id_links=idMaken(lks);
//PrintS("This is the links of a:\n"); id_print(id_links);
  nvl=Nabv(lks,em,bv);
//PrintS("This is the N set:\n"); id_print(idMaken(nvl));
  ntsl=nabtv(lks,nvl,em,bv);
//PrintS("This is N^2:\n"); listsprint(ntsl);
  solve=gpl2(id_links,e,b);
//PrintS("This is pre solution of N:\n"); listsprint(solve);
  if(solve.size() > 0)
  {
    solve=value2l(mv, nvl, mts, ntsl, solve, av, bv);
  }
//PrintS("This is solution of N:\n"); listsprint(solve);
  intvec *sl=Tmat(solve);
  return sl;
}

//for debugging
/*
void Tlink(ideal h,poly a,poly b,int n)
{
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<int> av=support1(a);
  std::vector<int> bv=support1(b);
  std::vector<std::vector<int> > vec=links(a, h);
  PrintS("This is the links of a:\n");
  listsprint(vec);
  ideal li=idMaken(vec);
  PrintS("This is the links of a(ideal version):\n");
  id_print(li);
  poly p=pOne();
  PrintS("1************************************************\n");
    PrintS("This is T_1 (m):\n");
    gradedpiece1(li,p,b);
  PrintS("2************************************************\n");
    PrintS("This is T_2 (m):\n");
    gradedpiece2(li,p,b);
  PrintS("3************************************************\n");
    PrintS("This is T_1 (n):\n");
    gradedpiece1n(li,p,b);
  PrintS("4************************************************\n");
    PrintS("This is T_2 (n):\n");
    gradedpiece2n(li,p,b);
}
*/

/******************************for triangulation***********************************/

//returns all the faces which are triangles
static ideal trisets(ideal h)
{
  int i;
  ideal ids=idInit(1,1);
  std::vector<int> pv;
  for(i=0;i<IDELEMS(h);i++)
  {
    pv= support1(h->m[i]);
    if(pv.size()==3)
      idInsertPoly(ids, pCopy(h->m[i]));
  }
  idSkipZeroes(ids);
  return ids;
}

// case 1 new faces
static std::vector<std::vector<int> > triface(poly p, int vert)
{
  std::vector<int> vec, fv=support1(p);
  std::vector<std::vector<int> > fvs0, fvs;
  vec.push_back(vert);
  fvs.push_back(vec);
  fvs0=b_subsets(fv);
  fvs0=vsMinusv(fvs0,fv);
  for(unsigned i=0;i<fvs0.size();i++)
  {
    vec=fvs0[i];
    vec.push_back(vert);
    fvs.push_back(vec);
  }
  return (fvs);
}

// the size of p's support must be 3
//returns the new complex which is a triangulation based on the face p
static ideal triangulations1(ideal h, poly p, int vert)
{
  std::vector<int> vec, pv=support1(p);
  std::vector<std::vector<int> > vecs=supports(h),vs,vs0;
  vs0=triface(p,vert);
  vecs=vsMinusv(vecs, pv);
  vecs=vsUnion(vecs,vs0);
  //PrintS("This is the new simplicial complex according to the face \n"); pWrite(p);
  //PrintS("is:\n");
  //listsprint(vecs);
  ideal re=idMaken(vecs);
  return re;
}

/*
static ideal triangulations1(ideal h)
{
  int i,vert=currRing->N+1;
  std::vector<int> vec;
  std::vector<std::vector<int> > vecs=supports(h),vs,vs0;
  for (i=0;i<vecs.size();i++)
  {
    if((vecs[i]).size()==3)
    {
      vs0=triface(vecs[i],vert);
      vs=vsMinusv(vecs,vecs[i]);
      vs=vsUnion(vs,vs0);
      PrintS("This is the new simplicial complex according to the face \n");listprint(vecs[i]);
      PrintS("is:\n");
      listsprint(vs);
    }
    //else if((vecs[i]).size()==4)
      //tetraface(vecs[i]);
  }
  //ideal hh=idMaken(vs);
  return h;
}*/

static std::vector<int> commonedge(poly p, poly q)
{
  std::vector<int> ev, fv1= support1(p), fv2= support2(q);
  for(unsigned i=0;i<fv1.size();i++)
  {
    if(IsinL(fv1[i], fv2))
      ev.push_back(fv1[i]);
  }
  return ev;
}

static intvec *edgemat(poly p, poly q)
{
  intvec *m;
  int i;
  std::vector<int> dg=commonedge(p, q);
  int lg=dg.size();
  m=new intvec(lg);
  if(lg!=0)
  {
    m=new intvec(lg);
    for(i=0;i<lg;i++)
    {
        (*m)[i]=dg[i];
    }
  }
  return (m);
}

// case 2 the new face
static std::vector<std::vector<int> > tetraface(poly p, poly q, int vert)
{
  std::vector<int> ev=commonedge(p, q), vec, fv1=support1(p), fv2=support1(q);
  std::vector<std::vector<int> > fvs1, fvs2, fvs;
  vec.push_back(vert);
  fvs.push_back(vec);
  fvs1=b_subsets(fv1);
  fvs2=b_subsets(fv2);
  fvs1=vsMinusv(fvs1, fv1);
  fvs2=vsMinusv(fvs2, fv2);
  fvs2=vsUnion(fvs1, fvs2);
  fvs2=vsMinusv(fvs2, ev);
  for(unsigned i=0;i<fvs2.size();i++)
  {
    vec=fvs2[i];
    vec.push_back(vert);
    fvs.push_back(vec);
  }
  return (fvs);
}

//if p and q have  a common edge
static ideal triangulations2(ideal h, poly p, poly q, int vert)
{
  std::vector<int> ev, fv1=support1(p), fv2=support1(q);
  std::vector<std::vector<int> > vecs=supports(h), vs1;
  ev=commonedge(p, q);
  vecs=vsMinusv(vecs, ev);
  vecs=vsMinusv(vecs,fv1);
  vecs=vsMinusv(vecs,fv2);
  vs1=tetraface(p, q, vert);
  vecs=vsUnion(vecs,vs1);
  ideal hh=idMaken(vecs);
  return hh;
}

// case 2 the new face
static std::vector<std::vector<int> > penface(poly p, poly q, poly g, int vert)
{
  int en=0;
  std::vector<int> ev1=commonedge(p, q), ev2=commonedge(p, g), ev3=commonedge(q, g), ind, vec, fv1=support1(p), fv2=support1(q), fv3=support1(g);
  std::vector<std::vector<int> > fvs1, fvs2, fvs3, fvs, evec;
  evec.push_back(ev1);
  evec.push_back(ev2);
  evec.push_back(ev3);
  for(unsigned i=0;i<evec.size();i++)
  {
    if(evec[i].size()==2)
    {
      en++;
    }
  }
  if(en==2)
  {
    vec.push_back(vert);
    fvs.push_back(vec);
    fvs1=b_subsets(fv1);
    fvs2=b_subsets(fv2);
    fvs3=b_subsets(fv3);
    fvs1=vsMinusv(fvs1, fv1);
    fvs2=vsMinusv(fvs2, fv2);
    fvs3=vsMinusv(fvs3, fv3);
    fvs3=vsUnion(fvs3, fvs2);
    fvs3=vsUnion(fvs3, fvs1);
    for(unsigned i=0;i<evec.size();i++)
    {
      if(evec[i].size()==2)
      {
        fvs3=vsMinusv(fvs3, evec[i]);
      }
    }
    for(unsigned i=0;i<fvs3.size();i++)
    {
      vec=fvs3[i];
      vec.push_back(vert);
      fvs.push_back(vec);
    }
  }
  return (fvs);
}

static ideal triangulations3(ideal h, poly p, poly q, poly g, int vert)
{
  std::vector<int> ev1=commonedge(p, q), ev2=commonedge(p, g), ev3=commonedge(q, g), fv1=support1(p), fv2=support1(q), fv3=support1(g);
  std::vector<std::vector<int> > vecs=supports(h), vs1, evec;
  evec.push_back(ev1);
  evec.push_back(ev2);
  evec.push_back(ev3);
  for(unsigned i=0;i<evec.size();i++)
  {
    if(evec[i].size()==2)
    {
      vecs=vsMinusv(vecs, evec[i]);
    }
  }
  vecs=vsMinusv(vecs,fv1);
  vecs=vsMinusv(vecs,fv2);
  vecs=vsMinusv(vecs,fv3);
  vs1=penface(p, q, g, vert);
  vecs=vsUnion(vecs,vs1);
  ideal hh=idMaken(vecs);
  return hh;
}

//returns p's valency in h
//p must be a vertex
static int valency(ideal h, poly p)
{
  int val=0;
  std::vector<int> ev=support1(pCopy(p));
  int ver=ev[0];
//PrintS("the vertex is :\n"); listprint(p);
  std::vector<std::vector<int> > vecs=supports(idCopy(h));
  for(unsigned i=0;i<vecs.size();i++)
  {
    if(vecs[i].size()==2 && IsinL(ver, vecs[i]))
      val++;
  }
  return (val);
}

/*ideal triangulations2(ideal h)
{
  int i,j,vert=currRing->N+1;
  std::vector<int> ev;
  std::vector<std::vector<int> > vecs=supports(h),vs,vs0,vs1;
  vs0=tetrasets(h);
  for (i=0;i<vs0.size();i++)
  {
    for(j=i;j<vs0.size();j++)
    {
      ev=commonedge(vs0[i],vs0[j]);
      if(ev.size()==2)
      {
        vecs=vsMinusv(vecs, ev);
        vs=vsMinusv(vecs,vs0[i]);
        vs=vsMinusv(vecs,vs0[j]);
        vs1=tetraface(vs0[i],vs0[j],vert);
        vs=vsUnion(vs,vs1);
        PrintS("This is the new simplicial complex according to the face 1 \n");listprint(vecs[i]);
PrintS("face 2: \n");
        PrintS("is:\n");
        listsprint(vs);
      }
    }

    //else if((vecs[i]).size()==4)
      //tetraface(vecs[i]);
  }
  //ideal hh=idMaken(vs);
  return h;
}*/

/*********************************For computation of X_n***********************************/
static std::vector<std::vector<int> > vsMinusvs(std::vector<std::vector<int> > vs1, std::vector<std::vector<int> > vs2)
{
  std::vector<std::vector<int> > vs=vs1;
  for(unsigned i=0;i<vs2.size();i++)
  {
    vs=vsMinusv(vs, vs2[i]);
  }
  return vs;
}

static std::vector<std::vector<int> > vs_subsets(std::vector<std::vector<int> > vs)
{
  std::vector<std::vector<int> >  sset, bv;
  for(unsigned i=0;i<vs.size();i++)
  {
    bv=b_subsets(vs[i]);
    sset=vsUnion(sset, bv);
  }
  return sset;
}

static std::vector<std::vector<int> > p_constant(ideal Xo,  ideal Sigma)
{
  std::vector<std::vector<int> > xs=supports(idCopy(Xo)), ss=supports(idCopy(Sigma)), fvs1;
  fvs1=vs_subsets(ss);
  fvs1=vsMinusvs(xs, fvs1);
  return fvs1;
}

static std::vector<std::vector<int> > p_change(ideal Sigma)
{
  std::vector<std::vector<int> > ss=supports(idCopy(Sigma)), fvs;
  fvs=vs_subsets(ss);
  return (fvs);
}

static std::vector<std::vector<int> > p_new(ideal Xo, ideal Sigma)
{
  int vert=0;
  std::vector<std::vector<int> > ss=supports(idCopy(Sigma)), fvs;
  for(int i=1;i<=currRing->N;i++)
  {
    for(int j=0;j<IDELEMS(Xo);j++)
    {
      if(pGetExp(Xo->m[j],i)>0)
      {
        vert=i+1;
        break;
      }
    }
  }
  int typ=ss.size();
  if(typ==1)
  {
    fvs=triface(Sigma->m[0], vert);
  }
  else if(typ==2)
  {
     fvs=tetraface(Sigma->m[0], Sigma->m[1], vert);
  }
  else
  {
     fvs=penface(Sigma->m[0], Sigma->m[1], Sigma->m[2], vert);
  }
  return (fvs);
}

static ideal c_New(ideal Io, ideal sig)
{
  std::vector<std::vector<int> > vs1=p_constant(Io, sig), vs2=p_change(sig), vs3=p_new(Io, sig), vsig=supports(sig), vs;
  std::vector<int> ev;
  int ednum=vsig.size();
  if(ednum==2)
  {
    vsig.push_back(commonedge(sig->m[0], sig->m[1]));
  }
  else if(ednum==3)
  {
    for(int i=0;i<IDELEMS(sig);i++)
    {
      for(int j=i+1;j<IDELEMS(sig);j++)
      {
        ev=commonedge(sig->m[i], sig->m[j]);
        if(ev.size()==2)
        {
          vsig.push_back(ev);
        }
      }
    }
  }
//PrintS("the first part is:\n");id_print(idMaken(vs1));
//PrintS("the second part is:\n");id_print(idMaken(vsig));
//PrintS("the third part is:\n");id_print(idMaken(vs3));
  vs2=vsMinusvs(vs2, vsig);
//PrintS("the constant part2 is:\n");id_print(idMaken(vs2));
  vs=vsUnion(vs2, vs1);
//PrintS("the constant part is:\n");id_print(idMaken(vs));
  vs=vsUnion(vs, vs3);
//PrintS("the whole part is:\n");id_print(idMaken(vs));
  return(idMaken(vs));
}

static std::vector<std::vector<int> > phi1(poly a,  ideal Sigma)
{
  std::vector<std::vector<int> > ss=supports(idCopy(Sigma)), fvs;
  std::vector<int> av=support1(a), intvec, vv;
  for(unsigned i=0;i<ss.size();i++)
  {
    intvec=vecIntersection(ss[i], av);
    if(intvec.size()==av.size())
    {
      vv=vecMinus(ss[i], av);
      fvs.push_back(vv);
    }
  }
  return fvs;
}

static std::vector<std::vector<int> > phi2(poly a, ideal Xo, ideal Sigma)
{

  std::vector<std::vector<int> > ss=p_new(Sigma, Xo), fvs;
  std::vector<int> av=support1(a), intvec, vv;
  for(unsigned i=0;i<ss.size();i++)
  {
    intvec=vecIntersection(ss[i], av);
    if(intvec.size()==av.size())
    {
      vv=vecMinus(ss[i], av);
      fvs.push_back(vv);
    }
  }
  return fvs;
}

static std::vector<std::vector<int> > links_new(poly a, ideal Xo, ideal Sigma, int vert, int ord)
{
  std::vector<int> av=support1(a);
  std::vector<std::vector<int> > lko, lkn, lk1, lk2;
  lko=links(a, Xo);
  if(ord==1)
    return lko;
  if(ord==2)
  {
    lk1=phi1(a, Sigma);
    lk2=phi2(a, Xo, Sigma);
    lkn=vsMinusvs(lko, lk1);
    lkn=vsUnion(lkn, lk2);
    return lkn;
  }
  if(ord==3)
  {
    lkn=phi2(a, Xo, Sigma);
    return lkn;
  }
  WerrorS("Cannot find the links smartly!");
  return lko;
}

//returns 1 if there is a real divisor of b not in Xs
static int existIn(poly b, ideal Xs)
{
  std::vector<int> bv=support1(pCopy(b));
  std::vector<std::vector<int> > xvs=supports(idCopy(Xs)), bs=b_subsets(bv);
  bs=vsMinusv(bs, bv);
  for(unsigned i=0;i<bs.size();i++)
  {
    if(!vInvsl(bs[i], xvs))
    {
      return 1;
    }
  }
  return 0;
}

static int isoNum(poly p, ideal I, poly a, poly b)
{
  int i;
  std::vector<std::vector<int> > vs=supports(idCopy(I));
  std::vector<int> v1=support1(a), v2=support1(b), v=support1(p);
  std::vector<int>  vp, iv=phimagel(v, v1, v2);
  for(i=0;i<IDELEMS(I);i++)
  {
    vp=support1(pCopy(I->m[i]));
    if(vEvl(iv, phimagel(vp, v1, v2)))
    {
      return (i+1);
    }
  }
  return (0);
}

static int ifIso(poly p, poly q, poly f, poly g, poly a, poly b)
{
  std::vector<int> va=support1(a), vb=support1(b), vp=support1(p),  vq=support1(q), vf=support1(f), vg=support1(g);
  std::vector<int>   v1=phimagel(vp, va, vb), v2=phimagel(vq, va, vb), v3=phimagel(vf, va, vb), v4=phimagel(vg, va, vb);
  if((vEvl(v1, v3)&& vEvl(v2,v4))||(vEvl(v1, v4)&& vEvl(v2,v3)) )
  {
    return (1);
  }
  return (0);
}

static ideal idMinusp(ideal I, poly p)
{
  ideal h=idInit(1,1);
  int i;
  for(i=0;i<IDELEMS(I);i++)
  {
    if(!p_EqualPolys(I->m[i], p, currRing))
    {
      idInsertPoly(h, pCopy(I->m[i]));
    }
  }
  idSkipZeroes(h);
  return h;
}

/****************************for the interface of .lib*********************************/

static std::vector<int> v_minus(std::vector<int> v1, std::vector<int> v2)
{
  std::vector<int> vec;
  for(unsigned i=0;i<v1.size();i++)
  {
    vec.push_back(v1[i]-v2[i]);
  }
  return vec;
}

static std::vector<int> gdegree(poly a, poly b)
{
  int i;
  std::vector<int> av,bv;
  for(i=1;i<=currRing->N;i++)
  {
    av.push_back(pGetExp(a,i));
    bv.push_back(pGetExp(b,i));
  }
  std::vector<int> vec=v_minus(av,bv);
  //PrintS("The degree is:\n");
  //listprint(vec);
  return vec;
}

/********************************for stellar subdivision******************************/

static std::vector<std::vector<int> > star(poly a, ideal h)
{
  int i;
  std::vector<std::vector<int> > st,X=supports(h);
  std::vector<int> U,av=support1(a);
  for(i=0;i<X.size();i++)
  {
    U=vecUnion(av,X[i]);
    if(vInvsl(U,X))
    {
      st.push_back(X[i]);
    }
  }
  return st;
}

static std::vector<std::vector<int> > boundary(poly a)
{
  std::vector<int> av=support1(a), vec;
  std::vector<std::vector<int> > vecs;
  vecs=b_subsets(av);
  vecs.push_back(vec);
  vecs=vsMinusv(vecs, av);
  return vecs;
}

static std::vector<std::vector<int> > stellarsub(poly a, ideal h)
{
  std::vector<std::vector<int> > vecs_minus, vecs_plus, lk=links(a,h), hvs=supports(h), sub, bys=boundary(a);
  std::vector<int> av=support1(a), vec, vec_n;
  int i,j,vert=0;
  for(i=1;i<=currRing->N;i++)
  {
    for(j=0;j<IDELEMS(h);j++)
    {
      if(pGetExp(h->m[j],i)>0)
      {
        vert=i+1;
        break;
      }
    }
  }
  vec_n.push_back(vert);
  for(i=0;i<lk.size();i++)
  {
    vec=vecUnion(av, lk[i]);
    vecs_minus.push_back(vec);
    for(j=0;j<bys.size();j++)
    {
      vec=vecUnion(lk[i], vec_n);
      vec=vecUnion(vec, bys[j]);
      vecs_plus.push_back(vec);
    }
  }
  sub=vsMinusvs(hvs, vecs_minus);
  sub=vsUnion(sub, vecs_plus);
  return(sub);
}

static std::vector<std::vector<int> > bsubsets_1(poly b)
{
  std::vector<int>  bvs=support1(b), vs;
  std::vector<std::vector<int> > bset;
  for(unsigned i=0;i<bvs.size();i++)
  {
    for(int j=0;j!=i; j++)
    {
      vs.push_back(bvs[j]);
    }
    bset.push_back(vs);
    vs.resize(0);
  }
  return bset;
}

/***************************for time testing******************************/
static ideal T_1h(ideal h)
{
  int i, j;
  //std::vector < intvec > T1;
  ideal ai=p_a(h), bi;
  //intvec *L;
  for(i=0;i<IDELEMS(ai);i++)
  {
    bi=p_b(h,ai->m[i]);
    if(!idIs0(bi))
    {
      for(j=0;j<IDELEMS(bi);j++)
      {
        //PrintS("This is for:\n");pWrite(ai->m[i]); pWrite(bi->m[j]);
        gradedpiece1nl(h,ai->m[i],bi->m[j], 0);
        //PrintS("Succeed!\n");
        //T1.push_back(L);
      }
    }
  }
  TimeShow(t_construct, t_solve, t_value, t_total);
  return h;
}

/**************************************interface T1****************************************/
/*
static BOOLEAN makeqring(leftv res, leftv args)
{
  leftv h=args;
  ideal h2= id_complement( hh);
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal hh=(ideal)h->Data();
       ideal h2=id_complement(hh);
       ideal h1=id_Init(1,1);
       idInsertPoly(h1,p);
       ideal gb=kStd(h2,NULL,testHomog,NULL,NULL,0,0,NULL);
       ideal idq=kNF(gb,NULL,h1);
       idSkipZeroes(h1);
         res->rtyp =POLY_CMD;
         res->data =h1->m[0];
       }
     }
  }
  return false;
}*/

static BOOLEAN SRideal(leftv res, leftv args)
{
  leftv h=args;
   if((h != NULL)&&(h->Typ() == IDEAL_CMD))
   {
     ideal hh=(ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =idsrRing(hh);
   }
  return false;
}

static BOOLEAN idcomplement(leftv res, leftv args)
{
  leftv h=args;
   if((h != NULL)&&(h->Typ() == IDEAL_CMD))
   {
     ideal hh=(ideal)h->Data();
     ideal h2= id_complement(hh);
     res->rtyp =IDEAL_CMD;
     res->data =h2;
   }
  return false;
}

static BOOLEAN t1h(leftv res, leftv args)
{
  leftv h=args;
   if((h != NULL)&&(h->Typ() == IDEAL_CMD))
   {
     ideal hh=(ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =T_1h(hh);
   }
  return false;
}

static BOOLEAN idsr(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =IDEAL_CMD;
         res->data =mingens(h1,p,q);
       }
     }
  }
  return false;
}

static intvec *dmat(poly a, poly b)
{
  intvec *m;
  int i;
  std::vector<int> dg=gdegree(a,b);
  int lg=dg.size();
  m=new intvec(lg);
  if(lg!=0)
  {
    m=new intvec(lg);
    for(i=0;i<lg;i++)
    {
        (*m)[i]=dg[i];
    }
  }
  return (m);
}

static BOOLEAN gd(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly q= (poly)h->Data();
       res->rtyp =INTVEC_CMD;
       res->data =dmat(p,q);
     }
  }
  return false;
}

static BOOLEAN comedg(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly q= (poly)h->Data();
       res->rtyp =INTVEC_CMD;
       res->data =edgemat(p,q);
     }
  }
  return false;
}

static BOOLEAN fb(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =findb(h1);
  }
  return false;
}

static BOOLEAN pa(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =p_a(h1);
  }
  return false;
}

static BOOLEAN makeSimplex(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =complementsimplex(h1);
  }
  return false;
}

static BOOLEAN pb(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =p_b(h1,p);
     }
  }
  return false;
}

static BOOLEAN fa(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly q= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == INT_CMD))
       {
         int d= (int)(long)h->Data();
         res->rtyp =IDEAL_CMD;
         res->data =finda(h1,q,d);
       }
     }
  }
  return false;
}

static BOOLEAN fgp(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =INTVEC_CMD;
         res->data =gradedpiece1n(h1,p,q);
       }
     }
  }
  return false;
}

static BOOLEAN fgpl(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         h   = h->next;
         if((h != NULL)&&(h->Typ() == INT_CMD))
         {
           int d= (int)(long)h->Data();
           res->rtyp =INTVEC_CMD;
           res->data =gradedpiece1nl(h1,p,q,d);
         }
       }
     }
  }
  return false;
}

static BOOLEAN genstt(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =IDEAL_CMD;
         res->data =genst(h1,p,q);
       }
     }
  }
  return false;
}

static BOOLEAN sgp(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =INTVEC_CMD;
         res->data =gradedpiece2n(h1,p,q);
       }
     }
  }
  return false;
}

static BOOLEAN sgpl(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =INTVEC_CMD;
         res->data =gradedpiece2nl(h1,p,q);
       }
     }
  }
  return false;
}

static BOOLEAN Links(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h1= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       std::vector<std::vector<int> > vecs=links(p,h1);
       res->data =idMaken(vecs);
     }
  }
  return false;
}

static BOOLEAN isSim(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =IsSimplex(h1);
  }
  return false;
}

BOOLEAN nfaces1(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == INT_CMD))
       {
         int d= (int)(long)h->Data();
         res->rtyp =IDEAL_CMD;
         res->data =triangulations1(h1, p, d);
       }
     }
  }
  return false;
}

static BOOLEAN nfaces2(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         h   = h->next;
         if((h != NULL)&&(h->Typ() == INT_CMD))
         {
           int d= (int)(long)h->Data();
           res->rtyp =IDEAL_CMD;
           res->data =triangulations2(h1,p,q,d);
         }
       }
     }
  }
  return false;
}

static BOOLEAN nfaces3(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         h   = h->next;
         if((h != NULL)&&(h->Typ() == POLY_CMD))
         {
           poly g= (poly)h->Data();
           h   = h->next;
           if((h != NULL)&&(h->Typ() == INT_CMD))
           {
             int d= (int)(long)h->Data();
             res->rtyp =IDEAL_CMD;
             res->data =triangulations3(h1,p,q,g,d);
           }
         }
       }
     }
  }
  return false;
}

static BOOLEAN eqsolve1(leftv res, leftv args)
{
  leftv h=args;int i;
  std::vector<int> bset,bs;
  std::vector<std::vector<int> > gset;
  if((h != NULL)&&(h->Typ() == INT_CMD))
  {
     int n= (int)(long)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal bi= (ideal)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == IDEAL_CMD))
       {
         ideal gi= (ideal)h->Data();
         for(i=0;i<IDELEMS(bi);i++)
         {
           bs=support1(bi->m[i]);
           if(bs.size()==1)
             bset.push_back(bs[0]);
           else if(bs.size()==0)
             ;
           else
           {
             WerrorS("Errors in T^1 Equations Solving!");
             usleep(1000000);
             assert(false);
           }

         }
         gset=supports2(gi);
         res->rtyp =INTVEC_CMD;
         std::vector<std::vector<int> > vecs=eli2(n,bset,gset);
         res->data =Tmat(vecs);
       }
     }
  }
  return false;
}

static BOOLEAN tsets(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =trisets(h1);
  }
  return false;
}

static BOOLEAN Valency(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       res->rtyp =INT_CMD;
       res->data =(void *)(long)valency(h1,p);
     }
  }
  return false;
}

static BOOLEAN nabvl(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =IDEAL_CMD;
         std::vector<std::vector<int> > vecs=supports(h1);
         std::vector<int> pv=support1(p), qv=support1(q);
         res->data =idMaken(Nabv(vecs,pv,qv));
       }
     }
  }
  return false;
}

static BOOLEAN tnabvl(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =IDEAL_CMD;
         std::vector<std::vector<int> > vecs=supports(h1), sbv,tnbr;
         std::vector<int> pv=support1(p), qv=support1(q);
         std::vector<std::vector<int> > nvs=Nabv(vecs, pv, qv);
         ideal sub=psubset(q);
         sbv=supports(sub);
         std::vector<int> tnv =tnab(vecs,nvs,sbv);
         for(unsigned i=0;i<tnv.size();i++)
         {
           tnbr.push_back(nvs[tnv[i]]);
         }
         res->data =idMaken(tnbr);
       }
     }
  }
  return false;
}

static BOOLEAN vsIntersec(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h2= (ideal)h->Data();
       res->rtyp =INT_CMD;
       std::vector<std::vector<int> > vs1=supports(h1), vs2=supports(h2);
       res->data =(void *)(long)(vsIntersection(vs1, vs2).size());
     }
  }
  return false;
}

static BOOLEAN mabvl(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         res->rtyp =IDEAL_CMD;
         res->data =idMaken(Mabv(h1,p,q));
       }
     }
  }
  return false;
}

static BOOLEAN nabtvl(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly q= (poly)h->Data();
         std::vector<std::vector<int> > hvs=supports(h1), nv, ntvs;
         std::vector<int> av=support1(p), bv=support1(q);
         nv=Nabv(hvs,av,bv);
         ntvs=nabtv( hvs, nv, av, bv);
         std::vector<std::vector<poly> > pvs=idMakei(nv,ntvs);
         ideal gens=idInit(1,1);
         for(unsigned i=0;i<pvs.size();i++)
         {
           idInsertPoly(gens,pvs[i][0]);
           idInsertPoly(gens,pvs[i][1]);
         }
         idSkipZeroes(gens);
         res->rtyp =IDEAL_CMD;
         res->data =gens;
       }
     }
  }
  return false;
}

static BOOLEAN linkn(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
    poly a= (poly)h->Data();
    h   = h->next;
    if((h != NULL)&&(h->Typ() == IDEAL_CMD))
    {
      ideal Xo= (ideal)h->Data();
      h   = h->next;
      if((h != NULL)&&(h->Typ() == IDEAL_CMD))
      {
        ideal Sigma= (ideal)h->Data();
        h   = h->next;
        if((h != NULL)&&(h->Typ() == INT_CMD))
        {
          int vert= (int)(long)h->Data();
          h   = h->next;
          if((h != NULL)&&(h->Typ() == INT_CMD))
          {
            int ord= (int)(long)h->Data();
            res->rtyp =IDEAL_CMD;
            res->data =idMaken(links_new(a,  Xo,  Sigma,  vert,  ord));
          }
        }
      }
    }
  }
  return false;
}

static BOOLEAN existsub(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h1= (ideal)h->Data();
       res->rtyp =INT_CMD;
       res->data =(void *)(long)existIn(p, h1);
     }
   }
  return false;
}

static BOOLEAN pConstant(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h2= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idMaken(p_constant(h1,h2));
     }
  }
  return false;
}

static BOOLEAN pChange(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =idMaken(p_change(h1));
  }
  return false;
}

static BOOLEAN p_New(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h2= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idMaken(p_new(h1,h2));
     }
  }
  return false;
}

static BOOLEAN support(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     res->rtyp =INT_CMD;
     res->data =(void *)(long)(support1(p).size());
  }
  return false;
}

static BOOLEAN bprime(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =idMaken(bsubsets_1(p));
  }
  return false;
}

static BOOLEAN psMinusp(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idMinusp(h1, p);
     }
  }
  return false;
}

static BOOLEAN stellarremain(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       std::vector<std::vector<int> > st=star(p, h1);
       std::vector<std::vector<int> > hvs=supports(h1);
       std::vector<std::vector<int> > re= vsMinusvs(hvs, st);
       res->rtyp =IDEAL_CMD;
       res->data =idMaken(re);
     }
  }
  return false;
}

static BOOLEAN cNew(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h2= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =c_New(h1, h2);
     }
  }
  return false;
}

static BOOLEAN stars(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h1= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idMaken(star(p, h1));
     }
   }
  return false;
}

static BOOLEAN stellarsubdivision(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h2= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly p= (poly)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idMaken(stellarsub(p, h2));
     }
  }
  return false;
}

static BOOLEAN idModulo(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h2= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idmodulo(h1, h2);
     }
  }
  return false;
}

static BOOLEAN idminus(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == IDEAL_CMD))
     {
       ideal h2= (ideal)h->Data();
       res->rtyp =IDEAL_CMD;
       res->data =idMinus(h1, h2);
     }
  }
  return false;
}

static BOOLEAN isoNumber(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
    poly p= (poly)h->Data();
    h   = h->next;
    if((h != NULL)&&(h->Typ() == IDEAL_CMD))
    {
      ideal h1= (ideal)h->Data();
      h   = h->next;
      if((h != NULL)&&(h->Typ() == POLY_CMD))
      {
        poly a= (poly)h->Data();
        h   = h->next;
        if((h != NULL)&&(h->Typ() == POLY_CMD))
        {
          poly b= (poly)h->Data();
          res->rtyp =INT_CMD;
          res->data =(void *)(long)isoNum(p, h1, a, b);
        }
      }
    }
  }
  return false;
}

static BOOLEAN ifIsomorphism(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
     poly p= (poly)h->Data();
     h   = h->next;
     if((h != NULL)&&(h->Typ() == POLY_CMD))
     {
       poly q= (poly)h->Data();
       h   = h->next;
       if((h != NULL)&&(h->Typ() == POLY_CMD))
       {
         poly f= (poly)h->Data();
         h   = h->next;
         if((h != NULL)&&(h->Typ() == POLY_CMD))
         {
           poly g= (poly)h->Data();
           h   = h->next;
           if((h != NULL)&&(h->Typ() == POLY_CMD))
           {
            poly a= (poly)h->Data();
             h   = h->next;
             if((h != NULL)&&(h->Typ() == POLY_CMD))
             {
               poly b= (poly)h->Data();
               res->rtyp =INT_CMD;
               res->data =(void *)(long)ifIso(p,q,f,g, a, b);
             }
           }
         }
       }
     }
  }
  return false;
}

static BOOLEAN newDegree(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == POLY_CMD))
  {
    poly p= (poly)h->Data();
    h   = h->next;
    if((h != NULL)&&(h->Typ() == INT_CMD))
    {
       int num= (int)(long)h->Data();
       res->rtyp =INT_CMD;
       res->data =(void *)(long)redefinedeg( p, num);
    }
  }
  return false;
}

static BOOLEAN nonf2f(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =IDEAL_CMD;
     res->data =complementsimplex(h1);
  }
  return false;
}

static BOOLEAN dimsim(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
     res->rtyp =INT_CMD;
     res->data =(void *)(long)dim_sim(h1);
  }
  return false;
}

static BOOLEAN numdim(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
    ideal h1= (ideal)h->Data();
    h   = h->next;
    if((h != NULL)&&(h->Typ() == INT_CMD))
    {
       int num= (int)(long)h->Data();
       res->rtyp =INT_CMD;
       res->data =(void *)(long)num4dim( h1, num);
    }
  }
  return false;
}

/**************************************interface T2****************************************/

static void firstorderdef_setup(SModulFunctions* p)
{
  p->iiAddCproc("","mg",FALSE,idsr);
  p->iiAddCproc("","gd",FALSE,gd);
  p->iiAddCproc("","findbset",FALSE,fb);
  p->iiAddCproc("","findaset",FALSE,fa);
  p->iiAddCproc("","fgp",FALSE,fgp);
  p->iiAddCproc("","fgpl",FALSE,fgpl);
  p->iiAddCproc("","idcomplement",FALSE,idcomplement);
  p->iiAddCproc("","genst",FALSE,genstt);
  p->iiAddCproc("","sgp",FALSE,sgp);
  p->iiAddCproc("","sgpl",FALSE,sgpl);
  p->iiAddCproc("","Links",FALSE,Links);
  p->iiAddCproc("","eqsolve1",FALSE,eqsolve1);
  p->iiAddCproc("","pb",FALSE,pb);
  p->iiAddCproc("","pa",FALSE,pa);
  p->iiAddCproc("","makeSimplex",FALSE,makeSimplex);
  p->iiAddCproc("","isSim",FALSE,isSim);
  p->iiAddCproc("","nfaces1",FALSE,nfaces1);
  p->iiAddCproc("","nfaces2",FALSE,nfaces2);
  p->iiAddCproc("","nfaces3",FALSE,nfaces3);
  p->iiAddCproc("","comedg",FALSE,comedg);
  p->iiAddCproc("","tsets",FALSE,tsets);
  p->iiAddCproc("","valency",FALSE,Valency);
  p->iiAddCproc("","nab",FALSE,nabvl);
  p->iiAddCproc("","tnab",FALSE,tnabvl);
  p->iiAddCproc("","mab",FALSE,mabvl);
  p->iiAddCproc("","SRideal",FALSE,SRideal);
  p->iiAddCproc("","Linkn",FALSE,linkn);
  p->iiAddCproc("","Existb",FALSE,existsub);
  p->iiAddCproc("","pConstant",FALSE,pConstant);
  p->iiAddCproc("","pChange",FALSE,pChange);
  p->iiAddCproc("","pNew",FALSE,p_New);
  p->iiAddCproc("","pSupport",FALSE,support);
  p->iiAddCproc("","psMinusp",FALSE,psMinusp);
  p->iiAddCproc("","cNew",FALSE,cNew);
  p->iiAddCproc("","isoNumber",FALSE,isoNumber);
  p->iiAddCproc("","vsInsec",FALSE,vsIntersec);
  p->iiAddCproc("","getnabt",FALSE,nabtvl);
  p->iiAddCproc("","idmodulo",FALSE,idModulo);
  p->iiAddCproc("","ndegree",FALSE,newDegree);
  p->iiAddCproc("","nonf2f",FALSE,nonf2f);
  p->iiAddCproc("","ifIsom",FALSE,ifIsomorphism);
  p->iiAddCproc("","stellarsubdivision",FALSE,stellarsubdivision);
  p->iiAddCproc("","star",FALSE,stars);
  p->iiAddCproc("","numdim",FALSE,numdim);
  p->iiAddCproc("","dimsim",FALSE,dimsim);
  p->iiAddCproc("","bprime",FALSE,bprime);
  p->iiAddCproc("","remainpart",FALSE,stellarremain);
  p->iiAddCproc("","idminus",FALSE,idminus);
  p->iiAddCproc("","time1",FALSE,t1h);
}

extern "C" int SI_MOD_INIT(cohomo)(SModulFunctions* p)
{
  firstorderdef_setup(p);
  return MAX_TOK;
}
#endif
#endif
