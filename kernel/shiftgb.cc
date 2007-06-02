/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: shiftgb.cc,v 1.1 2007-06-02 13:29:07 levandov Exp $ */
/*
* ABSTRACT: kernel: utils for shift GB and free GB
*/


ideal freegb(ideal I, int uptodeg, int lVblock)
{
}

poly pLPshift(poly p, int sh, int uptodeg, int lV)
{
  /* assume shift takes place */
  /* shifts the poly by sh */

  if (sh == 0) return(p); /* the zero shift */

  poly q = NULL;
  while (p!=NULL)
  {
    q = p_Add_q(q,pmLPshift(p,sh,uptodeg,lV));
    pIter(p);
  }

  /* int version: returns TRUE if it was successful */
}


poly pmLPshift(poly p, int sh, int uptodeg, int lV)
{
  /* pm is a monomial */

  if (sh == 0) return(p); /* the zero shift */

  int L = pmLastVblock(p,lV);
  if (L+sh > uptodeg)
  {
    return(NULL); /* violation, 2check */
  }
  int *e=(int *)omAlloc0((currRing->N)*sizeof(int));
  int *s=(int *)omAlloc0((currRing->N)*sizeof(int));
  pGetExpV(p,e);
  number c = pGetCoeff(p);
  int i,j;
  for (i=1; i<=currRing->N; i++)
  {
    if (e[j])
    {
      s[j+sh] = e[j]; /* actually 1 */
    }
  }
  poly m = pOne();
  pSetExpV(m,s);
  pSetCoeff0(m,c);
  freeT(e, currRing->N);
  freeT(s, currRing->N);
  /*  pSetm(m); */ /* done in the pSetExpV */
  return(m);
}

int pLastVblock(poly p, int lV)
{
  /* returns the number of maximal block */
  /* appearing among the monomials of p */
  poly q = pCopy(p); /* need it ? */
  int ans = 0; int ansnew;
  while (q!=NULL)
  {
    ansnew = pmLastVblock(q,lV);
    ans = si_max(ans,ansnew);
    pIter(q);
  }
  return(ans);
}

int pmLastVblock(poly p, int lV)
{
  /* for a monomial p, returns the number of the last block */
  /* where a nonzero exponent is sitting */
  int *e=(int *)omAlloc0((currRing->N)*sizeof(int));
  pGetExpV(p,e);
  int j,b;
  while ( (!e[j]) && (j>=1) ) j--;
  b = (int)(j/lV) + 1; /* the number of the block */
  return (b);
}

int isInV(poly p, int lV)
{
  if (lV<=0) return;
  /* returns 1 iff p is in V */
  /* that is in the same block there is only one nonzero exponent */
  /* lV = the length of V = the number of orig vars */
  int *e = (int *)omAlloc0((currRing->N)*sizeof(int));
  int  b = (int)(currRing->N)/lV; /* the number of blocks */
  int *B = (int *)omAlloc0((b)*sizeof(int)); /* the num of elements in a block */
  pGetExpV(p,e);
  int i,j;
  for (j=1; j<=b; j++)
  {
    /* we go through all the vars */
    /* by blocks in lV vars */
    for (i=(j-1)*lV + 1; i<= j*lV; i++)
    {
      if (!e[i]) B[j] = B[j]+1;
    }
  }
  j = b;
  while ( (!B[j]) && (j>=1)) j--;
  if (j==0)
  {
    /* it is a zero exp vector, which is in V */
    return(1);
  }
  /* now B[j] != 0 */
  for (j; j>=1; j--)
  {
    if (B[j]!=1)
    {
      return(0);
    }
  }
  return(1);
}

/* including the self pairs? */

/*1
* put the pairs (s[i],sh \dot p)  into the set B, ecart=ecart(p)
*/


void enterOnePairManyShifts (int i, poly p, int ecart, int isFromQ, kStrategy strat, int atR = -1, int uptodeg, int lV)
{

  int j;
  int lb = pLastVblock(p,lV);
  poly q;
  for (j=0; j<= uptodeg - lb; j++)
  {
    q = pLPshift(p,j,uptodeg,lV);
    enterOnePairShift(i, p, ecart, isFromQ, strat, -1, uptodeg, lV);
  }
}

/*2
* put the pair (s[i],p)  into the set B, ecart=ecart(p)
*/


void enterOnePairShift (int i, poly p, int ecart, int isFromQ, kStrategy strat, int atR = -1, int uptodeg, int lV)
{

  /* need additionally: int up_to_degree, poly V0 with the variables in (0)  or just the number lV = the length of the first block */
  /* should cycle through all shifts of s[i] until up_to_degree - lastVblock(s[i]) */
  /* that is create the pairs (f, s \dot g) for deg(s\dot g)= */

  assume(i<=strat->sl);
  if (strat->interred_flag) return;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

  pLcm(p,strat->S[i],Lp.lcm);
  pSetm(Lp.lcm);

  /* apply the V criterion */
  if (!isInV(Lp.lcm))
  {
    pLmFree(Lp.lcm);
    Lp.lcm=NULL;
    return;
  }


#ifdef HAVE_PLURAL
  const BOOLEAN bIsPluralRing = rIsPluralRing(currRing);
  const BOOLEAN bIsSCA        = rIsSCA(currRing) && strat->homog; // for prod-crit
  const BOOLEAN bNCProdCrit   = ( !bIsPluralRing || bIsSCA ); // commutative or homogeneous SCA
#else
  const BOOLEAN bIsPluralRing = FALSE;
  const BOOLEAN bIsSCA        = FALSE;
  const BOOLEAN bNCProdCrit   = TRUE;
#endif

  if (strat->sugarCrit && bNCProdCrit)
  {
    if((!((strat->ecartS[i]>0)&&(ecart>0)))
    && pHasNotCF(p,strat->S[i]))
    {
    /*
    *the product criterion has applied for (s,p),
    *i.e. lcm(s,p)=product of the leading terms of s and p.
    *Suppose (s,r) is in L and the leading term
    *of p divides lcm(s,r)
    *(==> the leading term of p divides the leading term of r)
    *but the leading term of s does not divide the leading term of r
    *(notice that this condition is automatically satisfied if r is still
    *in S), then (s,r) can be cancelled.
    *This should be done here because the
    *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
    *
    *Moreover, skipping (s,r) holds also for the noncommutative case.
    */
      strat->cp++;
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
    }
    else
      Lp.ecart = si_max(ecart,strat->ecartS[i]);
    if (strat->fromT && (strat->ecartS[i]>ecart))
    {
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
      /*the pair is (s[i],t[.]), discard it if the ecart is too big*/
    }
    /*
    *the set B collects the pairs of type (S[j],p)
    *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p)#lcm(r,p)
    *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
    *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
    */
    {
      j = strat->Bl;
      loop
      {
        if (j < 0)  break;
        compare=pDivComp(strat->B[j].lcm,Lp.lcm);
        if ((compare==1)
        &&(sugarDivisibleBy(strat->B[j].ecart,Lp.ecart)))
        {
          strat->c3++;
          if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
          {
            pLmFree(Lp.lcm);
            return;
          }
          break;
        }
        else
        if ((compare ==-1)
        && sugarDivisibleBy(Lp.ecart,strat->B[j].ecart))
        {
          deleteInL(strat->B,&strat->Bl,j,strat);
          strat->c3++;
        }
        j--;
      }
    }
  }
  else /*sugarcrit*/
  {
    if (bNCProdCrit)
    {
      // if currRing->nc_type!=quasi (or skew)
      // TODO: enable productCrit for super commutative algebras...
      if(/*(strat->ak==0) && productCrit(p,strat->S[i])*/
      pHasNotCF(p,strat->S[i]))
      {
      /*
      *the product criterion has applied for (s,p),
      *i.e. lcm(s,p)=product of the leading terms of s and p.
      *Suppose (s,r) is in L and the leading term
      *of p devides lcm(s,r)
      *(==> the leading term of p devides the leading term of r)
      *but the leading term of s does not devide the leading term of r
      *(notice that tis condition is automatically satisfied if r is still
      *in S), then (s,r) can be canceled.
      *This should be done here because the
      *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
      */
          strat->cp++;
          pLmFree(Lp.lcm);
          Lp.lcm=NULL;
          return;
      }
      if (strat->fromT && (strat->ecartS[i]>ecart))
      {
        pLmFree(Lp.lcm);
        Lp.lcm=NULL;
        return;
        /*the pair is (s[i],t[.]), discard it if the ecart is too big*/
      }
      /*
      *the set B collects the pairs of type (S[j],p)
      *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p)#lcm(r,p)
      *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
      *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
      */
      for(j = strat->Bl;j>=0;j--)
      {
        compare=pDivComp(strat->B[j].lcm,Lp.lcm);
        if (compare==1)
        {
          strat->c3++;
          if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
          {
            pLmFree(Lp.lcm);
            return;
          }
          break;
        }
        else
        if (compare ==-1)
        {
          deleteInL(strat->B,&strat->Bl,j,strat);
          strat->c3++;
        }
      }
    }
  }
  /*
  *the pair (S[i],p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
  if (strat->fromT && !TEST_OPT_INTSTRATEGY)
    pNorm(p);
  if ((strat->S[i]==NULL) || (p==NULL))
    return;
  if ((strat->fromQ!=NULL) && (isFromQ!=0) && (strat->fromQ[i]!=0))
    Lp.p=NULL;
  else
  {
    #ifdef HAVE_PLURAL
    if ( bIsPluralRing )
    {
      if(pHasNotCF(p, strat->S[i]))
      {
        if(ncRingType(currRing) == nc_lie)
        {
            // generalized prod-crit for lie-type
            strat->cp++;
            Lp.p = nc_p_Bracket_qq(pCopy(p),strat->S[i]);
        }
        else
        if( bIsSCA )
        {
            // product criterion for homogeneous case in SCA
            strat->cp++;
            Lp.p = NULL;
        }
        else
          Lp.p = nc_CreateSpoly(strat->S[i],p,currRing); // ?
      }
      else  Lp.p = nc_CreateSpoly(strat->S[i],p,currRing);
    }
    else
    #endif
    {
      Lp.p = ksCreateShortSpoly(strat->S[i],p, strat->tailRing);
    }
  }
  if (Lp.p == NULL)
  {
    /*- the case that the s-poly is 0 -*/
    if (strat->pairtest==NULL) initPairtest(strat);
    strat->pairtest[i] = TRUE;/*- hint for spoly(S^[i],p)=0 -*/
    strat->pairtest[strat->sl+1] = TRUE;
    /*hint for spoly(S[i],p) == 0 for some i,0 <= i <= sl*/
    /*
    *suppose we have (s,r),(r,p),(s,p) and spoly(s,p) == 0 and (r,p) is
    *still in B (i.e. lcm(r,p) == lcm(s,p) or the leading term of s does not
    *devide lcm(r,p)). In the last case (s,r) can be canceled if the leading
    *term of p devides the lcm(s,r)
    *(this canceling should be done here because
    *the case lcm(s,p) == lcm(s,r) is not covered in chainCrit)
    *the first case is handeled in chainCrit
    */
    if (Lp.lcm!=NULL) pLmFree(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = strat->S[i];
    Lp.p2 = p;

    if ( !bIsPluralRing )
      pNext(Lp.p) = strat->tail;

    if (atR >= 0)
    {
      Lp.i_r1 = strat->S_2_R[i];
      Lp.i_r2 = atR;
    }
    else
    {
      Lp.i_r1 = -1;
      Lp.i_r2 = -1;
    }
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);

    if (TEST_OPT_INTSTRATEGY)
    {
      if (!bIsPluralRing)
        nDelete(&(Lp.p->coef));
    }

    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
  }
}



/*3
*(s[0],h),...,(s[k],h) will be put to the pairset L
* additionally we put the pairs (h, s \sdot h) for s>=1 to L
*/
void initenterpairsShift (poly h,int k,int ecart,int isFromQ,kStrategy strat, int atR = -1)
{

  if ((strat->syzComp==0)
  || (pGetComp(h)<=strat->syzComp))
  {
    int j;
    BOOLEAN new_pair=FALSE;

    if (pGetComp(h)==0)
    {
      /* for Q!=NULL: build pairs (f,q),(f1,f2), but not (q1,q2)*/
      if ((isFromQ)&&(strat->fromQ!=NULL))
      {
        for (j=0; j<=k; j++)
        {
          if (!strat->fromQ[j])
          {
            new_pair=TRUE;
            enterOnePair(j,h,ecart,isFromQ,strat, atR);
          //Print("j:%d, Ll:%d\n",j,strat->Ll);
          }
        }
      }
      else
      {
        new_pair=TRUE;
        for (j=0; j<=k; j++)
        {
          enterOnePair(j,h,ecart,isFromQ,strat, atR);
        }
	/* HERE we put (h, s*h) pairs */
      }
    }
    else
    {
      for (j=0; j<=k; j++)
      {
        if ((pGetComp(h)==pGetComp(strat->S[j]))
        || (pGetComp(strat->S[j])==0))
        {
          new_pair=TRUE;
          enterOnePair(j,h,ecart,isFromQ,strat, atR);
        //Print("j:%d, Ll:%d\n",j,strat->Ll);
        }
      }
      /* HERE we put (h, s*h) pairs TOO */
    }

    if (new_pair) chainCrit(h,ecart,strat);

  }
}



ideal bbaShift(ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
#ifdef KDEBUG
  bba_count++;
  int loop_count = 0;
#endif
  om_Opts.MinTrack = 5;
  int   srmax,lrmax, red_result = 1;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;
  BOOLEAN withT = FALSE;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initHilbCrit(F,Q,&hilb,strat);
  initBba(F,strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  if (strat->minim>0) strat->M=idInit(IDELEMS(F),F->rank);
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;

#ifndef NO_BUCKETS
  if (!TEST_OPT_NOT_BUCKETS)
    strat->use_buckets = 1;
#endif

  // redtailBBa against T for inhomogenous input
  if (!K_TEST_OPT_OLDSTD)
    withT = ! strat->homog;

  // strat->posInT = posInT_pLength;
  kTest_TS(strat);

#ifdef HAVE_TAIL_RING
  kStratInitChangeTailRing(strat);
#endif

  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
#ifdef KDEBUG
    loop_count++;
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif
    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      /*
       *stops computation if
       * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
       *a predefined number Kstd1_deg
       */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg)))
        )
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    if (pNext(strat->P.p) == strat->tail)
    {
      // deletes the short spoly
      pLmFree(strat->P.p);
      strat->P.p = NULL;
      poly m1 = NULL, m2 = NULL;

      // check that spoly creation is ok
      while (strat->tailRing != currRing &&
             !kCheckSpolyCreation(&(strat->P), strat, m1, m2))
      {
        assume(m1 == NULL && m2 == NULL);
        // if not, change to a ring where exponents are at least
        // large enough
        kStratChangeTailRing(strat);
      }
      // create the real one
      ksCreateSpoly(&(strat->P), NULL, strat->use_buckets,
                    strat->tailRing, m1, m2, strat->R);
    }
    else if (strat->P.p1 == NULL)
    {
      if (strat->minim > 0)
        strat->P.p2=p_Copy(strat->P.p, currRing, strat->tailRing);
      // for input polys, prepare reduction
      strat->P.PrepareRed(strat->use_buckets);
    }

    if (strat->P.p == NULL && strat->P.t_p == NULL)
    {
      red_result = 0;
    }
    else
    {
      if (TEST_OPT_PROT)
        message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
                &olddeg,&reduc,strat, red_result);

      /* reduction of the element choosen from L */
      red_result = strat->red(&strat->P,strat);
    }

    // reduction to non-zero new poly
    if (red_result == 1)
    {
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");

      // get the polynomial (canonicalize bucket, make sure P.p is set)
      strat->P.GetP(strat->lmBin);

      int pos=posInS(strat,strat->sl,strat->P.p,strat->P.ecart);

      // reduce the tail and normalize poly
      if (TEST_OPT_INTSTRATEGY)
      {
        strat->P.pCleardenom();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
        {
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
      }

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS("new s:");strat->P.wrp();PrintLn();}
#endif

      // min_std stuff
      if ((strat->P.p1==NULL) && (strat->minim>0))
      {
        if (strat->minim==1)
        {
          strat->M->m[minimcnt]=p_Copy(strat->P.p,currRing,strat->tailRing);
          p_Delete(&strat->P.p2, currRing, strat->tailRing);
        }
        else
        {
          strat->M->m[minimcnt]=strat->P.p2;
          strat->P.p2=NULL;
        }
        if (strat->tailRing!=currRing && pNext(strat->M->m[minimcnt])!=NULL)
          pNext(strat->M->m[minimcnt])
            = strat->p_shallow_copy_delete(pNext(strat->M->m[minimcnt]),
                                           strat->tailRing, currRing,
                                           currRing->PolyBin);
        minimcnt++;
      }

      // enter into S, L, and T
      //if ((!TEST_OPT_IDLIFT) || (pGetComp(strat->P.p) <= strat->syzComp))
        enterT(strat->P, strat);
        enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
      // posInS only depends on the leading term
      if ((!TEST_OPT_IDLIFT) || (pGetComp(strat->P.p) <= strat->syzComp))
      {
      strat->enterS(strat->P, pos, strat, strat->tl);
      }
      else
      {
      //  strat->P.Delete(); // syzComp test: it is in T
      }
      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
//      Print("[%d]",hilbeledeg);
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      if (strat->sl>srmax) srmax = strat->sl;
    }
    else if (strat->P.p1 == NULL && strat->minim > 0)
    {
      p_Delete(&strat->P.p2, currRing, strat->tailRing);
    }
#ifdef KDEBUG
    memset(&(strat->P), 0, sizeof(strat->P));
#endif
    kTest_TS(strat);
  }
#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif
  /* complete reduction of the standard basis--------- */
  if (TEST_OPT_SB_1)
  {
    int k=1;
    int j;
    while(k<=strat->sl)
    {
      j=0;
      loop
      {
        if (j>=k) break;
        clearS(strat->S[j],strat->sevS[j],&k,&j,strat);
        j++;
      }
      k++;
    }
  }

  if (TEST_OPT_REDSB)
  {
    completeReduce(strat);
    if (strat->completeReduce_retry)
    {
      // completeReduce needed larger exponents, retry
      // to reduce with S (instead of T)
      // and in currRing (instead of strat->tailRing)
      cleanT(strat);strat->tailRing=currRing;
      int i;
      for(i=strat->sl;i>=0;i--) strat->S_2_R[i]=-1;
      completeReduce(strat);
    }
  }

  /* release temp data-------------------------------- */
  exitBuchMora(strat);
  if (TEST_OPT_WEIGHTM)
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}
