
// #define OM_CHECK 3
// #define OM_TRACK 5
// #define OM_KEEP  1
// TODO: 
//       deg -> poly_crit
//       multiple rings
//       shorten_tails und dessen Aufrufe pruefen wlength!!!
//       calculating with formal sums
//       try to create spolys as formal sums

#include "tgb.h"
static const int bundle_size=100;
#if 1
static omBin lm_bin=NULL;
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
#define LEN_VAR1
#define degbound(p) assume(pTotaldegree(p)<10)
//#define inDebug(p) assume((debug_Ideal==NULL)||(kNF(debug_Ideal,NULL,p,0,0)==0))

//die meisten Varianten stossen sich an coef_buckets
#ifdef LEN_VAR1
// erste Variante: Laenge: Anzahl der Monome
int pSLength(poly p, int l) {
  return l; }
int kSBucketLength(kBucket* bucket) {return bucket_guess(bucket);}
#endif

#ifdef LEN_VAR2
// 2. Variante: Laenge: Platz fuer die Koeff.
int pSLength(poly p,int l)
{
  int s=0;
  while (p!=NULL) { s+=nSize(pGetCoeff(p));pIter(p); }
  return s;
}
int kSBucketLength(kBucket* b)
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

#ifdef LEN_VAR3
// 3.Variante: Laenge: Platz fuer Leitk * Monomanzahl
int pSLength(poly p,int l)
{
  int c=nSize(pGetCoeff(p));
  return c*l /*pLength(p)*/;
}
int kSBucketLength(kBucket* b)
{
  int s=0;
  int c=nSize(pGetCoeff(kBucketGetLm(b)))+1;
  int i;
  for (i=MAX_BUCKET;i>0;i--)
  {
    s+=b->buckets_length[i] /*pLength(b->buckets[i])*/;
  }
  return s*c;
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
static int monom_poly_crit(const void* ap1, const void* ap2){
  monom_poly* p1;
  monom_poly* p2;
  p1=((monom_poly*) ap1);
  p2=((monom_poly*)ap2);
  if(((unsigned long) p1->f)>((unsigned long) p2->f)) return 1;
  if(((unsigned long) p1->f)<((unsigned long)p2->f)) return -1;  

  return pLmCmp(p1->m,p2->m);
 
}
static int pLmCmp_func(const void* ap1, const void* ap2){
    poly p1,p2;
  p1=*((poly*) ap1);
  p2=*((poly*)ap2);

  return pLmCmp(p1,p2);
}
static int pLmCmp_func_inverted(const void* ap1, const void* ap2){
    poly p1,p2;
  p1=*((poly*) ap1);
  p2=*((poly*)ap2);

  return -pLmCmp(p1,p2);
}

static int pair_better_gen2(const void* ap,const void* bp){
  return(-pair_better_gen(ap,bp));
}
static int kFindDivisibleByInS_easy(kStrategy strat,const red_object & obj){
  int i;
  long not_sev=~obj.sev;
  poly p=obj.p;
  for(i=0;i<=strat->sl;i++){
    if (pLmShortDivisibleBy(strat->S[i],strat->sevS[i],p,not_sev))
      return i;
  }
  return -1;
}
static int kFindDivisibleByInS_easy(kStrategy strat,poly p, long sev){
  int i;
  long not_sev=~sev;
  for(i=0;i<=strat->sl;i++){
    if (pLmShortDivisibleBy(strat->S[i],strat->sevS[i],p,not_sev))
      return i;
  }
  return -1;
}
static int posInPairs (sorted_pair_node**  p, int pn, sorted_pair_node* qe,calc_dat* c,int an=0)
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
static int posInPolys (poly*  p, int pn, poly qe,calc_dat* c)
{
  if(pn==0) return 0;

  int length=pn-1;
  int i;
  int an = 0;
  int en= length;

  if (pLmCmp(qe,p[en])==1)
    return length+1;

  while(1)
    {
      //if (an >= en-1)
      if(en-1<=an)
      {
        if (pLmCmp(p[an],qe)==1) return an;
        return en;
      }
      i=(an+en) / 2;
        if (pLmCmp(p[i],qe)==1)
          en=i;
      else an=i;
    }
}
static int posInMonomPolys (monom_poly*  p, int pn, monom_poly & qe,calc_dat* c)
{
  if(pn==0) return 0;

  int length=pn-1;
  int i;
  int an = 0;
  int en= length;

  if (monom_poly_crit(&qe,&p[en])==1)
    return length+1;

  while(1)
    {
      //if (an >= en-1)
      if(en-1<=an)
      {
        if (monom_poly_crit(&p[an],&qe)==1) return an;
        return en;
      }
      i=(an+en) / 2;
        if (monom_poly_crit(&p[i],&qe)==1)
          en=i;
      else an=i;
    }
}
static BOOLEAN  ascending(int* i,int top){
  if(top<1) return TRUE;
  if(i[top]<i[top-1]) return FALSE;
  return ascending(i,top-1);
}

sorted_pair_node**  merge(sorted_pair_node** p, int pn,sorted_pair_node **q, int qn,calc_dat* c){
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
    lastpos=posInPairs(p,pn,q[i],c, max(lastpos-1,0));
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


static BOOLEAN trivial_syzygie(int pos1,int pos2,poly bound,calc_dat* c){


  poly p1=c->S->m[pos1];
  poly p2=c->S->m[pos2];
  ring r=c->r;
  

  if (pGetComp(p1) > 0 || pGetComp(p2) > 0)
    return FALSE;
  int i = 1;
  poly m=NULL;
  poly gcd1=c->gcd_of_terms[pos1];
  poly gcd2=c->gcd_of_terms[pos2];
  
  if((gcd1!=NULL) && (gcd2!=NULL)) 
    {
      gcd1->next=gcd2; //may ordered incorrect
      poly m=gcd_of_terms(gcd1,c->r);
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

///returns position sets w as weight
int find_best(red_object* r,int l, int u, int &w, calc_dat* c){
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

#if 0 
int find_best(red_object* r,int l, int u, int &w, calc_dat* c){

  int sz=u-l+1;
  int n=sz/10+1;
  int filled=0;
  int* indizes=(int*) omalloc(n*sizeof(int));
  int* weight=(int*) omalloc(n*sizeof(int));
  int worst=-1;
  int i;  
  for(i=l;i<=u;i++){
    int q=r[i].guess_quality(c);
    if ((filled<n)||(q<worst)){
      if(filled<n){
	worst=max(q,worst);
	indizes[filled]=i;
	weight[filled]=q;
	filled++;
      }
    }
    else{
      int j;
      for(j=0;j<filled;j++){
	if (worst==weight[j]){
	  weight[j]=q;
	  indizes[j]=i;
	}
      }
      worst=-1;
      for(j=0;j<filled;j++){
	if (worst<weight[j]){
	  worst=weight[j];
	}
      }
    }
  }
  assume(filled==n);
  int pos=0;

  for(i=0;i<filled;i++){  
    r[indizes[i]].canonicalize();
    weight[i]=r[indizes[i]].guess_quality(c);
    if(weight[i]<weight[pos]) pos=i;
  }
  w=weight[pos];
  pos=indizes[pos];

  omfree(indizes);
  omfree(weight);

  assume(w==r[pos].guess_quality(c));
  assume(l<=pos);
  assume(u>=pos);
  return pos;
  
}

#endif
void red_object::canonicalize(){
  kBucketCanonicalize(bucket);
  if(sum)
    kBucketCanonicalize(sum->ac->bucket);
  
}
BOOLEAN good_has_t_rep(int i, int j,calc_dat* c){
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

static void notice_miss(int i, int j, calc_dat* c){
  if (TEST_OPT_PROT)
    PrintS("-");
 
}

static void cleanS(kStrategy strat, calc_dat* c){
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






static int add_to_reductors(calc_dat* c, poly h, int len){
  //inDebug(h);
  assume(lenS_correct(c->strat));
 
  int i;
  if (c->is_char0)
       i=simple_posInS(c->strat,h,pSLength(h,len),c->is_char0);
  else
    i=simple_posInS(c->strat,h,len,c->is_char0);
  
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

  c->strat->enterS(P,i,c->strat);
 
 

  c->strat->lenS[i]=len;
 
  if(c->strat->lenSw)
    c->strat->lenSw[i]=pSLength(P.p,len);
  return i;
 
}
static void length_one_crit(calc_dat* c, int pos, int len)
{
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
    shorten_tails(c,c->S->m[pos]);
  }
}
static sorted_pair_node* find_next_pair2(calc_dat* c, BOOLEAN go_higher){
  clean_top_of_pair_list(c);
  sorted_pair_node* s=pop_pair(c);


  return s;
}

static void move_forward_in_S(int old_pos, int new_pos,kStrategy strat, BOOLEAN is_char0)
{
  assume(old_pos>=new_pos);
  poly p=strat->S[old_pos];
  int ecart=strat->ecartS[old_pos];
  long sev=strat->sevS[old_pos];
  int s_2_r=strat->S_2_R[old_pos];
  int length=strat->lenS[old_pos];
  int length_w;
  if(is_char0)
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
  if(is_char0)
    strat->lenSw[new_pos]=length_w;
  //assume(lenS_correct(strat));
}
static void replace_pair(int & i, int & j, calc_dat* c)
{
  c->soon_free=NULL;
  int curr_deg;
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  int deciding_deg= pTotaldegree(lm);
  int* i_con =make_connections(i,j,lm,c);
  int z=0;

  for (int n=0;((n<c->n) && (i_con[n]>=0));n++){
    if (i_con[n]==j){
      now_t_rep(i,j,c);
      omfree(i_con);
      p_Delete(&lm,c->r);
      return;
    }
  }

  int* j_con =make_connections(j,lm,c);
  i= i_con[0];
  j=j_con[0];
  if(c->n>1){
    if (i_con[1]>=0)
      i=i_con[1];
    else {
      if (j_con[1]>=0)
        j=j_con[1];
    }
  }
  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  poly short_s;
  curr_deg=pTotaldegree(lm);
  int_pair_node* last=NULL;

  for (int n=0;((n<c->n) && (j_con[n]>=0));n++){
    for (int m=0;((m<c->n) && (i_con[m]>=0));m++){
      pLcm(c->S->m[i_con[m]], c->S->m[j_con[n]], lm);
      pSetm(lm);
      if (pTotaldegree(lm)>=deciding_deg)
      {
        soon_t_rep(i_con[m],j_con[n],c);
        int_pair_node* h= (int_pair_node*)omalloc(sizeof(int_pair_node));
        if (last!=NULL)
          last->next=h;
        else
          c->soon_free=h;
        h->next=NULL;
        h->a=i_con[m];
        h->b=j_con[n];
        last=h;
      }
      //      if ((comp_deg<curr_deg)
      //  ||
      //  ((comp_deg==curr_deg) &&
      short_s=ksCreateShortSpoly(c->S->m[i_con[m]],c->S->m[j_con[n]],c->r);
      if (short_s==NULL) {
        i=i_con[m];
        j=j_con[n];
        now_t_rep(i_con[m],j_con[n],c);
        p_Delete(&lm,c->r);
        omfree(i_con);
        omfree(j_con);

        return;
      }
#ifdef QUICK_SPOLY_TEST
      for (int dz=0;dz<=c->n;dz++){
        if (dz==c->n) {
          //have found not head reducing pair
          i=i_con[m];
          j=j_con[n];
          p_Delete(&short_s,c->r);
          p_Delete(&lm,c->r);
          omfree(i_con);
          omfree(j_con);

          return;
        }
        if (p_LmDivisibleBy(c->S->m[dz],short_s,c->r)) break;
      }
#endif
      int comp_deg(pTotaldegree(short_s));
      p_Delete(&short_s,c->r);
      if ((comp_deg<curr_deg))
         
      {
        curr_deg=comp_deg;
        i=i_con[m];
        j=j_con[n];
      }
    }
  }
  p_Delete(&lm,c->r);
  omfree(i_con);
  omfree(j_con);
  return;
}


static int* make_connections(int from, poly bound, calc_dat* c)
{
  ideal I=c->S;
  int s=pTotaldegree(bound);
  int* cans=(int*) omalloc(c->n*sizeof(int));
  int* connected=(int*) omalloc(c->n*sizeof(int));
  int cans_length=0;
  connected[0]=from;
  int connected_length=1;
  long neg_bounds_short= ~p_GetShortExpVector(bound,c->r);
  for (int i=0;i<c->n;i++){
    if (c->T_deg[i]>s) continue;
    if (i!=from){
      if(p_LmShortDivisibleBy(I->m[i],c->short_Exps[i],bound,neg_bounds_short,c->r)){
        cans[cans_length]=i;
        cans_length++;
      }
    }
  }
  int not_yet_found=cans_length;
  int con_checked=0;
  int pos;
  while((not_yet_found>0) && (con_checked<connected_length)){
    pos=connected[con_checked];
    for(int i=0;i<cans_length;i++){
      if (cans[i]<0) continue;
      if (has_t_rep(pos,cans[i],c))
      {
        connected[connected_length]=cans[i];
        connected_length++;
        cans[i]=-1;
        --not_yet_found;
      }
    }
    con_checked++;
  }
  if (connected_length<c->n){
    connected[connected_length]=-1;
  }
  omfree(cans);
  return connected;
}
static int* make_connections(int from, int to, poly bound, calc_dat* c)
{
  ideal I=c->S;
  int s=pTotaldegree(bound);
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
  BOOLEAN can_find_more=TRUE;
  while(TRUE){
    if ((con_checked<connected_length)&& (not_yet_found>0)){
      pos=connected[con_checked];
      for(int i=0;i<cans_length;i++){
        if (cans[i]<0) continue;
        if (has_t_rep(pos,cans[i],c))//||(trivial_syzygie(pos,cans[i],bound,c))
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



//len should be weighted length in char 0
static int simple_posInS (kStrategy strat, poly p,int len, BOOLEAN is_char0)
{


  if(strat->sl==-1) return 0;
  polyset set=strat->S;
  intset setL=strat->lenS;
  if (is_char0) setL=strat->lenSw;
  int length=strat->sl;
  int i;
  int an = 0;
  int en= length;

  if ((len>setL[length])
      || ((len==setL[length]) && (pLmCmp(set[length],p)== -1)))
    return length+1;

  loop
  {
    if (an >= en-1)
    {
      if ((len<setL[an])
          || ((len==setL[an]) && (pLmCmp(set[an],p) == 1))) return an;
      return en;
    }
    i=(an+en) / 2;
    if ((len<setL[i])
        || ((len==setL[i]) && (pLmCmp(set[i],p) == 1))) en=i;
    //else if ((len>setL[i])
    //|| ((len==setL[i]) && (pLmCmp(set[i],p) == -1))) an=i;
    else an=i;
  }
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
static sorted_pair_node** add_to_basis(poly h, int i_pos, int j_pos,calc_dat* c, int* ip)
{

  assume(h!=NULL);
//  BOOLEAN corr=lenS_correct(c->strat);
  BOOLEAN R_found=FALSE;
  void* hp;

  ++(c->n);
  ++(c->S->ncols);
  int i,j;
  i=c->n-1;
  sorted_pair_node** nodes=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*i);
  int spc=0;
  c->T_deg=(int*) omrealloc(c->T_deg,c->n*sizeof(int));
  c->T_deg[i]=pTotaldegree(h);
  c->tmp_pair_lm=(poly*) omrealloc(c->tmp_pair_lm,c->n*sizeof(poly));
  c->tmp_pair_lm[i]=pOne_Special(c->r);
  c->tmp_spn=(sorted_pair_node**) omrealloc(c->tmp_spn,c->n*sizeof(sorted_pair_node*));
  c->tmp_spn[i]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
  hp=omrealloc(c->rep, c->n *sizeof(int));
  if (hp!=NULL){
    c->rep=(int*) hp;
  } else {
    exit(1);
  }
  c->short_Exps=(long *) omrealloc(c->short_Exps ,c->n*sizeof(long));

  hp=omrealloc(c->lengths, c->n *sizeof(int));
  if (hp!=NULL){
    c->lengths=(int*) hp;
  } else {
    exit(1);
  }
  c->lengths[i]=pLength(h);
  hp=omrealloc(c->states, c->n * sizeof(char*));
 
    c->states=(char**) hp;
  c->gcd_of_terms=(poly*) omrealloc(c->gcd_of_terms, c->n *sizeof(poly));
  c->gcd_of_terms[i]=gcd_of_terms(h,c->r);
  c->rep[i]=i;
  hp=omalloc(i*sizeof(char));
  if (hp!=NULL){
    c->states[i]=(char*) hp;
  } else {
    exit(1);
  }
  hp=omrealloc(c->S->m,c->n*sizeof(poly));
  if (hp!=NULL){
    c->S->m=(poly*) hp;
  } else {
    exit(1);
  }
  c->S->m[i]=h;
  c->short_Exps[i]=p_GetShortExpVector(h,c->r);
  for (j=0;j<i;j++){
    if (c->rep[j]==j){
      //check product criterion

      c->states[i][j]=UNCALCULATED;
      assume(p_LmDivisibleBy(c->S->m[i],c->S->m[j],c->r)==
	     p_LmShortDivisibleBy(c->S->m[i],c->short_Exps[i],c->S->m[j],~(c->short_Exps[j]),c->r));
      if(!c->F4_mode)
      {
	assume(!(p_LmDivisibleBy(c->S->m[j],c->S->m[i],c->r)));
      }
      //lies I[i] under I[j] ?
      if(p_LmShortDivisibleBy(c->S->m[i],c->short_Exps[i],c->S->m[j],~(c->short_Exps[j]),c->r)){
        c->rep[j]=i;
	if (TEST_OPT_PROT)
	  PrintS("R"); 
	R_found=TRUE;

        c->Rcounter++;
        if((i_pos>=0) && (j_pos>=0)){
       
        }
        for(int z=0;z<j;z++){
          if(c->rep[z]!=z) continue;
          if (c->states[j][z]==UNCALCULATED){
            c->states[j][z]=UNIMPORTANT;
          }
        }
        for(int z=j+1;z<i;z++){
          if(c->rep[z]!=z) continue;
          if (c->states[z][j]==UNCALCULATED){
            c->states[z][j]=UNIMPORTANT;
          }
        }
      }
    }
    else {
      c->states[i][j]=UNIMPORTANT;
    }
    if ((c->lengths[i]==1) && (c->lengths[j]==1))
      c->states[i][j]=HASTREP;
    else if (pHasNotCF(c->S->m[i],c->S->m[j])){
      c->easy_product_crit++;
      c->states[i][j]=HASTREP;
    }
                        else if(extended_product_criterion(c->S->m[i],c->gcd_of_terms[i],c->S->m[j],c->gcd_of_terms[j],c)){
                                        c->states[i][j]=HASTREP;
					c->extended_product_crit++;
                                        //PrintS("E");
                        }
    if (c->states[i][j]==UNCALCULATED){

      
//      poly short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
      //    if (short_s)
      //    {
        sorted_pair_node* s=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
        s->i=max(i,j);
        s->j=min(i,j);
        s->expected_length=c->lengths[i]+c->lengths[j]-2;
      
        poly lm=pOne();

        pLcm(c->S->m[i], c->S->m[j], lm);
        pSetm(lm);
	s->deg=pTotaldegree(lm);
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
  }

  add_to_reductors(c, h, c->lengths[c->n-1]);
  //i=posInS(c->strat,c->strat->sl,h,0 ecart);

  if (c->lengths[c->n-1]==1)
    shorten_tails(c,c->S->m[c->n-1]);
  //you should really update c->lengths, c->strat->lenS, and the oder of polys in strat if you sort after lengths

  //for(i=c->strat->sl; i>0;i--)
  //  if(c->strat->lenS[i]<c->strat->lenS[i-1]) printf("fehler bei %d\n",i);
  if (c->Rcounter>50) {
    c->Rcounter=0;
    cleanS(c->strat,c);
  }
  if(!ip){
    qsort(nodes,spc,sizeof(sorted_pair_node*),pair_better_gen2);
 
    
    c->apairs=merge(c->apairs,c->pair_top+1,nodes,spc,c);
    c->pair_top+=spc;
    clean_top_of_pair_list(c);
    omfree(nodes);
    return NULL;
  }
  {
    *ip=spc;
    return nodes;
  }

  

}


static int iq_crit(const void* ap,const void* bp){

  sorted_pair_node* a=*((sorted_pair_node**)ap);
  sorted_pair_node* b=*((sorted_pair_node**)bp);
  assume(a->i>a->j);
  assume(b->i>b->j);
  int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  if(comp!=0)
    return comp;
  if (a->deg<b->deg) return -1;
  if (a->deg>b->deg) return 1;


  if (a->expected_length<b->expected_length) return -1;
  if (a->expected_length>b->expected_length) return 1;
  if (a->j>b->j) return 1;
  if (a->j<b->j) return -1;
  return 0;
}

static sorted_pair_node** add_to_basis_ideal_quotient(poly h, int i_pos, int j_pos,calc_dat* c, int* ip)
{

  assume(h!=NULL);
//  BOOLEAN corr=lenS_correct(c->strat);
  BOOLEAN R_found=FALSE;
  void* hp;

  ++(c->n);
  ++(c->S->ncols);
  int i,j;
  i=c->n-1;
  sorted_pair_node** nodes=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*i);
  int spc=0;
  c->T_deg=(int*) omrealloc(c->T_deg,c->n*sizeof(int));
  c->T_deg[i]=pTotaldegree(h);
  c->tmp_pair_lm=(poly*) omrealloc(c->tmp_pair_lm,c->n*sizeof(poly));
  c->tmp_pair_lm[i]=pOne_Special(c->r);
  c->tmp_spn=(sorted_pair_node**) omrealloc(c->tmp_spn,c->n*sizeof(sorted_pair_node*));
  c->tmp_spn[i]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));

  hp=omrealloc(c->rep, c->n *sizeof(int));
  if (hp!=NULL){
    c->rep=(int*) hp;
  } else {
    exit(1);
  }
  c->short_Exps=(long *) omrealloc(c->short_Exps ,c->n*sizeof(long));

  hp=omrealloc(c->lengths, c->n *sizeof(int));
  if (hp!=NULL){
    c->lengths=(int*) hp;
  } else {
    exit(1);
  }
  c->lengths[i]=pLength(h);
  hp=omrealloc(c->states, c->n * sizeof(char*));
 
    c->states=(char**) hp;
  c->gcd_of_terms=(poly*) omrealloc(c->gcd_of_terms, c->n *sizeof(poly));
  c->gcd_of_terms[i]=gcd_of_terms(h,c->r);
  c->rep[i]=i;
  hp=omalloc(i*sizeof(char));
  if (hp!=NULL){
    c->states[i]=(char*) hp;
  } else {
    exit(1);
  }
  hp=omrealloc(c->S->m,c->n*sizeof(poly));
  if (hp!=NULL){
    c->S->m=(poly*) hp;
  } else {
    exit(1);
  }
  c->S->m[i]=h;
  c->short_Exps[i]=p_GetShortExpVector(h,c->r);
  for (j=0;j<i;j++){
    
    //check product criterion
    
    c->states[i][j]=UNCALCULATED;
    assume(p_LmDivisibleBy(c->S->m[i],c->S->m[j],c->r)==
	   p_LmShortDivisibleBy(c->S->m[i],c->short_Exps[i],c->S->m[j],~(c->short_Exps[j]),c->r));
    if(!c->F4_mode)
    {
      assume(!(p_LmDivisibleBy(c->S->m[j],c->S->m[i],c->r)));
    }
    //lies I[i] under I[j] ?
    
    if ((c->lengths[i]==1) && (c->lengths[j]==1))
      c->states[i][j]=HASTREP;
    else if (pHasNotCF(c->S->m[i],c->S->m[j]))
    {
      c->easy_product_crit++;
      c->states[i][j]=HASTREP;
    }
    else if(extended_product_criterion(c->S->m[i],c->gcd_of_terms[i],c->S->m[j],c->gcd_of_terms[j],c))
    {
      c->states[i][j]=HASTREP;
      c->extended_product_crit++;
      //PrintS("E");
    }
      //  if (c->states[i][j]==UNCALCULATED){

      
//      poly short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
      //    if (short_s)
      //    {
    sorted_pair_node* s=c->tmp_spn[j];//(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
    s->i=max(i,j);
    s->j=min(i,j);
    assume(s->j==j);
    s->expected_length=c->lengths[i]+c->lengths[j]-2;
      
    poly lm=c->tmp_pair_lm[j];//=pOne_Special();
      
    pLcm(c->S->m[i], c->S->m[j], lm);
    pSetm(lm);
    s->deg=pTotaldegree(lm);
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
  
  assume(spc==i);
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
      nodes_final[spc_final++]=nodes[lower];
      c->tmp_spn[nodes[lower]->j]=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      nodes[lower]=NULL;
      for(lower=lower+1;lower<=upper;lower++)
      {
	//	free_sorted_pair_node(nodes[lower],c->r);
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

  if (c->lengths[c->n-1]==1)
    shorten_tails(c,c->S->m[c->n-1]);
  //you should really update c->lengths, c->strat->lenS, and the oder of polys in strat if you sort after lengths

  //for(i=c->strat->sl; i>0;i--)
  //  if(c->strat->lenS[i]<c->strat->lenS[i-1]) printf("fehler bei %d\n",i);
  if (c->Rcounter>50) {
    c->Rcounter=0;
    cleanS(c->strat,c);
  }
  if(!ip){
    qsort(nodes_final,spc_final,sizeof(sorted_pair_node*),pair_better_gen2);
 
    
    c->apairs=merge(c->apairs,c->pair_top+1,nodes_final,spc_final,c);
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






#if 0
static poly redNF (poly h,kStrategy strat)
{
  int j = 0;
  int z = 3;
  unsigned long not_sev;

  if (0 > strat->sl)
  {
    return h;
  }
  not_sev = ~ pGetShortExpVector(h);
  loop
    {
      if (pLmShortDivisibleBy(strat->S[j], strat->sevS[j], h, not_sev))
      {
        //if (strat->interpt) test_int_std(strat->kIdeal);
        /*- compute the s-polynomial -*/
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
	  
          PrintS("red:");
          wrp(h);
          PrintS(" with ");
          wrp(strat->S[j]);
        }
#endif
        h = ksOldSpolyRed(strat->S[j],h,strat->kNoether);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("\nto:");
          wrp(h);
          PrintLn();
        }
#endif
        if (h == NULL) return NULL;
        z++;
        if (z>=10)
        {
          z=0;
          pNormalize(h);
        }
        /*- try to reduce the s-polynomial -*/
        j = 0;
        not_sev = ~ pGetShortExpVector(h);
      }
      else
      {
        if (j >= strat->sl) return h;
        j++;
      }
    }
}
#else

static poly redNF2 (poly h,calc_dat* c , int &len, number&  m,int n)
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
  //int max_pos=simple_posInS(strat,P.p);
  loop
    {

      j=kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P);
      if ((j>=0) && ((!n)||(strat->lenS[j]<=n)))
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

  int sl=strat->sl;
  int i;
  int len=pLength(h);
  for(i=0;i<=strat->sl;i++){
    if(strat->lenS[i]>2)
      break;
  }
  return(redNFTail(h,i-1,strat, len));
}

static void line_of_extended_prod(int fixpos,calc_dat* c){
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
static void c_S_element_changed_hook(int pos, calc_dat* c){
  length_one_crit(c,pos, c->lengths[pos]);
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
struct int_poly_pair{
  poly p;
  int n;
};

void t2ippa_rec(poly* ip,int* ia, poly_tree_node* k, int &offset){
    if(!k) return;
    t2ippa_rec(ip,ia,k->l,offset);
    ip[offset]=k->p;
    ia[k->n]=offset;
    ++offset;

    t2ippa_rec(ip,ia,k->r,offset);
    delete k;
  }
void t2ippa(poly* ip,int* ia,exp_number_builder & e){

  int o=0;
  t2ippa_rec(ip,ia,e.top_level,o);
}
int anti_poly_order(const void* a, const void* b){
  return -pLmCmp(((int_poly_pair*) a)->p,((int_poly_pair*) b)->p );
}
mac_poly mac_p_add_ff_qq(mac_poly a, number f,mac_poly b){
  mac_poly erg;
  mac_poly* set_this;
  set_this=&erg;
  while((a!=NULL) &&(b!=NULL)){
    if (a->exp<b->exp){
      (*set_this)=a;
      a=a->next;
      set_this= &((*set_this)->next);
    } 
    else{
      if (a->exp>b->exp){
	mac_poly in =new mac_poly_r();
	in->exp=b->exp;
	in->coef=nMult(b->coef,f);
	(*set_this)=in;
	b=b->next;
	set_this= &((*set_this)->next);
      }
      else {
	//a->exp==b->ecp
	number n=nMult(b->coef,f);
	number n2=nAdd(a->coef,n);
	nDelete(&n);
	nDelete(&(a->coef));
	if (nIsZero(n2)){
	  nDelete(&n2);
	  mac_poly ao=a;
	  a=a->next;
	  delete ao;
	  b=b->next;
	  
	} else {
	  a->coef=n2;
	  b=b->next;
	  (*set_this)=a;
	  a=a->next;
	  set_this= &((*set_this)->next);
	}
 
      }
    
    }
  }
  if((a==NULL)&&(b==NULL)){
    (*set_this)=NULL;
    return erg;
  }
  if (b==NULL) {
    (*set_this=a);
    return erg;
  }
  
  //a==NULL
  while(b!=NULL){
    mac_poly mp= new mac_poly_r();
    mp->exp=b->exp;
    mp->coef=nMult(f,b->coef);
    (*set_this)=mp;
    set_this=&(mp->next);
    b=b->next;
  }
  (*set_this)=NULL;
  return erg;
  
}
void mac_mult_cons(mac_poly p,number c){
  while(p){
    number m=nMult(p->coef,c);
    nDelete(&(p->coef));
    p->coef=m;
    p=p->next;
  }
  
}
int mac_length(mac_poly p){
  int l=0;
  while(p){
    l++;
    p=p->next;
  }
  return l;
}
//contrary to delete on the mac_poly_r, the coefficients are also destroyed here
void mac_destroy(mac_poly p){
  mac_poly iter=p;
  while(iter)
  {
    mac_poly next=iter->next;
    nDelete(&iter->coef);
    delete iter;
    iter=next;
  }
}
BOOLEAN is_valid_ro(red_object & ro){
  red_object r2=ro;
  ro.validate();
  if ((r2.p!=ro.p)||(r2.sev!=ro.sev)||(r2.sum!=ro.sum)) return FALSE;
  return TRUE;
}
void pre_comp(poly* p,int & pn,calc_dat* c){
  if(!(pn))
    return;
  mac_poly* q=(mac_poly*) omalloc(pn*sizeof(mac_poly)); 
  int i;
  exp_number_builder e;
  for(i=0;i<pn;i++){
    assume(p[i]!=NULL);
    q[i]=new mac_poly_r();
    q[i]->exp=e.get_n(p[i]);
    q[i]->coef=nCopy(p[i]->coef);
    
    mac_poly qa=q[i];
    poly pa=p[i];
    while(pa->next!=NULL){
      qa->next = new mac_poly_r();
      qa=qa->next;
      pa=pa->next,
      qa->exp=e.get_n(pa);
      qa->coef=nCopy(pa->coef);
     
      
    }
    qa->next=NULL;
    pDelete(&p[i]);
  }
  poly* ip= (poly*)omalloc (e.n*sizeof(poly));
  int* ia=(int*) omalloc (e.n*sizeof(int));
  t2ippa(ip,ia,e);
  for(i=0;i<pn;i++){
    mac_poly mp=q[i];
    while(mp!=NULL){
      mp->exp=ia[mp->exp];
      mp=mp->next;
    }
    
  }
//gaus reduction start
  int col, row;
  col=0;
  row=0;
  assume(pn>0);
  while(row<pn-1){
    //row is the row where pivot should be
    // row== pn-1 means we have only to act on one row so no red nec.
    //we assume further all rows till the pn-1 row are non-zero
    
    //select column
    int i;
    col=q[row]->exp;
    int found_in_row=row;
    for(i=row;i<pn;i++){
      if(q[i]->exp<col){
	col=q[i]->exp;
	found_in_row=i;
      }
      
    }
    //select pivot
    int act_l=mac_length(q[found_in_row]);
    for(i=row+1;i<pn;i++){
      if((q[i]->exp==col)&&(mac_length(q[i])<act_l)){
	found_in_row=i;
	act_l=mac_length(q[i]);//should be optimized here
      }
    }
    mac_poly h=q[row];
    q[row]=q[found_in_row];
    q[found_in_row]=h;

    //reduction
    for(i=row+1;i<pn;i++){
      if(q[i]->exp==q[row]->exp){
	
	number c1=nNeg(nCopy(q[i]->coef));
	number c2=q[row]->coef;
	//use checkcoeff later
	mac_mult_cons(q[i],c2);
	q[i]=mac_p_add_ff_qq(q[i],c1,q[row]);
      }
	  
	
	
    }
    for(i=row+1;i<pn;i++){
      if(q[i]==NULL){
	q[i]=q[pn-1];
	pn--;
	if(i!=pn){i--;}
      }
    }
  
    row++;
  }


//gaus reduction end  

  for(i=0;i<pn;i++){
    poly pa;
    mac_poly qa;
    p[i]=pLmInit(ip[q[i]->exp]);
    pSetCoeff(p[i],q[i]->coef);
    pa=p[i];
    qa=q[i];
    while(qa->next!=NULL){
      qa=qa->next;
      pa->next=pLmInit(ip[qa->exp]);
      pa=pa->next;
      pa->coef=qa->coef;
    }
    pa->next=NULL;
  }
  for(i=0;i<e.n;i++){
    pDelete(&ip[i]);  
  }
  omfree(ip);
  omfree(ia);
}

static void go_on (calc_dat* c){
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
      //replace_pair(s->i,s->j,c);
    if(s->i==s->j) {
      free_sorted_pair_node(s,c->r);
      continue;
	}
    }
    poly h;
    if(s->i>=0)
      h=ksOldCreateSpoly(c->S->m[s->i], c->S->m[s->j], NULL, c->r);
    else
      h=s->lcm_of_lm;
    if(s->i>=0)
      now_t_rep(s->j,s->i,c);
    number coef;
    int mlen=pLength(h);
    h=redNF2(h,c,mlen,coef,2);
    redTailShort(h,c->strat);
    nDelete(&coef);

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
    buf[j].sum=NULL;
    buf[j].bucket = kBucketCreate(currRing);
    int len=pLength(p[j]);
    kBucketInit(buf[j].bucket,buf[j].p,len);
  }
  omfree(p);
  qsort(buf,i,sizeof(red_object),red_object_better_gen);
//    Print("\ncurr_deg:%i\n",curr_deg);
  if (TEST_OPT_PROT)
    Print("M[%i, ",i);
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
      int qal;
      if (c->strat->lenSw)
	qal=c->strat->lenSw[z2];
      else
	qal=c->strat->lenS[z2];
      int new_pos=simple_posInS(c->strat,c->strat->S[z2],qal,c->is_char0);
      if (new_pos<z2)
      { 
	move_forward_in_S(z2,new_pos,c->strat,c->is_char0);
      }
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
 //  for(j=0;j<i;j++){
//     if(buf[j].p==NULL) PrintS("\n ZERO ALERT \n");
//     int z;
//      for(z=0;z<j;z++){
//       if (pLmEqual(buf[z].p, buf[j].p))
// 	PrintS("\n Critical Warning!!!! \n");
      
//     }
//   }
  int* ibuf=(int*) omalloc(i*sizeof(int));
  sorted_pair_node*** sbuf=(sorted_pair_node***) omalloc(i*sizeof(sorted_pair_node**));
  for(j=0;j<i;j++)
  {
    int len;
    poly p;
    buf[j].flatten();
    kBucketClear(buf[j].bucket,&p, &len);
    kBucketDestroy(&buf[j].bucket);
    // delete buf[j];
    //remember to free res here
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

  qsort(big_sbuf,sum,sizeof(sorted_pair_node*),pair_better_gen2);
  c->apairs=merge(c->apairs,c->pair_top+1,big_sbuf,sum,c);
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
  return;
}

static poly redNF (poly h,kStrategy strat, int &len)
{
  len=0;
  if (h==NULL) return NULL;
  int j;

  len=pLength(h);
  if (0 > strat->sl)
  {
    return h;
  }
  LObject P(h);
  P.SetShortExpVector();
  P.bucket = kBucketCreate(currRing);
  kBucketInit(P.bucket,P.p,len /*pLength(P.p)*/);
  //int max_pos=simple_posInS(strat,P.p);
  loop
    {
      j=kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P);
      if (j>=0)
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
        nDelete(&coef);
        h = kBucketGetLm(P.bucket);
        if (h==NULL) return NULL;
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
        return P.p;
      }
    }
}
#endif
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

static void do_this_spoly_stuff(int i,int j,calc_dat* c){
  poly f=c->S->m[i];
  poly g=c->S->m[j];
  poly h=ksOldCreateSpoly(f, g, NULL, c->r);
  poly hr=NULL;
#ifdef FULLREDUCTIONS
  if (h!=NULL)
  {
    int len;

//    hr=redNF2(h,c,len);
//      hr=redNF(h,c->strat,len);

    if (hr!=NULL)
#ifdef REDTAIL_S
      hr = redNFTail(hr,c->strat->sl,c->strat,len);
#else
    hr = redtailBba(hr,c->strat->sl,c->strat);
#endif

  }
#else
  if (h!=NULL)
  {
    int len;
//    hr=redNF2(h,c,len);
  }
#endif
  c->normal_forms++;
  if (hr==NULL)
  {
    notice_miss(i, j, c);

  }
  else
  {

#ifdef HEAD_BIN
    hr=p_MoveHead(hr,c->HeadBin);
#endif
    add_to_basis(hr, i,j,c);
  }
}
//try to fill, return FALSE iff queue is empty
static void simplify(monom_poly& h, calc_dat* c){
  mp_array_list* F=c->F;
  poly_array_list* F_minus=c->F_minus;
  while(F)
  {
    assume(F_minus!=NULL);
    int i;
    int posm=posInMonomPolys (F->mp, F->size, h,c);
#ifdef TGB_DEBUG
#endif
     //for(i=0;i<F->size;i++)
    for(i=min(posm,F->size-1);i>=0;i--)
    {
      //      if((i>=posm)&&(F->mp[i].f!=h.f)) break;
      if((i<posm)&&(F->mp[i].f!=h.f)) break;
      if ((h.f==F->mp[i].f) &&(p_LmDivisibleBy(F->mp[i].m,h.m,c->r)))
      {
	
	//	Print("found");
       
	  //according to the algorithm you should test (!(pIsConstant(F[i].m)))
	  //but I think this is only because of bad formulation
	int j;
	
	poly lm=pLmInit(h.f);
	pSetCoeff(lm,nInit(1));

	lm=pMult_mm(lm,F->mp[i].m);

	int pos;
	j=-1;
	pos=posInPolys (F_minus->p, F_minus->size, lm,c);
      if((F_minus->size>pos)&&(pLmEqual(lm,F_minus->p[pos])))
	j=pos;
      if((pos>0) &&(pLmEqual(lm,F_minus->p[pos-1])))
	j=pos-1;
      assume(j!=-1);
      //        if(j==-1) Print("\n jAltert \n");
// 	for(j=0;j<F_minus->size;j++)
// 	{
// 	  if (pLmEqual(F_minus->p[j],lm))
// 	    break;
// 	}
// 	assume(j<F_minus->size);
	pDelete(&lm);
	if(j>=0)
	{
	  pExpVectorSub(h.m,F->mp[i].m);
	  h.f=F_minus->p[j];
	}
	break;
      }
    }
    F=F->next;
    F_minus=F_minus->next;
  }
  assume(F_minus==NULL);
}
tgb_matrix::tgb_matrix(int i, int j){
  n=(number**) omalloc(i*sizeof (number*));;
  int z;
  int z2;
  for(z=0;z<i;z++)
  {
    n[z]=(number*)omalloc(j*sizeof(number));
    for(z2=0;z2<j;z2++)
    {
      n[z][z2]=nInit(0);
    }
  }
  this->columns=j;
  this->rows=i;
  free_numbers=FALSE;
}
tgb_matrix::~tgb_matrix(){
  int z;
  for(z=0;z<rows;z++)
  {
    if(n[z])
    {
      if(free_numbers)
      {
	int z2;
	for(z2=0;z2<columns;z2++)
	{
	  nDelete(&(n[z][z2]));
	}
      }
      omfree(n[z]);
    }
  }
  omfree(n);
}
void tgb_matrix::print(){
  int i;
  int j;
  Print("\n");
  for(i=0;i<rows;i++)
  {
    Print("(");
    for(j=0;j<columns;j++)
    {
      StringSetS("");
      n_Write(n[i][j],currRing);
      Print(StringAppendS(""));
      Print("\t");
    }
    Print(")\n");
  }
}
//transfers ownership of n to the matrix
void tgb_matrix::set(int i, int j, number n){
  assume(i<rows);
  assume(j<columns);
  this->n[i][j]=n;
}
int tgb_matrix::get_rows(){
  return rows;
}
int tgb_matrix::get_columns(){
  return columns;
}
number tgb_matrix::get(int i, int j){
  assume(i<rows);
  assume(j<columns);
  return n[i][j];
}
BOOLEAN tgb_matrix::is_zero_entry(int i, int j){
  return (nIsZero(n[i][j]));
}
void tgb_matrix::perm_rows(int i, int j){
  number* h;
  h=n[i];
  n[i]=n[j];
  n[j]=h;
}
int tgb_matrix::min_col_not_zero_in_row(int row){
  int i;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      return i;
  }
  return columns;//error code
}
int tgb_matrix::next_col_not_zero(int row,int pre){
  int i;
  for(i=pre+1;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      return i;
  }
  return columns;//error code
}
BOOLEAN tgb_matrix::zero_row(int row){
  int i;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      return FALSE;
  }
  return TRUE;
}
int tgb_matrix::non_zero_entries(int row){
  int i;
  int z=0;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      z++;
  }
  return z;
}
//row add_to=row add_to +row summand*factor
void tgb_matrix::add_lambda_times_row(int add_to,int summand,number factor){
  int i;
  for(i=0;i<columns;i++){
    if(!(nIsZero(n[summand][i])))
    {
      number n1=n[add_to][i];
      number n2=nMult(factor,n[summand][i]);
      n[add_to][i]=nAdd(n1,n2);
      nDelete(&n1);
      nDelete(&n2);
    }
  }
}
void tgb_matrix::mult_row(int row,number factor){
  if (nIsOne(factor))
    return;
  int i;
  for(i=0;i<columns;i++){
    if(!(nIsZero(n[row][i])))
    {
      number n1=n[row][i];
      n[row][i]=nMult(n1,factor);
      nDelete(&n1);
    }
  }
}
void tgb_matrix::free_row(int row, BOOLEAN free_non_zeros){
  int i;
  for(i=0;i<columns;i++)
    if((free_non_zeros)||(!(nIsZero(n[row][i]))))
      nDelete(&(n[row][i]));
  omfree(n[row]);
  n[row]=NULL;
}


tgb_sparse_matrix::tgb_sparse_matrix(int i, int j, ring rarg){
  mp=(mac_poly*) omalloc(i*sizeof (mac_poly));;
  int z;
  int z2;
  for(z=0;z<i;z++)
  {
    mp[z]=NULL;
  }
  this->columns=j;
  this->rows=i;
  free_numbers=FALSE;
  r=rarg;
}
tgb_sparse_matrix::~tgb_sparse_matrix(){
  int z;
  for(z=0;z<rows;z++)
  {
    if(mp[z])
    {
      if(free_numbers)
      {
	mac_destroy(mp[z]);
      }
      else {
	while(mp[z])
	{
	 
	  mac_poly next=mp[z]->next;
	  delete mp[z];
	  mp[z]=next;
	}
      }
    }
  }
  omfree(mp);
}
int row_cmp_gen(const void* a, const void* b){
  const mac_poly ap= *((mac_poly*) a);
  const mac_poly bp=*((mac_poly*) b);
  if (ap==NULL) return 1;
  if (bp==NULL) return -1;
  if (ap->exp<bp->exp) return -1;
  return 1;
}
void tgb_sparse_matrix::sort_rows(){
  qsort(mp,rows,sizeof(mac_poly),row_cmp_gen);
}
void tgb_sparse_matrix::print(){
  int i;
  int j;
  Print("\n");
  for(i=0;i<rows;i++)
  {
    Print("(");
    for(j=0;j<columns;j++)
    {
      StringSetS("");
      number n=get(i,j);
      n_Write(n,currRing);
      Print(StringAppendS(""));
      Print("\t");
    }
    Print(")\n");
  }
}
//transfers ownership of n to the matrix
void tgb_sparse_matrix::set(int i, int j, number n){
  assume(i<rows);
  assume(j<columns);
  mac_poly* set_this=&mp[i];
  //  while(((*set_this)!=NULL)&&((*set_this)­>exp<j))
  while(((*set_this)!=NULL) && ((*set_this)->exp<j))
    set_this=&((*set_this)->next);

  if (((*set_this)==NULL)||((*set_this)->exp>j))
  {
    if (nIsZero(n)) return;
    mac_poly old=(*set_this);
    (*set_this)=new mac_poly_r();
    (*set_this)->exp=j;
    (*set_this)->coef=n;
    (*set_this)->next=old;
    return;
  }
  assume((*set_this)->exp==j);
  if(!nIsZero(n))
  {
    nDelete(&(*set_this)->coef);
    (*set_this)->coef=n;
  }
  else
  {
    nDelete(&(*set_this)->coef);
    mac_poly dt=(*set_this);
    (*set_this)=dt->next;
    delete dt;
    
   
  }
  return;
}



int tgb_sparse_matrix::get_rows(){
  return rows;
}
int tgb_sparse_matrix::get_columns(){
  return columns;
}
number tgb_sparse_matrix::get(int i, int j){
  assume(i<rows);
  assume(j<columns);
  mac_poly r=mp[i];
  while((r!=NULL)&&(r->exp<j))
    r=r->next;
  if ((r==NULL)||(r->exp>j))
  {
    number n=nInit(0);
    return n;
  }
  assume(r->exp==j);
  return r->coef;
}
BOOLEAN tgb_sparse_matrix::is_zero_entry(int i, int j){
  assume(i<rows);
  assume(j<columns);
  mac_poly r=mp[i];
  while((r!=NULL)&&(r->exp<j))
    r=r->next;
  if ((r==NULL)||(r->exp>j))
  {
    return TRUE;
  }
  assume(!nIsZero(r->coef));
  assume(r->exp==j);
  return FALSE;
  
}

int tgb_sparse_matrix::min_col_not_zero_in_row(int row){
  if(mp[row]!=NULL)
  {
    assume(!nIsZero(mp[row]->coef));
    return mp[row]->exp;
  }
  else

 
  return columns;//error code
}
int tgb_sparse_matrix::next_col_not_zero(int row,int pre){  
  mac_poly r=mp[row];
  while((r!=NULL)&&(r->exp<=pre))
    r=r->next;
  if(r!=NULL)
  {
    assume(!nIsZero(r->coef));
    return r->exp;
  }
  return columns;//error code
}
BOOLEAN tgb_sparse_matrix::zero_row(int row){
  assume((mp[row]==NULL)||(!nIsZero(mp[row]->coef)));
  if (mp[row]==NULL)
    return TRUE;
  else
    return FALSE;
}
void tgb_sparse_matrix::row_normalize(int row){
  if (!rField_has_simple_inverse(r))  /* Z/p, GF(p,n), R, long R/C */
  {
    mac_poly m=mp[row];
	while (m!=NULL)
	{
	  if (currRing==r) {nTest(m->coef);}
	  n_Normalize(m->coef,r);
	  m=m->next;
	}
  }
}
void tgb_sparse_matrix::row_content(int row){
  
  mac_poly ph=mp[row];
  number h,d;
  mac_poly p;

  if(TEST_OPT_CONTENTSB) return;
  if(ph->next==NULL)
  {
    nDelete(&ph->coef);
    ph->coef=nInit(1);
  }
  else
  {
    nNormalize(ph->coef);
    if(!nGreaterZero(ph->coef)) {
      //ph = pNeg(ph);
      p=ph;
      while(p)
      {
	p->coef=nNeg(p->coef);
	p=p->next;
      }
    }
    
    h=nCopy(ph->coef);
    p = ph->next;
    
    while (p!=NULL)
    {
      nNormalize(p->coef);
      d=nGcd(h,p->coef,currRing);
      nDelete(&h);
      h = d;
      if(nIsOne(h))
      {
        break;
      }
      p=p->next;
    }
    p = ph;
    //number tmp;
    if(!nIsOne(h))
    {
      while (p!=NULL)
      {
    
        d = nIntDiv(pGetCoeff(p),h);
	nDelete(&p->coef);
        p->coef=d;
        p=p->next;
      }
    }
    nDelete(&h);

  }
}
int tgb_sparse_matrix::non_zero_entries(int row){

  return mac_length(mp[row]);
}
//row add_to=row add_to +row summand*factor
void tgb_sparse_matrix::add_lambda_times_row(int add_to,int summand,number factor){
  mp[add_to]= mac_p_add_ff_qq(mp[add_to], factor,mp[summand]);

}
void tgb_sparse_matrix::mult_row(int row,number factor){
  if (nIsZero(factor))
  {
    mac_destroy(mp[row]);
    mp[row]=NULL;
   
    return;
  }
  if(nIsOne(factor))
    return;
  mac_mult_cons(mp[row],factor);
}
void tgb_sparse_matrix::free_row(int row, BOOLEAN free_non_zeros){
  if(free_non_zeros)
    mac_destroy(mp[row]);
  else
  {
    while(mp[row])
    {
      
      mac_poly next=mp[row]->next;
      delete mp[row];
      mp[row]=next;
    }
  }
  mp[row]=NULL;
}
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
    r=r->next;
    
  }
  return p;

}

void simple_gauss(tgb_sparse_matrix* mat, calc_dat* c){
  int col, row;
  int* row_cache=(int*) omalloc(mat->get_rows()*sizeof(int));
  col=0;
  row=0;
  int i;
  int pn=mat->get_rows();
  int matcol=mat->get_columns();
  int* area=(int*) omalloc(sizeof(int)*((matcol-1)/bundle_size+1));
  const int max_area_index=(matcol-1)/bundle_size;
    //rows are divided in areas 
  //if row begins with columns col, it is located in [area[col/bundle_size],area[col/bundle_size+1]-1]
  assume(pn>0);
  //first clear zeroes
  for(i=0;i<pn;i++)
  {
    if(mat->zero_row(i))
    {
      mat->perm_rows(i,pn-1);
      pn--;
      if(i!=pn){i--;}
    }
 
  }
  mat->sort_rows();
  for(i=0;i<pn;i++)
  {
      row_cache[i]=mat->min_col_not_zero_in_row(i);
      // Print("row_cache:%d\n",row_cache[i]);
  }
  int last_area=-1;
  for(i=0;i<pn;i++)
  {
    int this_area=row_cache[i]/bundle_size;
    assume(this_area>=last_area);
    if(this_area>last_area)
    {
      int j;
      for(j=last_area+1;j<=this_area;j++)
	area[j]=i;
      last_area=this_area;
    }
  }
  for(i=last_area+1;i<=max_area_index;i++)
  {
    area[i]=pn;
  }
  while(row<pn-1){
    //row is the row where pivot should be
    // row== pn-1 means we have only to act on one row so no red nec.
    //we assume further all rows till the pn-1 row are non-zero
    
    //select column
   
    //col=mat->min_col_not_zero_in_row(row);
    int max_in_area;
    {
      int tai=row_cache[row]/bundle_size;
      assume(tai<=max_area_index);
      if(tai==max_area_index)
	max_in_area=pn-1;
      else
	max_in_area=area[tai+1]-1;
    }
    assume(row_cache[row]==mat->min_col_not_zero_in_row(row));
    col=row_cache[row];

    
    assume(col!=matcol);
    int found_in_row;
    
    found_in_row=row;
    BOOLEAN must_reduce=FALSE;
    assume(pn<=mat->get_rows());
    for(i=row+1;i<=max_in_area;i++){
      int first;//=mat->min_col_not_zero_in_row(i);
      assume(row_cache[i]==mat->min_col_not_zero_in_row(i));
      first=row_cache[i];
      assume(first!=matcol);
      if(first<col)
      {
	col=first;
	found_in_row=i;
	must_reduce=FALSE;
      }
      else {
	if(first==col)
	  must_reduce=TRUE;
      }
    }
    //select pivot
    int act_l=nSize(mat->get(found_in_row,col))*mat->non_zero_entries(found_in_row);
    if(must_reduce)
    {
      for(i=found_in_row+1;i<=max_in_area;i++){
	assume(mat->min_col_not_zero_in_row(i)>=col);
	int first;
	assume(row_cache[i]==mat->min_col_not_zero_in_row(i));
	first=row_cache[i];
	assume(first!=matcol);
	//      if((!(mat->is_zero_entry(i,col)))&&(mat->non_zero_entries(i)<act_l))
	int nz;
	if((row_cache[i]==col)&&((nz=nSize(mat->get(i,col))*mat->non_zero_entries(i))<act_l))
	{
	  found_in_row=i;
	act_l=nz;
	}
	
      }
    }
    mat->perm_rows(row,found_in_row);
    int h=row_cache[row];
    row_cache[row]=row_cache[found_in_row];
    row_cache[found_in_row]=h;



    if(!must_reduce){
      row++;
      continue;
    }
    //reduction
    //must extract content and normalize here
    mat->row_content(row);
    mat->row_normalize(row);

    //for(i=row+1;i<pn;i++){
    for(i=max_in_area;i>row;i--)
    {
      int col_area_index=col/bundle_size;
      assume(col_area_index<=max_area_index);
      assume(mat->min_col_not_zero_in_row(i)>=col);
      int first;
      assume(row_cache[i]==mat->min_col_not_zero_in_row(i));
      first=row_cache[i];
      assume(first!=matcol);
      if(row_cache[i]==col)
      {
	
	number c1=mat->get(i,col);
	number c2=mat->get(row,col);
	number n1=c1;
	number n2=c2;

	ksCheckCoeff(&n1,&n2);
	//nDelete(&c1);
	n1=nNeg(n1);
	mat->mult_row(i,n2);
	mat->add_lambda_times_row(i,row,n1);
	nDelete(&n1);
	nDelete(&n2);
	assume(mat->is_zero_entry(i,col));
	row_cache[i]=mat->min_col_not_zero_in_row(i);
	assume(mat->min_col_not_zero_in_row(i)>col);
	if(row_cache[i]==matcol)
	{
	  int index;
	  index=i;
	  int last_in_area;
	  int this_cai=col_area_index;
	  while(this_cai<max_area_index)
	  {
	    last_in_area=area[this_cai+1]-1;
	    int h_c=row_cache[last_in_area];
	    row_cache[last_in_area]=row_cache[index];
	    row_cache[index]=h_c;
	    mat->perm_rows(index,last_in_area);
	    index=last_in_area;
	    this_cai++;
	    area[this_cai]--;
	  }
	  mat->perm_rows(index,pn-1);
	  row_cache[index]=row_cache[pn-1];
	  row_cache[pn-1]=matcol;
	  pn--;
	}
	else 
	{
	  int index;
	  index=i;
	  int last_in_area;
	  int this_cai=col_area_index;
	  int final_cai=row_cache[index]/bundle_size;
	  assume(final_cai<=max_area_index);
	  while(this_cai<final_cai)
	  {
	    last_in_area=area[this_cai+1]-1;
	    int h_c=row_cache[last_in_area];
	    row_cache[last_in_area]=row_cache[index];
	    row_cache[index]=h_c;
	    mat->perm_rows(index,last_in_area);
	    index=last_in_area;
	    this_cai++;
	    area[this_cai]--;
	  }
	}
      }
      else
	assume(mat->min_col_not_zero_in_row(i)>col);
    }

//     for(i=row+1;i<pn;i++)
//     {
//       assume(mat->min_col_not_zero_in_row(i)==row_cache[i]);
//       // if(mat->zero_row(i))
//       assume(matcol==mat->get_columns());
//       if(row_cache[i]==matcol)
//       {
// 	assume(mat->zero_row(i));
// 	mat->perm_rows(i,pn-1);
// 	row_cache[i]=row_cache[pn-1];
// 	row_cache[pn-1]=matcol;
// 	pn--;
// 	if(i!=pn){i--;}
//       }
//     }
#ifdef TGB_DEBUG
  {
  int last=-1;
  for(i=0;i<pn;i++)
  {
    int act=mat->min_col_not_zero_in_row(i);
    assume(act>last);
    
  }
  for(i=pn;i<mat->get_rows();i++)
  {
    assume(mat->zero_row(i));
    
  }
  

  }
#endif
    row++;
  }
  omfree(area);
  omfree(row_cache);
}
void simple_gauss2(tgb_matrix* mat){
  int col, row;
  col=0;
  row=0;
  int i;
  int pn=mat->get_rows();
  assume(pn>0);
  //first clear zeroes
//   for(i=0;i<pn;i++)
//   {
//     if(mat->zero_row(i))
//     {
//       mat->perm_rows(i,pn-1);
//       pn--;
//       if(i!=pn){i--;}
//     }
//   }
  while((row<pn-1)&&(col<mat->get_columns())){
    //row is the row where pivot should be
    // row== pn-1 means we have only to act on one row so no red nec.
    //we assume further all rows till the pn-1 row are non-zero
    
    //select column
   
    //    col=mat->min_col_not_zero_in_row(row);
    assume(col!=mat->get_columns());
    int found_in_row=-1;
    
    //    found_in_row=row;
    assume(pn<=mat->get_rows());
    for(i=row;i<pn;i++)
    {
      //    int first=mat->min_col_not_zero_in_row(i);
      //  if(first<col)
      if(!(mat->is_zero_entry(i,col))){
	
	found_in_row=i;
	break;
      }
    }
    if(found_in_row!=-1)
    {
    //select pivot
      int act_l=mat->non_zero_entries(found_in_row);
      for(i=i+1;i<pn;i++)
      {
	int vgl;
	assume(mat->min_col_not_zero_in_row(i)>=col);
	if((!(mat->is_zero_entry(i,col)))&&((vgl=mat->non_zero_entries(i))<act_l))
	{
	  found_in_row=i;
	  act_l=vgl;
	}
	
      }
      mat->perm_rows(row,found_in_row);
      
      
      //reduction
      for(i=row+1;i<pn;i++){
	assume(mat->min_col_not_zero_in_row(i)>=col);
	if(!(mat->is_zero_entry(i,col)))
	{
	  
	  number c1=nNeg(nCopy(mat->get(i,col)));
	  number c2=mat->get(row,col);
	  number n1=c1;
	  number n2=c2;
	  
	  ksCheckCoeff(&n1,&n2);
	  nDelete(&c1);
	  mat->mult_row(i,n2);
	  mat->add_lambda_times_row(i,row,n1);
	  assume(mat->is_zero_entry(i,col));
	  
	} 
	assume(mat->min_col_not_zero_in_row(i)>col);
      }
      row++;
    }
    col++;
    // for(i=row+1;i<pn;i++)
//     {
//       if(mat->zero_row(i))
//       {
// 	mat->perm_rows(i,pn-1);
// 	pn--;
// 	if(i!=pn){i--;}
//       }
//     }

  }
}

static tgb_matrix* build_matrix(poly* p,int p_index,poly* done, int done_index, calc_dat* c){
  tgb_matrix* t=new tgb_matrix(p_index,done_index);
  int i, pos;
  //  Print("\n 0:%s\n",pString(done[done_index-1]));
  //Print("\n 1:%s\n",pString(done[done_index-2]));
  assume((!(pLmEqual(done[done_index-1],done[done_index-2]))));
#ifdef TGB_DEGUG
  for(i=0;i<done_index;i++)
  {
    int j;
    for(j=0;j<i;j++)
    {
      assume((!(pLmEqual(done[i],done[j]))));
    }
  }
#endif
  for(i=0;i<p_index;i++)
  { 
    // Print("%i ter Eintrag:%s\n",i,pString(p[i]));
    poly p_i=p[i];
    while(p_i)
    {

      int v=-1;
      pos=posInPolys (done, done_index, p_i,c);
      if((done_index>pos)&&(pLmEqual(p_i,done[pos])))
	v=pos;
      if((pos>0) &&(pLmEqual(p_i,done[pos-1])))
	v=pos-1;
      assume(v!=-1);
      //v is ascending ordered, we need descending order
      v=done_index-1-v;
      number nt=t->get(i,v);
      nDelete(&nt);
      t->set(i,v,nCopy(p_i->coef));
      p_i=p_i->next;
    }
  }
  return t;
}
static tgb_sparse_matrix* build_sparse_matrix(poly* p,int p_index,poly* done, int done_index, calc_dat* c){
  tgb_sparse_matrix* t=new tgb_sparse_matrix(p_index,done_index,c->r);
  int i, pos;
  //  Print("\n 0:%s\n",pString(done[done_index-1]));
  //Print("\n 1:%s\n",pString(done[done_index-2]));
  //  assume((!(pLmEqual(done[done_index-1],done[done_index-2]))));
#ifdef TGB_DEGUG
  for(i=0;i<done_index;i++)
  {
    int j;
    for(j=0;j<i;j++)
    {
      assume((!(pLmEqual(done[i],done[j]))));
    }
  }
#endif
  for(i=0;i<p_index;i++)
  { 
    // Print("%i ter Eintrag:%s\n",i,pString(p[i]));
    mac_poly m=NULL;
    mac_poly* set_this=&m;
    poly p_i=p[i];
    while(p_i)
    {

      int v=-1;
      pos=posInPolys (done, done_index, p_i,c);
      if((done_index>pos)&&(pLmEqual(p_i,done[pos])))
	v=pos;
      if((pos>0) &&(pLmEqual(p_i,done[pos-1])))
	v=pos-1;
      assume(v!=-1);
      //v is ascending ordered, we need descending order
      v=done_index-1-v;
      (*set_this)=new mac_poly_r();
      (*set_this)->exp=v;
       
      (*set_this)->coef=nCopy(p_i->coef);
       set_this=&(*set_this)->next;
       p_i=p_i->next;

    }
    init_with_mac_poly(t,i,m);
  }
  return t;
}


static int retranslate(poly* m,tgb_sparse_matrix* mat,poly* done, calc_dat* c){
  int i;
  int m_index=0;
  for(i=0;i<mat->get_rows();i++)
  {
    if(mat->zero_row(i))
    {
      mat->free_row(i);
      continue;
    }
    m[m_index++]= free_row_to_poly(mat, i, done, mat->get_columns());

  }

  delete mat;
  return m_index;

}

//returns m_index and destroys mat
static int retranslate(poly* m,tgb_matrix* mat,poly* done, calc_dat* c){
  int i;
  int m_index=0;
  for(i=0;i<mat->get_rows();i++)
  {
    if(mat->zero_row(i))
    {
      mat->free_row(i);
      continue;
    }
    
    m[m_index]=pInit();
    int v=mat->min_col_not_zero_in_row(i);
    //v=done_index-1-pos; => pos=done_index-1-v=mat->get_columns()-1-v
    int pos=mat->get_columns()-1-v;
    int* ev=(int*) omalloc((c->r->N+1)*sizeof(int));
    pGetExpV(done[pos],ev);
    pSetExpV(m[m_index],ev);
    omfree(ev);
    
    poly p=m[m_index];
    pSetCoeff(p,mat->get(i,v));
    while((v=mat->next_col_not_zero(i,v))!=mat->get_columns())
    {
      poly pn=pInit();
      
      //v=done_index-1-pos; => pos=done_index-1-v=mat->get_columns()-1-v
      pos=mat->get_columns()-1-v;
      ev=(int*) omalloc((c->r->N+1)*sizeof(int));
      pGetExpV(done[pos],ev);
      pSetExpV(pn,ev);
      omfree(ev);
      pSetCoeff(pn,mat->get(i,v));
      p->next=pn;
      p=pn;
    }
    p->next=NULL;
    mat->free_row(i, FALSE);
    m_index++;
  }

  delete mat;
  return m_index;

}
static void go_on_F4 (calc_dat* c){
  //set limit of 1000 for multireductions, at the moment for
  //programming reasons
  int max_par;
  if (c->is_homog)
    max_par=PAR_N_F4;
  else
    max_par=200;
  int done_size=max_par*2;
  poly* done=(poly*) omalloc(done_size*sizeof(poly));
  int done_index=0; //done_index must always be smaller than done_size
  int chosen_size=max_par*2;
  monom_poly* chosen=(monom_poly*) omalloc(chosen_size*sizeof(monom_poly));
  int chosen_index=0;
  //  monom_poly* vgl=(monom_poly*) omalloc(chosen_size*sizeof(monom_poly));
  int i=0;
  c->average_length=0;
  for(i=0;i<c->n;i++){
    c->average_length+=c->lengths[i];
  }
  c->average_length=c->average_length/c->n;
  i=0;
  poly* p;
  int nfs=0;
  int curr_deg=-1;

  //choose pairs and preprocess symbolically
  while(chosen_index<max_par)
  {
    
    //    sorted_pair_node* s=c->apairs[c->pair_top];
    sorted_pair_node* s;
    if(c->pair_top>=0)
      s=top_pair(c);//here is actually chain criterium done
    else
      s=NULL;
    if (!s) break;
    nfs++;
    if(curr_deg>=0)
    {
      if (s->deg >curr_deg) break;
    }

    else curr_deg=s->deg;
    quick_pop_pair(c);
    if(s->i>=0)
    {
      //replace_pair(s->i,s->j,c);
      if(s->i==s->j) 
      {
	free_sorted_pair_node(s,c->r);
	continue;
      }
    }
    assume(s->i>=0);
    monom_poly h;
    BOOLEAN only_free=FALSE;
    if(s->i>=0)
    {
      
      poly lcm=pOne();
      
      pLcm(c->S->m[s->i], c->S->m[s->j], lcm);
      pSetm(lcm);
      assume(lcm!=NULL);
      poly factor1=pCopy(lcm);
      pExpVectorSub(factor1,c->S->m[s->i]);
      poly factor2=pCopy(lcm);
      pExpVectorSub(factor2,c->S->m[s->j]);

      if(done_index>=done_size)
      {
	done_size+=max_par;
	done=(poly*) omrealloc(done,done_size*sizeof(poly));
      }
      done[done_index++]=lcm;
      if(chosen_index+1>=chosen_size)
      {
	//max_par must be greater equal 2
	chosen_size+=max(max_par,2);
	chosen=(monom_poly*) omrealloc(chosen,chosen_size*sizeof(monom_poly));
      }
      h.f=c->S->m[s->i];
      h.m=factor1;
      chosen[chosen_index++]=h;
      h.f=c->S->m[s->j];
      h.m=factor2;
      chosen[chosen_index++]=h;
    }
    else
    {
      if(chosen_index>=chosen_size)
      {
	chosen_size+=max_par;
	chosen=(monom_poly*) omrealloc(chosen,chosen_size*sizeof(monom_poly));
      }
      h.f=s->lcm_of_lm;
      h.m=pOne();
      //pSetm(h.m); done by pOne
      chosen[chosen_index++]=h;
      //must carefull remember to destroy such a h;
      poly_list_node* next=c->to_destroy;
      
      c->to_destroy=(poly_list_node*) omalloc(sizeof(poly_list_node));
      c->to_destroy->p=h.f;
      c->to_destroy->next=next;
      only_free=TRUE;
    }

    if(s->i>=0)
      now_t_rep(s->j,s->i,c);

    if(!(only_free))
      free_sorted_pair_node(s,c->r);
    else
      omfree(s);
    
  

  }
  c->normal_forms+=nfs;
  if (TEST_OPT_PROT)
      Print("M[%i, ",nfs);
  //next Step, simplify all pairs
  for(i=0;i<chosen_index;i++)
  {
    //    PrintS("simplifying ");
    //Print("from %s",pString(chosen[i].m));
    //Print(", %s", pString(chosen[i].f));
     simplify(chosen[i],c);
    //Print(" to %s",pString(chosen[i].m));
    //Print(", %s \n", pString(chosen[i].f));
  }
  
  //next Step remove duplicate entries
  qsort(chosen,chosen_index,sizeof(monom_poly),monom_poly_crit);
  int pos=0;
  for(i=1;i<chosen_index;i++)
  {
    if(((chosen[i].f!=chosen[pos].f))||(!(pLmEqual(chosen[i].m,chosen[pos].m))))
      chosen[++pos]=chosen[i];
    else pDelete(&(chosen[i].m));
  }
  if(chosen_index>0)
    chosen_index=pos+1;
  //next step process polys
  int p_size=2*chosen_index;
  int p_index=0;
  p=(poly*) omalloc(p_size*sizeof(poly));
  for(p_index=0;p_index<chosen_index;p_index++)
  {
    p[p_index]=ppMult_mm(chosen[p_index].f,chosen[p_index].m);
  }

  qsort(done, done_index,sizeof(poly),pLmCmp_func);
  pos=0;
  //Print("Done_index:%i",done_index);
  if(done_index>0)
  {
    pTest(done[0]);
  }
  for(i=1;i<done_index;i++)
  {
    pTest(done[i]);
    if((!(pLmEqual(done[i],done[pos]))))
      done[++pos]=done[i];
    else pDelete(&(done[i]));
  }
  if(done_index>0)
    done_index=pos+1;
#ifdef TGB_DEBUG
  for(i=0;i<done_index;i++)
  {
    int j;
    for(j=0;j<i;j++)
    {
      assume((!pLmEqual(done[j],done[i])));
    }
  }
#endif
#ifdef TGB_DEBUG
  for(i=0;i<done_index;i++)
  {
    pTest(done[i]);
  }
#endif
  poly* m;
  int m_index=0;
  int m_size=0;
  for(i=0;i<p_index;i++)
  {
    m_size+=pLength(p[i]);
  }
  m=(poly*) omalloc(m_size*sizeof(poly));
  //q=(poly*) omalloc(m_size*sizeof(poly));

  

  for(i=0;i<p_index;i++)
  {

    poly p_i=p[i];

    pTest(p[i]);

    while(p_i)
    {

      m[m_index]=pLmInit(p_i);

      pSetCoeff(m[m_index],nInit(1));

       p_i=p_i->next;

      m_index++;

    }
  }

  int q_size=m_index;
  poly* q=(poly*) omalloc(q_size*sizeof(poly));
  int q_index=0;
  //next Step additional reductors

  while(m_index>0)
  {
#ifdef TGB_DEBUG
      
      for(i=0;i<done_index;i++)
      {
	pTest(done[i]);
      }
#endif

    qsort(m, m_index,sizeof(poly),pLmCmp_func);

    
    pos=0;
    #ifdef TGB_DEBUG
      
      for(i=0;i<done_index;i++)
      {
	
	pTest(done[i]);
      }
#endif
    for(i=1;i<m_index;i++)
    {
      pTest(m[i]);
      pTest(m[pos]);
      if((!(pLmEqual(m[i],m[pos]))))
	m[++pos]=m[i];
      else pDelete(&(m[i]));
    }
    if(m_index>1)
      m_index=pos+1;
    if(done_size<done_index+m_index)
    {
      done_size=done_index+2*m_index;
      done=(poly*) omrealloc(done,done_size*sizeof(poly));
    }
    if(chosen_size<chosen_index+m_index)
    {
      chosen_size=chosen_index+2*m_index;
      chosen=(monom_poly*) omrealloc(chosen,chosen_size*sizeof(monom_poly));
    }
    q_index=0;
    if(q_size<m_index)
    {
      q_size=2*m_index;
      omfree(q);
      q=(poly*) omalloc(q_size*sizeof(poly));
    }

    for(i=0;i<m_index;i++)
    {

#ifdef TGB_DEBUG
      {
	int my_i;
	for(my_i=0;my_i<done_index;my_i++)
	{
	  int my_j;
	  for(my_j=0;my_j<my_i;my_j++)
	  {
	    assume((!pLmEqual(done[my_j],done[my_i])));
	  }
	}
      }
#endif
      BOOLEAN in_done=FALSE;
      pTest(m[i]);

      pos=posInPolys (done, done_index, m[i],c);
#ifdef TGB_DEBUG
      {
	int my_i;
	for (my_i=0;my_i<done_index;my_i++)
	  pTest(done[my_i]);
      }
#endif      
      if(((done_index>pos)&&(pLmEqual(m[i],done[pos]))) ||(pos>0) &&(pLmEqual(m[i],done[pos-1])))
	in_done=TRUE;
      if (!(in_done))
      {
       
	int S_pos=kFindDivisibleByInS_easy(c->strat,m[i], pGetShortExpVector(m[i]));
	if(S_pos>=0)
	{
	  monom_poly h;
	  h.f=c->strat->S[S_pos];
	  h.m=pOne();
	  int* ev=(int*) omalloc((c->r->N+1)*sizeof(int));
	  pGetExpV(m[i],ev);
	  pSetExpV(h.m,ev);
	  omfree(ev);
	  pExpVectorSub(h.m,c->strat->S[S_pos]);
	  simplify(h,c);
	  q[q_index]=ppMult_mm(h.f,h.m);
	  chosen[chosen_index++]=h;
	  q_index++;
	}
	pTest(m[i]);
#ifdef TGB_DEBUG
	{
	  int my_i;
	  for (my_i=0;my_i<done_index;my_i++)
	    pTest(done[my_i]);
	}
#endif      
	memmove(&(done[pos+1]),&(done[pos]), (done_index-pos)*sizeof(poly));
	done[pos]=m[i];
	done_index++;
#ifdef TGB_DEBUG
	{
	  int my_i;
	  for (my_i=0;my_i<done_index;my_i++)
	  {
	    //	    Print("Position %i pos %i size %i\n",my_i,pos,done_index);
	    pTest(done[my_i]);
	  }
	}
#endif      
      }
      else
	pDelete(&m[i]);
#ifdef TGB_DEBUG
      {
	int my_i;
	for(my_i=0;my_i<done_index;my_i++)
	{
	  pTest(done[my_i]);
	  int my_j;
	  for(my_j=0;my_j<my_i;my_j++)
	  {
	    assume((!pLmEqual(done[my_j],done[my_i])));
	  }
	}
      }
#endif
    }
    if(p_size<p_index+q_index)
    {
      p_size=p_index+2*q_index;
      p=(poly*) omrealloc(p,p_size*sizeof(poly));
    }
    for (i=0;i<q_index;i++)
      p[p_index++]=q[i];
    m_index=0;
    int sum=0;
    for(i=0;i<p_index;i++)
    {
      sum+=pLength(p[i])-1;
    }
    if (m_size<sum)
    {
      omfree(m);
      m=(poly*) omalloc(sum*sizeof(poly));
    }
    m_size=sum;
    for(i=0;i<q_index;i++)
    {
      poly p_i=q[i]->next;
      while(p_i)
      {
	m[m_index]=pLmInit(p_i);
	pSetCoeff(m[m_index],nInit(1));
	p_i=p_i->next;
	m_index++;
      }
    }
    //qsort(done, done_index,sizeof(poly),pLmCmp_func);
#ifdef TGB_DEBUG
    for(i=0;i<done_index;i++)
    {
      pTest(done[i]);
    }
#endif
  }
#ifdef TGB_DEBUG
  for(i=0;i<done_index;i++)
  {
    int j;
    for(j=0;j<i;j++)
    {
      assume((!pLmEqual(done[j],done[i])));
    }
  }
#endif
  omfree(m);
  omfree(q);
  if (TEST_OPT_PROT)
    Print("Mat[%i x %i], ",chosen_index, done_index);
  //next Step build matrix
  #ifdef TGB_DEBUG
  for(i=0;i<done_index;i++)
  {
    int j;
    for(j=0;j<i;j++)
    {
      assume((!pLmEqual(done[j],done[i])));
    }
  }
#endif
  assume(p_index==chosen_index);
  
 //  tgb_matrix* mat=build_matrix(p,p_index,done, done_index,c);
 
//   simple_gauss2(mat);
  tgb_sparse_matrix* mat=build_sparse_matrix(p,p_index,done, done_index,c);
  simple_gauss(mat,c);
  m_size=mat->get_rows();
  m=(poly*) omalloc(m_size*sizeof(poly));
  m_index=retranslate(m,mat,done,c);
  
  mat=NULL;
  for(i=0;i<done_index;i++)
    pDelete(&done[i]);
  omfree(done);
  done=NULL;
  //next Step addElements to basis 
  int F_plus_size=m_index;
  poly* F_plus=(poly*)omalloc(F_plus_size*sizeof(poly));
  int F_plus_index=0;
  int F_minus_size=m_index;
  poly* F_minus=(poly*) omalloc(F_minus_size*sizeof(poly));
  int F_minus_index=0;

  //better algorithm replace p by its monoms, qsort,delete duplicates and binary search for testing if monom is contained in array
  qsort(p, p_index,sizeof(poly),pLmCmp_func);
  for(i=0;i<p_index;i++)
    pDelete(&p[i]->next);
  for(i=m_index-1;i>=0;--i)
  {
    int j;
    int pos=posInPolys (p,p_index, m[i],c);
    BOOLEAN minus=FALSE;
    if(((p_index>pos)&&(pLmEqual(m[i],p[pos]))) ||(pos>0) &&(pLmEqual(m[i],p[pos-1])))
      minus=TRUE;
   
    if(minus)
    {
      F_minus[F_minus_index++]=m[i];
      m[i]=NULL;
    }
    else
    {
      F_plus[F_plus_index++]=m[i];
      m[i]=NULL;
    }
  }

//   for(i=m_index-1;i>=0;--i)
//   {
//     int j;
//     BOOLEAN minus=FALSE;
//     for(j=0;j<p_index;j++)
//       if (pLmEqual(p[j],m[i]))
//       {
// 	minus=TRUE;
// 	break;
//       }
//     if(minus)
//     {
//       F_minus[F_minus_index++]=m[i];
//       m[i]=NULL;
//     }
//     else
//     {
//       F_plus[F_plus_index++]=m[i];
//       m[i]=NULL;
//     }
//   }
  //in this order F_minus will be automatically ascending sorted
  //to make this sure for foreign gauss
  //uncomment the following line
  //  qsort(F_minus, F_minus_index,sizeof(poly),pLmCmp_func);
  assume((F_plus_index+F_minus_index)==m_index);
  if (TEST_OPT_PROT)
    Print("%i]", F_plus_index);
  for(i=0;i<p_index;i++) 
    pDelete(&p[i]);
  omfree(p);
  p=NULL;
  omfree(m);
  m=NULL;

  //the F_minus list must be cleared separately at the end
  mp_array_list** F_i;
  poly_array_list** F_m_i;
  F_i=&(c->F);
  F_m_i=&(c->F_minus);

  while((*F_i)!=NULL)
  {
    assume((*F_m_i)!=NULL);
    F_i=(&((*F_i)->next));
    F_m_i=(&((*F_m_i)->next));
  }
  assume((*F_m_i)==NULL);
  //should resize the array to save memory
  //F and F_minus
  qsort(chosen,chosen_index,sizeof(monom_poly),monom_poly_crit);//important for simplify
  (*F_m_i)=(poly_array_list*) omalloc(sizeof(poly_array_list));
  (*F_m_i)->size=F_minus_index;
  (*F_m_i)->p=F_minus;
  (*F_m_i)->next=NULL;
  (*F_i)=(mp_array_list*) omalloc(sizeof(poly_array_list));
  (*F_i)->size=chosen_index;
  (*F_i)->mp=chosen;
  (*F_i)->next=NULL;
  
  if(F_plus_index>0)
  {
    int j;
    int* ibuf=(int*) omalloc(F_plus_index*sizeof(int));
    sorted_pair_node*** sbuf=(sorted_pair_node***) omalloc(F_plus_index*sizeof(sorted_pair_node**));
  
    for(j=0;j<F_plus_index;j++)
    {
      int len;
      poly p=F_plus[j];
    
    // delete buf[j];
    //remember to free res here
    //    p=redTailShort(p, c->strat);
      sbuf[j]=add_to_basis_ideal_quotient(p,-1,-1,c,ibuf+j);
    
    }
    int sum=0;
    for(j=0;j<F_plus_index;j++)
    {
      sum+=ibuf[j];
    }
    sorted_pair_node** big_sbuf=(sorted_pair_node**) omalloc(sum*sizeof(sorted_pair_node*));
    int partsum=0;
    for(j=0;j<F_plus_index;j++)
    {
      memmove(big_sbuf+partsum, sbuf[j],ibuf[j]*sizeof(sorted_pair_node*));
      omfree(sbuf[j]);
      partsum+=ibuf[j];
    }

    qsort(big_sbuf,sum,sizeof(sorted_pair_node*),pair_better_gen2);
    c->apairs=merge(c->apairs,c->pair_top+1,big_sbuf,sum,c);
    c->pair_top+=sum;
    clean_top_of_pair_list(c);
    omfree(big_sbuf);
    omfree(sbuf);
    omfree(ibuf);
  }
  omfree(F_plus);
#ifdef TGB_DEBUG
  int z;
  for(z=1;z<=c->pair_top;z++)
  {
    assume(pair_better(c->apairs[z],c->apairs[z-1],c));
  }
#endif

  return;
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

ideal t_rep_gb(ring r,ideal arg_I, BOOLEAN F4_mode){
  if (TEST_OPT_PROT)
    if (F4_mode)
      PrintS("F4 Modus \n");
    
     
  //debug_Ideal=arg_debug_Ideal;
  //if (debug_Ideal) PrintS("DebugIdeal received\n");
  // Print("Idelems %i \n----------\n",IDELEMS(arg_I));
  ideal I=idCompactify(arg_I);
  qsort(I->m,IDELEMS(I),sizeof(poly),poly_crit);
  //Print("Idelems %i \n----------\n",IDELEMS(I));
  calc_dat* c=(calc_dat*) omalloc(sizeof(calc_dat));
  
  c->r=currRing;
  c->is_homog=TRUE;
  {
    int hz;
    for(hz=0;hz<IDELEMS(I);hz++){
      assume(I->m[hz]!=NULL);
      int d=pTotaldegree(I->m[hz]);
      poly t=I->m[hz]->next;
      while(t)
      {
	if (d!=pTotaldegree(t,c->r))
	{
	  c->is_homog=FALSE;
	  break;
	}
	t=t->next;
      }
      if(!(c->is_homog)) break;
    }
  }
  //  Print("is homog:%d",c->is_homog);
  void* h;
  poly hp;
  int i,j;
  c->to_destroy=NULL;
  c->easy_product_crit=0;
  c->extended_product_crit=0;
  c->is_char0=(rChar()==0);
  c->F4_mode=F4_mode;
  c->reduction_steps=0;
  c->last_index=-1;

  c->F=NULL;
  c->F_minus=NULL;

  c->Rcounter=0;

  c->soon_free=NULL;

  c->tmp_lm=pOne();

  c->normal_forms=0;
  c->current_degree=1;
 
  c->max_pairs=5*I->idelems();
 
  c->apairs=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*c->max_pairs);
  c->pair_top=-1;

  int n=I->idelems();
  if (TEST_OPT_PROT)
    for (i=0;i<n;i++){
      wrp(I->m[i]);
      PrintS("\n");
    }
  i=0;
  c->n=0;
  c->T_deg=(int*) omalloc(n*sizeof(int));
  c->tmp_pair_lm=(poly*) omalloc(n*sizeof(poly));
  c->tmp_spn=(sorted_pair_node**) omalloc(n*sizeof(sorted_pair_node*));
  lm_bin=omGetSpecBin(POLYSIZE + (r->ExpL_Size)*sizeof(long));
#ifdef HEAD_BIN
  c->HeadBin=omGetSpecBin(POLYSIZE + (currRing->ExpL_Size)*sizeof(long));
#endif
  /* omUnGetSpecBin(&(c->HeadBin)); */
  h=omalloc(n*sizeof(char*));
  c->states=(char**) h;
  h=omalloc(n*sizeof(int));
  c->lengths=(int*) h;
  h=omalloc(n*sizeof(int));
        c->gcd_of_terms=(poly*) omalloc(n*sizeof(poly));
  c->rep=(int*) h;
  c->short_Exps=(long*) omalloc(n*sizeof(long));
  c->S=idInit(n,1);
  c->strat=new skStrategy;
  c->strat->syzComp = 0;
  initBuchMoraCrit(c->strat);
  initBuchMoraPos(c->strat);
  c->strat->initEcart = initEcartBBA;
  c->strat->enterS = enterSBba;
  c->strat->sl = -1;
  i=n;
  /* initS(c->S,NULL,c->strat); */
/* intS start: */
  i=((i+IDELEMS(c->S)+15)/16)*16;
  c->strat->ecartS=(intset)omAlloc(i*sizeof(int)); /*initec(i);*/
  c->strat->sevS=(unsigned long*)omAlloc0(i*sizeof(unsigned long));
  /*initsevS(i);*/
  c->strat->S_2_R=(int*)omAlloc0(i*sizeof(int));/*initS_2_R(i);*/
  c->strat->fromQ=NULL;
  c->strat->Shdl=idInit(1,1);
  c->strat->S=c->strat->Shdl->m;
  c->strat->lenS=(int*)omAlloc0(i*sizeof(int));
  if(c->is_char0)
    c->strat->lenSw=(int*)omAlloc0(i*sizeof(int));
  else
    c->strat->lenSw=NULL;
  sorted_pair_node* si;
  assume(n>0);
  add_to_basis(I->m[0],-1,-1,c);

  assume(c->strat->sl==c->strat->Shdl->idelems()-1);
  if(!(F4_mode))
  {
    for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
    {
      //     add_to_basis(I->m[i],-1,-1,c);
      si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      si->i=-1;
      si->j=-1;
      si->expected_length=pLength(I->m[i]);
      si->deg=pTotaldegree(I->m[i]);
      if (!rField_is_Zp(c->r)){ 
	pCleardenom(I->m[i]);
      }
      si->lcm_of_lm=I->m[i];
      
      //      c->apairs[n-1-i]=si;
      c->apairs[n-i-1]=si;
      ++(c->pair_top);
    }
  }
  else
  {
     for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
       add_to_basis(I->m[i],-1,-1,c);
  }
    

  while(c->pair_top>=0)
  {
    if(F4_mode)
      go_on_F4(c);
    else
      go_on(c);
  }
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
  for(int z=0;z<c->n;z++){
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
  omfree(c->short_Exps);
  omfree(c->T_deg);

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
// // 	if(c->strat->S[j]==c->S->m[i]){
// // 	  c->strat->S[j]=NULL;
// // 	  break;
// // 	}
// //       }
// //      PrintS("R_delete");
//       pDelete(&c->S->m[i]);
//     }
//   }

//  qsort(c->S->m, c->n,sizeof(poly),pLmCmp_func);
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
  omfree(c->rep);
  for(i=0;i<I->idelems();i++)
  {
    I->m[i]=NULL;
  }
  idDelete(&I);
  I=c->S;
  
  IDELEMS(I)=c->n;

  idSkipZeroes(c->S);
  for(i=0;i<=c->strat->sl;i++)
    c->strat->S[i]=NULL;
  id_Delete(&c->strat->Shdl,c->r);
  pDelete(&c->tmp_lm);
  omUnGetSpecBin(&lm_bin);
  delete c->strat;
  omfree(c);

  return(I);
}
static void now_t_rep(const int & arg_i, const int & arg_j, calc_dat* c){
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
static void soon_t_rep(const int& arg_i, const int& arg_j, calc_dat* c)
{
  assume(0<=arg_i);
  assume(0<=arg_j);
  assume(arg_i<c->n);
  assume(arg_j<c->n);
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
  if (!
      (c->states[j][i]==HASTREP))
    c->states[j][i]=SOONTREP;
}
static BOOLEAN has_t_rep(const int & arg_i, const  int & arg_j, calc_dat* state){
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
    n+=max( pGetExp(a,i), pGetExp(b,i));
  }
  return n;

}



static void shorten_tails(calc_dat* c, poly monom)
{
  return;
// BOOLEAN corr=lenS_correct(c->strat);
  for(int i=0;i<c->n;i++)
  {
    //enter tail
    if (c->rep[i]!=i) continue;
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
      int q;
      q=quality(c->S->m[i],c->lengths[i],c);
      new_pos=simple_posInS(c->strat,c->S->m[i],q,c->is_char0);

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
        move_forward_in_S(old_pos,new_pos,c->strat, c->is_char0);

      length_one_crit(c,i,c->lengths[i]);
    }
  }
}
static sorted_pair_node* pop_pair(calc_dat* c){
  clean_top_of_pair_list(c);

  if(c->pair_top<0) return NULL;
  else return (c->apairs[c->pair_top--]);
}
static sorted_pair_node* top_pair(calc_dat* c){
  super_clean_top_of_pair_list(c);//yeah, I know, it's odd that I use a different proc here

  if(c->pair_top<0) return NULL;
  else return (c->apairs[c->pair_top]);
}
static sorted_pair_node* quick_pop_pair(calc_dat* c){
  if(c->pair_top<0) return NULL;
  else return (c->apairs[c->pair_top--]);
}
static BOOLEAN no_pairs(calc_dat* c){
  clean_top_of_pair_list(c);
  return (c->pair_top==-1);
}


static void super_clean_top_of_pair_list(calc_dat* c){
  while((c->pair_top>=0)
  && (c->apairs[c->pair_top]->i>=0)
  && (good_has_t_rep(c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,c)))
  {

    free_sorted_pair_node(c->apairs[c->pair_top],c->r);
    c->pair_top--;

  }
}
static void clean_top_of_pair_list(calc_dat* c){
  while((c->pair_top>=0) && (c->apairs[c->pair_top]->i>=0) && (!state_is(UNCALCULATED,c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,c))){

    free_sorted_pair_node(c->apairs[c->pair_top],c->r);
    c->pair_top--;

  }
}
static BOOLEAN state_is(calc_state state, const int & arg_i, const  int & arg_j, calc_dat* c){
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
static void free_sorted_pair_node(sorted_pair_node* s, ring r){
  if (s->i>=0)
    p_Delete(&s->lcm_of_lm,r);
  omfree(s);
}
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, calc_dat* c){
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

static int pair_better_gen(const void* ap,const void* bp){

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
      pSetExp(m,i, min(pGetExp(t,i),pGetExp(m,i)));
      if (max_g_0==i)
	if (pGetExp(m,i)==0)
	  max_g_0=0;
      if ((max_g_0==0) && (pGetExp(m,i)>0)){
	max_g_0=i;
      }
    }
    t=t->next;
  }
  // for (i=pVariables;i;i--)
//   {
//     if(pGetExp(m,i)>0)
//       return m;
//   }
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
static inline BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, calc_dat* c){
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



static int guess_quality(const red_object & p, calc_dat* c){
  //looks only on bucket
  if (c->is_char0) return kSBucketLength(p.bucket);
  return (bucket_guess(p.bucket));
}
static int pQuality(poly p, calc_dat* c){
  if(c->is_char0) return pSLength(p,pLength(p));
  return pLength(p);
}
static int quality_of_pos_in_strat_S(int pos, calc_dat* c){
  if (c->is_char0) return c->strat->lenSw[pos];
  return c->strat->lenS[pos];
}
static int quality(poly p, int len, calc_dat* c){
  if (c->is_char0) return pSLength(p,len);
  return pLength(p);
}
static void multi_reduction_lls_trick(red_object* los, int losl,calc_dat* c,find_erg & erg){
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
      los[best].flatten();
      int b_pos=kBucketCanonicalize(los[best].bucket);
      los[best].p=los[best].bucket->buckets[b_pos];
      qc==pQuality(los[best].bucket->buckets[b_pos],c);
      if(qc<quality_a){
	red_object h=los[erg.to_reduce_u];
	los[erg.to_reduce_u]=los[best];
	los[best]=h;
	swap_roles=TRUE;
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
	  red_object h=los[erg.to_reduce_l];
	  los[erg.to_reduce_l]=los[best];
	  los[best]=h;
	  erg.reduce_by=erg.to_reduce_l;
	  erg.fromS=FALSE;
	  erg.to_reduce_l++;
	  }
	}
      }
      else 
      {
	assume(erg.to_reduce_u==erg.to_reduce_l);
	int quality_a=quality_of_pos_in_strat_S(erg.reduce_by,c);
	int qc=los[erg.to_reduce_u].guess_quality(c);
	if(qc<quality_a){
	  int best=erg.to_reduce_u;
	  los[best].flatten();
	  int b_pos=kBucketCanonicalize(los[best].bucket);
	  los[best].p=los[best].bucket->buckets[b_pos];
	  qc==pQuality(los[best].bucket->buckets[b_pos],c);
	  if(qc<quality_a){
	    BOOLEAN exp=FALSE;
	    if(qc<=2)
	      exp=TRUE;
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
      int quality_a=los[erg.reduce_by].guess_quality(c);
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
    int qal=quality(clear_into,new_length,c);
    int pos_in_c=-1;    
    int z;
    int new_pos;
    new_pos=simple_posInS(c->strat,clear_into,qal,c->is_char0);
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
      c_S_element_changed_hook(pos_in_c,c);
    }
    c->strat->S[j]=clear_into;
    c->strat->lenS[j]=new_length;
    if(c->strat->lenSw)
      c->strat->lenS[j]=qal;
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
      move_forward_in_S(j,new_pos,c->strat,c->is_char0);
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
       step=min(i2,step);
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
       
       step=min(i-i2,step);
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
static void multi_reduction_find(red_object* los, int losl,calc_dat* c,int startf,find_erg & erg){
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

//       int i2;
//       for(i2=i-1;i2>=0;i2--){
// 	if(!pLmEqual(los[i].p,los[i2].p))
// 	  break;
//      }
      
//      erg.to_reduce_l=i2+1;
      erg.to_reduce_l=i2;
      assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)==-1));
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
	return;
      }
      if(!(c->is_homog))
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
	    return;
	  }
	  //	else {assume(!p_LmDivisibleBy(los[i].p, los[i2].p,c->r));}
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

static void sort_region_down(red_object* los, int l, int u, calc_dat* c)
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
static void multi_reduction(red_object* los, int & losl, calc_dat* c)
{
  
  //initialize;
  assume(c->strat->sl>=0);
  assume(losl>0);
  int i;
  for(i=0;i<losl;i++){
    los[i].sev=pGetShortExpVector(los[i].p);
//SetShortExpVector();
    los[i].p=kBucketGetLm(los[i].bucket);
  }

  kStrategy strat=c->strat;
  int curr_pos=losl-1;


//  nicht reduzierbare einträge in ergebnisliste schreiben
  // nullen loeschen
  while(curr_pos>=0){
    
    find_erg erg;
    multi_reduction_find(los, losl,c,curr_pos,erg);//last argument should be curr_pos
   //  PrintS("\n erg:\n");
//     Print("upper:%i\n",erg.to_reduce_u);
//     Print("lower:%i\n",erg.to_reduce_l);
//     Print("reduce_by:%i\n",erg.reduce_by);
//     Print("fromS:%i\n",erg.fromS);
    if(erg.reduce_by<0) break;



    erg.expand=NULL;
    int d=erg.to_reduce_u-erg.to_reduce_l+1;
    //if ((!erg.fromS)&&(d>100)){
    if (0){
      PrintS("L");
      if(!erg.fromS){
	erg.to_reduce_u=max(erg.to_reduce_u,erg.reduce_by);
	if (pLmEqual(los[erg.reduce_by].p,los[erg.to_reduce_l].p))
	  erg.to_reduce_l=min(erg.to_reduce_l,erg.reduce_by);
      }
      int pn=erg.to_reduce_u+1-erg.to_reduce_l;
      poly* p=(poly*) omalloc((pn)*sizeof(poly));
      int i;
      for(i=0;i<pn;i++){
	int len;
	los[erg.to_reduce_l+i].flatten();
	kBucketClear(los[erg.to_reduce_l+i].bucket,&p[i],&len);
	
	redTailShort(p[i],c->strat);
      }
      pre_comp(p,pn,c);
      int j;
      for(j=0;j<pn;j++){
	los[erg.to_reduce_l+j].p=p[j];
	los[erg.to_reduce_l+j].sev=pGetShortExpVector(p[j]);
	los[erg.to_reduce_l+j].sum=NULL;
	int len=pLength(p[j]);
	kBucketInit(los[erg.to_reduce_l+j].bucket,los[erg.to_reduce_l+j].p,len);
      }
      for(j=erg.to_reduce_l+pn;j<=erg.to_reduce_u;j++){
	los[j].p=NULL;
	
      }

      omfree(p);
    }
    else {
    multi_reduction_lls_trick(los,losl,c,erg);
    int sum=0;

    
    int i;
    int len;
    
    multi_reduce_step(erg,los,c);
    }
//     reduction_step *rs=create_reduction_step(erg, los, c);
//     rs->reduce(los,erg.to_reduce_l,erg.to_reduce_u);
//     finalize_reduction_step(rs);
		 
    int deleted=multi_reduction_clear_zeroes(los, losl, erg.to_reduce_l, erg.to_reduce_u);
    if(erg.fromS==FALSE)
      curr_pos=max(erg.to_reduce_u,erg.reduce_by);
    else
      curr_pos=erg.to_reduce_u;
    losl -= deleted;
    curr_pos -= deleted;

    //Print("deleted %i \n",deleted);
    sort_region_down(los, erg.to_reduce_l, erg.to_reduce_u-deleted, c);
//   sort_region_down(los, 0, losl-1, c);
    //  qsort(los,losl,sizeof(red_object),red_object_better_gen);
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
  return;
}
void red_object::flatten(){
  if (sum!=NULL)
  {

 
    if(kBucketGetLm(sum->ac->bucket)!=NULL){
      number mult_my=n_Mult(sum->c_my,sum->ac->multiplied,currRing);
      poly add_this;
      if(!nIsOne(mult_my))
	kBucket_Mult_n(bucket,mult_my);
      int len;
      poly clear_into;
      kBucketClear(sum->ac->bucket,&clear_into,&len);
      if(sum->ac->counter>1){
	add_this=pCopy(clear_into);
	kBucketInit(sum->ac->bucket,clear_into,len);
      }
      else
	add_this=clear_into;
      if(!nIsOne(sum->c_ac))
	pMult_nn(add_this, sum->c_ac);
      
      kBucket_Add_q(bucket,add_this, &len);
       nDelete(&mult_my);
     
    }
    nDelete(&sum->c_ac);
    nDelete(&sum->c_my);
   
    sum->ac->decrease_counter();
    delete sum;
    p=kBucketGetLm(bucket);
    sum=NULL;
  }
  assume(p==kBucketGetLm(bucket));
  assume(sum==NULL);
}
void red_object::validate(){
  BOOLEAN flattened=FALSE;
  if(sum!=NULL)
  {
    poly lm=kBucketGetLm(bucket);
    poly lm_ac=kBucketGetLm(sum->ac->bucket);
    if ((lm_ac==NULL)||((lm!=NULL) && (pLmCmp(lm,lm_ac)!=-1))){
      flatten();
      assume(sum==NULL);
      flattened=TRUE;
      p=kBucketGetLm(bucket);
      if (p!=NULL)
	sev=pGetShortExpVector(p);
    } 
    else
    {
 
      p=lm_ac;
      assume(sum->ac->sev==pGetShortExpVector(p));
      sev=sum->ac->sev;
    }
    
  }
  else{
    p=kBucketGetLm(bucket);
    if(p)
    sev=pGetShortExpVector(p);
  }
  assume((sum==NULL)||(kBucketGetLm(sum->ac->bucket)!=NULL));
}
int red_object::clear_to_poly(){
  flatten();
  int l;
  kBucketClear(bucket,&p,&l);
  return l;
}

  

void red_object::adjust_coefs(number c_r, number c_ac_r){
  assume(this->sum!=NULL);
  number n1=nMult(sum->c_my, c_ac_r);
  number n2=nMult(sum->c_ac,c_r);
  nDelete(&sum->c_my);
  nDelete(&sum->c_ac);
 
  int ct = ksCheckCoeff(&n1, &n2);
  sum->c_my=n1;
  sum->c_ac=nNeg(n2);
  //  nDelete(&n2);
  

}
int red_object::guess_quality(calc_dat* c){
    //works at the moment only for lenvar 1, because in different
    //case, you have to look on coefs
    int s=0;
    if (c->is_char0)
      s=kSBucketLength(bucket);
    else 
      s=bucket_guess(bucket);
    if (sum!=NULL){
      if (c->is_char0)
      s+=kSBucketLength(sum->ac->bucket);
    else 
      s+=bucket_guess(sum->ac->bucket);
    }
    return s;
}
void reduction_step::reduce(red_object* r, int l, int u){}
void simple_reducer::target_is_no_sum_reduce(red_object & ro){
  kBucketPolyRed(ro.bucket,p,
		 p_len,
		 c->strat->kNoether);
}

void simple_reducer::target_is_a_sum_reduce(red_object & ro){
  pTest(p);
  kbTest(ro.bucket);
  kbTest(ro.sum->ac->bucket);
  assume(ro.sum!=NULL);
  assume(ro.sum->ac!=NULL);
  if(ro.sum->ac->last_reduction_id!=reduction_id){
    number n1=kBucketPolyRed(ro.sum->ac->bucket,p, p_len, c->strat->kNoether);
    number n2=nMult(n1,ro.sum->ac->multiplied);
    nDelete(&ro.sum->ac->multiplied);
    nDelete(&n1);
    ro.sum->ac->multiplied=n2;
    poly lm=kBucketGetLm(ro.sum->ac->bucket);
    if (lm)
      ro.sum->ac->sev=pGetShortExpVector(lm);
    ro.sum->ac->last_reduction_id=reduction_id;
  }
  ro.sev=ro.sum->ac->sev;
  ro.p=kBucketGetLm(ro.sum->ac->bucket);
}
void simple_reducer::reduce(red_object* r, int l, int u){
  this->pre_reduce(r,l,u);
  int i;
//debug start
  int im;
//  for(im=l;im<=u;im++)
  //  assume(is_valid_ro(r[im]));
  

//debug end

  for(i=l;i<=u;i++){
  

    if(r[i].sum==NULL)
      this->target_is_no_sum_reduce(r[i]);

    else 
    {
      this->target_is_a_sum_reduce(r[i]);
      //reduce and adjust multiplied
      r[i].sum->ac->last_reduction_id=reduction_id;
      
    }
    //most elegant would be multimethods at this point and subclassing
    //red_object for sum
 
  }
  for(i=l;i<=u;i++){
    r[i].validate();
    #ifdef TGB_DEBUG
    if (r[i].sum) r[i].sev=r[i].sum->ac->sev;

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
 join_simple_reducer::~join_simple_reducer(){
   if(fill_back!=NULL)
   {
     kBucketInit(fill_back,p,p_len);
   }
   fill_back=NULL;
    
}
void multi_reduce_step(find_erg & erg, red_object* r, calc_dat* c){
  static int id=0;
  id++;

  int rn=erg.reduce_by;
  poly red;
  int red_len;
  simple_reducer* pointer;
  BOOLEAN woc=FALSE;
  if(erg.fromS){
    red=c->strat->S[rn];
    red_len=c->strat->lenS[rn];
    
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
  }
  if (erg.to_reduce_u-erg.to_reduce_l>5){
    woc=TRUE;
    // poly m=pOne();
    poly m=c->tmp_lm;
    pSetCoeff(m,nInit(1));
    for(int i=1;i<=pVariables;i++)
      pSetExp(m,i,(pGetExp(r[erg.to_reduce_l].p, i)-pGetExp(red,i)));
    pSetm(m);
    poly red_cp=ppMult_mm(red,m);
    
    if(!erg.fromS){
      kBucketInit(r[rn].bucket,red,red_len);
    }
    //now reduce the copy
    //static poly redNF2 (poly h,calc_dat* c , int &len, number&  m,int n)
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


  int red_c=0;
  if(red_len>2*c->average_length){
    for(i=erg.to_reduce_l;i<=erg.to_reduce_u;i++){
      if((r[i].sum==NULL) ||(r[i].sum->ac->counter<=AC_FLATTEN)) red_c++;
    }
  }
  if (red_c>=AC_NEW_MIN)
    pointer=new join_simple_reducer(red,red_len,r[erg.to_reduce_l].p);
  else
    pointer=new simple_reducer(red,red_len,c);

  if ((!woc) && (!erg.fromS))
    pointer->fill_back=r[rn].bucket;
  else
    pointer->fill_back=NULL;
  pointer->reduction_id=id;
  pointer->c=c;

  pointer->reduce(r,erg.to_reduce_l, erg.to_reduce_u);
  if(woc) pDelete(&pointer->p);
  delete pointer;
  
};

void join_simple_reducer::target_is_no_sum_reduce(red_object & ro){
  
  ro.sum=new formal_sum_descriptor();
  ro.sum->ac=ac;
  ac->counter++;
  kBucket_pt bucket=ro.bucket;
  poly a1 = pNext(p), lm = kBucketExtractLm(bucket);
  BOOLEAN reset_vec=FALSE;
  number rn;
  assume(a1!=NULL);
  number an = pGetCoeff(p), bn = pGetCoeff(lm);
  lm->next=NULL;
  int ct = ksCheckCoeff(&an, &bn);
  ro.sum->c_ac=nNeg(bn);
  ro.sum->c_my=an;
  assume(nIsZero(nAdd(nMult(ro.sum->c_my,lm->coef),nMult(p->coef,ro.sum->c_ac) )));
  if (p_GetComp(p, bucket->bucket_ring) != p_GetComp(lm, bucket->bucket_ring))
  {
    p_SetCompP(a1, p_GetComp(lm, bucket->bucket_ring), bucket->bucket_ring);
    reset_vec = TRUE;
    p_SetComp(lm, p_GetComp(p, bucket->bucket_ring), bucket->bucket_ring);
    p_Setm(lm, bucket->bucket_ring);
  }


  

  p_DeleteLm(&lm, bucket->bucket_ring);
  if (reset_vec) p_SetCompP(a1, 0, bucket->bucket_ring);
  kbTest(bucket);

}

  reduction_accumulator::reduction_accumulator(poly p, int p_len, poly high_to){
    //sev needs to be removed from interfaces,makes no sense
    

    poly my=pOne();
    counter=0;
   
    for(int i=1;i<=pVariables;i++)
      pSetExp(my,i,(pGetExp(high_to, i)-pGetExp(p,i)));
    pSetm(my);

    last_reduction_id=-1;
    multiplied=nInit(1);
    bucket=kBucketCreate(currRing);
    poly a=pMult_mm(pCopy(p->next),my);

    this->sev=pGetShortExpVector(a);
    kBucketInit(bucket, a,p_len-1);
    pDelete(&my);
  }
void simple_reducer:: pre_reduce(red_object* r, int l, int u){}
void join_simple_reducer:: pre_reduce(red_object* r, int l, int u){
  for(int i=l;i<=u;i++)
    {
      if (r[i].sum){
	if(r[i].sum->ac->counter<=AC_FLATTEN) r[i].flatten();
	
      }
    }
}
