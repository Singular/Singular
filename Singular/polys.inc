#ifdef DRING
// D=k[x,d,y] is the Weyl-Algebra [y], y commuting with all others
// M=k[x,x^(-1),y] is a D-module
// all x(1..n),d,x(1..n)^(-1),y(1..k) are considered as "ring variables" v(1..N)
// the map from x(i) to v:
//#define pdX(i)  (i)
// d(i)
//#define pdDX(i) (pdN+i)
// x(i)^(-1)
//#define pdIX(i) (pdN+i)
// y(i)
//#define pdY(i)  (pdN*2+i+1)
// a monomial m belongs to a D-module M iff pdDFlag(m)==0
// a monomial m belongs to an ideal in the Weyl-Algebra D iff pdDFlag(m)==1
//#define pdDFlag(m) ((m)->exp[pdN*2+1])

int pdN; /* the number of x(i) / d(i) / x(i)^(-1) */
int pdK; /* the number of y(i) */
/* pVariables=pdN*2+1+pdK */

void pdSetDFlagP(poly p, int i)
{
  while (p!=NULL)
  {
    pdSetDFlag(p,1);
    pSetm(p);
    pIter(p);
  }
}

extern int binom(int n, int r);
#define c(A,B) binom(A+B-1,A)
/*2
* the commutator relation of var number n and exps d and x
*/
poly comm(int n, short d, short x)
{
  poly e1=pOne();
  poly e2=pOne();

  pdSetDFlag(e1,1);
  pdSetDFlag(e2,1);
  number t;

  if (x==1)
  {
    pSetExp(e1,pdX(n),1);
    pSetExp(e1,pdDX(n),d);
    pSetm(e1);
    pNext(e1)=e2;
    pSetExp(e2,pdDX(n),d-1);
    t = nInit(d);
    pSetCoeff(e2,t);
    pSetm(e2);
  }
  else if (d==1)
  {
    pSetExp(e1,pdX(n),x);
    pSetExp(e1,pdDX(n),1);
    pSetm(e1);
    pNext(e1)=e2;
    pSetExp(e2,n,x-1);
    t = nInit(x);
    pSetCoeff(e2,t);
    pSetm(e2);
  }
  else
  {
    int i,j,k;
    int p;
    int tp;
    poly h=NULL;

    pSetExp(e1,pdX(n),x);
    pSetExp(e1,pdDX(n),d);
    pSetm(e1);
    for (j=1;j<=min(x,d);j++)
    {
      p=1;
      for(k=0;k<j;k++)
      {
        p *= (d-k);
      }
      pSetExp(e2,pdX(n),x-j);
      pSetExp(e2,pdDX(n),d-j);
      pSetm(e2);
      tp=0;
      for(i=0;i<=x-j;i++)
      {
	tp+=c(i,j);
      }
      t = nInit(p*tp);
      pSetCoeff(e2,t);
      h=pAdd(h,e2);
      e2=pOne();
    }
    pNext(e1)=h;
    pFree1(e2);
  }
  return e1;
}
#undef c

/*2
* multiply 2 monomials (assume coeff of b == 1)
* DRING-case : a and b are operators, pdDFlag==1
*/
poly pMultDD(poly a, poly b)
{
  int i;
  poly resl=pOne();
  poly resr=pOne();
  poly multiply=NULL;
  short c; /* the component number of the result*/

  if((c=pGetComp(a))!=0)
  {
#ifdef TEST
    if (pGetComp(b)!=0)
    {
      Werror("mult vector * vector");
      return NULL;
    }
#endif
  }
  else
    c=pGetComp(b);

  pdSetDFlag(resl,1);
  pdSetDFlag(resr,1);
  pSetCoeff(resl,nCopy(pGetCoeff(a)));
  // put all x from a to the left result resl,
  // put all d from b to the right result resr
  for (i=1;i<=pdN;i++)
  {
    pSetExp(resl,pdX(i),pGetExp(a,pdX(i)));
    pSetExp(resr,pdDX(i),pGetExp(b,pdDX(i)));
  }
  // put all commutative vars y to the right result resr
  for (i=1;i<=pdK;i++)
  {
    pSetExp(resr,pdY(i),pGetExp(a,pdY(i))+pGetExp(b,pdY(i)));
  }
  // set the component number
  pSetComp(resl,c);
  for (i=1;i<=pdN;i++)
  {
    if ((a->exp[pdDX(i)] !=0) && (b->exp[pdX(i)] !=0))
    if ((pGetExp(a,pdDX(i)) !=0) && (pGetExp(b,pdX(i)) !=0))
    {
      if (multiply!=NULL)
      {
        multiply=pMult(multiply,comm(i,pGetExp(a,pdDX(i)),pGetExp(b,pdX(i))));
      }
      else
      {
        multiply=comm(i,pGetExp(a,pdDX(i)),pGetExp(b,pdX(i)));
      }
    }
    else
    {
      pSetExp(resl,pdX(i),pGetExp(resl,pdX(i))+pGetExp(b,pdX(i)));
      pSetExp(resr,pdDX(i),pGetExp(resr,pdDX(i))+pGetExp(a,pdDX(i)));
    }
  }
  pSetm(resl); /*poly or vector*/
  pSetm(resr); /*poly*/
  if (multiply!=NULL)
  {
    resl=pMult(resl,multiply);/*(poly or vector) * poly*/
    resl=pMult(resl,resr);
  }
  else
  {
    // now resl has only powers of x(i) and y(i), resr has only powers of d(i):
    for (i=1;i<=pdN;i++)
    {
      pSetExp(resl,pdDX(i),pGetExp(resr,pdDX(i)));
    }
    pSetm(resl);
    pFree1(resr);
  }
  return resl;
}

/*2
* multiply 2 monomials (assume coeff of b == 1)
* DRING-case : a is an operator, pdDFlag==1, b is in a D-module (DFlag==0)
*/
poly pMultDT(poly a, poly b)
{
  int i;
  short c; /* the component number of the result*/

  if((c=pGetComp(a))!=0)
  {
#ifdef TEST
    if (pGetComp(b)!=0)
    {
      Werror("mult vector * vector");
      return NULL;
    }
#endif
  }
  else
    c=pGetComp(b);

  // is the product 0 ?
  for (i=1;i<=pdN;i++)
  {
    if ((pGetExp(a,pdDX(i)) > pGetExp(b,pdX(i)))
    && (pGetExp(b,pdIX(i))==0))
      return NULL;
  }
  poly resl=pOne();
  pdSetDFlag(resl,0);
  pSetCoeff(resl,nCopy(pGetCoeff(a)));
  // put all x from a to the left result resl
  for (i=1;i<=pdN;i++)
  {
    pSetExp(resl,pdX(i),pGetExp(a,pdX(i)));
  }
  // put all commutative vars y to the left result resl
  for (i=1;i<=pdK;i++)
  {
    pSetExp(resl,pdY(i),pGetExp(a,pdY(i))+pGetExp(b,pdY(i)));
  }
  // set the component number
  pSetComp(resl,c);
  int q,p;
  number n,h1,h2;
  for (i=1;i<=pdN;i++)
  {
    if (((p=pGetExp(a,pdDX(i))) !=0) && ((q=pGetExp(b,pdX(i))) !=0))
    {
      // d^p(x^q): q*(q-1)*...*(q-p+1)* x^(q-p)
      pSetExp(resl,pdX(i),pGetExp(resl,pdX(i))+q-p);
      n=nInit(q);
      q--;p--;
      while (p>0)
      {
        h1=nInit(q);
        h2=nMult(n,h1);
        nDelete(&h1);
        nDelete(&n);
        n=h2;
        q--;
        p--;
      }
      h1=nMult(pGetCoeff(resl),n);
      pSetCoeff(resl,h1);
      nDelete(&n);
    }
    else
    if (((p=pGetExp(a,pdDX(i))) !=0) && ((q=pGetExp(b,pdIX(i))) !=0))
    {
      // d^p(x^(-q)): (-1)^p*q*(q+1)*...*(q+p-1)* x^(-(q+p))
      pSetExp(resl,pdIX(i),pGetExp(resl,pdIX(i))+q+p);
      if (p & 1) n=nInit(-q);
      else       n=nInit(q);
      // last index: p=q+p-1
      q++;p--;
      while (p>0)
      {
        h1=nInit(q);
        h2=nMult(n,h1);
        nDelete(&h1);
        nDelete(&n);
        n=h2;
        q++;p--;
      }
      h1=nMult(pGetCoeff(resl),n);
      pSetCoeff(resl,h1);
      nDelete(&n);
    }
    else
    {
      pSetExp(resl,pdX(i),pGetExp(resl,pdX(i))+pGetExp(b,pdX(i)));
      pSetExp(resl,pdIX(i),pGetExp(resl,pdIX(i))+pGetExp(b,pdIX(i)));
    }
  }
  for(i=1;i<=pdN;i++)
  {
    if (pGetExp(resl,pdX(i))>=pGetExp(resl,pdIX(i)))
    {
      pSetExp(resl,pdX(i),pGetExp(resl,pdX(i))-pGetExp(resl,pdIX(i)));
      pSetExp(resl,pdIX(i),0);
    }
    else
    {
      pSetExp(resl,pdIX(i),pGetExp(resl,pdIX(i))-pGetExp(resl,pdX(i)));
      pSetExp(resl,pdX(i),0);
    }
  }
  pSetm(resl); /*poly or vector*/
  return resl;
}

/*2
* multiply 2 monomials (assume coeff of b == 1)
* DRING-case : a and b is in a D-module (DFlag==0)
*/
poly pMultTT(poly a, poly b)
{
  int i;
  short c; /* the component number of the result*/

  if((c=pGetComp(a))!=0)
  {
#ifdef TEST
    if (pGetComp(b)!=0)
    {
      Werror("mult vector * vector");
      return NULL;
    }
#endif
  }
  else
    c=pGetComp(b);

  poly resl=pOne();
  int t;
  pdSetDFlag(resl,0);
  pSetCoeff(resl,nCopy(pGetCoeff(a)));
  for (i=1;i<=pdN;i++)
  {
    t=pGetExp(a,pdX(i))+pGetExp(b,pdX(i))-pGetExp(a,pdIX(i))-pGetExp(b,pdIX(i));
    if (t>=0)  pSetExp(resl,pdX(i),t);
    else       pSetExp(resl,pdIX(i),-t);
  }
  // put all commutative vars y to the result resl
  for (i=1;i<=pdK;i++)
  {
    pSetExp(resl,pdY(i),pGetExp(a,pdY(i))+pGetExp(b,pdY(i)));
  }
  // set the component number
  pSetComp(resl,c);
  pSetm(resl); /*poly or vector*/
  return resl;
}
#endif

#ifdef SRING
int psFirst(poly p, int i)
{
  loop
  {
    if (i>pVariables) return pVariables;
    if (pGetExp(p,i) != 0) return i;
    i++;
  }
}
#endif

#ifdef SRING
int psLast(poly p, int i)
{
  loop
  {
    if (i<pAltVars) return pAltVars;
    if (pGetExp(p,i) != 0) return i;
    i--;
  }
}
#endif

#ifdef SRING
/*2
* returns TRUE, if the product of the monomials p1
* and p2 is not zero
*/
BOOLEAN psMultTest(poly p1, poly p2)
{
  int i1,i2,j;

  if ((pGetComp(p1) !=0) && (pGetComp(p2) != 0))
    return FALSE;
  j=pAltVars;
  loop
  {
    if (j>pVariables) return TRUE;
    i1=pGetExp(p1,j);
    i2=pGetExp(p2,j);
    if ((i1==1) && (i2==1)) return FALSE;
    if ((i1>1) || (i2>1))
    {
      Werror("internal error in psMultTest");
      HALT();
    }
    j++;
  }
}
#endif

#ifdef SRING
/*2
* multiply  monomials a and b (without coeffs)
* take the coeff from a
*/
poly psMultM(poly a, poly b)
{
  int i, j;
  BOOLEAN positiv=TRUE;
  poly res=NULL;

  if (psMultTest(a,b))
  {
    res=pNew();
    memset(res,0,pMonomSize);
    pGetCoeff(res)=nCopy(pGetCoeff(a));
    i=pAltVars-1;
    do
    {
      i=psFirst(b,i+1);
      j=pVariables+1;
      loop
      {
        j=psLast(a,j-1);
        if (j>i)
          positiv= !positiv;
        else
          break;
      }
    } while (i != pVariables);
    for (i=0;i<=pVariables; i++)
    {
      pSetExp(res,i,pGetExp(a,i)+pGetExp(b,i));
    }
    pSetm(res);
    if (!positiv)
      pGetCoeff(res)=nNeg(pGetCoeff(res));
  }
  return res;
}
#endif

#ifdef SDRING
/*2
*puts a poly into a polyset,
*returns FALSE, if there is a trivial multiple in the set
*/
BOOLEAN psEnter(poly p,polyset *s, int *l, int *m)
{
  int i=*l;

  /*is there already a multiple in s ?*/
  pTest(p);
  //Print("psEnter:  ");pWrite0(p);
  for(;i > 0;i--)
  {
   pTest((*s)[i]);
   if (pComparePolys(p,(*s)[i]))
   {
     //Print("-- ist vielfaches von ");
     //pWrite((*s)[i]);
     pDelete(&p);
     return FALSE;
   }
  }
  /*is there enough space in s ?*/
  if (*l==(*m-1))
  {
    pEnlargeSet(s,*m,16);
    *m+=16;
  }
  (*l)++;
  //Print("++an pos %d\n",*l);
  (*s)[*l]=p;
  pTest(p);
  return TRUE;
}
#endif

#ifdef SRING
/*2
* create the augmentation set of p
* p and done should be destroyed
* done is the product of all vars already multiplied with
*/
//int auglev=0;
void psAug(poly p, poly done, polyset *s, int *l, int *m)
{
  int an;
  poly doneCopy, q;

  if (p==NULL)
  {
    pDelete(&done);
    return;
  }
//  auglev++;
//  Print("lev %d, aug of ",auglev); wrp(p); Print("\n");
  if (pSRING)
  for (an=pAltVars; an<=pVariables; an++)
  {
    if ((pGetExp(p,an)==1) && (pGetExp(done,an)==0))
    {
      q=pOne();
      doneCopy=pCopy(done);
      pSetExp(doneCopy,an,1);
      pSetExp(q,an,1);
      pSetm(q);
      q=pMult(q,pCopy(p));
      if (q!=NULL)
      {
//        Print("lev %d, to ",auglev); wrp(q); Print("\n");
        if (psEnter(q,s,l,m))
        {
          psAug(pCopy(q),doneCopy,s,l,m);
        }
        else
        {
          q=NULL;
          pDelete(&doneCopy);
        }
      }
    }
  }
//  Print("end aug");
//  auglev--;
  pDelete(&done);
  pDelete(&p);
}
#endif
#ifdef DRING
/*2
* create the augmentation set of p
* p and done should be destroyed
* done is the product of all vars already multiplied with
* x^i --> dx^(i+1) und x^-i --> dx*x^i
*/
void pdAug(poly p, polyset *s, int *l, int *m)
{
  int an;
  poly q, dif;

  if ((p==NULL) || pdDFlag(p)==1)
  {
    return;
  }
  //Print(" aug of ");  wrp(p); Print("\n");
  if (pDRING)
  for (an=1; an<=pdN; an++)
  {
    if(pGetExp(p,pdIX(an))==0)
    {
      q=pOne();
      pSetExp(q,pdDX(an),pGetExp(p,pdX(an))+1);
      pdSetDFlag(q,1);
      pSetm(q);
      //Print("1:  q ");wrp(q); Print("\n");
      q=pMult(q,pCopy(p));
      if (q!=NULL)
      {
          //Print("1:  to ");  wrp(q); Print("\n");
        if (psEnter(q,s,l,m))
        {
          pdAug(pCopy(q),s,l,m);
        }
        else
        {
          pDelete(&q);
        }
      }
    }
  }
  for (an=1; an<=pdN; an++)
  {
    if (pGetExp(p,pdIX(an))!=0)
    {
      q=pOne();
      dif=pOne();
      pSetExp(q,pdX(an),pGetExp(p,pdIX(an)));
      pSetExp(dif,pdDX(an),1);
      pdSetDFlag(q,1);
      pdSetDFlag(dif,1);
      pSetm(q);
      //Print("2: q ");wrp(q); Print("\n");
      pSetm(dif);
      //Print("2: dif ");wrp(dif);  Print("\n");
      q=pMult(q,pCopy(p));
      //wrp(q); Print("\n");
      q=pMult(dif,q);
      if (q!=NULL)
      {
        //Print("2: to ");wrp(q); Print("\n");
        if (psEnter(q,s,l,m))
        {
          pdAug(pCopy(q),s,l,m);
        }
        else
        {
          pDelete(&q);
        }
      }
    }
  }
//Print("end aug");Print("\n");
  pDelete(&p);
}
/*2
* returns the differential LCM of the head terms of a and b in *m
*/
void pdLcm(poly a, poly b, poly m)
{
  int i;
  for (i=2*pdN; i>=0; i--)
  {
     pSetExp(m,i, min(pGetExp(a,i),pGetExp(b,i)));
  }
  for (i=pdY(1); i<=pdK; i++)
  {
     pSetExp(m,i, max(pGetExp(a,i),pGetExp(b,i)));
  }
}

BOOLEAN pdIsConstantComp(poly p)
{
  if (!pDRING) return FALSE;

  int i;

  for (i=pdN;i>0;i--)
  {
    if (pGetExp(p,pdX(i))!=0) return FALSE;
    if (pGetExp(p,pdDX(i))!=0) return FALSE;
  }
  for (i=pdK;i>0;i--)
  {
    if (pGetExp(p,pdY(i))!=0) return FALSE;
  }
  return TRUE;
}


/*2
* returns the differential Spolynomial of a and b in *m
*/

poly pdSpolyCreate(poly p1,poly p2)
{
  poly m1=pOne();
  poly m2=pOne();
  poly pp1=p1,pp2=p2;
  number n1,n2;
  int i,j;
  int co=0;
  if (pGetComp(p1)!=pGetComp(p2))
  {
    if (pGetComp(p1)==0)
    {
      co=1;
      pSetCompP(p1,pGetComp(p2));
    }
    else
    {
      co=2;
      pSetCompP(p2,pGetComp(p1));
    }
  }
  for (i=1;i<=pdN;i++)
  {
    j=pGetExp(p2,pdX(i))-pGetExp(p1,pdX(i));
    if (j>0)
    {
      pSetExp(m2,pdDX(i),j);
    }
    else
    {
      pSetExp(m1,pdDX(i),(-j));
    }
  }
  for (i=1;i<=pdN;i++)
  {
    j=pGetExp(p2,pdIX(i))-pGetExp(p1,pdIX(i));
    if (j>0)
    {
      pSetExp(m2,pdX(i),j);
    }
    else
    {
      pSetExp(m1,pdX(i),(-j));
    }
  }
  for (i=1;i<=pdK;i++)
  {
    j=pGetExp(p2,pdY(i))-pGetExp(p1,pdY(i));
    if (j>0)
    {
      pSetExp(m1,pdY(i),j);
    }
    else
    {
      pSetExp(m2,pdY(i),(-j));
    }
  }
  pSetm(m1);
  pSetm(m2);

  p1=pMult(m1,pCopy(p1));
  p2=pMult(m2,pCopy(p2));

  n1=nCopy(pGetCoeff(p2));
  n2=nCopy(pGetCoeff(p1));

  pDelete1(&p1);
  pDelete1(&p2);

  n1=nNeg(n1);
  pMultN(p1,n2);
  nDelete(&n2);
  pMultN(p2,n1);
  nDelete(&n1);

  m1=pAdd(p1,p2);
  if (co==1) spModuleToPoly(pp1);
  else if (co==2) spModuleToPoly(pp2);
  return m1;
}
#endif
