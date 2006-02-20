//! \file tgb.cc
//       multiple rings
//       shorten_tails und dessen Aufrufe pruefen wlength!!!
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tgb.cc,v 1.58 2006-02-20 11:57:03 bricken Exp $ */
/*
* ABSTRACT: slimgb and F4 implementation
*/
//#include <vector>
//using namespace std;
#include "mod2.h"
#include "tgb.h"
#include "tgb_internal.h"
#include "tgbgauss.h"
#include "F4.h"
#include "digitech.h"
#include "gring.h"

#include "longrat.h"
static const int bundle_size=100;
static const int delay_factor=3;
int QlogSize(number n);
#if 1
static omBin lm_bin=NULL;
//static const BOOLEAN up_to_radical=TRUE;

static int slim_nsize(number n, ring r) {
    if (rField_is_Zp(r)){
        return 1;
    }
 if (rField_is_Q(r)){
      return QlogSize(n);

  }
    else{
      return n_Size(n,r);
     
    }
}
static BOOLEAN monomial_root(poly m, ring r){
    BOOLEAN changed=FALSE;
    int i;
    for(i=1;i<=rVar(r);i++){
        int e=p_GetExp(m,i,r);
        if (e>1){
            p_SetExp(m,i,1,r);
            changed=TRUE;
        }
    }
    if (changed) {
        p_Setm(m,r);
    }
    return changed;
}
static BOOLEAN polynomial_root(poly h, ring r){
  poly got=gcd_of_terms(h,r);
  BOOLEAN changed=FALSE;
  if((got!=NULL) &&(TEST_V_UPTORADICAL)) {
    poly copy=p_Copy(got,r);
    //p_wrp(got,c->r);
    changed=monomial_root(got,r);
    if (changed)
    {
         poly div_by=pDivide(copy, got);
         poly iter=h;
         while(iter){
            pExpVectorSub(iter,div_by);
            pIter(iter);
         }
         p_Delete(&div_by, r);
         if (TEST_OPT_PROT)
             PrintS("U");
    }
    p_Delete(&copy,r);
  }
  p_Delete(&got,r);
  return changed;
}
static inline poly p_Init_Special(const ring r)
{
  return p_Init(r,lm_bin);
}
static inline poly pOne_Special(const ring r=currRing)
{
  poly rc = p_Init_Special(r);
  pSetCoeff0(rc,r->cf->nInit(1));
  return rc;
}
// zum Initialiseren: in t_rep_gb plazieren:

#endif
#define LEN_VAR3
#define degbound(p) assume(pTotaldegree(p)<10)
//#define inDebug(p) assume((debug_Ideal==NULL)||(kNF(debug_Ideal,NULL,p,0,0)==0))

//die meisten Varianten stossen sich an coef_buckets



#ifdef LEN_VAR1
// erste Variante: Laenge: Anzahl der Monome
inline int pSLength(poly p, int l) {
  return l; }
inline int kSBucketLength(kBucket* bucket, poly lm) {return bucket_guess(bucket);}
#endif

#ifdef LEN_VAR2
// 2. Variante: Laenge: Platz fuer die Koeff.
int pSLength(poly p,int l)
{
  int s=0;
  while (p!=NULL) { s+=nSize(pGetCoeff(p));pIter(p); }
  return s;
}
int kSBucketLength(kBucket* b, poly lm)
{
  int s=0;
  int i;
  for (i=MAX_BUCKET;i>=0;i--)
  {
    s+=pSLength(b->buckets[i],0);
  }
  return s;
}
#endif


// 3.Variante: Laenge: Platz fuer Leitk * Monomanzahl

int QlogSizeClassic(number bigint){
  if(nlIsZero(bigint)) return 0;
  number absv=nlCopy(bigint);
  number two=nlInit(2);
  if(!(nlGreaterZero(absv)))
    absv=nlNeg(absv);
  int ls=0;
  number comp=nlInit(1);
  BOOLEAN equal=FALSE;
  while(!(nlGreater(comp,absv))) {
    ls++;
    if (nlEqual(comp,absv)){
      equal=TRUE;
      break;
    }
    number temp=comp;
    comp=nlMult(comp,two);
    nlDelete(&temp, currRing);
    
  }
  if(!equal)
    ls++;
  nlDelete(&two,currRing);
  nlDelete(&absv,currRing);
  nlDelete(&comp,currRing);
  //Print("%d\n",ls);
  return ls;
}
int QlogSize(number bigint){
  int size=nlSize(bigint);
  if(size<=1) return QlogSizeClassic(bigint);
  else return sizeof(mp_limb_t)*size*8;
  }

#ifdef LEN_VAR3
inline int pSLength(poly p,int l)
{
  int c;
  number coef=pGetCoeff(p);
  if (rField_is_Q(currRing)){
    c=QlogSize(coef);
  }
  else
    c=nSize(coef);
  
  return c*l /*pLength(p)*/;
}
//! TODO CoefBuckets berücksichtigen
int kSBucketLength(kBucket* b, poly lm=NULL)
{
  int s=0;
  int c;
  number coef;
  if(lm==NULL)
    coef=pGetCoeff(kBucketGetLm(b));
    //c=nSize(pGetCoeff(kBucketGetLm(b)));
  else
    coef=pGetCoeff(lm);
    //c=nSize(pGetCoeff(lm));
  if (rField_is_Q(currRing)){
    c=QlogSize(coef);
  }
  else
    c=nSize(coef);
  
  int i;
  for (i=b->buckets_used;i>=0;i--)
  {
    assume((b->buckets_length[i]==0)||(b->buckets[i]!=NULL));
    s+=b->buckets_length[i] /*pLength(b->buckets[i])*/;
  }
  #ifdef HAVE_COEF_BUCKETS
  assume(b->buckets[0]==kBucketGetLm(b));
  if (b->coef[0]!=NULL){
    
    if (rField_is_Q(currRing)){
      int modifier=QlogSize(pGetCoeff(b->coef[0]));
      c+=modifier;
  }
    else{
      int modifier=nSize(pGetCoeff(b->coef[0]));
      c*=modifier;}
    }
  #endif
  return s*c;
}
#endif
#ifdef LEN_VAR5
inline wlen_type pSLength(poly p,int l)
{
  int c;
  number coef=pGetCoeff(p);
  if (rField_is_Q(currRing)){
    c=QlogSize(coef);
  }
  else
    c=nSize(coef);
  wlen_type erg=l;
  erg*=c;
  erg*=c;
  //PrintS("lenvar 5");
  assume(erg>=0);
  return erg; /*pLength(p)*/;
}
//! TODO CoefBuckets berücksichtigen
wlen_type kSBucketLength(kBucket* b, poly lm=NULL)
{
  wlen_type s=0;
  int c;
  number coef;
  if(lm==NULL)
    coef=pGetCoeff(kBucketGetLm(b));
    //c=nSize(pGetCoeff(kBucketGetLm(b)));
  else
    coef=pGetCoeff(lm);
    //c=nSize(pGetCoeff(lm));
  if (rField_is_Q(currRing)){
    c=QlogSize(coef);
  }
  else
    c=nSize(coef);
  
  int i;
  for (i=b->buckets_used;i>=0;i--)
  {
    assume((b->buckets_length[i]==0)||(b->buckets[i]!=NULL));
    s+=b->buckets_length[i] /*pLength(b->buckets[i])*/;
  }
  #ifdef HAVE_COEF_BUCKETS
  assume(b->buckets[0]==kBucketGetLm(b));
  if (b->coef[0]!=NULL){
    
    if (rField_is_Q(currRing)){
      int modifier=QlogSize(pGetCoeff(b->coef[0]));
      c+=modifier;
  }
    else{
      int modifier=nSize(pGetCoeff(b->coef[0]));
      c*=modifier;}
    }
  #endif
  wlen_type erg=s;
  erg*=c;
  erg*=c;
  return erg;
}
#endif

#ifdef LEN_VAR4
// 4.Variante: Laenge: Platz fuer Leitk * (1+Platz fuer andere Koeff.)
int pSLength(poly p, int l)
{
  int s=1;
  int c=nSize(pGetCoeff(p));
  pIter(p);
  while (p!=NULL) { s+=nSize(pGetCoeff(p));pIter(p); }
  return s*c;
}
int kSBucketLength(kBucket* b)
{
  int s=1;
  int c=nSize(pGetCoeff(kBucketGetLm(b)));
  int i;
  for (i=MAX_BUCKET;i>0;i--)
  {
    if(b->buckets[i]==NULL) continue;
    s+=pSLength(b->buckets[i],0);
  }
  return s*c;
}
#endif
static int do_pELength(poly p, slimgb_alg* c, int dlm=-1){
  if(p==NULL) return 0;
  int s=0;
  poly pi=p;
  if(dlm<0){
    dlm=pTotaldegree(p,c->r);
    s=1;
    pi=p->next;
  }
  
  while(pi){
    int d=pTotaldegree(pi,c->r);
    if(d>dlm)
      s+=1+d-dlm;
    else
      ++s;
    pi=pi->next;
  }
  return s;
}
int kEBucketLength(kBucket* b, poly lm,slimgb_alg* ca)
{
  int s=0;
  if(lm==NULL){
    lm=kBucketGetLm(b);
  }
  if(lm==NULL) return 0;
  int d=pTotaldegree(lm,ca->r);
  int i;
  for (i=b->buckets_used;i>=0;i--)
  {
    if(b->buckets[i]==NULL) continue;
    s+=do_pELength(b->buckets[i],ca,d);
  }
  return s;
}

static inline int pELength(poly p, slimgb_alg* c,int l){
  if (p==NULL) return 0;
  return do_pELength(p,c);
}




static inline wlen_type pQuality(poly p, slimgb_alg* c, int l=-1){
  
  if(l<0)
    l=pLength(p);
  if(c->is_char0) {
    if((pLexOrder) &&(!c->is_homog)){
      int cs;
      number coef=pGetCoeff(p);
      if (rField_is_Q(currRing)){
         cs=QlogSize(coef);
      }
      else
  cs=nSize(coef);
     wlen_type erg=cs;
     //erg*=cs;//for quadratic
     erg*=pELength(p,c,l);
    //FIXME: not quadratic coeff size
      //return cs*pELength(p,c,l);
      return erg;
    }
    //Print("I am here");
    wlen_type r=pSLength(p,l);
    assume(r>=0);
    return r;
  }
  if((pLexOrder) &&(!c->is_homog)) return pELength(p,c,l);
  return l;
}

static inline int pTotaldegree_full(poly p){
  int r=0;
  while(p){
    int d=pTotaldegree(p);
    r=si_max(r,d);
    pIter(p);
  }
  return r;
}

wlen_type red_object::guess_quality(slimgb_alg* c){
    //works at the moment only for lenvar 1, because in different
    //case, you have to look on coefs
    wlen_type s=0;
    if (c->is_char0){
      //s=kSBucketLength(bucket,this->p);
      if((pLexOrder) &&(!c->is_homog)){
    int cs;
    number coef;
    
    coef=pGetCoeff(kBucketGetLm(bucket));
    //c=nSize(pGetCoeff(kBucketGetLm(b)));
   
    //c=nSize(pGetCoeff(lm));
    if (rField_is_Q(currRing)){
      cs=QlogSize(coef);
    }
    else
      cs=nSize(coef);
    #ifdef HAVE_COEF_BUCKETS
    if (bucket->coef[0]!=NULL){
      if (rField_is_Q(currRing)){
        int modifier=QlogSize(pGetCoeff(bucket->coef[0]));
        cs+=modifier;
      }
      else{
        int modifier=nSize(pGetCoeff(bucket->coef[0]));
        cs*=modifier;}
    }
    #endif
    //FIXME:not quadratic
    wlen_type erg=kEBucketLength(this->bucket,this->p,c);
    //erg*=cs;//for quadratic
    erg*=cs;
    //return cs*kEBucketLength(this->bucket,this->p,c);
    return erg;
      }
      s=kSBucketLength(bucket,NULL);
    }
    else 
    {
      if((pLexOrder) &&(!c->is_homog))
  //if (false)
  s=kEBucketLength(this->bucket,this->p,c);
      else s=bucket_guess(bucket);
    }
 
    return s;
}



static void finalize_reduction_step(reduction_step* r){
  delete r;
}
static int LObject_better_gen(const void* ap, const void* bp)
{
  LObject* a=*(LObject**)ap;
  LObject* b=*(LObject**)bp;
  return(pLmCmp(a->p,b->p));
}
static int red_object_better_gen(const void* ap, const void* bp)
{


  return(pLmCmp(((red_object*) ap)->p,((red_object*) bp)->p));
}


static int pLmCmp_func_inverted(const void* ap1, const void* ap2){
    poly p1,p2;
  p1=*((poly*) ap1);
  p2=*((poly*)ap2);

  return -pLmCmp(p1,p2);
}

int tgb_pair_better_gen2(const void* ap,const void* bp){
  return(-tgb_pair_better_gen(ap,bp));
}
int kFindDivisibleByInS_easy(kStrategy strat,const red_object & obj){
  int i;
  long not_sev=~obj.sev;
  poly p=obj.p;
  for(i=0;i<=strat->sl;i++){
    if (pLmShortDivisibleBy(strat->S[i],strat->sevS[i],p,not_sev))
      return i;
  }
  return -1;
}
int kFindDivisibleByInS_easy(kStrategy strat,poly p, long sev){
  int i;
  long not_sev=~sev;
  for(i=0;i<=strat->sl;i++){
    if (pLmShortDivisibleBy(strat->S[i],strat->sevS[i],p,not_sev))
      return i;
  }
  return -1;
}
static int posInPairs (sorted_pair_node**  p, int pn, sorted_pair_node* qe,slimgb_alg* c,int an=0)
{
  if(pn==0) return 0;

  int length=pn-1;
  int i;
  //int an = 0;
  int en= length;

  if (pair_better(qe,p[en],c))
    return length+1;

  while(1)
    {
      //if (an >= en-1)
      if(en-1<=an)
      {
        if (pair_better(p[an],qe,c)) return an;
        return en;
      }
      i=(an+en) / 2;
        if (pair_better(p[i],qe,c))
          en=i;
      else an=i;
    }
}

static BOOLEAN  ascending(int* i,int top){
  if(top<1) return TRUE;
  if(i[top]<i[top-1]) return FALSE;
  return ascending(i,top-1);
}

sorted_pair_node**  spn_merge(sorted_pair_node** p, int pn,sorted_pair_node **q, int qn,slimgb_alg* c){
  int i;
  int* a= (int*) omalloc(qn*sizeof(int));
//   int mc;
//   PrintS("Debug\n");
//   for(mc=0;mc<qn;mc++)
// {

//     wrp(q[mc]->lcm_of_lm);
//     PrintS("\n");
// }
//    PrintS("Debug they are in\n");
//   for(mc=0;mc<pn;mc++)
// {

//     wrp(p[mc]->lcm_of_lm);
//     PrintS("\n");
// }
  int lastpos=0;
  for(i=0;i<qn;i++){
    lastpos=posInPairs(p,pn,q[i],c, si_max(lastpos-1,0));
    //   cout<<lastpos<<"\n";
    a[i]=lastpos;

  }
  if((pn+qn)>c->max_pairs){
    p=(sorted_pair_node**) omrealloc(p,2*(pn+qn)*sizeof(sorted_pair_node*));
    c->max_pairs=2*(pn+qn);
  }
  for(i=qn-1;i>=0;i--){
    size_t size;
    if(qn-1>i)
      size=(a[i+1]-a[i])*sizeof(sorted_pair_node*);
    else
      size=(pn-a[i])*sizeof(sorted_pair_node*); //as indices begin with 0
    memmove (p+a[i]+(1+i), p+a[i], size);
    p[a[i]+i]=q[i];
  }
  omfree(a);
  return p;
}


static BOOLEAN trivial_syzygie(int pos1,int pos2,poly bound,slimgb_alg* c){


  poly p1=c->S->m[pos1];
  poly p2=c->S->m[pos2];
    

  if (pGetComp(p1) > 0 || pGetComp(p2) > 0)
    return FALSE;
  int i = 1;
  poly m=NULL;
  poly gcd1=c->gcd_of_terms[pos1];
  poly gcd2=c->gcd_of_terms[pos2];
  
  if((gcd1!=NULL) && (gcd2!=NULL)) 
    {
      gcd1->next=gcd2; //may ordered incorrect
      m=gcd_of_terms(gcd1,c->r);
      gcd1->next=NULL;
      
    } 

  if (m==NULL) 
  {
     loop
      {
  if (pGetExp(p1, i)+ pGetExp(p2, i) > pGetExp(bound,i))   return FALSE;
  if (i == pVariables){
    //PrintS("trivial");
    return TRUE;
  }
  i++;
      }
  }
  else 
  {
    loop
      {
  if (pGetExp(p1, i)-pGetExp(m,i) + pGetExp(p2, i) > pGetExp(bound,i))   return FALSE;
  if (i == pVariables){
    pDelete(&m);
    //PrintS("trivial");
    return TRUE;
  }
  i++;
      }
  }

  

  
}

//! returns position sets w as weight
int find_best(red_object* r,int l, int u, int &w, slimgb_alg* c){
  int best=l;
  int i;
  w=r[l].guess_quality(c);
  for(i=l+1;i<=u;i++){
    int w2=r[i].guess_quality(c);
    if(w2<w){
      w=w2;
      best=i;
    }
   
  }
 return best;
}


void red_object::canonicalize(){
  kBucketCanonicalize(bucket);
  
  
}
BOOLEAN good_has_t_rep(int i, int j,slimgb_alg* c){
  assume(i>=0);
    assume(j>=0);
  if (has_t_rep(i,j,c)) return TRUE;
  //poly lm=pOne();
  assume (c->tmp_lm!=NULL);
  poly lm=c->tmp_lm;

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  assume(lm!=NULL);
  //int deciding_deg= pTotaldegree(lm);
  int* i_con =make_connections(i,j,lm,c);
  //p_Delete(&lm,c->r);


  for (int n=0;((n<c->n) && (i_con[n]>=0));n++){
    if (i_con[n]==j){
      now_t_rep(i,j,c);
      omfree(i_con);

      return TRUE;
    }
  }
  omfree(i_con);

  return FALSE;
}
BOOLEAN lenS_correct(kStrategy strat){
  int i;
  for(i=0;i<=strat->sl;i++){
    if (strat->lenS[i]!=pLength(strat->S[i]))
      return FALSE;
  }
  return TRUE;
}


static void cleanS(kStrategy strat, slimgb_alg* c){
  int i=0;
  LObject P;
  while(i<=strat->sl){
    P.p=strat->S[i];
    P.sev=strat->sevS[i];
    if(kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P)!=i)
    {
      deleteInS(i,strat);
      //remember destroying poly
      BOOLEAN found=FALSE;
      int j;
      for(j=0;j<c->n;j++)
  if(c->S->m[j]==P.p)
  {
    found=TRUE;
    break;
  }
      if (!found)
  pDelete(&P.p);
      //remember additional reductors
    }
    else i++;
  }
}
static int bucket_guess(kBucket* bucket){
  int sum=0;
  int i;
  for (i=bucket->buckets_used;i>=0;i--){
    if(bucket->buckets[i])
       sum+=bucket->buckets_length[i];
  }
  return sum;
}






static int add_to_reductors(slimgb_alg* c, poly h, int len){
  //inDebug(h);
  assume(lenS_correct(c->strat));
  assume(len==pLength(h));
  int i;
//   if (c->is_char0)
//        i=simple_posInS(c->strat,h,pSLength(h,len),c->is_char0);
//   else
//     i=simple_posInS(c->strat,h,len,c->is_char0);

  LObject P; memset(&P,0,sizeof(P));
  P.tailRing=c->r;
  P.p=h; /*p_Copy(h,c->r);*/
  P.FDeg=pFDeg(P.p,c->r);
 
  if (!rField_is_Zp(c->r)){ 
    pCleardenom(P.p);
    pContent(P.p); //is a duplicate call, but belongs here
    
  }
  else                     
    pNorm(P.p);
  pNormalize(P.p);
  wlen_type pq=pQuality(h,c,len);
  i=simple_posInS(c->strat,h,len,pq);
  c->strat->enterS(P,i,c->strat,-1);
 
 

  c->strat->lenS[i]=len;
  assume(pLength(c->strat->S[i])==c->strat->lenS[i]);
  if(c->strat->lenSw!=NULL)
    c->strat->lenSw[i]=pq;
 
  return i;
 
}
static void length_one_crit(slimgb_alg* c, int pos, int len)
{
  if (c->nc)
    return;
  if (len==1)
  {
    int i;
    for ( i=0;i<pos;i++)
    {
      if (c->lengths[i]==1)
        c->states[pos][i]=HASTREP;
    }
    for ( i=pos+1;i<c->n;i++){
      if (c->lengths[i]==1)
        c->states[i][pos]=HASTREP;
    }
    if (!c->nc)
      shorten_tails(c,c->S->m[pos]);
  }
}


static void move_forward_in_S(int old_pos, int new_pos,kStrategy strat)
{
  assume(old_pos>=new_pos);
  poly p=strat->S[old_pos];
  int ecart=strat->ecartS[old_pos];
  long sev=strat->sevS[old_pos];
  int s_2_r=strat->S_2_R[old_pos];
  int length=strat->lenS[old_pos];
  assume(length==pLength(strat->S[old_pos]));
  wlen_type length_w;
  if(strat->lenSw!=NULL)
    length_w=strat->lenSw[old_pos];
  int i;
  for (i=old_pos; i>new_pos; i--)
  {
    strat->S[i] = strat->S[i-1];
    strat->ecartS[i] = strat->ecartS[i-1];
    strat->sevS[i] = strat->sevS[i-1];
    strat->S_2_R[i] = strat->S_2_R[i-1];
  }
  if (strat->lenS!=NULL)
    for (i=old_pos; i>new_pos; i--)
      strat->lenS[i] = strat->lenS[i-1];
  if (strat->lenSw!=NULL)
    for (i=old_pos; i>new_pos; i--)
      strat->lenSw[i] = strat->lenSw[i-1];

  strat->S[new_pos]=p;
  strat->ecartS[new_pos]=ecart;
  strat->sevS[new_pos]=sev;
  strat->S_2_R[new_pos]=s_2_r;
  strat->lenS[new_pos]=length;
  if(strat->lenSw!=NULL)
    strat->lenSw[new_pos]=length_w;
  //assume(lenS_correct(strat));
}

static int* make_connections(int from, int to, poly bound, slimgb_alg* c)
{
  ideal I=c->S;
  int* cans=(int*) omalloc(c->n*sizeof(int));
  int* connected=(int*) omalloc(c->n*sizeof(int));
  cans[0]=to;
  int cans_length=1;
  connected[0]=from;
  int last_cans_pos=-1;
  int connected_length=1;
  long neg_bounds_short= ~p_GetShortExpVector(bound,c->r);

  int not_yet_found=cans_length;
  int con_checked=0;
  int pos;
  
  while(TRUE){
    if ((con_checked<connected_length)&& (not_yet_found>0)){
      pos=connected[con_checked];
      for(int i=0;i<cans_length;i++){
        if (cans[i]<0) continue;
        //FIXME: triv. syz. does not hold on noncommutative, check it for modules
        if ((has_t_rep(pos,cans[i],c)) ||((!rIsPluralRing(c->r))&&(trivial_syzygie(pos,cans[i],bound,c))))
{

          connected[connected_length]=cans[i];
          connected_length++;
          cans[i]=-1;
          --not_yet_found;

          if (connected[connected_length-1]==to){
            if (connected_length<c->n){
              connected[connected_length]=-1;
            }
            omfree(cans);
            return connected;
          }
        }
      }
      con_checked++;
    }
    else
    {
      for(last_cans_pos++;last_cans_pos<=c->n;last_cans_pos++){
        if (last_cans_pos==c->n){
          if (connected_length<c->n){
            connected[connected_length]=-1;
          }
          omfree(cans);
          return connected;
        }
        if ((last_cans_pos==from)||(last_cans_pos==to))
          continue;
        if(p_LmShortDivisibleBy(I->m[last_cans_pos],c->short_Exps[last_cans_pos],bound,neg_bounds_short,c->r)){
          cans[cans_length]=last_cans_pos;
          cans_length++;
          break;
        }
      }
      not_yet_found++;
      for (int i=0;i<con_checked;i++){
        if (has_t_rep(connected[i],last_cans_pos,c)){

          connected[connected_length]=last_cans_pos;
          connected_length++;
          cans[cans_length-1]=-1;

          --not_yet_found;
          if (connected[connected_length-1]==to){
            if (connected_length<c->n){
              connected[connected_length]=-1;
            }

            omfree(cans);
            return connected;
          }
          break;
        }
      }
    }
  }
  if (connected_length<c->n){
    connected[connected_length]=-1;
  }

  omfree(cans);
  return connected;
}
#ifdef HEAD_BIN
static inline poly p_MoveHead(poly p, omBin b)
{
  poly np;
  omTypeAllocBin(poly, np, b);
  memmove(np, p, b->sizeW*sizeof(long));
  omFreeBinAddr(p);
  return np;
}
#endif



static void add_later(poly p, char* prot, slimgb_alg* c){
    int i=0;
    //check, if it is already in the queue
   
    
    while(c->add_later->m[i]!=NULL){
        if (p_LmEqual(c->add_later->m[i],p,c->r))
            return;
        i++;
    }
    if (TEST_OPT_PROT)
        PrintS(prot);
    c->add_later->m[i]=p;
}
static int simple_posInS (kStrategy strat, poly p,int len, wlen_type wlen)
{


  if(strat->sl==-1) return 0;
  if (strat->lenSw) return pos_helper(strat,p,(wlen_type) wlen,(wlen_set) strat->lenSw,strat->S);
  return pos_helper(strat,p,len,strat->lenS,strat->S);

}

/*2
 *if the leading term of p
 *divides the leading term of some S[i] it will be canceled
 */
static inline void clearS (poly p, unsigned long p_sev,int l, int* at, int* k,
                           kStrategy strat)
{
  assume(p_sev == pGetShortExpVector(p));
  if (!pLmShortDivisibleBy(p,p_sev, strat->S[*at], ~ strat->sevS[*at])) return;
  if (l>=strat->lenS[*at]) return;
  if (TEST_OPT_PROT)
    PrintS("!");
  mflush();
  //pDelete(&strat->S[*at]);
  deleteInS((*at),strat);
  (*at)--;
  (*k)--;
//  assume(lenS_correct(strat));
}



static int iq_crit(const void* ap,const void* bp){

  sorted_pair_node* a=*((sorted_pair_node**)ap);
  sorted_pair_node* b=*((sorted_pair_node**)bp);
  assume(a->i>a->j);
  assume(b->i>b->j);
  
  
  if (a->deg<b->deg) return -1;
  if (a->deg>b->deg) return 1;
  int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  if(comp!=0)
    return comp;
  if (a->expected_length<b->expected_length) return -1;
  if (a->expected_length>b->expected_length) return 1;
  if (a->j>b->j) return 1;
  if (a->j<b->j) return -1;
  return 0;
}
static wlen_type pair_weighted_length(int i, int j, slimgb_alg* c){
    if ((c->is_char0) && (pLexOrder))  {
        int c1=slim_nsize(p_GetCoeff(c->S->m[i],c->r),c->r);
        int c2=slim_nsize(p_GetCoeff(c->S->m[j],c->r),c->r);
        wlen_type el1=c->weighted_lengths[i]/c1;
        assume(el1!=0);
        assume(c->weighted_lengths[j] %c1==0);
        wlen_type el2=c->weighted_lengths[i]/c2;
        assume(el2!=0);
        assume(c->weighted_lengths[j] %c2==0);
        //should be * for function fields
        return (c1+c2) * (el1+el2-2);
        
        
    }
    if (c->is_char0) {
        int cs=slim_nsize(p_GetCoeff(c->S->m[i],c->r),c->r)+
            slim_nsize(p_GetCoeff(c->S->m[j],c->r),c->r);
        return (c->lengths[i]+c->lengths[j]-2)*cs;
    }
    return c->lengths[i]+c->lengths[j]-2;
    
}
sorted_pair_node** add_to_basis_ideal_quotient(poly h, int i_pos, int j_pos,slimgb_alg* c, int* ip)
{

  assume(h!=NULL);
  poly got=gcd_of_terms(h,c->r);
  if((got!=NULL) &&(TEST_V_UPTORADICAL)) {
    poly copy=p_Copy(got,c->r);
    //p_wrp(got,c->r);
    BOOLEAN changed=monomial_root(got,c->r);
    if (changed)
    {
         poly div_by=pDivide(copy, got);
         poly iter=h;
         while(iter){
            pExpVectorSub(iter,div_by);
            pIter(iter);
         }
         p_Delete(&div_by, c->r);
         PrintS("U");
    }
    p_Delete(&copy,c->r);
  }

#define ENLARGE(pointer, type) pointer=(type*) omrealloc(pointer, c->array_lengths*sizeof(type))
//  BOOLEAN corr=lenS_correct(c->strat);
  BOOLEAN R_found=FALSE;
  void* hp;

  ++(c->n);
  ++(c->S->ncols);
  int i,j;
  i=c->n-1;
  sorted_pair_node** nodes=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*i);
  int spc=0;
  if(c->n>c->array_lengths){
    c->array_lengths=c->array_lengths*2;
    assume(c->array_lengths>=c->n);
    ENLARGE(c->T_deg, int);
    ENLARGE(c->tmp_pair_lm,poly);
    ENLARGE(c->tmp_spn,sorted_pair_node*);

    ENLARGE(c->short_Exps,long);
    ENLARGE(c->lengths,int);
    ENLARGE(c->states, char*);
    ENLARGE(c->gcd_of_terms,poly);
    //if (c->weighted_lengths!=NULL) {
    ENLARGE(c->weighted_lengths,wlen_type);
    //}
    //ENLARGE(c->S->m,poly);
    
  }
  pEnlargeSet(&c->S->m,c->n-1,1);
  if (c->T_deg_full)
    ENLARGE(c->T_deg_full,int);
  c->T_deg[i]=pTotaldegree(h);
  if(c->T_deg_full){
    c->T_deg_full[i]=pTotaldegree_full(h);
  }
  

  c->tmp_pair_lm[i]=pOne_Special(c->r);


  c->tmp_spn[i]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));


  c->lengths[i]=pLength(h);
  
  //necessary for correct weighted length
  
   if (!rField_is_Zp(c->r)){ 
    pCleardenom(h);
    pContent(h); //is a duplicate call, but belongs here
    
  }
  else                     
    pNorm(h);
  c->weighted_lengths[i]=pQuality(h, c, c->lengths[i]);
  c->gcd_of_terms[i]=got;
  
  if (i>0)
    c->states[i]=(char*)  omalloc(i*sizeof(char));
  else
    c->states[i]=NULL;
  
  
  c->S->m[i]=h;
  c->short_Exps[i]=p_GetShortExpVector(h,c->r);
 
#undef ENLARGE
  for (j=0;j<i;j++){
    
    //check product criterion
    
    c->states[i][j]=UNCALCULATED;
    assume(p_LmDivisibleBy(c->S->m[i],c->S->m[j],c->r)==
     p_LmShortDivisibleBy(c->S->m[i],c->short_Exps[i],c->S->m[j],~(c->short_Exps[j]),c->r));
    if(!c->F4_mode)
    {
      //      assume(!(p_LmDivisibleBy(c->S->m[j],c->S->m[i],c->r)));
    }
    
    if (_p_GetComp(c->S->m[i],c->r)!=_p_GetComp(c->S->m[j],c->r)){
      c->states[i][j]=UNIMPORTANT;
      //WARNUNG: be careful
      continue;
    } else
    if ((!c->nc) && (c->lengths[i]==1) && (c->lengths[j]==1)){
      c->states[i][j]=HASTREP;
      
      }
    else if ((!(c->nc)) &&  (pHasNotCF(c->S->m[i],c->S->m[j])))
    {
      c->easy_product_crit++;
      c->states[i][j]=HASTREP;
      continue;
    }
    else if(extended_product_criterion(c->S->m[i],c->gcd_of_terms[i],c->S->m[j],c->gcd_of_terms[j],c))
    {
      c->states[i][j]=HASTREP;
      c->extended_product_crit++;
      
      //PrintS("E");
    }
      //  if (c->states[i][j]==UNCALCULATED){

    if ((TEST_V_FINDMONOM) &&(!c->nc)) {
        //PrintS("COMMU");
       //  if (c->lengths[i]==c->lengths[j]){
//             poly short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
//             if (short_s==NULL){
//                 c->states[i][j]=HASTREP;
//             } else
//             {
//                 p_Delete(&short_s, currRing);
//             }
//         }
        if (c->lengths[i]+c->lengths[j]==3){
            poly                 short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
            if (short_s==NULL){
                c->states[i][j]=HASTREP;
            } else
            {
                assume(pLength(short_s)==1);
                if (TEST_V_UPTORADICAL)
                   monomial_root(short_s,c->r);
                int iS=
                   kFindDivisibleByInS_easy(c->strat,short_s, p_GetShortExpVector(short_s,c->r));
                if (iS<0){
                    //PrintS("N");
                    c->states[i][j]=HASTREP;
                    add_later(short_s,"N",c);
                }
                else {
                    if (c->strat->lenS[iS]>1){
                        //PrintS("O");
                        c->states[i][j]=HASTREP;
                        add_later(short_s,"O",c);
                    }
                    else
                     p_Delete(&short_s, currRing);
                }
                
               
            }
        }
    }
      //    if (short_s)
      //    {
    assume(spc<=j);
    sorted_pair_node* s=c->tmp_spn[spc];//(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
    s->i=si_max(i,j);
    s->j=si_min(i,j);
    assume(s->j==j);
    s->expected_length=pair_weighted_length(i,j,c);//c->lengths[i]+c->lengths[j]-2;
      
    poly lm=c->tmp_pair_lm[spc];//=pOne_Special();
      
    pLcm(c->S->m[i], c->S->m[j], lm);
    pSetm(lm);
    s->deg=pTotaldegree(lm);

    if(c->T_deg_full)//Sugar
    {
      int t_i=c->T_deg_full[s->i]-c->T_deg[s->i];
      int t_j=c->T_deg_full[s->j]-c->T_deg[s->j];
      s->deg+=si_max(t_i,t_j);
      //Print("\n max: %d\n",max(t_i,t_j));
    }
    s->lcm_of_lm=lm;
    //          pDelete(&short_s);
    //assume(lm!=NULL);
    nodes[spc]=s;
    spc++;
  
  // }
  //else
  //{
        //c->states[i][j]=HASTREP;
  //}
  }
  
  assume(spc<=i);
  //now ideal quotient crit
  qsort(nodes,spc,sizeof(sorted_pair_node*),iq_crit);
  
    sorted_pair_node** nodes_final=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*i);
  int spc_final=0;
  j=0;
  while(j<spc)
  {
    int lower=j;
    int upper;
    BOOLEAN has=FALSE;
    for(upper=lower+1;upper<spc;upper++)
    {
      
      if(!pLmEqual(nodes[lower]->lcm_of_lm,nodes[upper]->lcm_of_lm))
      {
  break;
      }
      if (has_t_rep(nodes[upper]->i,nodes[upper]->j,c))
  has=TRUE;

    }
    upper=upper-1;
    int z;
    assume(spc_final<=j);
    for(z=0;z<spc_final;z++)
    {
      if(p_LmDivisibleBy(nodes_final[z]->lcm_of_lm,nodes[lower]->lcm_of_lm,c->r))
      {
  has=TRUE;
  break;
      }
    }
    
    if(has)
    {
      for(;lower<=upper;lower++)
      {
  //free_sorted_pair_node(nodes[lower],c->r);
  //omfree(nodes[lower]);
  nodes[lower]=NULL;
      }
      j=upper+1;
      continue;
    }
    else
    {
      nodes[lower]->lcm_of_lm=pCopy(nodes[lower]->lcm_of_lm);
      assume(_p_GetComp(c->S->m[nodes[lower]->i],c->r)==_p_GetComp(c->S->m[nodes[lower]->j],c->r));
      nodes_final[spc_final]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      
      *(nodes_final[spc_final++])=*(nodes[lower]);
      //c->tmp_spn[nodes[lower]->j]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      nodes[lower]=NULL;
      for(lower=lower+1;lower<=upper;lower++)
      {
  //  free_sorted_pair_node(nodes[lower],c->r);
  //omfree(nodes[lower]);
  nodes[lower]=NULL;
      }
      j=upper+1;
      continue;
    }
  }

  //  Print("i:%d,spc_final:%d",i,spc_final);




  assume(spc_final<=spc);
  omfree(nodes);
  nodes=NULL;

  add_to_reductors(c, h, c->lengths[c->n-1]);
  //i=posInS(c->strat,c->strat->sl,h,0 ecart);
  if (!(c->nc)){
    if (c->lengths[c->n-1]==1)
      shorten_tails(c,c->S->m[c->n-1]);
  }
  //you should really update c->lengths, c->strat->lenS, and the oder of polys in strat if you sort after lengths

  //for(i=c->strat->sl; i>0;i--)
  //  if(c->strat->lenS[i]<c->strat->lenS[i-1]) printf("fehler bei %d\n",i);
  if (c->Rcounter>50) {
    c->Rcounter=0;
    cleanS(c->strat,c);
  }
  if(!ip){
    qsort(nodes_final,spc_final,sizeof(sorted_pair_node*),tgb_pair_better_gen2);
 
    
    c->apairs=spn_merge(c->apairs,c->pair_top+1,nodes_final,spc_final,c);
    c->pair_top+=spc_final;
    clean_top_of_pair_list(c);
    omfree(nodes_final);
    return NULL;
  }
  {
    *ip=spc_final;
    return nodes_final;
  }

  

}


static poly redNF2 (poly h,slimgb_alg* c , int &len, number&  m,int n)
{
  m=nInit(1);
  if (h==NULL) return NULL;

  assume(len==pLength(h));
  kStrategy strat=c->strat;
  if (0 > strat->sl)
  {
    return h;
  }
  int j;
 
  LObject P(h);
  P.SetShortExpVector();
  P.bucket = kBucketCreate(currRing);
  // BOOLEAN corr=lenS_correct(strat);
  kBucketInit(P.bucket,P.p,len /*pLength(P.p)*/);
  //wlen_set lenSw=(wlen_set) c->strat->lenS;
  //FIXME: plainly wrong
  //strat->lenS;
  //if (strat->lenSw!=NULL)
  //  lenSw=strat->lenSw;
  //int max_pos=simple_posInS(strat,P.p);
  loop
    {

      j=kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P);
      if ((j>=0) && ((!n)||
        ((strat->lenS[j]<=n) &&
         ((strat->lenSw==NULL)||
         (strat->lenSw[j]<=n)))))
      {

  
        nNormalize(pGetCoeff(P.p));
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("red:");
          wrp(h);
          PrintS(" with ");
          wrp(strat->S[j]);
        }
#endif
       
        number coef=kBucketPolyRed(P.bucket,strat->S[j],
                                   strat->lenS[j]/*pLength(strat->S[j])*/,
                                   strat->kNoether);
  number m2=nMult(m,coef);
  nDelete(&m);
  m=m2;
        nDelete(&coef);
        h = kBucketGetLm(P.bucket);
  
  if (h==NULL) {
    len=0;
    kBucketDestroy(&P.bucket);
    return 
    NULL;}
        P.p=h;
        P.t_p=NULL;
        P.SetShortExpVector();
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("\nto:");
          wrp(h);
          PrintLn();
        }
#endif
      }
      else
      {
        kBucketClear(P.bucket,&(P.p),&len);
        kBucketDestroy(&P.bucket);
        pNormalize(P.p);
  assume(len==(pLength(P.p)));
        return P.p;
      }
    }
}



static poly redTailShort(poly h, kStrategy strat){
  if (h==NULL) return NULL;//n_Init(1,currRing);
  if (TEST_V_MODPSOLVSB){
    bit_reduce(pNext(h), strat->tailRing);
  }
  int sl=strat->sl;
  int i;
  int len=pLength(h);
  for(i=0;i<=strat->sl;i++){
    if((strat->lenS[i]>2) || ((strat->lenSw!=NULL) && (strat->lenSw[i]>2)))
      break;
  }
  return(redNFTail(h,i-1,strat, len));
}

static void line_of_extended_prod(int fixpos,slimgb_alg* c){
    if (c->gcd_of_terms[fixpos]==NULL)
  {
    c->gcd_of_terms[fixpos]=gcd_of_terms(c->S->m[fixpos],c->r);
    if (c->gcd_of_terms[fixpos])
    {
      int i;
      for(i=0;i<fixpos;i++)
        if((c->states[fixpos][i]!=HASTREP)&& (extended_product_criterion(c->S->m[fixpos],c->gcd_of_terms[fixpos], c->S->m[i],c->gcd_of_terms[i],c)))
{
          c->states[fixpos][i]=HASTREP;
    c->extended_product_crit++;
}     
      for(i=fixpos+1;i<c->n;i++)
        if((c->states[i][fixpos]!=HASTREP)&& (extended_product_criterion(c->S->m[fixpos],c->gcd_of_terms[fixpos], c->S->m[i],c->gcd_of_terms[i],c)))
  {        c->states[i][fixpos]=HASTREP;
  c->extended_product_crit++;
  }
    }
  }
}
static void c_S_element_changed_hook(int pos, slimgb_alg* c){
  length_one_crit(c,pos, c->lengths[pos]);
  if (!c->nc)
    line_of_extended_prod(pos,c);
}
class poly_tree_node {
public:
  poly p;
  poly_tree_node* l;
  poly_tree_node* r;
  int n;
  poly_tree_node(int sn):l(NULL),r(NULL),n(sn){}
};
class exp_number_builder{
public:
  poly_tree_node* top_level;
  int n;
  int get_n(poly p);
  exp_number_builder():top_level(0),n(0){}
};
int exp_number_builder::get_n(poly p){
  poly_tree_node** node=&top_level;
  while(*node!=NULL){
    int c=pLmCmp(p,(*node)->p);
    if (c==0) return (*node)->n;
    if (c==-1) node=&((*node)->r);
    else
      node=&((*node)->l);
  }
  (*node)= new poly_tree_node(n);
  n++;
  (*node)->p=pLmInit(p);
  return (*node)->n;
}

//mac_polys exp are smaller iff they are greater by monomial ordering
//corresponding to solving linear equations notation

//! obsolete
struct int_poly_pair{
  poly p;
  int n;
};


//! obsolete
void t2ippa_rec(poly* ip,int* ia, poly_tree_node* k, int &offset){
    if(!k) return;
    t2ippa_rec(ip,ia,k->l,offset);
    ip[offset]=k->p;
    ia[k->n]=offset;
    ++offset;

    t2ippa_rec(ip,ia,k->r,offset);
    delete k;
  }

//! obsolete
void t2ippa(poly* ip,int* ia,exp_number_builder & e){

  int o=0;
  t2ippa_rec(ip,ia,e.top_level,o);
}
int anti_poly_order(const void* a, const void* b){
  return -pLmCmp(((int_poly_pair*) a)->p,((int_poly_pair*) b)->p );
}

BOOLEAN is_valid_ro(red_object & ro){
  red_object r2=ro;
  ro.validate();
  if ((r2.p!=ro.p)||(r2.sev!=ro.sev)) return FALSE;
  return TRUE;
}



static void go_on (slimgb_alg* c){
  //set limit of 1000 for multireductions, at the moment for
  //programming reasons
  int i=0;
  c->average_length=0;
  for(i=0;i<c->n;i++){
    c->average_length+=c->lengths[i];
  }
  c->average_length=c->average_length/c->n;
  i=0;
  poly* p=(poly*) omalloc((PAR_N+1)*sizeof(poly));//nullterminated

  int curr_deg=-1;
  while(i<PAR_N){
    sorted_pair_node* s=top_pair(c);//here is actually chain criterium done
    if (!s) break;
    if(curr_deg>=0){
      if (s->deg >curr_deg) break;
    }

    else curr_deg=s->deg;
    quick_pop_pair(c);
    if(s->i>=0){
      //be careful replace_pair use createShortSpoly which is not noncommutative
      //replace_pair(s->i,s->j,c);
    if(s->i==s->j) {
      free_sorted_pair_node(s,c->r);
      continue;
  }
    }
    poly h;
    if(s->i>=0){
      if (!c->nc)
  h=ksOldCreateSpoly(c->S->m[s->i], c->S->m[s->j], NULL, c->r);
      else
  h= nc_CreateSpoly(c->S->m[s->i], c->S->m[s->j], NULL, c->r);
    } 
    else
      h=s->lcm_of_lm;
    if(s->i>=0)
      now_t_rep(s->j,s->i,c);
    number coef;
    int mlen=pLength(h);
    if (!c->nc){
      h=redNF2(h,c,mlen,coef,2);
      redTailShort(h,c->strat);
      nDelete(&coef);
    }
    free_sorted_pair_node(s,c->r);
    if(!h) continue;
    int len=pLength(h);
    p[i]=h;
    
    i++;
  }
  p[i]=NULL;
//  pre_comp(p,i,c);
  if(i==0){
    omfree(p);
    return;
  }
  red_object* buf=(red_object*) omalloc(i*sizeof(red_object));
  c->normal_forms+=i;
  int j;
  for(j=0;j<i;j++){
    buf[j].p=p[j];
    buf[j].sev=pGetShortExpVector(p[j]);
    buf[j].bucket = kBucketCreate(currRing);
    
    int len=pLength(p[j]);
    kBucketInit(buf[j].bucket,buf[j].p,len);
    buf[j].initial_quality=buf[j].guess_quality(c);
    assume(buf[j].initial_quality>=0);
  }
  omfree(p);
  qsort(buf,i,sizeof(red_object),red_object_better_gen);
//    Print("\ncurr_deg:%i\n",curr_deg);
  if (TEST_OPT_PROT)
  {
    Print("%dM[%d,",curr_deg,i);
  }
#ifdef FIND_DETERMINISTIC
  c->modifiedS=(BOOLEAN*) omalloc((c->strat->sl+1)*sizeof(BOOLEAN));
  c->expandS=(poly*) omalloc((1)*sizeof(poly));
  c->expandS[0]=NULL;
  int z2;
  for(z2=0;z2<=c->strat->sl;z2++)
    c->modifiedS[z2]=FALSE;
#endif
  multi_reduction(buf, i, c);
#ifdef TGB_DEBUG
 {
   int k;
   for(k=0;k<i;k++)
   {
     assume(kFindDivisibleByInS_easy(c->strat,buf[k])<0);
     int k2;
     for(k2=0;k2<i;k2++)
     {
       if(k==k2) continue;
       assume((!(p_LmDivisibleBy(buf[k].p,buf[k2].p,c->r)))||(wrp(buf[k].p),Print(" k %d k2 %d ",k,k2),wrp(buf[k2].p),FALSE));
     }
   }
 }
#endif
  //resort S
#ifdef FIND_DETERMINISTIC
  for(z2=0;z2<=c->strat->sl;z2++)
  {
    if (c->modifiedS[z2])
    {
      wlen_type qual;
      int new_pos;
      if (c->strat->lenSw!=NULL)
          new_pos=simple_posInS(c->strat,c->strat->S[z2],strat->lenS[z2],strat->Sw[z2]);
      else
          new_pos=simple_posInS(c->strat,c->strat->S[z2],strat->lenS[z2],lenS[z2]);
      
      if (new_pos<z2)
      { 
         move_forward_in_S(z2,new_pos,c->strat);
      }
      
      assume(new_pos<=z2);
    }
  }
  for(z2=0;c->expandS[z2]!=NULL;z2++)
  {
    add_to_reductors(c,c->expandS[z2],pLength(c->expandS[z2]));
    // PrintS("E");
  }
  omfree(c->modifiedS);
  c->modifiedS=NULL;
  omfree(c->expandS);
  c->expandS=NULL;
#endif
  if (TEST_OPT_PROT)
      Print("%i]",i); 

  int* ibuf=(int*) omalloc(i*sizeof(int));
  sorted_pair_node*** sbuf=(sorted_pair_node***) omalloc(i*sizeof(sorted_pair_node**));
  for(j=0;j<i;j++)
  {
    int len;
    poly p;
    buf[j].flatten();
    kBucketClear(buf[j].bucket,&p, &len);
    kBucketDestroy(&buf[j].bucket);

    if (!c->nc)
      p=redTailShort(p, c->strat);
    sbuf[j]=add_to_basis_ideal_quotient(p,-1,-1,c,ibuf+j);
    //sbuf[j]=add_to_basis(p,-1,-1,c,ibuf+j);
  }
  int sum=0;
  for(j=0;j<i;j++){
    sum+=ibuf[j];
  }
  sorted_pair_node** big_sbuf=(sorted_pair_node**) omalloc(sum*sizeof(sorted_pair_node*));
  int partsum=0;
  for(j=0;j<i;j++)
  {
    memmove(big_sbuf+partsum, sbuf[j],ibuf[j]*sizeof(sorted_pair_node*));
    omfree(sbuf[j]);
    partsum+=ibuf[j];
  }

  qsort(big_sbuf,sum,sizeof(sorted_pair_node*),tgb_pair_better_gen2);
  c->apairs=spn_merge(c->apairs,c->pair_top+1,big_sbuf,sum,c);
  c->pair_top+=sum;
  clean_top_of_pair_list(c);
  omfree(big_sbuf);
  omfree(sbuf);
  omfree(ibuf);
  omfree(buf);
#ifdef TGB_DEBUG
  int z;
  for(z=1;z<=c->pair_top;z++)
  {
    assume(pair_better(c->apairs[z],c->apairs[z-1],c));
  }
#endif
  if (TEST_OPT_PROT)
      Print("(%d)",c->pair_top+1); 
  while(!(idIs0(c->add_later))){
    ideal add=c->add_later;
    
    ideal add2=kInterRed(add,NULL);
    id_Delete(&add,currRing);
    idSkipZeroes(add2);
    c->add_later=idInit(5000,c->S->rank);
    memset(c->add_later->m,0,5000*sizeof(poly));
    for(i=0;i<add2->idelems();i++){
      if (add2->m[i]!=NULL)
          add_to_basis_ideal_quotient(add2->m[i],-1,-1,c,NULL);
      add2->m[i]=NULL;
    }
    id_Delete(&add2, c->r);
  }
  return;
}



#ifdef REDTAIL_S

static poly redNFTail (poly h,const int sl,kStrategy strat, int len)
{
  if (h==NULL) return NULL;
  pTest(h);
  if (0 > sl)
    return h;
  if (pNext(h)==NULL) return h;

  int j;
  poly res=h;
  poly act=res;
  LObject P(pNext(h));
  pNext(res)=NULL;
  P.bucket = kBucketCreate(currRing);
  len--;
  h=P.p;
  if (len <=0) len=pLength(h);
  kBucketInit(P.bucket,h /*P.p*/,len /*pLength(P.p)*/);
  pTest(h);
  loop
  {
      P.p=h;
      P.t_p=NULL;
      P.SetShortExpVector();
      loop
      {
          j=kFindDivisibleByInS(strat->S,strat->sevS,sl,&P);
          if (j>=0)
          {
#ifdef REDTAIL_PROT
            PrintS("r");
#endif
            nNormalize(pGetCoeff(P.p));
#ifdef KDEBUG
            if (TEST_OPT_DEBUG)
            {
              PrintS("red tail:");
              wrp(h);
              PrintS(" with ");
              wrp(strat->S[j]);
            }
#endif
            number coef;
            pTest(strat->S[j]);
            coef=kBucketPolyRed(P.bucket,strat->S[j],
                                strat->lenS[j]/*pLength(strat->S[j])*/,strat->kNoether);
            pMult_nn(res,coef);
            nDelete(&coef);
            h = kBucketGetLm(P.bucket);
            pTest(h);
            if (h==NULL)
            {
#ifdef REDTAIL_PROT
              PrintS(" ");
#endif
        kBucketDestroy(&P.bucket);
              return res;
            }
            pTest(h);
            P.p=h;
            P.t_p=NULL;
            P.SetShortExpVector();
#ifdef KDEBUG
            if (TEST_OPT_DEBUG)
            {
              PrintS("\nto tail:");
              wrp(h);
              PrintLn();
            }
#endif
          }
          else
          {
#ifdef REDTAIL_PROT
            PrintS("n");
#endif
            break;
          }
      } /* end loop current mon */
      //   poly tmp=pHead(h /*kBucketGetLm(P.bucket)*/);
      //act->next=tmp;pIter(act);
      act->next=kBucketExtractLm(P.bucket);pIter(act);
      h = kBucketGetLm(P.bucket);
      if (h==NULL)
      {
#ifdef REDTAIL_PROT
        PrintS(" ");
#endif
  kBucketDestroy(&P.bucket);
        return res;
      }
      pTest(h);
  }
}
#endif


//try to fill, return FALSE iff queue is empty

//transfers ownership of m to mat
void init_with_mac_poly(tgb_sparse_matrix* mat, int row, mac_poly m){
  assume(mat->mp[row]==NULL);
  mat->mp[row]=m;
#ifdef TGB_DEBUG
  mac_poly r=m;
  while(r){
    assume(r->exp<mat->columns);
    r=r->next;
  }
#endif
}
poly free_row_to_poly(tgb_sparse_matrix* mat, int row, poly* monoms, int monom_index){
  poly p=NULL;
  poly* set_this=&p;
  mac_poly r=mat->mp[row];
  mat->mp[row]=NULL;
  while(r)
  {
    (*set_this)=pLmInit(monoms[monom_index-1-r->exp]);
    pSetCoeff((*set_this),r->coef);
    set_this=&((*set_this)->next);
    mac_poly old=r;
    r=r->next;
    delete old;
    
  }
  return p;

}











static int poly_crit(const void* ap1, const void* ap2){
  poly p1,p2;
  p1=*((poly*) ap1);
  p2=*((poly*)ap2);

  int c=pLmCmp(p1,p2);
  if (c !=0) return c;
  int l1=pLength(p1);
  int l2=pLength(p2);
  if (l1<l2) return -1;
  if (l1>l2) return 1;
  return 0;
}
slimgb_alg::slimgb_alg(ideal I, BOOLEAN F4){
  
  
  r=currRing;
  nc=rIsPluralRing(r);
  
  is_homog=TRUE;
  {
    int hz;
    for(hz=0;hz<IDELEMS(I);hz++){
      assume(I->m[hz]!=NULL);
      int d=pTotaldegree(I->m[hz]);
      poly t=I->m[hz]->next;
      while(t)
      {
        if (d!=pTotaldegree(t,r))
        {
          is_homog=FALSE;
          break;
        }
        t=t->next;
      }
      if(!(is_homog)) break;
    }
  }
  //  Print("is homog:%d",c->is_homog);
  void* h;
  poly hp;
  int i,j;
  to_destroy=NULL;
  easy_product_crit=0;
  extended_product_crit=0;
  if (rField_is_Zp(r))
    is_char0=FALSE;
  else
    is_char0=TRUE;
  //not fully correct
  //(rChar()==0);
  F4_mode=F4;
  
  if ((!F4_mode)&&(!is_homog) &&(pLexOrder)){
    this->doubleSugar=TRUE;
  }
  else this->doubleSugar=FALSE;
  reduction_steps=0;
  last_index=-1;

  F=NULL;
  F_minus=NULL;

  Rcounter=0;

  soon_free=NULL;

  tmp_lm=pOne();

  normal_forms=0;
  current_degree=1;
 
  max_pairs=5*I->idelems();
 
  apairs=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*max_pairs);
  pair_top=-1;

  int n=I->idelems();
  array_lengths=n;

  
  i=0;
  this->n=0;
  T_deg=(int*) omalloc(n*sizeof(int));
  if((!(is_homog)) &&(pLexOrder))
    T_deg_full=(int*) omalloc(n*sizeof(int));
  else
    T_deg_full=NULL;
  tmp_pair_lm=(poly*) omalloc(n*sizeof(poly));
  tmp_spn=(sorted_pair_node**) omalloc(n*sizeof(sorted_pair_node*));
  lm_bin=omGetSpecBin(POLYSIZE + (r->ExpL_Size)*sizeof(long));
#ifdef HEAD_BIN
  HeadBin=omGetSpecBin(POLYSIZE + (currRing->ExpL_Size)*sizeof(long));
#endif
  /* omUnGetSpecBin(&(c->HeadBin)); */
  h=omalloc(n*sizeof(char*));
  states=(char**) h;
  h=omalloc(n*sizeof(int));
  lengths=(int*) h;
  weighted_lengths=(wlen_type*)omalloc(n*sizeof(wlen_type));
  gcd_of_terms=(poly*) omalloc(n*sizeof(poly));
  
  short_Exps=(long*) omalloc(n*sizeof(long));
  if (F4_mode)
    S=idInit(n,I->rank);
  else
    S=idInit(1,I->rank);
  strat=new skStrategy;
  strat->syzComp = 0;
  initBuchMoraCrit(strat);
  initBuchMoraPos(strat);
  strat->initEcart = initEcartBBA;
  strat->tailRing=r;
  strat->enterS = enterSBba;
  strat->sl = -1;
  i=n;
  i=1;//some strange bug else
  /* initS(c->S,NULL,c->strat); */
  /* intS start: */
  // i=((i+IDELEMS(c->S)+15)/16)*16;
  strat->ecartS=(intset)omAlloc(i*sizeof(int)); /*initec(i);*/
  strat->sevS=(unsigned long*)omAlloc0(i*sizeof(unsigned long));
  /*initsevS(i);*/
  strat->S_2_R=(int*)omAlloc0(i*sizeof(int));/*initS_2_R(i);*/
  strat->fromQ=NULL;
  strat->Shdl=idInit(1,1);
  strat->S=strat->Shdl->m;
  strat->lenS=(int*)omAlloc0(i*sizeof(int));
  if((is_char0)||((pLexOrder) &&(!is_homog)))
    strat->lenSw=(wlen_type*)omAlloc0(i*sizeof(wlen_type));
  else
    strat->lenSw=NULL;
  sorted_pair_node* si;
  assume(n>0);
  add_to_basis_ideal_quotient(I->m[0],-1,-1,this,NULL);

  assume(strat->sl==strat->Shdl->idelems()-1);
  if(!(F4_mode))
  {
    for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
    {
      //     add_to_basis(I->m[i],-1,-1,c);
      si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      si->i=-1;
      si->j=-2;
      si->expected_length=pLength(I->m[i]);
      si->deg=pTotaldegree(I->m[i]);
      if (!rField_is_Zp(r)){ 
        pCleardenom(I->m[i]);
      }
      si->lcm_of_lm=I->m[i];
      
      //      c->apairs[n-1-i]=si;
      apairs[n-i-1]=si;
      ++(pair_top);
    }
  }
  else
  {
    for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
      add_to_basis_ideal_quotient(I->m[i],-1,-1,this,NULL);
  }
  for(i=0;i<I->idelems();i++)
  {
    I->m[i]=NULL;
  }
  idDelete(&I);
  add_later=idInit(5000,S->rank);
  memset(add_later->m,0,5000*sizeof(poly));
}
slimgb_alg::~slimgb_alg(){
  id_Delete(&add_later,r);
  int i,j;
  slimgb_alg* c=this;
  while(c->to_destroy)
  {
    pDelete(&(c->to_destroy->p));
    poly_list_node* old=c->to_destroy;
    c->to_destroy=c->to_destroy->next;
    omfree(old);
  }
  while(c->F)
  {
    for(i=0;i<c->F->size;i++){
      pDelete(&(c->F->mp[i].m));
    }
    omfree(c->F->mp);
    c->F->mp=NULL;
    mp_array_list* old=c->F;
    c->F=c->F->next;
    omfree(old);
  }
  while(c->F_minus)
  {
    for(i=0;i<c->F_minus->size;i++){
      pDelete(&(c->F_minus->p[i]));
    }
    omfree(c->F_minus->p);
    c->F_minus->p=NULL;
    poly_array_list* old=c->F_minus;
    c->F_minus=c->F_minus->next;
    omfree(old);
  }
  for(int z=1 /* zero length at 0 */;z<c->n;z++){
    omfree(c->states[z]);
  }
  omfree(c->states);
  omfree(c->lengths);
  for(int z=0;z<c->n;z++)
  {
    pDelete(&c->tmp_pair_lm[z]);
    omfree(c->tmp_spn[z]);
  }
  omfree(c->tmp_pair_lm);
  omfree(c->tmp_spn);
 
  omfree(c->T_deg);
  if(c->T_deg_full)
    omfree(c->T_deg_full);

  omFree(c->strat->ecartS);
  omFree(c->strat->sevS);
//   initsevS(i);
  omFree(c->strat->S_2_R);
   

  omFree(c->strat->lenS);

  if(c->strat->lenSw)  omFree(c->strat->lenSw);




  for(i=0;i<c->n;i++){
    if(c->gcd_of_terms[i])
      pDelete(&(c->gcd_of_terms[i]));
  }
  omfree(c->gcd_of_terms);

  omfree(c->apairs);
  if (TEST_OPT_PROT)
  {
    Print("calculated %d NFs\n",c->normal_forms);
    Print("applied %i product crit, %i extended_product crit \n", c->easy_product_crit, c->extended_product_crit);
  }
  int deleted_form_c_s=0;
  
  for(i=0;i<=c->strat->sl;i++){
    if (!c->strat->S[i]) continue;
    BOOLEAN found=FALSE;
    for(j=0;j<c->n;j++){
      if (c->S->m[j]==c->strat->S[i]){
        found=TRUE;
        break;
      }
    }
    if(!found) pDelete(&c->strat->S[i]);
  }
//   for(i=0;i<c->n;i++){
//     if (c->rep[i]!=i){
// //       for(j=0;j<=c->strat->sl;j++){
// //   if(c->strat->S[j]==c->S->m[i]){
// //     c->strat->S[j]=NULL;
// //     break;
// //   }
// //       }
// //      PrintS("R_delete");
//       pDelete(&c->S->m[i]);
//     }
//   }

 
  for(i=0;i<c->n;i++)
  {
    assume(c->S->m[i]!=NULL);
    for(j=0;j<c->n;j++)
    {
      if((c->S->m[j]==NULL)||(i==j)) 
        continue;
      assume(p_LmShortDivisibleBy(c->S->m[j],c->short_Exps[j],
             c->S->m[i],~c->short_Exps[i],
             c->r)==p_LmDivisibleBy(c->S->m[j],
             c->S->m[i],
             c->r));
      if (p_LmShortDivisibleBy(c->S->m[j],c->short_Exps[j],
          c->S->m[i],~c->short_Exps[i],
          c->r))
      {
        pDelete(&c->S->m[i]);
        break;
      }
    }
  }
  omfree(c->short_Exps);
  

  ideal I=c->S;
  
  IDELEMS(I)=c->n;

  idSkipZeroes(I);
  for(i=0;i<=c->strat->sl;i++)
    c->strat->S[i]=NULL;
  id_Delete(&c->strat->Shdl,c->r);
  pDelete(&c->tmp_lm);
  omUnGetSpecBin(&lm_bin);
  delete c->strat;
}
ideal t_rep_gb(ring r,ideal arg_I, BOOLEAN F4_mode){
  
  //  Print("QlogSize(0) %d, QlogSize(1) %d,QlogSize(-2) %d, QlogSize(5) %d\n", QlogSize(nlInit(0)),QlogSize(nlInit(1)),QlogSize(nlInit(-2)),QlogSize(nlInit(5)));
  
  if (TEST_OPT_PROT)
    if (F4_mode)
      PrintS("F4 Modus \n");
     
  //debug_Ideal=arg_debug_Ideal;
  //if (debug_Ideal) PrintS("DebugIdeal received\n");
  // Print("Idelems %i \n----------\n",IDELEMS(arg_I));
  ideal I=idCompactify(arg_I);
  if (idIs0(I)) return I;

  qsort(I->m,IDELEMS(I),sizeof(poly),poly_crit);
  //Print("Idelems %i \n----------\n",IDELEMS(I));
  //slimgb_alg* c=(slimgb_alg*) omalloc(sizeof(slimgb_alg));
  slimgb_alg* c=new slimgb_alg(I, F4_mode);
    
  int i;
  while(c->pair_top>=0)
  {
    if(F4_mode)
      go_on_F4(c);
    else
      go_on(c);
  }
  I=c->S;
  delete c;
  if (TEST_OPT_REDSB){
    ideal erg=kInterRed(I,NULL);
    assume(I!=erg);
    id_Delete(&I, currRing);
    return erg;
  }
  //qsort(I->m, IDELEMS(I),sizeof(poly),pLmCmp_func);
  assume(I->rank>=idRankFreeModule(I));
  return(I);
}
void now_t_rep(const int & arg_i, const int & arg_j, slimgb_alg* c){
  int i,j;
  if (arg_i==arg_j){
    return;
  }
  if (arg_i>arg_j){
    i=arg_j;
    j=arg_i;
  } else {
    i=arg_i;
    j=arg_j;
  }
  c->states[j][i]=HASTREP;
}

static BOOLEAN has_t_rep(const int & arg_i, const  int & arg_j, slimgb_alg* state){
  assume(0<=arg_i);
  assume(0<=arg_j);
  assume(arg_i<state->n);
  assume(arg_j<state->n);
  if (arg_i==arg_j)
  {
    return (TRUE);
  }
  if (arg_i>arg_j)
  {
    return (state->states[arg_i][arg_j]==HASTREP);
  } else
  {
    return (state->states[arg_j][arg_i]==HASTREP);
  }
}
static int pLcmDeg(poly a, poly b)
{
  int i;
  int n=0;
  for (i=pVariables; i; i--)
  {
    n+=si_max( pGetExp(a,i), pGetExp(b,i));
  }
  return n;

}



static void shorten_tails(slimgb_alg* c, poly monom)
{
  return;
// BOOLEAN corr=lenS_correct(c->strat);
  for(int i=0;i<c->n;i++)
  {
    //enter tail
    
    if (c->S->m[i]==NULL) continue;
    poly tail=c->S->m[i]->next;
    poly prev=c->S->m[i];
    BOOLEAN did_something=FALSE;
    while((tail!=NULL)&& (pLmCmp(tail, monom)>=0))
    {
      if (p_LmDivisibleBy(monom,tail,c->r))
      {
        did_something=TRUE;
        prev->next=tail->next;
        tail->next=NULL;
        p_Delete(& tail,c->r);
        tail=prev;
        //PrintS("Shortened");
        c->lengths[i]--;
      }
      prev=tail;
      tail=tail->next;
    }
    if (did_something)
    {
      int new_pos;
      wlen_type q;
      q=pQuality(c->S->m[i],c,c->lengths[i]);
      new_pos=simple_posInS(c->strat,c->S->m[i],c->lengths[i],q);

      int old_pos=-1;
      //assume new_pos<old_pos
      for (int z=0;z<=c->strat->sl;z++)
      {
        if (c->strat->S[z]==c->S->m[i])
        {
          old_pos=z;
          break;
        }
      }
      if (old_pos== -1)
        for (int z=new_pos-1;z>=0;z--)
        {
          if (c->strat->S[z]==c->S->m[i])
          {
            old_pos=z;
            break;
          }
        }
      assume(old_pos>=0);
      assume(new_pos<=old_pos);
      assume(pLength(c->strat->S[old_pos])==c->lengths[i]);
      c->strat->lenS[old_pos]=c->lengths[i];
      if (c->strat->lenSw)
        c->strat->lenSw[old_pos]=q;

      if (new_pos<old_pos)
        move_forward_in_S(old_pos,new_pos,c->strat);

      length_one_crit(c,i,c->lengths[i]);
    }
  }
}
static sorted_pair_node* pop_pair(slimgb_alg* c){
  clean_top_of_pair_list(c);

  if(c->pair_top<0) return NULL;
  else return (c->apairs[c->pair_top--]);
}
sorted_pair_node* top_pair(slimgb_alg* c){
  super_clean_top_of_pair_list(c);//yeah, I know, it's odd that I use a different proc here

  if(c->pair_top<0) return NULL;
  else return (c->apairs[c->pair_top]);
}
sorted_pair_node* quick_pop_pair(slimgb_alg* c){
  if(c->pair_top<0) return NULL;
  else return (c->apairs[c->pair_top--]);
}



static void super_clean_top_of_pair_list(slimgb_alg* c){
  while((c->pair_top>=0)
  && (c->apairs[c->pair_top]->i>=0)
  && (good_has_t_rep(c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,c)))
  {

    free_sorted_pair_node(c->apairs[c->pair_top],c->r);
    c->pair_top--;

  }
}
void clean_top_of_pair_list(slimgb_alg* c){
  while((c->pair_top>=0) && (c->apairs[c->pair_top]->i>=0) && (!state_is(UNCALCULATED,c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,c))){

    free_sorted_pair_node(c->apairs[c->pair_top],c->r);
    c->pair_top--;

  }
}
static BOOLEAN state_is(calc_state state, const int & arg_i, const  int & arg_j, slimgb_alg* c){
  assume(0<=arg_i);
  assume(0<=arg_j);
  assume(arg_i<c->n);
  assume(arg_j<c->n);
  if (arg_i==arg_j)
  {
    return (TRUE);
  }
  if (arg_i>arg_j)
  {
    return (c->states[arg_i][arg_j]==state);
  }
  else return(c->states[arg_j][arg_i]==state);
}
void free_sorted_pair_node(sorted_pair_node* s, ring r){
  if (s->i>=0)
    p_Delete(&s->lcm_of_lm,r);
  omfree(s);
}
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, slimgb_alg* c){
  if (a->deg<b->deg) return TRUE;
  if (a->deg>b->deg) return FALSE;

//  if (a->expected_length<b->expected_length) return TRUE;
  //  if (a->expected_length>b->expected_length) return FALSE;
  int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  if (comp==1) return FALSE;
  if (-1==comp) return TRUE;
  if (a->i+a->j<b->i+b->j) return TRUE;
   if (a->i+a->j>b->i+b->j) return FALSE;
  if (a->i<b->i) return TRUE;
  if (a->i>b->i) return FALSE;
  return TRUE;
}

static int tgb_pair_better_gen(const void* ap,const void* bp){

  sorted_pair_node* a=*((sorted_pair_node**)ap);
  sorted_pair_node* b=*((sorted_pair_node**)bp);
  assume(a->i>a->j);
  assume(b->i>b->j);
  if (a->deg<b->deg) return -1;
  if (a->deg>b->deg) return 1;


//  if (a->expected_length<b->expected_length) return -1;
  // if (a->expected_length>b->expected_length) return 1;
 int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  
  if (comp==1) return 1;
  if (-1==comp) return -1;
  if (a->i+a->j<b->i+b->j) return -1;
   if (a->i+a->j>b->i+b->j) return 1;
  if (a->i<b->i) return -1;
   if (a->i>b->i) return 1;
  return 0;
}


static poly gcd_of_terms(poly p, ring r){
  int max_g_0=0;
  assume(p!=NULL);
  int i;
  poly m=pOne();
  poly t;
  for (i=pVariables; i; i--)
  {
      pSetExp(m,i, pGetExp(p,i));
      if (max_g_0==0)
  if (pGetExp(m,i)>0)
    max_g_0=i;
  }
  
  t=p->next;
  while (t!=NULL){
    
    if (max_g_0==0) break;
    for (i=max_g_0; i; i--)
    {
      pSetExp(m,i, si_min(pGetExp(t,i),pGetExp(m,i)));
      if (max_g_0==i)
  if (pGetExp(m,i)==0)
    max_g_0=0;
      if ((max_g_0==0) && (pGetExp(m,i)>0)){
  max_g_0=i;
      }
    }
    t=t->next;
  }

  if (max_g_0>0)
    return m;
  pDelete(&m);
  return NULL;
}
static inline BOOLEAN pHasNotCFExtended(poly p1, poly p2, poly m)
{

  if (pGetComp(p1) > 0 || pGetComp(p2) > 0)
    return FALSE;
  int i = 1;
  loop
  {
    if ((pGetExp(p1, i)-pGetExp(m,i) >0) && (pGetExp(p2, i) -pGetExp(m,i)> 0))   return FALSE;
    if (i == pVariables)                                return TRUE;
    i++;
  }
}


//for impl reasons may return false if the the normal product criterion matches
static inline BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, slimgb_alg* c){
  if (c->nc)
    return FALSE;
  if(gcd1==NULL) return FALSE;
        if(gcd2==NULL) return FALSE;
        gcd1->next=gcd2; //may ordered incorrect
        poly m=gcd_of_terms(gcd1,c->r);
        gcd1->next=NULL;
        if (m==NULL) return FALSE;

        BOOLEAN erg=pHasNotCFExtended(p1,p2,m);
        pDelete(&m);
        return erg;
}
static poly kBucketGcd(kBucket* b, ring r)
{
  int s=0;
  int i;
  poly m, n;
  BOOLEAN initialized=FALSE;
  for (i=MAX_BUCKET-1;i>=0;i--)
  { 
    if (b->buckets[i]!=NULL){
      if (!initialized){
  m=gcd_of_terms(b->buckets[i],r);
  initialized=TRUE;
  if (m==NULL) return NULL;
      }
      else
  {
    n=gcd_of_terms(b->buckets[i],r);
    if (n==NULL) {
      pDelete(&m);
      return NULL;    
    }
    n->next=m;
    poly t=gcd_of_terms(n,r);
    n->next=NULL;
    pDelete(&m);
    pDelete(&n);
    m=t;
    if (m==NULL) return NULL;
    
  }
    }
  }
  return m;
}




static inline int quality_of_pos_in_strat_S(int pos, slimgb_alg* c){
  if (c->strat->lenSw!=NULL) return c->strat->lenSw[pos];
  return c->strat->lenS[pos];
}
static inline int quality_of_pos_in_strat_S_mult_high(int pos, poly high, slimgb_alg* c)
  //meant only for nc
{
  poly m=pOne();
  pExpVectorDiff(m,high ,c->strat->S[pos]);
  poly product=nc_mm_Mult_p(m, pCopy(c->strat->S[pos]), c->r);
  int erg=pQuality(product,c);
  pDelete(&m);
  pDelete(&product);
  return erg;
}

static void multi_reduction_lls_trick(red_object* los, int losl,slimgb_alg* c,find_erg & erg){
  erg.expand=NULL;
  BOOLEAN swap_roles; //from reduce_by, to_reduce_u if fromS
  if(erg.fromS){
    if(pLmEqual(c->strat->S[erg.reduce_by],los[erg.to_reduce_u].p))
    {
      int i;
      int quality_a=quality_of_pos_in_strat_S(erg.reduce_by,c);
      int best=erg.to_reduce_u+1;
/*
      for (i=erg.to_reduce_u;i>=erg.to_reduce_l;i--){
  int qc=los[i].guess_quality(c);
  if (qc<quality_a){
    best=i;
    quality_a=qc;
  }
      }
      if(best!=erg.to_reduce_u+1){*/
      int qc;
      best=find_best(los,erg.to_reduce_l,erg.to_reduce_u,qc,c);
      if(qc<quality_a){
  los[best].flatten();
  int b_pos=kBucketCanonicalize(los[best].bucket);
  los[best].p=los[best].bucket->buckets[b_pos];
  qc=pQuality(los[best].bucket->buckets[b_pos],c);
  if(qc<quality_a){
    red_object h=los[erg.to_reduce_u];
    los[erg.to_reduce_u]=los[best];
    los[best]=h;
    swap_roles=TRUE;
  }
  else
    swap_roles=FALSE;
      }
      else{
  
  swap_roles=FALSE;
      }
  
    }
      else
    {
      if (erg.to_reduce_u>erg.to_reduce_l){

  int i;
  int quality_a=quality_of_pos_in_strat_S(erg.reduce_by,c);
  if (c->nc)
    quality_a=quality_of_pos_in_strat_S_mult_high(erg.reduce_by, los[erg.to_reduce_u].p, c);
  int best=erg.to_reduce_u+1;
  int qc;
  best=find_best(los,erg.to_reduce_l,erg.to_reduce_u,qc,c);
  assume(qc==los[best].guess_quality(c));
  if(qc<quality_a){
    los[best].flatten();
    int b_pos=kBucketCanonicalize(los[best].bucket);
    los[best].p=los[best].bucket->buckets[b_pos];
    qc==pQuality(los[best].bucket->buckets[b_pos],c);
    //(best!=erg.to_reduce_u+1)
    if(qc<quality_a){
    red_object h=los[erg.to_reduce_u];
    los[erg.to_reduce_u]=los[best];
    los[best]=h;
    erg.reduce_by=erg.to_reduce_u;
    erg.fromS=FALSE;
    erg.to_reduce_u--;
    }
  }
      }
      else 
      {
  assume(erg.to_reduce_u==erg.to_reduce_l);
  wlen_type quality_a=
        quality_of_pos_in_strat_S(erg.reduce_by,c);
  wlen_type qc=los[erg.to_reduce_u].guess_quality(c);
  if (qc<0) PrintS("Wrong wlen_type");
  if(qc<quality_a){
    int best=erg.to_reduce_u;
    los[best].flatten();
    int b_pos=kBucketCanonicalize(los[best].bucket);
    los[best].p=los[best].bucket->buckets[b_pos];
    qc=pQuality(los[best].bucket->buckets[b_pos],c);
    assume(qc>=0);
    if(qc<quality_a){
      BOOLEAN exp=FALSE;
      if(qc<=2){
         //Print("\n qc is %lld \n",qc);
         exp=TRUE;
      }
        
      else {
         if (qc<quality_a/2)
          exp=TRUE;
         else
       if(erg.reduce_by<c->n/4)
          exp=TRUE;
      }
      if (exp){
        poly clear_into;
        los[erg.to_reduce_u].flatten();
        kBucketClear(los[erg.to_reduce_u].bucket,&clear_into,&erg.expand_length);
        erg.expand=pCopy(clear_into);
        kBucketInit(los[erg.to_reduce_u].bucket,clear_into,erg.expand_length);
        if (TEST_OPT_PROT)
    PrintS("e");
        
      }
    }
  }

  
      }
      
      swap_roles=FALSE;
      return;
      }
    
  }
  else{
    if(erg.reduce_by>erg.to_reduce_u){
      //then lm(rb)>= lm(tru) so =
      assume(erg.reduce_by==erg.to_reduce_u+1);
      int best=erg.reduce_by;
      wlen_type quality_a=los[erg.reduce_by].guess_quality(c);
      int qc;
      best=find_best(los,erg.to_reduce_l,erg.to_reduce_u,qc,c);
      
      int i;
      if(qc<quality_a){
    red_object h=los[erg.reduce_by];
    los[erg.reduce_by]=los[best];
    los[best]=h;
  }
  swap_roles=FALSE;
  return;
  
    
    }
    else
    {
      assume(!pLmEqual(los[erg.reduce_by].p,los[erg.to_reduce_l].p));
      assume(erg.to_reduce_u==erg.to_reduce_l);
      //further assume, that reduce_by is the above all other polys
      //with same leading term
      int il=erg.reduce_by;
      int quality_a =los[erg.reduce_by].guess_quality(c);
      int qc;
      while((il>0) && pLmEqual(los[il-1].p,los[il].p)){
  il--;
  qc=los[il].guess_quality(c);
  if (qc<quality_a){
    quality_a=qc;
    erg.reduce_by=il;
  }
      }
      swap_roles=FALSE;
    }
  
  }
  if(swap_roles)
  {
    if (TEST_OPT_PROT)
      PrintS("b");
    poly clear_into;
    int dummy_len;
    int new_length;
    int bp=erg.to_reduce_u;//bucket_positon
    //kBucketClear(los[bp].bucket,&clear_into,&new_length);
    new_length=los[bp].clear_to_poly();
    clear_into=los[bp].p;
    poly p=c->strat->S[erg.reduce_by];
    int j=erg.reduce_by;
    int old_length=c->strat->lenS[j];// in view of S
    los[bp].p=p;
    kBucketInit(los[bp].bucket,p,old_length);
    wlen_type qal=pQuality(clear_into,c,new_length);
    int pos_in_c=-1;    
    int z;
    int new_pos;
    new_pos=simple_posInS(c->strat,clear_into,new_length, qal);
    assume(new_pos<=j);
    for (z=c->n;z;z--)
    {
      if(p==c->S->m[z-1])
      {
  pos_in_c=z-1;
  break;
      }
    }
    if(pos_in_c>=0)
    {
      c->S->m[pos_in_c]=clear_into;
      c->lengths[pos_in_c]=new_length;
      if (c->T_deg_full)
  c->T_deg_full[pos_in_c]=pTotaldegree_full(clear_into);
      c_S_element_changed_hook(pos_in_c,c);
    }
    c->strat->S[j]=clear_into;
    c->strat->lenS[j]=new_length;
    assume(pLength(clear_into)==new_length);
    if(c->strat->lenSw!=NULL)
      c->strat->lenSw[j]=qal;
    if (!rField_is_Zp(c->r))
    {
      pContent(clear_into);
      pCleardenom(clear_into);//should be unnecessary
    }
    else                     
      pNorm(clear_into);
#ifdef FIND_DETERMINISTIC
    erg.reduce_by=j;
    //resort later see diploma thesis, find_in_S must be deterministic
    //during multireduction if spolys are only in the span of the
    //input polys
#else
    
    if (new_pos<j)
    { 
      move_forward_in_S(j,new_pos,c->strat);
      erg.reduce_by=new_pos;
    }
#endif
  }
}
static int fwbw(red_object* los, int i){
   int i2=i;
   int step=1;
   
   BOOLEAN bw=FALSE;
   BOOLEAN incr=TRUE;
   
   while(1)
   {
     if(!bw)
     {
       step=si_min(i2,step);
       if (step==0) break;
       i2-=step;
    
       if(!pLmEqual(los[i].p,los[i2].p))
       {
   bw=TRUE;
   incr=FALSE;
       }
       else
       {
   if ((!incr) &&(step==1)) break;
       }
       
       
     }
     else
     {
       
       step=si_min(i-i2,step);
       if (step==0) break;
       i2+=step;
       if(pLmEqual(los[i].p,los[i2].p)){
   if(step==1) break;
   else
   {
     bw=FALSE;
   }
       }
       
     }
     if (incr)
       step*=2;
     else
     {
       if (step%2==1)
   step=(step+1)/2;
       else
   step/=2;
       
     }
   }
   return i2;
}
static void canonicalize_region(red_object* los, int l, int u,slimgb_alg* c){
    assume(l<=u+1);
    int i;
    for(i=l;i<=u;i++){
        kBucketCanonicalize(los[i].bucket);
    }

}
static void multi_reduction_find(red_object* los, int losl,slimgb_alg* c,int startf,find_erg & erg){
  kStrategy strat=c->strat;

  assume(startf<=losl);
  assume((startf==losl-1)||(pLmCmp(los[startf].p,los[startf+1].p)==-1));
  int i=startf;
  
  int j;
  while(i>=0){
    assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)<=0));
    assume(is_valid_ro(los[i]));
    j=kFindDivisibleByInS_easy(strat,los[i]);
    if(j>=0){
     
      erg.to_reduce_u=i;
      erg.reduce_by=j;
      erg.fromS=TRUE;
      int i2=fwbw(los,i);
      assume(pLmEqual(los[i].p,los[i2].p));
      assume((i2==0)||(!pLmEqual(los[i2].p,los[i2-1].p)));
      assume(i>=i2);


      erg.to_reduce_l=i2;
      assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)==-1));
      canonicalize_region(los,erg.to_reduce_u+1,startf,c);
      return;
    }
    if (j<0){
      
      //not reduceable, try to use this for reducing higher terms
      int i2=fwbw(los,i);
      assume(pLmEqual(los[i].p,los[i2].p));
      assume((i2==0)||(!pLmEqual(los[i2].p,los[i2-1].p)));
      assume(i>=i2);
      if(i2!=i){
  
  
  erg.to_reduce_u=i-1;
  erg.to_reduce_l=i2;
  erg.reduce_by=i;
  erg.fromS=FALSE;
  assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)==-1));
  canonicalize_region(los,erg.to_reduce_u+1,startf,c);
  return;
      }
      if((!(c->is_homog)) &&(!(c->doubleSugar)))
      {

  for (i2=i+1;i2<losl;i2++){
    if (p_LmShortDivisibleBy(los[i].p,los[i].sev,los[i2].p,~los[i2].sev,
           c->r)){
      int i3=i2;
      while((i3+1<losl) && (pLmEqual(los[i2].p, los[i3+1].p)))
        i3++;
      erg.to_reduce_u=i3;
      erg.to_reduce_l=i2;
      erg.reduce_by=i;
      erg.fromS=FALSE;
      assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)==-1));
      canonicalize_region(los,erg.to_reduce_u+1,startf,c);
      return;
    }
    //  else {assume(!p_LmDivisibleBy(los[i].p, los[i2].p,c->r));}
  }
      }
      i--;
    }
  }
  erg.reduce_by=-1;//error code
  return;
}

 //  nicht reduzierbare eintraege in ergebnisliste schreiben
//   nullen loeschen
//   while(finde_groessten leitterm reduzierbar(c,erg)){
  
static int multi_reduction_clear_zeroes(red_object* los, int  losl, int l, int u)
{


  int deleted=0;
  int  i=l;
  int last=-1;
  while(i<=u)
  {
    
    if(los[i].p==NULL){
      kBucketDestroy(&los[i].bucket);
//      delete los[i];//here we assume los are constructed with new
      //destroy resources, must be added here   
     if (last>=0)
     {
       memmove(los+(int)(last+1-deleted),los+(last+1),sizeof(red_object)*(i-1-last));
     }
     last=i;
     deleted++;
    }
    i++;
  }
  if((last>=0)&&(last!=losl-1))
      memmove(los+(int)(last+1-deleted),los+last+1,sizeof(red_object)*(losl-1-last));
  return deleted;
  
}

static void sort_region_down(red_object* los, int l, int u, slimgb_alg* c)
{
  qsort(los+l,u-l+1,sizeof(red_object),red_object_better_gen);
  int i;

  for(i=l;i<=u;i++)
  {
    BOOLEAN moved=FALSE;
    int j;
    for(j=i;j;j--)
    {
      if(pLmCmp(los[j].p,los[j-1].p)==-1){
  red_object h=los[j];
  los[j]=los[j-1];
  los[j-1]=h;
  moved=TRUE;
      }
      else break;
    }
    if(!moved) return;
  }
}

//assume that los is ordered ascending by leading term, all non zero
static void multi_reduction(red_object* los, int & losl, slimgb_alg* c)
{
  poly* delay=(poly*) omalloc(losl*sizeof(poly));
  int delay_s=0;
  //initialize;
  assume(c->strat->sl>=0);
  assume(losl>0);
  int i;
  wlen_type max_initial_quality=0;
  
  for(i=0;i<losl;i++){
    los[i].sev=pGetShortExpVector(los[i].p);
//SetShortExpVector();
    los[i].p=kBucketGetLm(los[i].bucket);
    if (los[i].initial_quality>max_initial_quality)
        max_initial_quality=los[i].initial_quality;
    // else
//         Print("init2_qal=%lld;", los[i].initial_quality);
//     Print("initial_quality=%lld;",max_initial_quality);
  }

  kStrategy strat=c->strat;
  int curr_pos=losl-1;


//  nicht reduzierbare einträge in ergebnisliste schreiben
  // nullen loeschen
  while(curr_pos>=0){
    
    find_erg erg;
    multi_reduction_find(los, losl,c,curr_pos,erg);//last argument should be curr_pos
    if(erg.reduce_by<0) break;



    erg.expand=NULL;
    int d=erg.to_reduce_u-erg.to_reduce_l+1;
   
    
    multi_reduction_lls_trick(los,losl,c,erg);
    
    
    int i;
    int len;
    //    wrp(los[erg.to_reduce_u].p);
    //Print("\n");
    multi_reduce_step(erg,los,c);
   

    if(!K_TEST_OPT_REDTHROUGH){
  for(i=erg.to_reduce_l;i<=erg.to_reduce_u;i++){
     if  (los[i].p!=NULL)  //the check (los[i].p!=NULL) might be invalid
     {
         //
         assume(los[i].initial_quality>0);
         
               if(los[i].guess_quality(c)
                  >1.5*delay_factor*max_initial_quality){
                       if (TEST_OPT_PROT)
                           PrintS("v");
                       los[i].canonicalize();
                       if(los[i].guess_quality(c)
                           >delay_factor*max_initial_quality){
                               if (TEST_OPT_PROT)
                                   PrintS(".");
                               los[i].clear_to_poly();
                               //delay.push_back(los[i].p);
                               delay[delay_s]=los[i].p;
                               delay_s++;

                               los[i].p=NULL;
                      
                      }
                  }
            
            }
     }
  }
    int deleted=multi_reduction_clear_zeroes(los, losl, erg.to_reduce_l, erg.to_reduce_u);
    if(erg.fromS==FALSE)
      curr_pos=si_max(erg.to_reduce_u,erg.reduce_by);
    else
      curr_pos=erg.to_reduce_u;
    losl -= deleted;
    curr_pos -= deleted;

    //Print("deleted %i \n",deleted);
    if ((TEST_V_UPTORADICAL) &&(!(erg.fromS)))
        sort_region_down(los,si_min(erg.to_reduce_l,erg.reduce_by),(si_max(erg.to_reduce_u,erg.reduce_by))-deleted,c);
    else    
    sort_region_down(los, erg.to_reduce_l, erg.to_reduce_u-deleted, c);


    if(erg.expand)
    {
#ifdef FIND_DETERMINISTIC
      int i;
      for(i=0;c->expandS[i];i++);
      c->expandS=(poly*) omrealloc(c->expandS,(i+2)*sizeof(poly));
      c->expandS[i]=erg.expand;
      c->expandS[i+1]=NULL;
#else
      add_to_reductors(c,erg.expand,erg.expand_length);
#endif
    }
      
  }
  

  sorted_pair_node** pairs=(sorted_pair_node**)
    omalloc(delay_s*sizeof(sorted_pair_node*)); 
  for(i=0;i<delay_s;i++){
      poly p=delay[i];
      sorted_pair_node* si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      si->i=-1;
      si->j=-1;
       if (!rField_is_Zp(c->r)){
        if (!c->nc)
            p=redTailShort(p, c->strat);
        pCleardenom(p);
        pContent(p);
      }
      si->expected_length=pLength(p);
      si->deg=pTotaldegree(p);
     
      si->lcm_of_lm=p;
      pairs[i]=si;
  }
  qsort(pairs,delay_s,sizeof(sorted_pair_node*),tgb_pair_better_gen2);
  c->apairs=spn_merge(c->apairs,c->pair_top+1,pairs,delay_s,c);
  c->pair_top+=delay_s;
  omfree(delay);
  return;
}
void red_object::flatten(){
  assume(p==kBucketGetLm(bucket));
}
void red_object::validate(){
  p=kBucketGetLm(bucket);
  if(p)
    sev=pGetShortExpVector(p);
}
int red_object::clear_to_poly(){
  flatten();
  int l;
  kBucketClear(bucket,&p,&l);
  return l;
}

  



void reduction_step::reduce(red_object* r, int l, int u){}
void simple_reducer::do_reduce(red_object & ro){
  number coef;
  if (!c->nc)
    coef=kBucketPolyRed(ro.bucket,p,
       p_len,
       c->strat->kNoether);
  else
    nc_kBucketPolyRed_Z(ro.bucket, p, &coef);
  nDelete(&coef);
}


void simple_reducer::reduce(red_object* r, int l, int u){
  this->pre_reduce(r,l,u);
  int i;
//debug start
  int im;


  for(i=l;i<=u;i++){
  


    this->do_reduce(r[i]);
 
  }
  for(i=l;i<=u;i++){
  
    kBucketSimpleContent(r[i].bucket);
    r[i].validate();
    #ifdef TGB_DEBUG
    #endif
  }
}
reduction_step::~reduction_step(){}
simple_reducer::~simple_reducer(){
  if(fill_back!=NULL)
  {
    kBucketInit(fill_back,p,p_len);
  }
  fill_back=NULL;
    
}
 
void multi_reduce_step(find_erg & erg, red_object* r, slimgb_alg* c){
  static int id=0;
  id++;
  unsigned long sev;
    BOOLEAN lt_changed=FALSE;
  int rn=erg.reduce_by;
  poly red;
  int red_len;
  simple_reducer* pointer;
  BOOLEAN work_on_copy=FALSE;
  if(erg.fromS){
    red=c->strat->S[rn];
    red_len=c->strat->lenS[rn];
    assume(red_len==pLength(red));
  }
  else
  {
    r[rn].flatten();
    kBucketClear(r[rn].bucket,&red,&red_len);
    
    if (!rField_is_Zp(c->r))
    {
      pContent(red);
      pCleardenom(red);//should be unnecessary
     
    }
    pNormalize(red);
    

    if ((!(erg.fromS))&&(TEST_V_UPTORADICAL)){
         
         if (polynomial_root(red,c->r))
            lt_changed=TRUE;
            sev=p_GetShortExpVector(red,c->r);}
    red_len=pLength(red);
  }
  if (((TEST_V_MODPSOLVSB)&&(red_len>1))||((c->nc)||(erg.to_reduce_u-erg.to_reduce_l>5))){
    work_on_copy=TRUE;
    // poly m=pOne();
    poly m=c->tmp_lm;
    pSetCoeff(m,nInit(1));
    for(int i=1;i<=pVariables;i++)
      pSetExp(m,i,(pGetExp(r[erg.to_reduce_l].p, i)-pGetExp(red,i)));
    pSetm(m);
    poly red_cp;
    if (!c->nc)
      red_cp=ppMult_mm(red,m);
    else
      red_cp=nc_mm_Mult_p(m, pCopy(red), c->r);
    if(!erg.fromS){
      kBucketInit(r[rn].bucket,red,red_len);
    }
    //now reduce the copy
    //static poly redNF2 (poly h,slimgb_alg* c , int &len, number&  m,int n)

    if (!c->nc)
      redTailShort(red_cp,c->strat);
    //number mul;
    // red_len--;
//     red_cp->next=redNF2(red_cp->next,c,red_len,mul,c->average_length);
//     pSetCoeff(red_cp,nMult(red_cp->coef,mul));
//     nDelete(&mul);
//     red_len++;
    red=red_cp;
    red_len=pLength(red);
    // pDelete(&m);
    
  }
  int i;



  assume(red_len==pLength(red));
 
  pointer=new simple_reducer(red,red_len,c);

  if ((!work_on_copy) && (!erg.fromS))
    pointer->fill_back=r[rn].bucket;
  else
    pointer->fill_back=NULL;
  pointer->reduction_id=id;
  pointer->c=c;

  pointer->reduce(r,erg.to_reduce_l, erg.to_reduce_u);
  if(work_on_copy) pDelete(&pointer->p);
  delete pointer;
  if (lt_changed){
    assume(!erg.fromS);
    r[erg.reduce_by].sev=sev;
  }
  
};



 
void simple_reducer:: pre_reduce(red_object* r, int l, int u){}

