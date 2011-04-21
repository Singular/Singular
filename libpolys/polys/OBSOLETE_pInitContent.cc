number pInitContent(poly ph);
number pInitContent_a(poly ph);

void p_Content(poly ph, const ring r)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    if (ph!=NULL)
    {
      number k = nGetUnit(pGetCoeff(ph));
      if (!nGreaterZero(pGetCoeff(ph))) k = nNeg(k); // in-place negation
      if (!nIsOne(k))
      {
        number tmpNumber = k;
        k = nInvers(k);
        nDelete(&tmpNumber);
        poly h = pNext(ph);
	p_Mult_nn(ph,k,currRing);
	pNormalize(ph);
      }
      nDelete(&k);
    }
    return;
  }
#endif
  number h,d;
  poly p;

  //  if(TEST_OPT_CONTENTSB) return;
  if(pNext(ph)==NULL)
  {
    pSetCoeff(ph,nInit(1));
  }
  else
  {
    nNormalize(pGetCoeff(ph));
    if(!nGreaterZero(pGetCoeff(ph))) ph = pNeg(ph);
    if (rField_is_Q())
    {
      h=pInitContent(ph);
      p=ph;
    }
    else if ((rField_is_Extension(r))
    && ((rPar(r)>1)||(r->cf->minpoly==NULL)))
    {
      h=pInitContent_a(ph);
      p=ph;
    }
    else
    {
      h=nCopy(pGetCoeff(ph));
      p = pNext(ph);
    }
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=n_Gcd(h,pGetCoeff(p),r);
      nDelete(&h);
      h = d;
      if(nIsOne(h))
      {
        break;
      }
      pIter(p);
    }
    p = ph;
    //number tmp;
    if(!nIsOne(h))
    {
      while (p!=NULL)
      {
        //d = nDiv(pGetCoeff(p),h);
        //tmp = nIntDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringAppendS("\n"));
        //}
        //nDelete(&tmp);
        if (rField_is_Zp_a(currRing) || rField_is_Q_a(currRing))
          d = nDiv(pGetCoeff(p),h);
        else
          d = nIntDiv(pGetCoeff(p),h);
        pSetCoeff(p,d);
        pIter(p);
      }
    }
    nDelete(&h);
#ifdef HAVE_FACTORY
    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    {
      singclap_divide_content(ph);
      if(!nGreaterZero(pGetCoeff(ph))) ph = pNeg(ph);
    }
#endif
    if (rField_is_Q_a(r))
    {
      number hzz = nlInit(1, r);
      h = nlInit(1, r);
      p=ph;
      while (p!=NULL)
      { // each monom: coeff in Q_a
        lnumber c_n_n=(lnumber)pGetCoeff(p);
        napoly c_n=c_n_n->z;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(hzz,pGetCoeff(c_n),r->algring);
          n_Delete(&hzz,r->algring);
          hzz=d;
          pIter(c_n);
        }
        c_n=c_n_n->n;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(h,pGetCoeff(c_n),r->algring);
          n_Delete(&h,r->algring);
          h=d;
          pIter(c_n);
        }
        pIter(p);
      }
      /* hzz contains the 1/lcm of all denominators in c_n_n->z*/
      /* h contains the 1/lcm of all denominators in c_n_n->n*/
      number htmp=nlInvers(h);
      number hzztmp=nlInvers(hzz);
      number hh=nlMult(hzz,h);
      nlDelete(&hzz,r->algring);
      nlDelete(&h,r->algring);
      number hg=nlGcd(hzztmp,htmp,r->algring);
      nlDelete(&hzztmp,r->algring);
      nlDelete(&htmp,r->algring);
      h=nlMult(hh,hg);
      nlDelete(&hg,r->algring);
      nlDelete(&hh,r->algring);
      nlNormalize(h);
      if(!nlIsOne(h))
      {
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a
          lnumber c_n_n=(lnumber)pGetCoeff(p);
          napoly c_n=c_n_n->z;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n));
            nlNormalize(d);
            nlDelete(&pGetCoeff(c_n),r->algring);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          c_n=c_n_n->n;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n));
            nlNormalize(d);
            nlDelete(&pGetCoeff(c_n),r->algring);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          pIter(p);
        }
      }
      nlDelete(&h,r->algring);
    }
  }
}

void pSimpleContent(poly ph,int smax)
{
  //if(TEST_OPT_CONTENTSB) return;
  if (ph==NULL) return;
  if (pNext(ph)==NULL)
  {
    pSetCoeff(ph,nInit(1));
    return;
  }
  if ((pNext(pNext(ph))==NULL)||(!rField_is_Q()))
  {
    return;
  }
  number d=pInitContent(ph);
  if (nlSize(d)<=smax)
  {
    //if (TEST_OPT_PROT) PrintS("G");
    return;
  }
  poly p=ph;
  number h=d;
  if (smax==1) smax=2;
  while (p!=NULL)
  {
#if 0
    d=nlGcd(h,pGetCoeff(p),currRing);
    nlDelete(&h,currRing);
    h = d;
#else
    nlInpGcd(h,pGetCoeff(p),currRing);
#endif
    if(nlSize(h)<smax)
    {
      //if (TEST_OPT_PROT) PrintS("g");
      return;
    }
    pIter(p);
  }
  p = ph;
  if (!nlGreaterZero(pGetCoeff(p))) h=nlNeg(h);
  if(nlIsOne(h)) return;
  //if (TEST_OPT_PROT) PrintS("c");
  //
  number inv=nlInvers(h);
  p_Mult_nn(p,inv,currRing);
  pNormalize(p);
  //while (p!=NULL)
  //{
#if 1
  //  d = nlIntDiv(pGetCoeff(p),h);
  //  pSetCoeff(p,d);
#else
  //  nlInpIntDiv(pGetCoeff(p),h,currRing);
#endif
  //  pIter(p);
  //}
  nlDelete(&inv,currRing);
  nlDelete(&h,currRing);
}

number pInitContent(poly ph)
// only for coefficients in Q
#if 0
{
  //assume(!TEST_OPT_CONTENTSB);
  assume(ph!=NULL);
  assume(pNext(ph)!=NULL);
  assume(rField_is_Q());
  if (pNext(pNext(ph))==NULL)
  {
    return nlGetNom(pGetCoeff(pNext(ph)),currRing);
  }
  poly p=ph;
  number n1=nlGetNom(pGetCoeff(p),currRing);
  pIter(p);
  number n2=nlGetNom(pGetCoeff(p),currRing);
  pIter(p);
  number d;
  number t;
  loop
  {
    nlNormalize(pGetCoeff(p));
    t=nlGetNom(pGetCoeff(p),currRing);
    if (nlGreaterZero(t))
      d=nlAdd(n1,t);
    else
      d=nlSub(n1,t);
    nlDelete(&t,currRing);
    nlDelete(&n1,currRing);
    n1=d;
    pIter(p);
    if (p==NULL) break;
    nlNormalize(pGetCoeff(p));
    t=nlGetNom(pGetCoeff(p),currRing);
    if (nlGreaterZero(t))
      d=nlAdd(n2,t);
    else
      d=nlSub(n2,t);
    nlDelete(&t,currRing);
    nlDelete(&n2,currRing);
    n2=d;
    pIter(p);
    if (p==NULL) break;
  }
  d=nlGcd(n1,n2,currRing);
  nlDelete(&n1,currRing);
  nlDelete(&n2,currRing);
  return d;
}
#else
{
  number d=pGetCoeff(ph);
  if(SR_HDL(d)&SR_INT) return d;
  int s=mpz_size1(d->z);
  int s2=-1;
  number d2;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return nlCopy(d);
      break;
    }
    if (SR_HDL(pGetCoeff(ph))&SR_INT)
    {
      s2=s;
      d2=d;
      s=0;
      d=pGetCoeff(ph);
      if (s2==0) break;
    }
    else
    if (mpz_size1((pGetCoeff(ph)->z))<=s)
    {
      s2=s;
      d2=d;
      d=pGetCoeff(ph);
      s=mpz_size1(d->z);
    }
  }
  return nlGcd(d,d2,currRing);
}
#endif

number pInitContent_a(poly ph)
// only for coefficients in K(a) anf K(a,...)
{
  number d=pGetCoeff(ph);
  int s=naParDeg(d);
  if (s /* naParDeg(d)*/ <=1) return naCopy(d);
  int s2=-1;
  number d2;
  int ss;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return naCopy(d);
      break;
    }
    if ((ss=naParDeg(pGetCoeff(ph)))<s)
    {
      s2=s;
      d2=d;
      s=ss;
      d=pGetCoeff(ph);
      if (s2<=1) break;
    }
  }
  return naGcd(d,d2,currRing);
}


//void pContent(poly ph)
//{
//  number h,d;
//  poly p;
//
//  p = ph;
//  if(pNext(p)==NULL)
//  {
//    pSetCoeff(p,nInit(1));
//  }
//  else
//  {
//#ifdef PDEBUG
//    if (!pTest(p)) return;
//#endif
//    nNormalize(pGetCoeff(p));
//    if(!nGreaterZero(pGetCoeff(ph)))
//    {
//      ph = pNeg(ph);
//      nNormalize(pGetCoeff(p));
//    }
//    h=pGetCoeff(p);
//    pIter(p);
//    while (p!=NULL)
//    {
//      nNormalize(pGetCoeff(p));
//      if (nGreater(h,pGetCoeff(p))) h=pGetCoeff(p);
//      pIter(p);
//    }
//    h=nCopy(h);
//    p=ph;
//    while (p!=NULL)
//    {
//      d=n_Gcd(h,pGetCoeff(p));
//      nDelete(&h);
//      h = d;
//      if(nIsOne(h))
//      {
//        break;
//      }
//      pIter(p);
//    }
//    p = ph;
//    //number tmp;
//    if(!nIsOne(h))
//    {
//      while (p!=NULL)
//      {
//        d = nIntDiv(pGetCoeff(p),h);
//        pSetCoeff(p,d);
//        pIter(p);
//      }
//    }
//    nDelete(&h);
//#ifdef HAVE_FACTORY
//    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
//    {
//      pTest(ph);
//      singclap_divide_content(ph);
//      pTest(ph);
//    }
//#endif
//  }
//}
#if 0
void p_Content(poly ph, ring r)
{
  number h,d;
  poly p;

  if(pNext(ph)==NULL)
  {
    pSetCoeff(ph,n_Init(1,r));
  }
  else
  {
    n_Normalize(pGetCoeff(ph),r);
    if(!n_GreaterZero(pGetCoeff(ph),r)) ph = p_Neg(ph,r);
    h=n_Copy(pGetCoeff(ph),r);
    p = pNext(ph);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r);
      d=n_Gcd(h,pGetCoeff(p),r);
      n_Delete(&h,r);
      h = d;
      if(n_IsOne(h,r))
      {
        break;
      }
      pIter(p);
    }
    p = ph;
    //number tmp;
    if(!n_IsOne(h,r))
    {
      while (p!=NULL)
      {
        //d = nDiv(pGetCoeff(p),h);
        //tmp = nIntDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringAppendS("\n"));
        //}
        //nDelete(&tmp);
        d = n_IntDiv(pGetCoeff(p),h,r);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
    }
    n_Delete(&h,r);
#ifdef HAVE_FACTORY
    //if ( (n_GetChar(r) == 1) || (n_GetChar(r) < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    //{
    //  singclap_divide_content(ph);
    //  if(!n_GreaterZero(pGetCoeff(ph),r)) ph = p_Neg(ph,r);
    //}
#endif
  }
}
#endif

poly p_Cleardenom(poly ph, const ring r)
{
  poly start=ph;
  number d, h;
  poly p;

#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    p_Content(ph,r);
    return start;
  }
#endif
  if (rField_is_Zp(r) && TEST_OPT_INTSTRATEGY) return start;
  p = ph;
  if(pNext(p)==NULL)
  {
    /*
    if (TEST_OPT_CONTENTSB)
    {
      number n=nGetDenom(pGetCoeff(p));
      if (!nIsOne(n))
      {
        number nn=nMult(pGetCoeff(p),n);
        nNormalize(nn);
        pSetCoeff(p,nn);
      }
      nDelete(&n);
    }
    else
    */
      pSetCoeff(p,nInit(1));
  }
  else
  {
    h = nInit(1);
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nLcm(h,pGetCoeff(p),currRing);
      nDelete(&h);
      h=d;
      pIter(p);
    }
    /* contains the 1/lcm of all denominators */
    if(!nIsOne(h))
    {
      p = ph;
      while (p!=NULL)
      {
        /* should be:
        * number hh;
        * nGetDenom(p->coef,&hh);
        * nMult(&h,&hh,&d);
        * nNormalize(d);
        * nDelete(&hh);
        * nMult(d,p->coef,&hh);
        * nDelete(&d);
        * nDelete(&(p->coef));
        * p->coef =hh;
        */
        d=nMult(h,pGetCoeff(p));
        nNormalize(d);
        pSetCoeff(p,d);
        pIter(p);
      }
      nDelete(&h);
      if (nGetChar()==1)
      {
        loop
        {
          h = nInit(1);
          p=ph;
          while (p!=NULL)
          {
            d=nLcm(h,pGetCoeff(p),currRing);
            nDelete(&h);
            h=d;
            pIter(p);
          }
          /* contains the 1/lcm of all denominators */
          if(!nIsOne(h))
          {
            p = ph;
            while (p!=NULL)
            {
              /* should be:
              * number hh;
              * nGetDenom(p->coef,&hh);
              * nMult(&h,&hh,&d);
              * nNormalize(d);
              * nDelete(&hh);
              * nMult(d,p->coef,&hh);
              * nDelete(&d);
              * nDelete(&(p->coef));
              * p->coef =hh;
              */
              d=nMult(h,pGetCoeff(p));
              nNormalize(d);
              pSetCoeff(p,d);
              pIter(p);
            }
            nDelete(&h);
          }
          else
          {
            nDelete(&h);
            break;
          }
        }
      }
    }
    if (h!=NULL) nDelete(&h);
  
    p_Content(ph,r);
#ifdef HAVE_RATGRING
    if (rIsRatGRing(r))
    {
      /* quick unit detection in the rational case is done in gr_nc_bba */
      pContentRat(ph);
      start=ph;
    }
#endif
  }
  return start;
}

void p_Cleardenom_n(poly ph,const ring r,number &c)
{
  number d, h;
  poly p;

  p = ph;
  if(pNext(p)==NULL)
  {
    c=nInvers(pGetCoeff(p));
    pSetCoeff(p,nInit(1));
  }
  else
  {
    h = nInit(1);
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nLcm(h,pGetCoeff(p),r);
      nDelete(&h);
      h=d;
      pIter(p);
    }
    c=h;
    /* contains the 1/lcm of all denominators */
    if(!nIsOne(h))
    {
      p = ph;
      while (p!=NULL)
      {
        /* should be:
        * number hh;
        * nGetDenom(p->coef,&hh);
        * nMult(&h,&hh,&d);
        * nNormalize(d);
        * nDelete(&hh);
        * nMult(d,p->coef,&hh);
        * nDelete(&d);
        * nDelete(&(p->coef));
        * p->coef =hh;
        */
        d=nMult(h,pGetCoeff(p));
        nNormalize(d);
        pSetCoeff(p,d);
        pIter(p);
      }
      if (nGetChar()==1)
      {
        loop
        {
          h = nInit(1);
          p=ph;
          while (p!=NULL)
          {
            d=nLcm(h,pGetCoeff(p),r);
            nDelete(&h);
            h=d;
            pIter(p);
          }
          /* contains the 1/lcm of all denominators */
          if(!nIsOne(h))
          {
            p = ph;
            while (p!=NULL)
            {
              /* should be:
              * number hh;
              * nGetDenom(p->coef,&hh);
              * nMult(&h,&hh,&d);
              * nNormalize(d);
              * nDelete(&hh);
              * nMult(d,p->coef,&hh);
              * nDelete(&d);
              * nDelete(&(p->coef));
              * p->coef =hh;
              */
              d=nMult(h,pGetCoeff(p));
              nNormalize(d);
              pSetCoeff(p,d);
              pIter(p);
            }
            number t=nMult(c,h);
            nDelete(&c);
            c=t;
          }
          else
          {
            break;
          }
          nDelete(&h);
        }
      }
    }
  }
}

number p_GetAllDenom(poly ph, const ring r)
{
  number d=n_Init(1,r);
  poly p = ph;

  while (p!=NULL)
  {
    number h=n_GetDenom(pGetCoeff(p),r);
    if (!n_IsOne(h,r))
    {
      number dd=n_Mult(d,h,r);
      n_Delete(&d,r);
      d=dd;
    }
    n_Delete(&h,r);
    pIter(p);
  }
  return d;
}

/*2
*tests if p is homogeneous with respect to the actual weigths
*/
BOOLEAN pIsHomogeneous (poly p)
{
  poly qp=p;
  int o;

  if ((p == NULL) || (pNext(p) == NULL)) return TRUE;
  pFDegProc d;
  if (pLexOrder && (currRing->order[0]==ringorder_lp))
    d=p_Totaldegree;
  else 
    d=pFDeg;
  o = d(p,currRing);
  do
  {
    if (d(qp,currRing) != o) return FALSE;
    pIter(qp);
  }
  while (qp != NULL);
  return TRUE;
}

/*2
*returns a re-ordered copy of a polynomial, with permutation of the variables
*/
poly pPermPoly (poly p, int * perm, const ring oldRing, nMapFunc nMap,
   int *par_perm, int OldPar)
{
  int OldpVariables = oldRing->N;
  poly result = NULL;
  poly result_last = NULL;
  poly aq=NULL; /* the map coefficient */
  poly qq; /* the mapped monomial */

  while (p != NULL)
  {
    if ((OldPar==0)||(rField_is_GF(oldRing)))
    {
      qq = pInit();
      number n=nMap(pGetCoeff(p));
      if ((currRing->cf->minpoly!=NULL)
      && ((rField_is_Zp_a()) || (rField_is_Q_a())))
      {
        nNormalize(n);
      }
      pGetCoeff(qq)=n;
    // coef may be zero:  pTest(qq);
    }
    else
    {
      qq=pOne();
      aq=napPermNumber(pGetCoeff(p),par_perm,OldPar, oldRing);
      if ((aq!=NULL) && (currRing->cf->minpoly!=NULL)
      && ((rField_is_Zp_a()) || (rField_is_Q_a())))
      {
        pNormalize(aq);
      }
      pTest(aq);
      if (aq==NULL)
        pSetCoeff(qq,nInit(0));
    }
    if (rRing_has_Comp(currRing)) pSetComp(qq, p_GetComp(p,oldRing));
    if (nIsZero(pGetCoeff(qq)))
    {
      pLmDelete(&qq);
    }
    else
    {
      int i;
      int mapped_to_par=0;
      for(i=1; i<=OldpVariables; i++)
      {
        int e=p_GetExp(p,i,oldRing);
        if (e!=0)
        {
          if (perm==NULL)
          {
            pSetExp(qq,i, e);
          }
          else if (perm[i]>0)
            pAddExp(qq,perm[i], e/*p_GetExp( p,i,oldRing)*/);
          else if (perm[i]<0)
          {
            if (rField_is_GF())
            {
              number c=pGetCoeff(qq);
              number ee=nfPar(1);
              number eee;nfPower(ee,e,&eee); //nfDelete(ee,currRing);
              ee=nfMult(c,eee);
              //nfDelete(c,currRing);nfDelete(eee,currRing);
              pSetCoeff0(qq,ee);
            }
            else
            {
              lnumber c=(lnumber)pGetCoeff(qq);
              if (c->z->next==NULL)
                napAddExp(c->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/);
              else /* more difficult: we have really to multiply: */
              {
                lnumber mmc=(lnumber)naInit(1,currRing);
                napSetExp(mmc->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/);
                napSetm(mmc->z);
                pGetCoeff(qq)=naMult((number)c,(number)mmc);
                nDelete((number *)&c);
                nDelete((number *)&mmc); 
              }
              mapped_to_par=1;
            }
          }
          else
          {
            /* this variable maps to 0 !*/
            pLmDelete(&qq);
            break;
          }
        }
      }
      if (mapped_to_par
      && (currRing->cf->minpoly!=NULL))
      {
        number n=pGetCoeff(qq);
        nNormalize(n);
        pGetCoeff(qq)=n;
      }
    }
    pIter(p);
#if 1
    if (qq!=NULL)
    {
      pSetm(qq);
      pTest(aq);
      pTest(qq);
      if (aq!=NULL) qq=pMult(aq,qq);
      aq = qq;
      while (pNext(aq) != NULL) pIter(aq);
      if (result_last==NULL)
      {
        result=qq;
      }
      else
      {
        pNext(result_last)=qq;
      }
      result_last=aq;
      aq = NULL;
    }
    else if (aq!=NULL)
    {
      pDelete(&aq);
    }
  }
  result=pSortAdd(result);
#else
  //  if (qq!=NULL)
  //  {
  //    pSetm(qq);
  //    pTest(qq);
  //    pTest(aq);
  //    if (aq!=NULL) qq=pMult(aq,qq);
  //    aq = qq;
  //    while (pNext(aq) != NULL) pIter(aq);
  //    pNext(aq) = result;
  //    aq = NULL;
  //    result = qq;
  //  }
  //  else if (aq!=NULL)
  //  {
  //    pDelete(&aq);
  //  }
  //}
  //p = result;
  //result = NULL;
  //while (p != NULL)
  //{
  //  qq = p;
  //  pIter(p);
  //  qq->next = NULL;
  //  result = pAdd(result, qq);
  //}
#endif
  pTest(result);
  return result;
}

