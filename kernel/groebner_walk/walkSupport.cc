#include "kernel/mod2.h"

#include "misc/intvec.h"
#include "misc/int64vec.h"

#include "polys/monomials/ring.h"
#include "polys/prCopy.h"
#include "polys/matpol.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/groebner_walk/walkSupport.h"
#include "kernel/GBEngine/kstd1.h"

THREAD_VAR extern BOOLEAN overflow_error;

///////////////////////////////////////////////////////////////////
//Support functions for Groebner Walk and Fractal Walk
///////////////////////////////////////////////////////////////////
//v1.2 2004-06-17
///////////////////////////////////////////////////////////////////
//implemented by Henrik Strohmayer
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//tdeg
///////////////////////////////////////////////////////////////////
//Description: returns the normal degree of the input polynomial
///////////////////////////////////////////////////////////////////
//Uses: pTotaldegree
///////////////////////////////////////////////////////////////////

int tdeg(poly p)
{
  int res=0;
  if(p!=NULL) res=pTotaldegree(p);
  return(res);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getMaxTdeg
///////////////////////////////////////////////////////////////////
//Description: returns the maximum normal degree of the
//polynomials of the input ideal
///////////////////////////////////////////////////////////////////
//Uses: pTotaldegree
///////////////////////////////////////////////////////////////////

int getMaxTdeg(ideal I)
{
  int res=-1;
  int length=(int)I->ncols;
  for(int j=length-1;j>=0;j--)
  {
    if ((I->m)[j]!=NULL)
    {
      int temp=pTotaldegree((I->m)[j]);
      if(temp>res) {res=temp;}
    }
  }
  return(res);
}
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getMaxPosOfNthRow
///////////////////////////////////////////////////////////////////
//Description: returns the greatest integer of row n of the
//input intvec
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

int getMaxPosOfNthRow(intvec *v,int n)
{
  int res=0;
  assume( (0<n) && (n<=(v->rows())) );
  {
    int c=v->cols();
    int cc=(n-1)*c;
    res=abs((*v)[0+cc /*(n-1)*c*/]);
    for (int i=c-1;i>=0;i--)
    {
      int temp=abs((*v)[i+cc /*(n-1)*c*/]);
      if(temp>res){res=temp;}
    }
  }
 return(res);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getInvEps64
///////////////////////////////////////////////////////////////////
//Description: returns the inverse of epsilon (see relevant
//part of thesis for definition of epsilon)
///////////////////////////////////////////////////////////////////
//Uses: getMaxPosOfNthRow
///////////////////////////////////////////////////////////////////

int64 getInvEps64(ideal G,intvec *targm,int pertdeg)
{
  int n;
  int64 temp64;
  int64 sum64=0;
//think n=2 is enough (instead of n=1)
  for (n=pertdeg; n>1; n--)
  {
    temp64=getMaxPosOfNthRow(targm,n);
    sum64 += temp64;
  }
  int64 inveps64=getMaxTdeg(G)*sum64+1;

  //overflow test
  if( sum64!=0 && (((inveps64-1)/sum64)!=getMaxTdeg(G)) )
    overflow_error=11;

  return(inveps64);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//invEpsOk64
///////////////////////////////////////////////////////////////////
//Description: checks whether the input inveps64 is the same
//as the one you get from the current ideal I
///////////////////////////////////////////////////////////////////
//Uses: getInvEps64
///////////////////////////////////////////////////////////////////

int invEpsOk64(ideal I, intvec *targm, int pertdeg, int64 inveps64)
{
  int64 temp64=getInvEps64(I,targm,pertdeg);
  if (inveps64>=temp64)
  {
    return(1);
  }
  else
  {
    return(0);
  }
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getNthRow
///////////////////////////////////////////////////////////////////
//Description: returns an intvec containing the nth row of v
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

intvec* getNthRow(intvec *v, int n)
{
  int r=v->rows();
  int c=v->cols();
  intvec *res=new intvec(c);
  if((0<n) && (n<=r))
  {
    int cc=(n-1)*c;
    for (int i=0; i<c; i++)
    {
      (*res)[i]=(*v)[i+cc /*(n-1)*c*/ ];
    }
  }
  return(res);
}

int64vec* getNthRow64(intvec *v, int n)
{
  int r=v->rows();
  int c=v->cols();
  int64vec *res=new int64vec(c);
  if((0<n) && (n<=r))
  {
    int cc=(n-1)*c;
    for (int i=0; i<c; i++)
    {
      (*res)[i]=(int64)(*v)[i+cc /*(n-1)*c*/ ];
    }
  }
  return(res);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getTaun64
///////////////////////////////////////////////////////////////////
//Description: returns a list containing the nth perturbed vector
//and the inveps64 used to calculate the vector
///////////////////////////////////////////////////////////////////
//Uses: getNthRow,getInvEps64
///////////////////////////////////////////////////////////////////

void getTaun64(ideal G,intvec* targm,int pertdeg, int64vec** v64, int64 & i64)
{
  int64vec* taun64=getNthRow64(targm,1);
  int64vec *temp64,*add64;
  int64 inveps64=1;
  if (pertdeg>1) inveps64=getInvEps64(G,targm,pertdeg);

  int n;
  //temp64 is used to check for overflow:
  for (n=2; n<=pertdeg; n++)
  {
    if (inveps64!=1)
    {
      temp64=iv64Copy(taun64);
      (*taun64)*=inveps64;
      for(int i=0; i<rVar(currRing);i++)
      {
        if((*temp64)[i]!=0 && (((*taun64)[i])/((*temp64)[i]))!=inveps64)
        overflow_error=12;
      }
      delete temp64;
    }
    temp64=iv64Copy(taun64);
    add64=getNthRow64(targm,n);
    taun64=iv64Add(add64,taun64);
    for(int i=0; i<rVar(currRing);i++)
    {
      if( ( ((*temp64)[i]) > 0 ) && ( ((*add64)[i]) > 0 ) )
      {
        if( ((*taun64)[i]) < ((*temp64)[i])  )
          overflow_error=13;
      }
      if( ( ((*temp64)[i]) < 0 ) && ( ((*add64)[i]) < 0 ) )
      {
        if( ((*taun64)[i]) > ((*temp64)[i])  )
          overflow_error=13;
      }
    }
    delete temp64;
  }

  //lists taunlist64=makeTaunList64(taun64,inveps64);
  //return(taunlist64);
  *v64=taun64;
  i64=inveps64;
}

///////////////////////////////////////////////////////////////////
//scalarProduct64
///////////////////////////////////////////////////////////////////
//Description: returns the scalar product of int64vecs a and b
///////////////////////////////////////////////////////////////////
//Assume: Overflow tests assumes input has nonnegative entries
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

static inline int64  scalarProduct64(int64vec* a, int64vec* b)
{
  assume( a->length() ==  b->length());
  int i, n = a->length();
  int64 result = 0;
  int64 temp1,temp2;
  for(i=n-1; i>=0; i--)
  {
    temp1=(*a)[i] * (*b)[i];
    if((*a)[i]!=0 && (temp1/(*a)[i])!=(*b)[i]) overflow_error=1;

    temp2=result;
    result += temp1;

    //since both vectors always have nonnegative entries in init64
    //result should be >=temp2
    if(temp2>result) overflow_error=2;
  }

  return result;
}
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//init64
///////////////////////////////////////////////////////////////////
//Description: returns the initial form ideal of the input ideal
//I w.r.t. the weight vector currw64
///////////////////////////////////////////////////////////////////
//Uses: idealSize,getNthPolyOfId,leadExp64,pLength
///////////////////////////////////////////////////////////////////

ideal init64(ideal G,int64vec *currw64)
{
  int length=IDELEMS(G);
  ideal I=idInit(length,G->rank);
  int j;
  int64 leadingweight,templeadingweight;
  poly p=NULL;
  poly leadp=NULL;
  for (j=1; j<=length; j++)
  {
    p=getNthPolyOfId(G,j);
    int64vec *tt=leadExp64(p);
    leadingweight=scalarProduct64(currw64,tt /*leadExp64(p)*/);
    delete tt;
    while (p!=NULL)
    {
      tt=leadExp64(p);
      templeadingweight=scalarProduct64(currw64,tt /*leadExp64(p)*/);
      delete tt;
      if(templeadingweight==leadingweight)
      {
        leadp=pAdd(leadp,pHead(p));
      }
      if(templeadingweight>leadingweight)
      {
        pDelete(&leadp);
        leadp=pHead(p);
        leadingweight=templeadingweight;
      }
      pIter(p);
    }
    (I->m)[j-1]=leadp;
    p=NULL;
    leadp=NULL;
  }
  return(I);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//currwOnBorder64
///////////////////////////////////////////////////////////////////
//Description: returns TRUE if currw64 lies on the border of the
//groebner cone of the order w.r.t. which the reduced GB G
//is calculated, otherwise FALSE
///////////////////////////////////////////////////////////////////
//Uses: init64
///////////////////////////////////////////////////////////////////

BOOLEAN currwOnBorder64(ideal G, int64vec* currw64)
{
  ideal J=init64(G,currw64);
  int length=idealSize(J);
  BOOLEAN res=FALSE;
  for(int i=length; i>0; i--)
  {
    //if(pLength(getNthPolyOfId(J,i))>1)
    poly p=getNthPolyOfId(J,i);
    if ((p!=NULL) && (pNext(p)!=NULL))
    {
      res=TRUE;break;
    }
  }
  idDelete(&J);
  return res;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//noPolysWithMoreThanTwoTerms
///////////////////////////////////////////////////////////////////
//Description: returns TRUE if all polynomials of Gw are of length
//<=2, otherwise FALSE
///////////////////////////////////////////////////////////////////
//Uses: idealSize, getNthPolyOfId
///////////////////////////////////////////////////////////////////

BOOLEAN noPolysWithMoreThanTwoTerms(ideal Gw)
{
  int length=idealSize(Gw);
  for(int i=length; i>0; i--)
  {
    //if(pLength(getNthPolyOfId(Gw,i))>2)
    poly p=getNthPolyOfId(Gw,i);
    if ((p!=NULL) && (pNext(p)!=NULL) && (pNext(pNext(p))!=NULL))
    {
      return FALSE;
    }
  }
  return TRUE;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//DIFFspy
///////////////////////////////////////////////////////////////////
//Description: returns the length of the list to be created in
//DIFF
///////////////////////////////////////////////////////////////////
//Uses: idealSize,pLength,getNthPolyOfId
///////////////////////////////////////////////////////////////////

int DIFFspy(ideal G)
{
  int s=idealSize(G);
  int j;
  int temp;
  int sum=0;
  poly p;
  for (j=1; j<=s; j++)
  {
    p=getNthPolyOfId(G,j);
    if((temp=pLength(p))>0) {sum += (temp-1);}
  }
  return(sum);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//DIFF
///////////////////////////////////////////////////////////////////
//Description: returns a list of all differences of leading
//exponents and nonleading exponents of elements of the current
//GB (see relevant part of thesis for further details)
///////////////////////////////////////////////////////////////////
//Uses: DIFFspy,idealSize,getNthPolyOfId,leadExp,pLength
///////////////////////////////////////////////////////////////////

intvec* DIFF(ideal G)
{
  intvec  *v,*w;
  poly p;
  int s=idealSize(G);
  int n=rVar(currRing);
  int m=DIFFspy(G);
  intvec* diffm=new intvec(m,n,0);
  int j,l;
  int inc=0;
  for (j=1; j<=s; j++)
  {
    p=getNthPolyOfId(G,j);
    v=leadExp(p);
    pIter(p);
    while(p!=NULL)
    {
      inc++;
      intvec *lep=leadExp(p);
      w=ivSub(v,lep /*leadExp(p)*/);
      delete lep;
      pIter(p);
      for (l=1; l<=n; l++)
      {
        // setPosOfIM(diffm,inc,l,(*w)[l-1]);
        IMATELEM(*diffm,inc,l) =(*w)[l-1] ;
      }
      delete w;
    }
    delete v;
  }
  return(diffm);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//gett64
///////////////////////////////////////////////////////////////////
//Description: returns the t corresponding to the vector listw
//which contains a vector from the list returned by DIFF
///////////////////////////////////////////////////////////////////
//Uses: ivSize
///////////////////////////////////////////////////////////////////

void gett64(intvec* listw, int64vec* currw64, int64vec* targw64, int64 &tvec0, int64 &tvec1)
{
  int s=ivSize(listw);
  int j;
  int64 zaehler64=0;
  int64 nenner64=0;
  int64 temp1,temp2,temp3,temp4; //overflowstuff
  for(j=1; j<=s; j++)
  {

    temp3=zaehler64;
    temp1=((int64)((*listw)[j-1]));
    temp2=((*currw64)[j-1]);
    temp4=temp1*temp2;
    zaehler64=temp3-temp4;

    //overflow test
    if(temp1!=0 && (temp4/temp1)!=temp2) overflow_error=3;

    if( ( temp3<0 && temp4>0 ) || ( temp3>0 && temp4<0 ) )
    {
      int64 abs_t3=abs64(temp3);
      if( (abs_t3+abs64(temp4))<abs_t3 ) overflow_error=4;
    }

    //overflow test
    temp1=((*targw64)[j-1])-((*currw64)[j-1]);
    //this subtraction can never yield an overflow since both number
    //will always be positive
    temp2=((int64)((*listw)[j-1]));
    temp3=nenner64;
    temp4=temp1*temp2;
    nenner64=temp3+temp4;

    //overflow test
    if(temp1!=0 && ((temp1*temp2)/temp1)!=temp2) overflow_error=5;

    if( (temp3>0 && temp4>0) ||
      (temp3<0 && temp4<0)    )
    {
      int64 abs_t3=abs64(temp3);
      if( (abs_t3+abs64(temp4))<abs_t3 )
      {
        overflow_error=6;
      }
    }
  }

  if (nenner64==0)
  {
    zaehler64=2;
  }
  else
  {
    if ( (zaehler64<=0) && (nenner64<0) )
    {
      zaehler64=-zaehler64;
      nenner64=-nenner64;
    }
  }

  int64 g=gcd64(zaehler64,nenner64);

  tvec0=zaehler64/g;
  tvec1=nenner64/g;

}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//nextt64
///////////////////////////////////////////////////////////////////
//Description: returns the t determining the next weight vector
///////////////////////////////////////////////////////////////////
//Uses:
///////////////////////////////////////////////////////////////////

void nextt64(ideal G,int64vec* currw64,int64vec* targw64, int64 & tvec0, int64 & tvec1)
{
  intvec* diffm=DIFF(G);
  int s=diffm->rows();
  tvec0=(int64)2;
  tvec1=(int64)0;
  intvec *tt;
  for(int j=1; j<=s; j++)
  {
    tt=getNthRow(diffm,j);
    int64 temptvec0, temptvec1;
    gett64(tt,currw64,targw64,temptvec0, temptvec1);
    delete tt;

    //if tempt>0 both parts will be>0
    if ( (temptvec1!=0) //that tempt is defined
       &&
       (temptvec0>0) && (temptvec1>0) //that tempt>0
     )
    {
      if( ( (temptvec0) <= (temptvec1) ) //that tempt<=1
        &&
        ( ( (temptvec0) * (tvec1) ) <
          ( (temptvec1) * (tvec0) ) )
      )
      {  //that tempt<t
        tvec0=temptvec0;
        tvec1=temptvec1;
      }
    }
  }
  delete diffm;
  return;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//nextw64
///////////////////////////////////////////////////////////////////
//Uses:iv64Size,gcd,
///////////////////////////////////////////////////////////////////

int64vec* nextw64(int64vec* currw, int64vec* targw,
                  int64 nexttvec0, int64 nexttvec1)
{
  //to do (targw-currw)*tvec[0]+currw*tvec[1]
  int64vec* tempv;
  int64vec* nextweight;
  int64vec* a=iv64Sub(targw,currw);
  //no overflow can occur since both are>=0

  //to test overflow
  tempv=iv64Copy(a);
  *a *= (nexttvec0);
  for(int i=0; i<rVar(currRing); i++)
  {
    if( (nexttvec0) !=0 &&
        (((*a)[i])/(nexttvec0))!=((*tempv)[i]) )
    {
      overflow_error=7;
      break;
    }
  }
  delete tempv;
  int64vec* b=currw;
  tempv=iv64Copy(b);
  *b *= (nexttvec1);
  for(int i=0; i<rVar(currRing); i++)
  {
    if( (nexttvec1) !=0 &&
        (((*b)[i])/(nexttvec1))!=((*tempv)[i]) )
    {
      overflow_error=8;
      break;
    }
  }
  delete tempv;
  nextweight=iv64Add(a,b);

  for(int i=0; i<rVar(currRing); i++)
  {
    if( (((*a)[i])>=0 && ((*b)[i])>=0) ||
        (((*a)[i])<0 && ((*b)[i])<0) )
    {
      if( (abs64((*a)[i]))>abs64((*nextweight)[i]) ||
          (abs64((*b)[i]))>abs64((*nextweight)[i])
        )
      {
        overflow_error=9;
        break;
      }
    }
  }

  //to reduce common factors of nextweight
  int s=iv64Size(nextweight);
  int64 g,temp;
  g=(*nextweight)[0];
  for (int i=1; i<s; i++)
  {
    temp=(*nextweight)[i];
    g=gcd64(g,temp);
    if (g==1) break;
  }

  if (g!=1) *nextweight /= g;

  return(nextweight);
}

///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//FUNCTIONS NOT ORIGINATING FROM THE SINGULAR IMPLEMENTATION CODE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getNthPolyOfId
///////////////////////////////////////////////////////////////////
//Description: returns the nth poly of ideal I
///////////////////////////////////////////////////////////////////
poly getNthPolyOfId(ideal I,int n)
{
  if(0<n && n<=((int)I->ncols))
  {
    return (I->m)[n-1];
  }
  else
  {
    return(NULL);
  }
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//idealSize
///////////////////////////////////////////////////////////////////
//Description: returns the number of generator of input ideal I
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////
// #define idealSize(I) IDELEMS(I)
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//ivSize
///////////////////////////////////////////////////////////////////
//Description: returns the number of entries of v
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

// inline int ivSize(intvec* v){ return((v->rows())*(v->cols())); }

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//iv64Size
///////////////////////////////////////////////////////////////////
//Description: returns the number of entries of v
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

// int iv64Size(int64vec* v){ return((v->rows())*(v->cols())); }

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//leadExp
///////////////////////////////////////////////////////////////////
//Description: returns an intvec containg the exponet vector of p
///////////////////////////////////////////////////////////////////
//Uses: sizeof,omAlloc,omFree
///////////////////////////////////////////////////////////////////

intvec* leadExp(poly p)
{
  int N=rVar(currRing);
  int *e=(int*)omAlloc((N+1)*sizeof(int));
  p_GetExpV(p,e,currRing);
  intvec* iv=new intvec(N);
  for(int i=N;i>0;i--) { (*iv)[i-1]=e[i];}
  omFree(e);
  return(iv);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//leadExp64
///////////////////////////////////////////////////////////////////
//Description: returns an int64vec containing the exponent
//vector of p
///////////////////////////////////////////////////////////////////
//Uses: sizeof,omAlloc,omFree
///////////////////////////////////////////////////////////////////

int64vec* leadExp64(poly p)
{
  int N=rVar(currRing);
  int *e=(int*)omAlloc((N+1)*sizeof(int));
  p_GetExpV(p,e,currRing);
  int64vec* iv64=new int64vec(N);
  for(int i=N;i>0;i--) { (*iv64)[i-1]=(int64)e[i];}
  omFree(e);
  return(iv64);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//setPosOfIM
///////////////////////////////////////////////////////////////////
//Description: sets entry i,j of im to val
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

//void setPosOfIM(intvec* im,int i,int j,int val){
//  int r=im->rows();
//  int c=im->cols();
//  if( (0<i && i<=r) && (0<j && j<=c) ){
//    (*im)[(i-1)*c+j-1]=val;
//    }
//  return;
//}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//scalarProduct
///////////////////////////////////////////////////////////////////
//Description: returns the scalar product of intvecs a and b
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

static inline long  scalarProduct(intvec* a, intvec* b)
{
  assume( a->length() ==  b->length());
  int i, n = a->length();
  long result = 0;
  for(i=n-1; i>=0; i--)
    result += (*a)[i] * (*b)[i];
  return result;
}

///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//gcd
///////////////////////////////////////////////////////////////////
//Description: returns the gcd of a and b
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

int gcd(int a, int b)
{
  int r, p0 = a, p1 = b;
  if(p0 < 0)
    p0 = -p0;

  if(p1 < 0)
    p1 = -p1;
  while(p1 != 0)
  {
    r = p0 % p1;
    p0 = p1;
    p1 = r;
  }
  return p0;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//gcd64
///////////////////////////////////////////////////////////////////
//Description: returns the gcd of a and b
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

int64 gcd64(int64 a, int64 b)
{
  int64 r, p0 = a, p1 = b;
  if(p0 < 0)
    p0 = -p0;

  if(p1 < 0)
    p1 = -p1;

  while(p1 != ((int64)0) )
  {
    r = p0 % p1;
    p0 = p1;
    p1 = r;
  }

  return p0;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//abs64
///////////////////////////////////////////////////////////////////
//Description: returns the absolute value of int64 i
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

//int64 abs64(int64 i)
//{
//  if(i>=0)
//  return(i);
//else
//  return((-i));
//}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//makeTaunList64
///////////////////////////////////////////////////////////////////
//Description: makes a list of an int64vec and an int64
///////////////////////////////////////////////////////////////////
//Uses: omAllocBin
///////////////////////////////////////////////////////////////////

#if 0
lists makeTaunList64(int64vec *iv64,int64 i64)
{
  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);
  l->m[0].rtyp=INTVEC_CMD;
  l->m[1].rtyp=INT_CMD;
  l->m[0].data=(void *)iv64;
  l->m[1].data=(void *)i64;
  return l;
}
#endif

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//idStd
///////////////////////////////////////////////////////////////////
//Description: returns the GB of G calculated w.r.t. the order of
//currRing
///////////////////////////////////////////////////////////////////
//Uses: kStd,idSkipZeroes
///////////////////////////////////////////////////////////////////

ideal idStd(ideal G)
{
  ideal GG = kStd(G, NULL, testHomog, NULL);
  idSkipZeroes(GG);
  return GG;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//idInterRed
///////////////////////////////////////////////////////////////////
//Description: returns the interreduction of G
///////////////////////////////////////////////////////////////////
//Assumes: that the input is a GB
///////////////////////////////////////////////////////////////////
//Uses: kInterRed,idSkipZeroes
///////////////////////////////////////////////////////////////////

ideal idInterRed(ideal G)
{
  assume(G != NULL);

  ideal GG = kInterRedOld(G, NULL);
  idDelete(&G);
  return GG;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//matIdLift
///////////////////////////////////////////////////////////////////
//Description: yields the same reslut as lift in Singular
///////////////////////////////////////////////////////////////////
//Uses: idLift,idModule2formatedMatrix
///////////////////////////////////////////////////////////////////

matrix matIdLift(ideal Gomega, ideal M)
{
  ideal Mtmp = idLift(Gomega, M, NULL, FALSE, FALSE, TRUE, NULL);
  int rows=IDELEMS(Gomega);
  int cols=IDELEMS(Mtmp);
  matrix res=id_Module2formatedMatrix(Mtmp,rows,cols,currRing);
  return res;
}
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//rCopyAndChangeA
///////////////////////////////////////////////////////////////////
//Description: changes currRing to a copy of currRing with the
//int64vec w instead of the old one
///////////////////////////////////////////////////////////////////
//Assumes: that currRing is alterd as mentioned in rCopy0AndAddA
///////////////////////////////////////////////////////////////////
//Uses: rCopy0,rComplete,rChangeCurrRing,rSetWeightVec
///////////////////////////////////////////////////////////////////

void rCopyAndChangeA(int64vec* w)
{
  ring rnew=rCopy0(currRing);
  rComplete(rnew);
  rSetWeightVec(rnew,w->iv64GetVec());
  rChangeCurrRing(rnew);
}

///////////////////////////////////////////////////////////////////
//rGetGlobalOrderMatrix
///////////////////////////////////////////////////////////////////
//Description: returns a matrix corresponding to the order of r
///////////////////////////////////////////////////////////////////
//Assumes: the order of r is a combination of the orders M,lp,dp,
//Dp,wp,Wp,C
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

int64vec* rGetGlobalOrderMatrix(ring r)
{
  int n=rVar(r);
  int64vec* res=new int64vec(n,n,(int64)0);
  if (rHasLocalOrMixedOrdering(r)) return res;
  int pos1=0;
  int pos2=0;
  int i=0;
  while(r->order[i]!=0 && pos2<n)
  {
    pos2=pos2+r->block1[i] - r->block0[i];

    if(r->order[i]==ringorder_lp)
    {
      for(int j=pos1; j<=pos2; j++)
        (*res)[j*n+j]=(int64)1;
    }
    else if(r->order[i]==ringorder_dp)
    {
      for(int j=pos1;j<=pos2;j++)
        (*res)[pos1*n+j]=(int64)1;
      for(int j=1;j<=(pos2-pos1);j++)
        (*res)[(pos1+j)*n+(pos2+1-j)]=(int64)-1;
    }
    else if(r->order[i]==ringorder_Dp)
    {
      for(int j=pos1;j<=pos2;j++)
        (*res)[pos1*n+j]=(int64)1;
      for(int j=1;j<=(pos2-pos1);j++)
        (*res)[(pos1+j)*n+(pos1+j-1)]=(int64)1;
    }
    else if(r->order[i]==ringorder_wp)
    {
      int* weights=r->wvhdl[i];
      for(int j=pos1;j<=pos2;j++)
        (*res)[pos1*n+j]=(int64)weights[j-pos1];
      for(int j=1;j<=(pos2-pos1);j++)
        (*res)[(pos1+j)*n+(pos2+1-j)]=(int64)-1;
    }
    else if(r->order[i]==ringorder_Wp)
    {
      int* weights=r->wvhdl[i];
      for(int j=pos1;j<=pos2;j++)
        (*res)[pos1*n+j]=(int64)weights[j-pos1];
      for(int j=1;j<=(pos2-pos1);j++)
        (*res)[(pos1+j)*n+(pos1+j-1)]=(int64)1;
    }

    else if(r->order[0]==ringorder_M)
    {
      int* weights=r->wvhdl[0];
      for(int j=pos1;j<((pos2+1)*(pos2+1));j++)
        (*res)[j]=(int64)weights[j];
    }

    pos1=pos2+1;
    pos2=pos2+1;
    i++;
  }

  return(res);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//rGetGlobalOrderWeightVec
///////////////////////////////////////////////////////////////////
//Description: returns a weight vector corresponding to the first
//row of a matrix corresponding to the order of r
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

int64vec* rGetGlobalOrderWeightVec(ring r)
{
  int n=rVar(r);
  int64vec* res=new int64vec(n);

  if (rHasLocalOrMixedOrdering(r)) return res;

  int length;

  if(r->order[0]==ringorder_lp)
  {
    (*res)[0]=(int64)1;
  }
  else if( (r->order[0]==ringorder_dp) || (r->order[0]==ringorder_Dp) )
  {
    length=r->block1[0] - r->block0[0];
    for (int j=0;j<=length;j++)
      (*res)[j]=(int64)1;
  }
  else if( (r->order[0]==ringorder_wp) || (r->order[0]==ringorder_Wp) ||
      (r->order[0]==ringorder_a)  || (r->order[0]==ringorder_M)    )
  {
    int* weights=r->wvhdl[0];
    length=r->block1[0] - r->block0[0];
    for (int j=0;j<=length;j++)
      (*res)[j]=(int64)weights[j];
  }
  else if(  /*(*/ r->order[0]==ringorder_a64 /*)*/  )
  {
    int64* weights=(int64*)r->wvhdl[0];
    length=r->block1[0] - r->block0[0];
    for (int j=0;j<=length;j++)
      (*res)[j]=weights[j];
  }

  return(res);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//sortRedSB
///////////////////////////////////////////////////////////////////
//Description: sorts a reduced GB of an ideal after the leading
//terms of the polynomials with the smallest one first
///////////////////////////////////////////////////////////////////
//Assumes: that the given input is a minimal GB
///////////////////////////////////////////////////////////////////
//Uses:idealSize,idCopy,pLmCmp
///////////////////////////////////////////////////////////////////

ideal sortRedSB(ideal G)
{
  int s=idealSize(G);
  poly* m=G->m;
  poly p,q;
  for (int i=0; i<(s-1); i++)
  {
    for (int j=0; j<((s-1)-i); j++)
    {
      p=m[j];
      q=m[j+1];
      if (pLmCmp(p,q)==1)
      {
        m[j+1]=p;
        m[j]=q;
      }
    }
  }
  return(G);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//int64VecToIntVec
///////////////////////////////////////////////////////////////////
//Description: converts an int64vec to an intvec
// deletes the input
///////////////////////////////////////////////////////////////////
//Assumes: that the int64vec contains no entries larger than 2^32
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

intvec* int64VecToIntVec(int64vec* source)
{
  int r=source->rows();
  int c=source->cols();
  intvec* res=new intvec(r,c,0);
  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      (*res)[i*c+j]=(int)(*source)[i*c+j];
    }
  }
  delete source;
  return(res);
}

///////////////////////////////////////////////////////////////////
