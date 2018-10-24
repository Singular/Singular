/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Stainly
*/

#include "kernel/mod2.h"

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


// #include <kernel/structs.h>
// #include <kernel/polys.h>
//ADICHANGES:
#include <kernel/ideals.h>
// #include <kernel/GBEngine/kstd1.h>
// #include<gmp.h>
// #include<vector>

//# define omsai 1
//#if omsai
//#if HAVE_STANLEYREISNER
#if 1

#include<libpolys/polys/ext_fields/transext.h>
#include<libpolys/coeffs/coeffs.h>
#include<kernel/linear_algebra/linearAlgebra.h>
#include <coeffs/numbers.h>
#include <vector>
#include <Singular/ipshell.h>
#include <Singular/libsingular.h>







/******************************************************************************/





/***************************print***********************************************/
//print vector
void listprint(std::vector<int> vec)
{
  int i;
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


//print vector of vectors
void listsprint(std::vector<std::vector<int> > posMat)
{
  int i,j;
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



void id_print(ideal h)
{
  int i;
  for(i=0;i<IDELEMS(h);i++)
  {
    Print(" [%d]\n",i+1);
    pWrite(h->m[i]);
    PrintLn();
    //PrintS(",");
  }
}




/************************only for T^2**********************************/
void lpprint( std::vector<poly> pv)
{
  for(int i=0;i<pv.size();i++)
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




void lpsprint(std::vector<std::vector<poly> > pvs)
{
  for(int i=0;i<pvs.size();i++)
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






/*****************************About simplicial complex and stanly-reisner ring**************************/










//h1 minus h2
ideal idMinus(ideal h1,ideal h2)
{
  ideal h=idInit(1,1);
  int i,j,eq=0;
  for(i=0;i<IDELEMS(h1);i++)
  {
    eq=0;
    for(j=0;j<IDELEMS(h2);j++)
    {
      if(p_EqualPolys(h1->m[i],h2->m[j], currRing))
      {
        eq=1;
        break;
      }
    }
    if(eq==0)
    {
      idInsertPoly(h, h1->m[i]);
    }
  }
  idSkipZeroes(h);
  return h;
}



//If poly P is squarefree, returns 1
//returns 0 otherwise,
bool p_Ifsfree(poly P)
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

/*find all squarefree monomials of degree deg in ideal h*/
ideal sfreemon(ideal h,int deg)
{
  int i,j,t;
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
ideal id_sfmon()
{
  ideal asfmons,sfmons,mons,p;
  int j;
  mons=id_MaxIdeal(1, currRing);
  asfmons=sfreemon(mons,1);
  for(j=2;j<=rVar(currRing);j++)
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
//returns the complement of the ideal h
ideal id_complement(ideal h)
{
  int j;
  ideal i1=id_sfmon();
  ideal i2=idMinus(i1,h);
  return (i2);
}












//returns the monomials in the quotient ring R/(h1+h2) which have degree deg,
//and R is currRing
ideal qringadd(ideal h1, ideal h2,int deg)
{
  ideal h,qrh;
  int i;
  h=idAdd(h1,h2);
  qrh=scKBase(deg,h);
  return qrh;
}




//returns the maximal degree of the monomials in ideal h
int id_maxdeg(ideal h)
{
  int i,max;
  max=pTotaldegree(h->m[0]);
  for(i=1;i<IDELEMS(h);i++)
  {
    if(pTotaldegree(h->m[i])>max)
    max=pTotaldegree(h->m[i]);
  }
  return (max);
}

//input ideal h is the ideal associated to simplicial complex,
//and returns the Stanley-Reisner ideal(minimal generators)
ideal idsrRing(ideal h)
{
  int max,i,j,n;
  ideal hc=idCopy(h);
  //Print("This is the complement generators\n");
  //id_print(hc);
  ideal pp,qq,rsr,ppp;
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
    if(!idIs0(qq))
    {
      pp=qringadd(qq,rsr,n);
      ppp=sfreemon(pp,n);
  //Print("This is the quotient generators %d:\n",n);
  //id_print(ppp);
      rsr=idAdd(rsr,ppp);
  //Print("This is the current minimal set\n");
  //id_print(rsr);
    }
  }
  idSkipZeroes(rsr);
  //PrintS("This is the minimal generators:\n");
  //id_print(rsr);
  return rsr;
}



//returns  the set of subset of p
ideal SimFacset(poly p)
{
  int i,j;
  ideal h1,mons;
  int max=pTotaldegree(p);
  ideal id_re=idInit(1,1);
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

//complicated version(print the polynomial set which can make the input to be a simplex)
//input h is at least part of faces
bool IsSimplex(ideal h)
{
  int i,j,ifbreak=0;
  ideal id_re;
  ideal id_so=idCopy(h);
  int max=id_maxdeg(h);
  for(i=0;i<IDELEMS(h);i++)
  {
    id_re=SimFacset(h->m[i]);
    //id_print(id_re);
    if(!idIs0(id_re))
    {
      id_so=idAdd(id_so, id_re);
    }
  }
  idSkipZeroes(id_so);
  if(!idIs0(idMinus(id_so,id_re)))
  {
    PrintS("It is not simplex.\n");
    PrintS("This is the simplicial complex:\n");
    id_print(id_so);
    return false;
  }
  PrintS("It is simplex.\n");
  return true;
}


//input is the subset of the Stainly-Reisner ideal
//returns the faces
ideal complementsimplex(ideal h)
{
  int i,j;poly p;
  ideal h1=idInit(1,1);
  ideal pp;
  ideal h3=idInit(1,1);
  for(i=1;i<=rVar(currRing);i++)
  {
    p = pOne(); pSetExp(p, i, 2); pSetm(p); pSetCoeff(p, nInit(1));
    //pWrite(p);
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
  PrintS("This is the simplicial complex:\n");
  id_print(h3);
  return (h3);
}




/*************operations for vectors(sets)*********/

//returns true if integer n is in vector vec,
//otherwise returns false
bool IsinL(int a,std::vector<int> badset)
{
  int i;
  for(i=0;i<badset.size();i++)
  {
    if(a==badset[i])
    {
      return true;
    }
  }
  return false;
}





//intersection of vectors p and q, returns empty if they are disjoint
std::vector<int> vecIntersection(std::vector<int> p,std::vector<int> q)
{
  int i;
  std::vector<int> inte;
  for(i=0;i<p.size();i++)
  {
    if(IsinL(p[i],q))
      inte.push_back(p[i]);
  }
  //listprint(inte);
  return inte;
}









//returns true if vec1 is equal to vec2 (including the order)
bool vEv(std::vector<int> vec1,std::vector<int> vec2)
{
  int i,j;
  int lg1=vec1.size(),lg2=vec2.size();
  if(lg1!=lg2)
  {
    return false;
  }
  else
  {
    for(j=0;j<vec1.size();j++)
    {
      if(vec1[j]!=vec2[j])
        return false;
    }
  }
  return true;
}




//returns true if vec1 is contained in vec2
bool vsubset(std::vector<int> vec1,std::vector<int> vec2)
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
bool vEvl(std::vector<int> vec1,std::vector<int> vec2)
{
  if(vec1.size()==0 && vec2.size()==0)
    return true;
  if(vsubset(vec1,vec2)&&vsubset(vec2,vec1))
    return true;
  return false;
}


//the length of vec must be same to it of the elements of vecs (not strictly same)
//returns false if vec is not in vecs
bool vInvsl(std::vector<int> vec, std::vector<std::vector<int> > vecs)
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


//the length of vec must be same to it of the elements of vecs (strictly same)
//returns the position of vec in vecs,
//returns -1 if vec is not in vecs
int vInvs(std::vector<int> vec, std::vector<std::vector<int> > vecs)
{
  int i;
  for(i=0;i<vecs.size();i++)
  {
    if(vEv(vec,vecs[i]))
    {
      //Print("This is the %dth variable\n",i+1);
      return i+1;
    }
  }
  //Print("This is not the new variable\n");
  //listprint(vec);
  return -1;
}



//returns the union of two vectors(like the union of sets)
std::vector<int> vecUnion(std::vector<int> vec1, std::vector<int> vec2)
{
  std::vector<int> vec=vec1;
  int i;
  for(i=0;i<vec2.size();i++)
  {
    if(!IsinL(vec2[i],vec))
      vec.push_back(vec2[i]);
  }
  return vec;
}



std::vector<int> vecMinus(std::vector<int> vec1,std::vector<int> vec2)
{
  std::vector<int> vec;
  for(int i=0;i<vec1.size();i++)
  {
    if(!IsinL(vec1[i],vec2))
    {
      vec.push_back(vec1[i]);
    }
  }
  return vec;
}




/********************Procedures for T1(M method and N method) ***********/









//P should be monomial(not used) vector version of poly support(poly p)
std::vector<int> support1(poly p)
{
  int j;
  std::vector<int> supset;
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
std::vector<std::vector<int> >  supports(ideal h)
{
  std::vector<std::vector<int> > vecs;
  std::vector<int> vec;
  if(!idIs0(h))
  {
    for(int s=0;s<IDELEMS(h);s++)
    {
      //pWrite(h->m[s]);
      vec=support1(h->m[s]);
      vecs.push_back(vec);
    }
  }
  return vecs;
}





//h is ideal( monomial ideal) associated to simplicial complex
//returns the all the monomials x^b (x^b must be able to divide at least one monomial in Stanley-Reisner ring)
ideal findb(ideal h)
{
  ideal ib=id_sfmon();
  ideal nonf=id_complement(h);
  ideal bset=idInit(1,1);
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
ideal finda(ideal h,poly S,int ddeg)
{
  ideal aset=idInit(1,1);
  poly e=pOne();
  ideal h2=id_complement(h);
  poly p;
  int i,j,deg1=pTotaldegree(S);
  int tdeg=deg1+ddeg;
  if(tdeg!=0)
  {
    std::vector<int> v,bv=support1(S),in;
    std::vector<std::vector<int> > hvs=supports(h);
    ideal ia=id_MaxIdeal(tdeg, currRing);
    for(i=0;i<IDELEMS(ia);i++)
    {
    //p=support(ia->m[i]);
      v=support1(ia->m[i]);
      in=vecIntersection(v,bv);
      if(vInvsl(v,hvs)&&in.size()==0)
      {
        idInsertPoly(aset, ia->m[i]);
      }
    }
    //idInsertPoly(aset,e);
    idSkipZeroes(aset);
  }
  else idInsertPoly(aset,e);
  return(aset);
}








//returns true if support(p) union support(a) minus support(b) is face,
//otherwise returns false
//not be used yet, (the vector version of mabcondition)
bool mabconditionv(std::vector<std::vector<int> > hvs,std::vector<int> pv,std::vector<int> av,std::vector<int> bv)
{
  std::vector<int> uv=vecUnion(pv,av);
  uv=vecMinus(uv,bv);
//Print("this is the support of p union a minus b\n");
//listprint(uv);
  //Print("this is the support of ideal\n");
  //listsprint(hvs);
  if(vInvsl(uv,hvs))
  {
    return(true);
  }
  return(false);
}


std::vector<std::vector<int> > Mabv(ideal h,poly a,poly b)
{
  std::vector<int> pv;
  std::vector<std::vector<int> > vecs;
  //Print("this is the support of p\n");
  //listprint(pv);
  std::vector<int> av=support1(a);
//Print("this is the support of a\n");
//listprint(av);
  std::vector<int> bv=support1(b);
//Print("this is the support of b\n");
//listprint(bv);
  ideal h2=id_complement(h);
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<std::vector<int> > h2v=supports(h2);
  std::vector<int> vec;
  for(int i=0;i<h2v.size();i++)
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
//For solving the equations which has form of x_i-x_j.(equations got by T_1)
/***************************************************************************/



//subroutine for soleli1
std::vector<int> eli1(std::vector<int> eq1,std::vector<int> eq2)
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


//get triangular form(eqs.size()>0)
std::vector<std::vector<int> > soleli1( std::vector<std::vector<int> > eqs)
{
  int i,j;std::vector<std::vector<int> > re;
  std::vector<std::vector<int> >  pre=eqs,ppre;
  if(eqs.size()>0)
  {
    re.push_back(eqs[0]);
    pre.erase(pre.begin());
  }
  //listsprint(pre);
  std::vector<int> yaya;
  for(i=0;(i<re.size()) && (pre.size()>0);i++)
  {
    yaya=eli1(re[i],pre[0]);
    //listprint(yaya);
    re.push_back(yaya);
    for(j=1;j<pre.size();j++)
    {
      //listprint(pre[j]);
      ppre.push_back(eli1(re[i],pre[j]));
    }
    pre=ppre;
    ppre.resize(0);
  }
  return re;
}


// input is a set of equations who is of triangular form(every equations has a form of x_i-x_j) n is the number of variables
//get the free variables and the dimension
std::vector<int> freevars(int n,  std::vector<int> bset, std::vector<std::vector<int> > gset)
{
  int ql=gset.size();
  int bl=bset.size();
  std::vector<int> mvar;
  std::vector<int> fvar;
  int i;
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


//return the set of free variables except vnum
std::vector<int> fvarsvalue(int vnum, std::vector<int> fvars)
{
  int i;std::vector<int> fset=fvars;
  for(i=0;i<fset.size();i++)
  {
    if(fset[i]==vnum)
    {
      fset.erase(fset.begin()+i);
      return fset;
    }
  }
}





std::vector<std::vector<int> > vAbsorb( std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  int i,j,m;
  std::vector<int> badset=bset;
  int bl=badset.size();
  int gl=gset.size();
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






//the new variables are started from 1
std::vector<int> vecbase1(int num, std::vector<int> oset)
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
std::vector<int> make0(int n)
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
std::vector<int> make1(int n)
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
std::vector<int> ofindbases1(int num, int vnum, std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  int i,j,m;std::vector<std::vector<int> > goodset;
  std::vector<int> fvars=freevars(num,   bset,  gset);
  std::vector<int> zset=fvarsvalue(vnum, fvars);
  zset=vecUnion(zset,bset);
  std::vector<int> oset;
  oset.push_back(vnum);
  goodset=vAbsorb(oset, gset);
  oset=goodset[goodset.size()-1];
  goodset.erase(goodset.end());
  //PrintS("This is the 1 set:\n");
  //listprint(oset);
  //goodset=vAbsorb(zset, goodset);
  //zset=goodset[goodset.size()-1];
  //goodset.erase(goodset.end());
  //PrintS("This is the 0 set:\n");
  //listprint(zset);
  //Print("thet size of goodset is %ld\n", goodset.size());
  std::vector<int> base= vecbase1(num,  oset);
  return base;
}








//input gset must be the triangular form after zero absorbing according to the badset
//bset must be the zero set after absorbing
std::vector<std::vector<int> > ofindbases(int num,  std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  int i,j,m;
  std::vector<int> base1;
  std::vector<std::vector<int> > bases;
  std::vector<int> fvars=freevars(num,   bset,  gset);
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
std::vector<std::vector<int> > eli2(int num,std::vector<int> bset,std::vector<std::vector<int> > gset)
{
  int i,j;
  std::vector<int> badset;
  std::vector<std::vector<int> > goodset;
  std::vector<std::vector<int> > solve;
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

    goodset=soleli1(goodset);//get the triangular form of goodset
    //PrintS("the triangular form of the good set is:\n");
    solve=ofindbases(num,badset,goodset);
   // goodset.push_back(badset);
   //listsprint(goodset);
  }
  else
  {
    solve=ofindbases(num,bset,gset);
  }
  return solve;
}


/********************************************************************/


//convert the vector to a polynomial w.r.t. current ring
//vector vbase has length of currRing->N.
poly pMake(std::vector<int> vbase)
{
  int n=vbase.size();poly p,q=0;
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
ideal idMake(std::vector<std::vector<int> > vecs)
{
  int lv=vecs.size();poly p;
  int i,j;
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
ideal idmodulo(ideal h1,ideal h2)
{
  int i;
  ideal gb=kStd(h2,NULL,testHomog,NULL,NULL,0,0,NULL);
  ideal idq=kNF(gb,NULL,h1);
  idSkipZeroes(idq);
  return idq;
}

//returns the coeff of the monomial of polynomial p which involves the mth varialbe
//assume the polynomial p has form of y1+y2+...
int pcoef(poly p, int m)
{
  int i,j,co;poly q=pCopy(p);
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
bool vInp(int m,poly p)
{
  int i;
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
std::vector<int> vMake(poly p)
{
  int i;poly q=pCopy(p);
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
std::vector<std::vector<int> > vsMake(ideal h)
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
std::vector<std::vector<int> > vecqring(std::vector<std::vector<int> > vec1, std::vector<std::vector<int> > vec2)
{
  int i,j;
  ideal h1=idMake(vec1);
  //id_print(h1);
  ideal h2=idMake(vec2);
  //id_print(h2);
  ideal h=idmodulo(h1,h2);
  std::vector<std::vector<int> > vecs= vsMake(h);
  return vecs;
}

/***********************************************************/




//returns the link of face a in simplicial complex X
std::vector<std::vector<int> > links(poly a, ideal h)
{
  int i;
  std::vector<std::vector<int> > lk,X=supports(h);
  std::vector<int> U,In,av=support1(a);
  for(i=0;i<X.size();i++)
  {
    U=vecUnion(av,X[i]);
    //PrintS("**********************\n");
    //listprint(X[i]);
    //PrintS("The union of them is:\n");
    //listprint(U);
    In=vecIntersection(av,X[i]);
    //PrintS("The intersection of them is:\n");
    //listprint(In);
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




/*vector version
//returns the link of face a in simplicial complex X
std::vector<std::vector<int> > links(std::vector<int> a, std::vector<std::vector<int> > X)
{
  int i;
  std::vector<std::vector<int> > lk;
  std::vector<int> U;
  std::vector<int> In;
  for(i=0;i<X.size();i++)
  {
    U=vecUnion(a,X[i]);
    //PrintS("**********************\n");
    //listprint(X[i]);
    //PrintS("The union of them is:\n");
    //listprint(U);
    In=vecIntersection(a,X[i]);
    //PrintS("The intersection of them is:\n");
    //listprint(In);
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
*/



//input is a squarefree monomial p
//output is all the squarefree monomials which could divid p(including p itself?)
ideal psubset(poly p)
{
  int i,j;
  ideal h1,mons;
  int max=pTotaldegree(p);
  ideal id_re=idInit(1,1);
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
std::vector<std::vector<int> > listsinsertlist(std::vector<std::vector<int> > gset, int a, int b)
{
  std::vector<int> eq;
  eq.push_back(a);
  eq.push_back(b);
  gset.push_back(eq);
  return gset;
}





std::vector<int> makeequation(int i,int j, int t)
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
poly pMake3(std::vector<int> vbase)
{
  int n=vbase.size();poly p,q=0;
  int co=1;
  //equmab(n);
  for(int i=0;i<3;i++)
  {
    if(vbase[i]!=0)
    {
      if(i==1) co=-1;
      p = pOne();pSetExp(p, vbase[i], 1);pSetm(p);pSetCoeff(p, nInit(co));
      //Print("attention! ");pWrite(p);
    }
    else p=0;
      q = pAdd(q, p);
    co=1;
  }
  //PrintS("the polynomial according to the metrix M is:\n");
  //listprint(vbase);
  //pWrite(q);
  return q;
}


ideal idMake3(std::vector<std::vector<int> > vecs)
{
  ideal id_re=idInit(1,1);
  poly p;
  int i,lv=vecs.size();
  for(i=0;i<lv;i++)
  {
    //Print("The vector is:  ");
    //listprint(vecs[i]);
    p=pMake3(vecs[i]);
    //Print("The polynomial is:  ");
    //pWrite(p);
    idInsertPoly(id_re, p);
  }
  //PrintS("This is the metrix M:\n");
  //listsprint(vecs);
  //PrintS("the ideal according to metrix M is:\n");
  idSkipZeroes(id_re);
  return id_re;
}

/****************************************************************/

//change the current ring to a new ring which is in num new variables
void equmab(int num)
{
  int i,j;
  //Print("There are %d variables in total.\n",num);
  ring r=currRing;
  char** tt;
  coeffs cf=nCopyCoeff(r->cf);
  tt=(char**)omAlloc(num*sizeof(char *));
  for(i=0; i <num; i++)
  {
    tt[i] = (char*)omalloc(10); //if required enlarge it later
    sprintf (tt[i], "t(%d)", i+1);
    tt[i]=omStrDup(tt[i]);
    //Print("%s\n",tt[i]);
  }
  ring R=rDefault(cf,num,tt,ringorder_lp);
  idhdl h=enterid(omStrDup("Re"),0,RING_CMD,&IDROOT,FALSE);
  IDRING(h)=rCopy(R);
  rSetHdl(h);
}


//returns the trivial case of T^1
//b must only contain one variable
std::vector<int> subspace1(std::vector<std::vector<int> > mv, std::vector<int> bv)
{
  int i;
  int num=mv.size();
  std::vector<int> base;
  std::vector<int> pv;
  for(i=0;i<num;i++)
  {
    if(IsinL(bv[0],mv[i]))
      base.push_back(1);
    else
      base.push_back(0);
  }
  return base;
}






/****************************************************************/
//construct a monomial based on the support of it
//returns a squarefree monomial
poly pMaken(std::vector<int> vbase)
{
  int n=vbase.size();
  poly p,q=pOne();
  //equmab(n);
  for(int i=0;i<n;i++)
  {
      p = pOne();pSetExp(p, vbase[i], 1);pSetm(p);pSetCoeff(p, nInit(1));
      //Print("attention! ");pWrite(p);
      q=pp_Mult_mm(q,p,currRing);
  }
  //PrintS("the polynomial according to the metrix M is:\n");
  //listprint(vbase);
  //pWrite(q);
  return q;
}

// returns a ideal according to a set of supports
ideal idMaken(std::vector<std::vector<int> > vecs)
{
  ideal id_re=idInit(1,1);
  poly p;
  int i,lv=vecs.size();
  for(i=0;i<lv;i++)
  {
    //Print("The vector is:  ");
    //listprint(vecs[i]);

    p=pMaken(vecs[i]);
    //Print("The polynomial is:  ");
    //pWrite(p);
    idInsertPoly(id_re, p);
  }
  //PrintS("This is the metrix M:\n");
  //listsprint(vecs);
  //PrintS("the ideal according to metrix M is:\n");
  idSkipZeroes(id_re);
  //id_print(id_re);
  return id_re;
}





/***************************only for T^2*************************************/
//vbase only has two elements which records the position of the monomials in mv


std::vector<poly> pMakei(std::vector<std::vector<int> > mv,std::vector<int> vbase)
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
 std::vector<std::vector<poly> > idMakei(std::vector<std::vector<int> > mv,std::vector<std::vector<int> > vecs)
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
void gradedpiece1(ideal h,poly a,poly b)
{
  int i,j;
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<int> av=support1(a);
  std::vector<int> bv=support1(b);
  ideal sub=psubset(b);
  std::vector<std::vector<int> > sbv=supports(sub);
  //ideal M=Mab(h,a,b);
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  PrintS("The homophisim is map onto the set:\n");
  id_print(idMaken(mv));
  int m=mv.size();
  std::vector<std::vector<int> > good;
  std::vector<int> bad,vv;
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
      PrintS("This is the solution of coefficients:\n");
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
      PrintS("This is the solution of coefficients:\n");
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
bool condition1for2(std::vector<int > pv,std::vector<int > qv,std::vector<int > bv)
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
bool condition2for2(std::vector<std::vector<int> > hvs, std::vector<int> pv,  std::vector<int> qv, std::vector<int> sv, std::vector<int> av,  std::vector<int> bv)
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






bool condition3for2(std::vector<std::vector<int> > hvs, std::vector<int> pv,  std::vector<int> qv,  std::vector<int> av,  std::vector<int> bv)
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

ideal getpresolve(ideal h)
{
  //ring r=currRing;
  int i;
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



std::vector<int> numfree(ideal h)
{
  int i,j,num=0;poly p;
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





std::vector<std::vector<int> > canonicalbase(int n)
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





std::vector<std::vector<int> > getvector(ideal h,int n)
{
  std::vector<int> vec;
  std::vector<std::vector<int> > vecs;
  ideal h2=idCopy(h);
  if(!idIs0(h))
  {
    ideal h1=getpresolve(h2);
    poly q,e=pOne();
    int lg=IDELEMS(h1);
    std::vector<int> fvar=numfree(h1);
    int n=fvar.size();
    //Print("*************&&&&&&&&&&&*******************There are %d free variables in total\n",n);
    int i,j,t;
    //Print("lg is %d\n",lg);
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
          //Print("The polynomial is the %dth one:\n",i+1);
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
              //Print("aiyamaya is %d \n",n_Int(pGetCoeff(q),currRing->cf));
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
std::vector<int> findalpha(std::vector<std::vector<int> > mv, std::vector<int> bv)
{
  std::vector<int> alset;
  for(int i=0;i<mv.size();i++)
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








std::vector<int> subspacet1(int num, std::vector<std::vector<int> > ntvs)
{
  int i,j,t;
  int n=ntvs.size();
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
std::vector<std::vector<int> > subspacet(std::vector<std::vector<int> > mv, std::vector<int> bv,std::vector<std::vector<int> > ntvs)
{
  int i,j;
  std::vector<int> alset=findalpha(mv,bv);
  std::vector<int> subase;
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





std::vector<std::vector<int> > mabtv(std::vector<std::vector<int> > hvs,  std::vector<std::vector<int> > Mv,   std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> v1,var;
  std::vector<std::vector<int> > vars;
  for(int i=0;i<Mv.size();i++)
  {
    for(int j=i+1;j<Mv.size();j++)
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
void gradedpiece2(ideal h,poly a,poly b)
{
  int t0,t1,t2,i,j,t;
  ring r=rCopy(currRing);
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<int> av=support1(a);
  std::vector<int> bv=support1(b);
  ideal sub=psubset(b);
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  std::vector<std::vector<int> > mts=mabtv(hvs,mv,av,bv);
  PrintS("The homomorphism should map onto:\n");
  lpsprint(idMakei(mv,mts));
  int m=mv.size();
  //ideal M=Mab(h,a,b);
  std::vector<std::vector<int> > vecs,vars;
  std::vector<int> vec,var;
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
bool nabconditionv(std::vector<std::vector<int> > hvs, std::vector<int> pv,  std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> vec1=vecIntersection(pv,bv);
  std::vector<int> vec2=vecUnion(pv,bv);
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
std::vector<std::vector<int> > Nabv(std::vector<std::vector<int> > hvs,  std::vector<int> av,  std::vector<int> bv)
{
  std::vector<std::vector<int> > vecs;
  int num=hvs.size();
  for(int i=0;i<num;i++)
  {
    if(nabconditionv(hvs,hvs[i],av,bv))
      vecs.push_back(hvs[i]);
  }
  return vecs;
}






//returns true if pv union qv union av minus bv is in hvs
//hvs is simplicial complex
bool nabtconditionv(std::vector<std::vector<int> > hvs,  std::vector<int> pv, std::vector<int> qv, std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> v1;
  v1=vecUnion(pv,qv);
  if(vInvsl(v1,hvs))
  {
    //PrintS("They are in Nab2\n");
    return (true);
  }
  //PrintS("They are not in Nab2\n");
  return (false);
}


//returns N_{a-b}^(2)
std::vector<std::vector<int> > nabtv(std::vector<std::vector<int> > hvs,    std::vector<std::vector<int> > Nv,   std::vector<int> av,  std::vector<int> bv)
{
  std::vector<int> v1,var;
  std::vector<std::vector<int> > vars;
  for(int i=0;i<Nv.size();i++)
  {
    for(int j=i+1;j<Nv.size();j++)
    {
      var.clear();
      if(nabtconditionv(hvs, Nv[i], Nv[j], av, bv))
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
bool tNab(std::vector<std::vector<int> > hvs, std::vector<int> pv, std::vector<std::vector<int> > bvs)
{
  std::vector<int> sv;
  if(bvs.size()<=1) return false;
  for(int i=0;i<bvs.size();i++)
  {
    sv=vecUnion(pv,bvs[i]);
    if(!vInvsl(sv,hvs))
    {
      //PrintS("TRUE! It is in tilde Nab!\n");
      return true;
    }
  }
  //PrintS("FALSE! It is not in tilde Nab!\n");
  return false;
}







std::vector<int>  tnab(std::vector<std::vector<int> > hvs,std::vector<std::vector<int> > nvs,std::vector<std::vector<int> > bvs)
{
  std::vector<int> pv;
  std::vector<int> vec;
  for(int j=0;j<nvs.size();j++)
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
std::vector<int> phimage(std::vector<int> pv,  std::vector<int> av, std::vector<int> bv)
{
  std::vector<int> qv=vecUnion(pv,av);
  qv=vecMinus(qv,bv);
  return qv;
}



//mvs and nvs are the supports of ideal Mab and Nab
//vecs is the solution of nab
std::vector<std::vector<int> > value1(std::vector<std::vector<int> > mvs, std::vector<std::vector<int> > nvs, std::vector<std::vector<int> > vecs,std::vector<int> av, std::vector<int> bv)
{
  std::vector<int> pv;
  std::vector<int> base;
  std::vector<std::vector<int> > bases;
  for(int t=0;t<vecs.size();t++)
  {
    for(int i=0;i<mvs.size();i++)
    {
      pv=phimage(mvs[i],av,bv);
      for(int j=0;j<nvs.size();j++)
      {
        if(vEvl(pv,nvs[j]))
          base.push_back(vecs[t][j]);
      }
    }
    if(base.size()!=mvs.size())
    {
      WerrorS("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");
      usleep(1000000);
      assert(false);
      WerrorS("Errors in Nab set!");
    }

    bases.push_back(base);
    base.clear();
  }
  return bases;
}









intvec *Tmat(std::vector<std::vector<int> > vecs)
{
    //std::vector<std::vector<int> > solve=gradedpiece1n(h,a,b);
   //Print("the size of solve is: %ld\n",solve.size());
 //vtm(solve);
  intvec *m;
  int i,j;
  int a=vecs.size();
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






/*void ppppp(int l)
{
  int i;
std::vector<std::vector<int> > vecs;
std::vector<int> vec;
  for(i=0;i<l;i++)
  {
    for(int j=i*l;j<i*l+l;j++)
    {
      vec.push_back(j);
    }
    vecs.push_back(vec);
    vec.clear();
  }
PrintS("May I have your attention please!\n");
listsprint(vecs);
    intvec *m=T1mat(vecs);
PrintS("May I have your attention again!\n");
  m->show(0,0);
}*/





std::vector<int> gensindex(ideal M, ideal ids)
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



ideal mingens(ideal h, poly a, poly b)
{
  int i;
  ideal hi=idInit(1,1);
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  ideal M=idMaken(mv);
//PrintS("mab\n");
  //id_print(M);
  //PrintS("idsrRing\n");
  //id_print(idsrRing(h));
  std::vector<int> index = gensindex(M, idsrRing(h));
//PrintS("index\n");
  //listprint(index);
  for(i=0;i<index.size();i++)
  {
    idInsertPoly(hi,M->m[index[i]]);
  }
  idSkipZeroes(hi);
//PrintS("over\n");
 // id_print(hi);
  return (hi);
}



std::vector<std::vector<int> >  minisolve(std::vector<std::vector<int> > solve,  std::vector<int> index)
{
  int i,j;
  std::vector<int> vec;
  std::vector<std::vector<int> > solsm;
  std::vector<int> solm;
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

intvec * gradedpiece1n(ideal h,poly a,poly b)
{
  int i,j,co;
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<int> av=support1(a);
  //listprint(av);
  std::vector<int> bv=support1(b);
  //listprint(bv);
  ideal sub=psubset(b);
//id_print(sub);
  std::vector<std::vector<int> > sbv=supports(sub);
//listsprint(sbv);
  std::vector<std::vector<int> > nv=Nabv(hvs,av,bv);
  //PrintS("The N set is:\n");
  //listsprint(nv);
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  //listsprint(mv);
  ideal M=idMaken(mv);
  std::vector<int> index = gensindex(M, idsrRing(h));
  //ideal gens=mingens(M,index);
  int n=nv.size();
  //PrintS("The homophisim is map onto the set:\n");
  //id_print(M);
  std::vector<std::vector<int> > good,solve;
  std::vector<int> bad;
  ring r=currRing;
  std::vector<int> tnv;
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
        if(nabtconditionv(hvs,nv[i],nv[j],av,bv))
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



/*
void vtm(std::vector<std::vector<int> > vecs)
{
  int i,j;

  intvec *m;

  int r=vecs.size();
  Print("r is %d\n",r);
 // c=(vecs[0]).size();

  PrintS("no error yet:\n");
  m=new intvec(r);

for(i=0;i<r;i++)
  {

    for(j=0;j<r;j++)
    {
   (*m)[i]=*(vecs[i]);
       Print("%d",IMATELEM(*m,i,j));
    }
  }
 // return matt;
}
*/





void T1(ideal h)
{
  ideal bi=findb(h),ai;
  int mm=0,index=0;
  id_print(bi);
  poly a,b;
  std::vector<std::vector<int> > solve;
  for(int i=0;i<IDELEMS(bi);i++)
  {
    PrintS("This is aset according to:");
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






bool condition2for2nv(std::vector<std::vector<int> > hvs, std::vector<int> pv, std::vector<int> qv,  std::vector<int> fv)
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
std::vector<int> findalphan(std::vector<std::vector<int> >  N, std::vector<int>  tN)
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
std::vector<std::vector<int> > subspacetn(std::vector<std::vector<int> >  N, std::vector<int>   tN, std::vector<std::vector<int> > ntvs)
{
  int i,j;
  std::vector<int> alset=findalphan(N,tN);
  std::vector<int> subase;
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
std::vector<std::vector<int> > value2(std::vector<std::vector<int> > mvs, std::vector<std::vector<int> > nvs, std::vector<std::vector<int> > mts, std::vector<std::vector<int> > nts, std::vector<std::vector<int> > vecs,std::vector<int> av,   std::vector<int> bv)
{
  std::vector<int> pv,qv;
  std::vector<int> base;
  int row,col;
  std::vector<std::vector<int> > bases;
  //PrintS("This is the nabt:\n");
  //listsprint(nts);
  //PrintS("nabt ends:\n");
  //PrintS("This is the mabt:\n");
  //listsprint(mts);
  //PrintS("mabt ends:\n");

  for(int t=0;t<vecs.size();t++)
  {
    for(int i=0;i<mts.size();i++)
    {
      row=mts[i][0];
      col=mts[i][1];
      pv=phimage(mvs[row],av,bv);
      qv=phimage(mvs[col],av,bv);
      if(vEvl(pv,qv))
        base.push_back(0);
      //PrintS("This is image of p and q:\n");
      //listprint(pv);  PrintS("*********************\n");listprint(qv);
      //PrintS("nabt ends:\n");
      else
      {
        for(int j=0;j<nts.size();j++)
        {
          row=nts[j][0];
          col=nts[j][1];
          //PrintS("This is nvs:\n");
          //listprint(nvs[row]); PrintS("*********************\n");listprint(nvs[col]);
          //PrintS("nabt ends:\n");
          if(vEvl(pv,nvs[row])&&vEvl(qv,nvs[col]))
          {
            base.push_back(vecs[t][j]);break;
            //PrintS("This is nvs,they are the same:\n");
            //listprint(nvs[row]); listprint(nvs[col]);
            //PrintS("nabt ends:\n");
          }
          else
          {
            base.push_back(-vecs[t][j]);break;
            //PrintS("This is nvs,they are the same:\n");
            //listprint(nvs[row]); listprint(nvs[col]);
            //PrintS("nabt ends:\n");
          }
        }
      }
    }
    if(base.size()!=mts.size())
    {
       WerrorS("Errors in Nab set!");
        //WerrorS("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");
      usleep(1000000);
      assert(false);
    }
    bases.push_back(base);
    base.clear();
  }
  return bases;
}




ideal genst(ideal h, poly a, poly b)
{
  int i,j;
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<int> av=support1(a), bv=support1(b);
  std::vector<std::vector<int> > mv=Mabv(h,a,b), mts=mabtv(hvs,mv,av,bv);
  std::vector<std::vector<poly> > pvs=idMakei(mv,mts);
  ideal gens=idInit(1,1);
  for(i=0;i<pvs.size();i++)
  {
    idInsertPoly(gens,pvs[i][0]);
    idInsertPoly(gens,pvs[i][1]);
  }
  idSkipZeroes(gens);
//PrintS("This is the mix set of mab2!\n");
//id_print(gens);
  return (gens);
}








intvec * gradedpiece2n(ideal h,poly a,poly b)
{
  int i,j,t;
  std::vector<std::vector<int> > hvs=supports(h);
  std::vector<int> av=support1(a);
  std::vector<int> bv=support1(b);
  ideal sub=psubset(b);
  std::vector<std::vector<int> > sbv=supports(sub);
  std::vector<std::vector<int> > nv=Nabv(hvs,av,bv);
  int n=nv.size();
  std::vector<int> tnv=tnab(hvs,nv,sbv);
  ring r=currRing;
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  std::vector<std::vector<int> > mts=mabtv(hvs,mv,av,bv);
  //PrintS("The relations are:\n");
  //listsprint(mts);
  //PrintS("The homomorphism should map onto:\n");
  //lpsprint(idMakei(mv,mts));
  std::vector<std::vector<int> > vecs,vars,ntvs;
  std::vector<int> vec,var;
  std::vector<std::vector<int> > solve;
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
        //pWrite(pMaken(nv[i]));pWrite(pMaken(nv[j]));
        //PrintS("They are both in tilde N.\n");
        //PrintS("tilde N is:\n");  listprint(tnv);
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







void T2(ideal h)
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
        gp++;
      }
    }
    mm=mm+1;
  }
  if(mm==IDELEMS(bi))
      PrintS("Finished!\n");
  Print("There are %d graded pieces in total.\n",gp);
}






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
















/****************************for the interface of .lib*********************************/

ideal makemab(ideal h, poly a, poly b)
{
  std::vector<std::vector<int> > mv=Mabv(h,a,b);
  ideal M=idMaken(mv);
  return M;
}


std::vector<int> v_minus(std::vector<int> v1, std::vector<int> v2)
{
  std::vector<int> vec;
  for(int i=0;i<v1.size();i++)
  {
    vec.push_back(v1[i]-v2[i]);
  }
  return vec;
}


std::vector<int> gdegree(poly a, poly b)
{
  int i,j;
  std::vector<int> av,bv;
  for(i=1;i<=currRing->N;i++)
  {
    av.push_back(pGetExp(a,i));
    bv.push_back(pGetExp(b,i));
  }
  std::vector<int> vec=v_minus(av,bv);
//PrintS("the degree is:\n");
//listprint(vec);
  return vec;
}




/**************************************interface T1****************************************/
/*
BOOLEAN makeqring(leftv res, leftv args)
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




BOOLEAN idcomplement(leftv res, leftv args)
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



BOOLEAN idsr(leftv res, leftv args)
{
  leftv h=args;
  if((h != NULL)&&(h->Typ() == IDEAL_CMD))
  {
     ideal h1= (ideal)h->Data();
//T1(h1);
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

intvec *dmat(poly a, poly b)
{
  intvec *m;
  int i,j;
  std::vector<int> dg=gdegree(a,b);
//PrintS("This is the degree of a and b\n");
//listprint(dg);
  int lg=dg.size();
  m=new intvec(lg);
  if(lg!=0)
  {
    m=new intvec(lg);
    for(i=0;i<lg;i++)
    {
        (*m)[i]=dg[i];
        //Print("This is the %dth degree of a and b: %d, and %d is copied\n",i,dg[i],(*m)[i]);
    }
  }
  /*for(j=0;j<lg;j++)
  {
    Print("[%d]: %d\n",j+1,(*m)[j]);
  }*/
  //(m)->show(1,1);
return (m);
}



BOOLEAN gd(leftv res, leftv args)
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



BOOLEAN fb(leftv res, leftv args)
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



BOOLEAN fa(leftv res, leftv args)
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


BOOLEAN fgp(leftv res, leftv args)
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



BOOLEAN genstt(leftv res, leftv args)
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


BOOLEAN sgp(leftv res, leftv args)
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


BOOLEAN Links(leftv res, leftv args)
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


/**************************************interface T2****************************************/



void firstorderdef_setup(SModulFunctions* p)
{
  p->iiAddCproc("","mg",FALSE,idsr);
  p->iiAddCproc("","gd",FALSE,gd);
  p->iiAddCproc("","findbset",FALSE,fb);
  p->iiAddCproc("","findaset",FALSE,fa);
  p->iiAddCproc("","fgp",FALSE,fgp);
  p->iiAddCproc("","idcomplement",FALSE,idcomplement);
  p->iiAddCproc("","genst",FALSE,genstt);
  p->iiAddCproc("","sgp",FALSE,sgp);
  p->iiAddCproc("","Links",FALSE,Links);
}



extern "C" int SI_MOD_INIT(cohomo)(SModulFunctions* p)
{
  firstorderdef_setup(p);
  return MAX_TOK;
}


#endif


