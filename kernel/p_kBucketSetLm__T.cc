/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_KBucketSetLm__Template.cc
 *  Purpose: template for setting the Lm of a bucket
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id: p_kBucketSetLm__T.cc,v 1.2 2005-12-19 12:11:15 bricken Exp $
 *******************************************************************/
#undef USE_COEF_BUCKETS
#ifdef HAVE_COEF_BUCKETS
#define USE_COEF_BUCKETS
#endif

#ifdef USE_COEF_BUCKETS
#define MULTIPLY_BUCKET(B,I) do                                        \
  { if (B->coef[I]!=NULL)                                              \
    {                                                                  \
      B->buckets[I]=p_Mult_q(B->buckets[I],B->coef[I],B->bucket_ring); \
      B->coef[I]=NULL;                                                 \
    }                                                                  \
  } while(0)
#else
#define MULTIPLY_BUCKET(B,I)
#endif
#if 0
LINKAGE void p_kBucketSetLm(kBucket_pt bucket)
{
  int j = 0;
  poly lt;
  BOOLEAN zero = FALSE;
  ring r = bucket->bucket_ring;
  assume(bucket->buckets[0] == NULL && bucket->buckets_length[0] == 0);
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);
  poly p;

  do
  {
    j = 0;
    for (int i = 1; i<=bucket->buckets_used; i++)
    {
      if (bucket->buckets[i] != NULL)
      {
        MULTIPLY_BUCKET(bucket,i);
        p =  bucket->buckets[j];
        if (j == 0)
        {
          if (p != NULL) goto Greater;
          j = i;
          goto Continue;
        }
        assume(p != NULL);
        p_MemCmp(bucket->buckets[i]->exp, p->exp, length, ordsgn, goto Equal, goto Greater, goto Continue);

        Greater:
        {
          if (n_IsZero(pGetCoeff(p), r))
          {
            n_Delete(&pGetCoeff(p), r);
            pIter(bucket->buckets[j]);
            p_FreeBinAddr(p, r);
            (bucket->buckets_length[j])--;
          }
          j = i;
          goto Continue;
        }

        Equal:
        {
          MULTIPLY_BUCKET(bucket,i);
          number tn = pGetCoeff(p);
          pSetCoeff0(p, n_Add(pGetCoeff(bucket->buckets[i]), tn, r));
          n_Delete(&tn, r);
          p = bucket->buckets[i];
          pIter(bucket->buckets[i]);
          n_Delete(&pGetCoeff(p), r);
          p_FreeBinAddr(p, r);
          (bucket->buckets_length[i])--;
        }

        Continue:;
      }
    }
    p = bucket->buckets[j];
    if (j > 0 && n_IsZero(pGetCoeff(p), r))
    {
      n_Delete(&pGetCoeff(p), r);
      pIter(bucket->buckets[j]);
      p_FreeBinAddr(p, r);
      (bucket->buckets_length[j])--;
      j = -1;
    }
  }
  while (j < 0);

  if (j == 0)
  {
    return;
  }

  assume(bucket->buckets[j] != NULL);
  lt = bucket->buckets[j];
  bucket->buckets[j] = pNext(lt);
  bucket->buckets_length[j]--;
  pNext(lt) = NULL;
  bucket->buckets[0] = lt;
  bucket->buckets_length[0] = 1;

  kBucketAdjustBucketsUsed(bucket);
}
#else
LINKAGE void p_kBucketSetLm(kBucket_pt bucket)
{
  //int j = 0;
  poly lt;
  BOOLEAN zero = FALSE;
  ring r = bucket->bucket_ring;
  assume((bucket->buckets[0] == NULL) && (bucket->buckets_length[0] == 0) && (bucket->coef[0]==0));
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);
  poly p=NULL;
  while(p==NULL){
      int found=-1000;
       for (int i = 1; i<=bucket->buckets_used; i++)
        {
          if (bucket->buckets[i] != NULL)
          {
           
            if (p == NULL)
            {
                p=bucket->buckets[i];
                found=i;
                continue;
            }
            assume(p != NULL);
            p_MemCmp(bucket->buckets[i]->exp, p->exp, length, ordsgn, goto Continue, goto Greater, goto Continue);
            //assume(p_LmCmp(bucket->buckets[i],p,r)==1);
          Greater:
            //if (p_LmCmp(bucket->buckets[i],p,r)!=1) continue;
            found=i;
            p=bucket->buckets[i];
          Continue:;
          }
        }
      
      
      if (found<0) return;
      assume(p==bucket->buckets[found]);
      assume(p!=NULL);
      
      p=kBucketExtractLmOfBucket(bucket, found);
      assume(p!=NULL);
      p_Test(p,r);
      poly copy=p_LmInit(p, r);
      
      for (int i = found+1; i<=bucket->buckets_used; i++)
        {
          
          if (bucket->buckets[i] != NULL)
          {
            if(p_LmEqual(bucket->buckets[i], copy,r)){
                poly q=kBucketExtractLmOfBucket(bucket,i);
                assume(p!=q);
                p=p_Add_q(p, q,r);
                assume(pLength(bucket->buckets[i])==bucket->buckets_length[i]);
            }
            
    
            
          }
        }
        p_Delete(&copy, r);
  }
 
  //assume(bucket->buckets[j] != NULL);
  assume(pLength(p)==1);
  lt = p;
  
  bucket->buckets[0] = lt;
  bucket->buckets_length[0] = 1;

  kBucketAdjustBucketsUsed(bucket);
  kbTest(bucket);
  }
#endif