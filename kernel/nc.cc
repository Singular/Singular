#include "mod2.h"
#include <misc/auxiliary.h>

#include <misc/options.h>

#include <polys/simpleideals.h>
#include <polys/polys.h>

#include <polys/prCopy.h>

#include "ideals.h"
#include "kstd1.h"

#include "nc.h"

ideal twostd(ideal I) // works in currRing only!
{
  ideal J = kStd(I, currQuotient, testHomog, NULL, NULL, 0, 0, NULL); // in currRing!!!
  idSkipZeroes(J); // ring independent!

  const int rN = currRing->N;

  loop
  {
    ideal     K    = NULL;
    const int s    = idElem(J); // ring independent

    for(int i = 0; i < s; i++)
    {
      const poly p = J->m[i];

#ifdef PDEBUG
      p_Test(p, currRing);
#if 0
      Print("p: "); // !
      p_Write(p, currRing);
#endif
#endif

      for (int j = 1; j <= rN; j++) // for all j = 1..N
      {
        poly varj = p_One( currRing);
        p_SetExp(varj, j, 1, currRing);
        p_Setm(varj, currRing);

        poly q = pp_Mult_mm(p, varj, currRing); // q = J[i] * var(j),

#ifdef PDEBUG
        p_Test(varj, currRing);
        p_Test(p, currRing);
        p_Test(q, currRing);
#if 0
        Print("Reducing p: "); // !
        p_Write(p, currRing);
        Print("With q: "); // !
        p_Write(q, currRing);
#endif
#endif

        p_Delete(&varj, currRing);

        if (q != NULL)
        {
#ifdef PDEBUG
#if 0
          Print("Reducing q[j = %d]: ", j); // !
          p_Write(q, currRing);

          Print("With p:");
          p_Write(p, currRing);

#endif
#endif

          // bug: lm(p) may not divide lm(p * var(i)) in a SCA!
          if( p_LmDivisibleBy(p, q, currRing) )
            q = nc_ReduceSpoly(p, q, currRing);


#ifdef PDEBUG
          p_Test(q, currRing);
#if 0
          Print("reductum q/p: ");
          p_Write(q, currRing);

          // Print("With J!\n");
#endif
#endif

//          if( q != NULL)
          q = kNF(J, currQuotient, q, 0, KSTD_NF_NONORM); // in currRing!!!

#ifdef PDEBUG
          p_Test(q, currRing);
#if 0
          Print("NF(J/currQuotient)=> q: "); // !
          p_Write(q, currRing);
#endif
#endif
          if (q!=NULL)
          {
            if (p_IsConstant(q, currRing)) // => return (1)!
            {
              p_Delete(&q, currRing);
              id_Delete(&J, currRing);

              if (K != NULL)
                id_Delete(&K, currRing);

              ideal Q = idInit(1,1); // ring independent!
              Q->m[0] = p_One(currRing);

              return(Q);
            }

//            flag = false;

            // K += q:

            ideal Q = idInit(1,1); // ring independent
            Q->m[0]=q;

            if( K == NULL )
              K = Q;
            else
            {
              ideal id_tmp = idSimpleAdd(K, Q); // in currRing
              id_Delete(&K, currRing);
              id_Delete(&Q, currRing);
              K = id_tmp; // K += Q
            }
          }


        } // if q != NULL
      } // for all variables

    }

    if (K == NULL) // nothing new: i.e. all elements are two-sided
      return(J);
    // now we update GrBasis J with K
    //    iSize=IDELEMS(J);
#ifdef PDEBUG
    idTest(J); // in currRing!
#if 0
    Print("J:");
    idPrint(J);
    PrintLn();
#endif // debug
#endif



#ifdef PDEBUG
    idTest(K); // in currRing!
#if 0
    Print("+K:");
    idPrint(K);
    PrintLn();
#endif // debug
#endif


    int iSize = idElem(J); // ring independent

    // J += K:
    ideal id_tmp = idSimpleAdd(J,K); // in currRing
    id_Delete(&K, currRing); id_Delete(&J, currRing);

#if 1
    BITSET save_test=test;
    test|=Sy_bit(OPT_SB_1); // ring independent
    J = kStd(id_tmp, currQuotient, testHomog, NULL, NULL, 0, iSize); // J = J + K, J - std // in currRing!
    test = save_test;
#else
    J=kStd(id_tmp, currQuotient,testHomog,NULL,NULL,0,0,NULL);
#endif

    id_Delete(&id_tmp, currRing);
    idSkipZeroes(J); // ring independent

#ifdef PDEBUG
    idTest(J); // in currRing!
#if 0
    Print("J:");
    idPrint(J);
    PrintLn();
#endif // debug
#endif
  } // loop
}




static ideal idPrepareStd(ideal T, ideal s,  int k)
{
  // T is a left SB, without zeros, s is a list with zeros
#ifdef PDEBUG
  if (IDELEMS(s)!=IDELEMS(T))
  {
    Print("ideals of diff. size!!!");
  }
#endif
  ideal t = idCopy(T);
  int j,rs=id_RankFreeModule(s, currRing);
  poly p,q;

  ideal res = idInit(2*idElem(t),1+idElem(t));
  if (rs == 0)
  {
    for (j=0; j<IDELEMS(t); j++)
    {
      if (s->m[j]!=NULL) pSetCompP(s->m[j],1);
      if (t->m[j]!=NULL) pSetCompP(t->m[j],1);
    }
    k = si_max(k,1);
  }
  for (j=0; j<IDELEMS(t); j++)
  {
    if (s->m[j]!=NULL)
    {
      p = s->m[j];
      q = pOne();
      pSetComp(q,k+1+j);
      pSetmComp(q);
#if 0
      while (pNext(p)) pIter(p);
      pNext(p) = q;
#else
      p = pAdd(p,q);
      s->m[j] = p;
#ifdef PDEBUG
    pTest(p);
#endif
#endif
    }
  }
  res = idSimpleAdd(t,s);
  idDelete(&t);
  res->rank = 1+idElem(T);
  return(res);
}


ideal Approx_Step(ideal L)
{
  int N=currRing->N;
  int i,j; // k=syzcomp
  int flag, flagcnt=0, syzcnt=0;
  int syzcomp = 0;
  int k=1; // for ideals not modules
  ideal I = kStd(L, currQuotient,testHomog,NULL,NULL,0,0,NULL);
  idSkipZeroes(I);
  ideal s_I;
  int idI = idElem(I);
  ideal trickyQuotient;
  if (currQuotient !=NULL)
  {
    trickyQuotient = idSimpleAdd(currQuotient,I);
  }
  else
    trickyQuotient = I;
  idSkipZeroes(trickyQuotient);
  poly *var = (poly *)omAlloc0((N+1)*sizeof(poly));
  //  poly *W = (poly *)omAlloc0((2*N+1)*sizeof(poly));
  resolvente S = (resolvente)omAlloc0((N+1)*sizeof(ideal));
  ideal SI, res;
  matrix MI;
  poly x=pOne();
  var[0]=x;
  ideal   h2, h3, s_h2, s_h3;
  poly    p,q,qq;
  // init vars
  for (i=1; i<=N; i++ )
  {
    x = pOne();
    pSetExp(x,i,1);
    pSetm(x);
    var[i]=pCopy(x);
  }
  // init NF's 
  for (i=1; i<=N; i++ )
  {
    h2 = idInit(idI,1);
    flag = 0;
    for (j=0; j< idI; j++ )
    {
      q = pp_Mult_mm(I->m[j],var[i],currRing);
      q = kNF(I,currQuotient,q,0,0);
      if (q!=0)
      {
    h2->m[j]=pCopy(q);
    //  p_Shift(&(h2->m[flag]),1, currRing);
    flag++;
    pDelete(&q);
      }
      else
    h2->m[j]=0;
    }
    // W[1..idElems(I)] 
    if (flag >0)
    {
      // compute syzygies with values in I
      //      idSkipZeroes(h2);
      //      h2 = idSimpleAdd(h2,I);
      //      h2->rank=flag+idI+1;
      idTest(h2);
      //idShow(h2);
      ring orig_ring = currRing;
      ring syz_ring = rAssure_SyzComp(orig_ring, TRUE);
      syzcomp = 1;
      rSetSyzComp(syzcomp, syz_ring);
      if (orig_ring != syz_ring)
      {
        rChangeCurrRing(syz_ring);
        s_h2=idrCopyR_NoSort(h2,orig_ring, syz_ring);
        //  s_trickyQuotient=idrCopyR_NoSort(trickyQuotient,orig_ring);
        //  rDebugPrint(syz_ring);
        s_I=idrCopyR_NoSort(I,orig_ring, syz_ring);
      }
      else
      {
        s_h2 = h2;
        s_I  = I;
        //  s_trickyQuotient=trickyQuotient;
      }
      idTest(s_h2);
      //      idTest(s_trickyQuotient);
      Print(".proceeding with the variable %d\n",i);
      s_h3 = idPrepareStd(s_I, s_h2, 1);
      BITSET save_test=test;
      test|=Sy_bit(OPT_SB_1);
      idTest(s_h3);
      idDelete(&s_h2);
      s_h2=idCopy(s_h3);
      idDelete(&s_h3);
      Print("...computing Syz");
      s_h3 = kStd(s_h2, currQuotient,(tHomog)FALSE,NULL,NULL,syzcomp,idI);
      test=save_test;
      //idShow(s_h3);
      if (orig_ring != syz_ring)
      {
        idDelete(&s_h2);
        for (j=0; j<IDELEMS(s_h3); j++)
        {
          if (s_h3->m[j] != NULL)
          {
            if (p_MinComp(s_h3->m[j],syz_ring) > syzcomp) // i.e. it is a syzygy
              p_Shift(&s_h3->m[j], -syzcomp, currRing);
            else
              pDelete(&s_h3->m[j]);
          }
        }
        idSkipZeroes(s_h3);
        s_h3->rank -= syzcomp;
        rChangeCurrRing(orig_ring);
        //  s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
        s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
        rKill(syz_ring);
      }
      idTest(s_h3);
      S[syzcnt]=kStd(s_h3,currQuotient,(tHomog)FALSE,NULL,NULL);
      syzcnt++;
      idDelete(&s_h3);
    } // end if flag >0 
    else
    {
      flagcnt++;
    }
  }
  if (flagcnt == N)
  {
    Print("the input is a two--sided ideal");
    return(I);
  }
  if (syzcnt >0)
  {
    Print("..computing Intersect of %d modules\n",syzcnt);
    if (syzcnt == 1)
      SI = S[0];
    else
      SI = idMultSect(S, syzcnt);
    //idShow(SI);
    MI = id_Module2Matrix(SI,currRing);
    res= idInit(MATCOLS(MI),1);
    for (i=1; i<= MATCOLS(MI); i++)
    {
      p = NULL;
      for (j=0; j< idElem(I); j++)
      {
        q = pCopy(MATELEM(MI,j+1,i));
        if (q!=NULL)
        {
          q = pMult(q,pCopy(I->m[j]));
          p = pAdd(p,q);
        }
      }
      res->m[i-1]=p;
    }
    Print("final std");
    res = kStd(res, currQuotient,testHomog,NULL,NULL,0,0,NULL);
    idSkipZeroes(res);
    return(res);
  }
  else
  {
    Print("No syzygies");
    return(I);
  }
}

