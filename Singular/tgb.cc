
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
#define OM_KEEP 0
#define LEN_VAR1

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
	  PrintS("trivial");
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
	  PrintS("trivial");
	  return TRUE;
	}
	i++;
      }
  }

  

  
}

BOOLEAN good_has_t_rep(int i, int j,calc_dat* c){
  assume(i>=0);
    assume(j>=0);
  if (has_t_rep(i,j,c)) return TRUE;
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  assume(lm!=NULL);
  int deciding_deg= pTotaldegree(lm);
  int* i_con =make_connections(i,j,lm,c);
  p_Delete(&lm,c->r);


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
  PrintS("-");
 
}

static void cleanS(kStrategy strat){
  int i=0;
  LObject P;
  while(i<=strat->sl){
    P.p=strat->S[i];
    P.sev=strat->sevS[i];
    if(kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P)!=i){
      deleteInS(i,strat);
      //remember destroying poly
    }
    else i++;
  }
}
static int bucket_guess(kBucket* bucket){
  int sum=0;
  int i;
  for (i=MAX_BUCKET;i>=0;i--){
    sum+=bucket->buckets_length[i];
  }
  return sum;
}






static int add_to_reductors(calc_dat* c, poly h, int len){
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
  while(((not_yet_found>0) && (con_checked<connected_length))||can_find_more){
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



//very important: ILM

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
  PrintS("!");mflush();
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

      //lies I[i] under I[j] ?
      if(p_LmShortDivisibleBy(c->S->m[i],c->short_Exps[i],c->S->m[j],~(c->short_Exps[j]),c->r)){
        c->rep[j]=i;
	
        PrintS("R"); R_found=TRUE;

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

      
      poly short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
      if (short_s)
      {
        sorted_pair_node* s=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
        s->i=max(i,j);
        s->j=min(i,j);
        s->expected_length=c->lengths[i]+c->lengths[j]-2;
        s->deg=pTotaldegree(short_s);
        poly lm=pOne();

        pLcm(c->S->m[i], c->S->m[j], lm);
        pSetm(lm);
        s->lcm_of_lm=lm;
          pDelete(&short_s);
        //assume(lm!=NULL);
        nodes[spc]=s;
        spc++;
      }
      else
      {
        c->states[i][j]=HASTREP;
      }
    }
  }

  add_to_reductors(c, h, c->lengths[c->n-1]);
  //i=posInS(c->strat,c->strat->sl,h,0 /*ecart*/);

  if (c->lengths[c->n-1]==1)
    shorten_tails(c,c->S->m[c->n-1]);
  //you should really update c->lengths, c->strat->lenS, and the oder of polys in strat if you sort after lengths

  //for(i=c->strat->sl; i>0;i--)
  //  if(c->strat->lenS[i]<c->strat->lenS[i-1]) printf("fehler bei %d\n",i);
  if (c->Rcounter>50) {
    c->Rcounter=0;
    cleanS(c->strat);
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

static poly redNF2 (poly h,calc_dat* c , int &len)
{
  len=0;
  if (h==NULL) return NULL;

  len=pLength(h);
  kStrategy strat=c->strat;
  if (0 > strat->sl)
  {
    return h;
  }
  int j;
  int len_upper_bound=len;
  LObject P(h);
  P.SetShortExpVector();
  P.bucket = kBucketCreate(currRing);
  // BOOLEAN corr=lenS_correct(strat);
  kBucketInit(P.bucket,P.p,len /*pLength(P.p)*/);
  //int max_pos=simple_posInS(strat,P.p);
  loop
    {
//       if (corr){

//      corr=lenS_correct(strat);
//      if(!corr){
//        PrintS("korupt");
//      }
//       }
      int compare_bound;
      compare_bound=bucket_guess(P.bucket);
      len_upper_bound=min(compare_bound,len_upper_bound);
      j=kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P);
      if (j>=0)
      {
        poly sec_copy=NULL;
        //pseudo code
        BOOLEAN must_expand=FALSE;
        BOOLEAN must_replace_in_basis=(len_upper_bound<strat->lenS[j]);//first test
        if (must_replace_in_basis)
        {
          //second test
          if (pLmEqual(P.p,strat->S[j]))
          {
            PrintS("b");
            sec_copy=kBucketClear(P.bucket);
            sec_copy=redTailShort(sec_copy, strat);
            kBucketInit(P.bucket,pCopy(sec_copy),pLength(sec_copy));
          }
          else
          {
            must_replace_in_basis=FALSE;
            if ((len_upper_bound==1)
                ||(len_upper_bound==2)
                ||(len_upper_bound<strat->lenS[j]/2))
            {
              PrintS("e");
              int dummy_len;
              kBucketClear(P.bucket,&sec_copy,&dummy_len);
              kBucketInit(P.bucket,pCopy(sec_copy),dummy_len
                                                  /*pLength(sec_copy)*/);
              must_expand=TRUE;
            }
          }
        }
//        must_expand=FALSE;
//        must_replace_in_basis=FALSE;
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
        len_upper_bound=len_upper_bound+strat->lenS[j]-2;
        number coef=kBucketPolyRed(P.bucket,strat->S[j],
                                   strat->lenS[j]/*pLength(strat->S[j])*/,
                                   strat->kNoether);
        nDelete(&coef);
        h = kBucketGetLm(P.bucket);

        if (must_replace_in_basis){
          int pos_in_c=-1;
          poly p=strat->S[j];
          int z;

          int new_length=pLength(sec_copy);
          Print("%i",strat->lenS[j]-new_length);
          len_upper_bound=new_length +strat->lenS[j]-2;//old entries length
          int new_pos;
          if(c->is_char0)
            new_pos=simple_posInS(c->strat,sec_copy,
                                  pSLength(sec_copy,new_length),
                                  c->is_char0);//hac
          else
            new_pos=simple_posInS(c->strat,sec_copy,new_length,c->is_char0);//hack
          assume(new_pos<=j);
//          p=NULL;
          for (z=c->n;z;z--)
          {
            if(p==c->S->m[z-1])
            {


              pos_in_c=z-1;

              break;
            }
          }
          if (z<=0){
            //not in c->S
            //LEAVE
            deleteInS(j,c->strat);

            add_to_reductors(c,sec_copy,pLength(sec_copy));
          }
          else {
//shorten_tails may alter position (not the length, even not by recursion in GLOBAL case)

            strat->S[j]=sec_copy;
            c->strat->lenS[j]=new_length;
            pDelete(&p);

            //        replace_quietly(c,j,sec_copy);
            // have to do many additional things for consistency
            {




              int old_pos=j;
              new_pos=min(old_pos, new_pos);
              assume(new_pos<=old_pos);


              c->strat->lenS[old_pos]=new_length;
              if(c->strat->lenSw)
                c->strat->lenSw[old_pos]=pSLength(sec_copy,new_length);
              int i=0;
              for(i=new_pos;i<old_pos;i++){
                if (strat->lenS[i]<=new_length)
                  new_pos++;
                else
                  break;
              }
              if (new_pos<old_pos)
                move_forward_in_S(old_pos,new_pos,c->strat, c->is_char0);

              c->S->m[pos_in_c]=sec_copy;

              c->lengths[pos_in_c]=new_length;
              if (new_length==1)
              {
                int i;
                for ( i=0;i<pos_in_c;i++)
                {
                  if (c->lengths[i]==1)
                    c->states[pos_in_c][i]=HASTREP;
                }
                for ( i=z;i<c->n;i++){
                  if (c->lengths[i]==1)
                    c->states[i][pos_in_c]=HASTREP;
                }
                shorten_tails(c,sec_copy);
              }
            }
          }
        }
        if(must_expand){

          add_to_reductors(c,sec_copy,pLength(sec_copy));
        }
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

static void go_on (calc_dat* c){
  //set limit of 1000 for multireductions, at the moment for
  //programming reasons
  int i=0;
  red_object* buf=(red_object*) omalloc(PAR_N*sizeof(red_object));
  int curr_deg=-1;
  while(i<PAR_N){
    sorted_pair_node* s=top_pair(c);
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
    free_sorted_pair_node(s,c->r);
    if(!h) continue;
    int len=pLength(h);
    buf[i].p=h;
    buf[i].sev=pGetShortExpVector(h);
    buf[i].sum=NULL;
    buf[i].bucket = kBucketCreate(currRing);
    kBucketInit(buf[i].bucket,buf[i].p,len);
    i++;
  }
  c->normal_forms+=i;
  qsort(buf,i,sizeof(red_object),red_object_better_gen);
//    Print("\ncurr_deg:%i\n",curr_deg);
  Print("M[%i, ",i);  
  multi_reduction(buf, i, c);
  Print("%i]",i);
  int j;
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
  for(j=0;j<i;j++){
 
 
    int len;
    poly p;
    kBucketClear(buf[j].bucket,&p, &len);
    kBucketDestroy(&buf[j].bucket);
    // delete buf[j];
    //remember to free res here
    p=redTailShort(p, c->strat);
    sbuf[j]=add_to_basis(p,-1,-1,c,ibuf+j);
    
  }
  int sum=0;
  for(j=0;j<i;j++){
    sum+=ibuf[j];
  }
  sorted_pair_node** big_sbuf=(sorted_pair_node**) omalloc(sum*sizeof(sorted_pair_node*));
  int partsum=0;
  for(j=0;j<i;j++){
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
      poly tmp=pHead(h /*kBucketGetLm(P.bucket)*/);
      act->next=tmp;pIter(act);
      poly tmp2=pHead(h);
      pNeg(tmp2);
      int ltmp2=1;
      pTest(tmp2);
      kBucket_Add_q(P.bucket, tmp2, &ltmp2);

      h = kBucketGetLm(P.bucket);
      if (h==NULL)
      {
#ifdef REDTAIL_PROT
        PrintS(" ");
#endif
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

    hr=redNF2(h,c,len);
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
    hr=redNF2(h,c,len);
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
ideal t_rep_gb(ring r,ideal arg_I){
   Print("Idelems %i \n----------\n",IDELEMS(arg_I));
  ideal I=idCompactify(arg_I);
  qsort(I->m,IDELEMS(I),sizeof(poly),poly_crit);
  Print("Idelems %i \n----------\n",IDELEMS(I));
  calc_dat* c=(calc_dat*) omalloc(sizeof(calc_dat));
  c->r=currRing;
  void* h;
  poly hp;
  int i,j;
  c->easy_product_crit=0;
  c->extended_product_crit=0;
  c->is_char0=(rChar()==0);
  c->reduction_steps=0;
  c->last_index=-1;



  c->Rcounter=0;

  c->soon_free=NULL;


  c->normal_forms=0;
  c->current_degree=1;
 
  c->max_pairs=5*I->idelems();
 
  c->apairs=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*c->max_pairs);
  c->pair_top=-1;
  int n=I->idelems();
  for (i=0;i<n;i++){
    wrp(I->m[i]);
    PrintS("\n");
  }
    i=0;
  c->n=0;
  c->T_deg=(int*) omalloc(n*sizeof(int));
 
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

  for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
   {
//     add_to_basis(I->m[i],-1,-1,c);
     si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      si->i=-1;
      si->j=-1;
      si->expected_length=pLength(I->m[i]);
      si->deg=pTotaldegree(I->m[i]);
      si->lcm_of_lm=I->m[i];

//      c->apairs[n-1-i]=si;
      c->apairs[n-i-1]=si;
      ++(c->pair_top);
   }
 

  while(c->pair_top>=0)
    go_on(c);

  for(int z=0;z<c->n;z++){
    omfree(c->states[z]);
  }
  omfree(c->states);
  omfree(c->lengths);


  omfree(c->short_Exps);
  omfree(c->T_deg);

     omFree(c->strat->ecartS);
     omFree(c->strat->sevS);
//   /*initsevS(i);*/
   omFree(c->strat->S_2_R);
   

  omFree(c->strat->lenS);

   if(c->strat->lenSw)  omFree(c->strat->lenSw);




  for(i=0;i<c->n;i++){
    if(c->gcd_of_terms[i])
      pDelete(&(c->gcd_of_terms[i]));
  }
  omfree(c->gcd_of_terms);

  omfree(c->apairs);
  printf("calculated %d NFs\n",c->normal_forms);
  printf("applied %i product crit, %i extended_product crit \n", c->easy_product_crit, c->extended_product_crit);
  int deleted_form_c_s=0;

  for(i=0;i<c->n;i++){
    if (c->rep[i]!=i){
      for(j=0;j<=c->strat->sl;j++){
	if(c->strat->S[j]==c->S->m[i]){
	  c->strat->S[j]=NULL;
	  break;
	}
      }
      PrintS("R_delete");
      pDelete(&c->S->m[i]);
    }
  }
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
  omfree(c->rep);
  I=c->S;
  
  IDELEMS(I)=c->n;

  idSkipZeroes(c->S);
  for(i=0;i<=c->strat->sl;i++)
    c->strat->S[i]=NULL;
  id_Delete(&c->strat->Shdl,c->r);
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
  while((c->pair_top>=0) && (c->apairs[c->pair_top]->i>=0) && (good_has_t_rep(c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,c))){

    free_sorted_pair_node(c->apairs[c->pair_top],c->r);
    c->pair_top--;

  }
}
static void clean_top_of_pair_list(calc_dat* c){
  while((c->pair_top>0) && (c->apairs[c->pair_top]->i>=0) && (!state_is(UNCALCULATED,c->apairs[c->pair_top]->j, c->apairs[c->pair_top]->i,c))){

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

  if (a->expected_length<b->expected_length) return TRUE;
  if (a->expected_length>b->expected_length) return FALSE;
  int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  if (comp==1) return FALSE;
  if (-1==comp) return TRUE;
  if (a->i<b->i) return TRUE;
  if (a->j<b->j) return TRUE;
  return FALSE;
}

static int pair_better_gen(const void* ap,const void* bp){

  sorted_pair_node* a=*((sorted_pair_node**)ap);
  sorted_pair_node* b=*((sorted_pair_node**)bp);
  assume(a->i>a->j);
  assume(b->i>b->j);
  if (a->deg<b->deg) return -1;
  if (a->deg>b->deg) return 1;


  if (a->expected_length<b->expected_length) return -1;
  if (a->expected_length>b->expected_length) return 1;
 int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  
  if (comp==1) return 1;
  if (-1==comp) return -1;
  if (a->i<b->i) return -1;
  if (a->j<b->j) return -1;
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
    for (i=pVariables; i; i--)
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
        for (i=pVariables;i;i--)
        {
                if(pGetExp(m,i)>0)
                        return m;
  }
        pDelete(&m);
  return NULL;
}
static BOOLEAN pHasNotCFExtended(poly p1, poly p2, poly m)
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
static BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, calc_dat* c){
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
      for (i=erg.to_reduce_u;i>=erg.to_reduce_l;i--){
	int qc=los[i].guess_quality(c);
	if (qc<quality_a){
	  best=i;
	  quality_a=qc;
	}
      }
      if(best!=erg.to_reduce_u+1){
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
	for (i=erg.to_reduce_u;i>=erg.to_reduce_l;i--){
	  int qc=los[i].guess_quality(c);
	  if (qc<quality_a){
	    best=i;
	    quality_a=qc;
	  }
	}
	if(best!=erg.to_reduce_u+1){
	  red_object h=los[erg.to_reduce_l];
	  los[erg.to_reduce_l]=los[best];
	  los[best]=h;
	  erg.reduce_by=erg.to_reduce_l;
	  erg.fromS=FALSE;
	  erg.to_reduce_l++;
	  
	}
      }
      else 
      {
	assume(erg.to_reduce_u==erg.to_reduce_l);
	int quality_a=quality_of_pos_in_strat_S(erg.reduce_by,c);
	int qc=los[erg.to_reduce_u].guess_quality(c);
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
	    
	    kBucketClear(los[erg.to_reduce_u].bucket,&clear_into,&erg.expand_length);
	    erg.expand=pCopy(clear_into);
	    kBucketInit(los[erg.to_reduce_u].bucket,clear_into,erg.expand_length);
	    PrintS("e");
	    
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
      int i;
	for (i=erg.to_reduce_u;i>=erg.to_reduce_l;i--){
	  int qc=los[i].guess_quality(c);
	  if (qc<quality_a){
	    best=i;
	    quality_a=qc;
	  }
	}
	if(best!=erg.reduce_by){
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
  if(swap_roles){
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
    if(c->is_char0)
    {
      pContent(clear_into);
      pCleardenom(clear_into);
    }
  else                     
    pNorm(clear_into);
    if (new_pos<j){
      move_forward_in_S(j,new_pos,c->strat,c->is_char0);
      erg.reduce_by=new_pos;
    }
  }
}
static void multi_reduction_find(red_object* los, int losl,calc_dat* c,int startf,find_erg & erg){
  kStrategy strat=c->strat;

  assume(startf<=losl);
  assume((startf==losl-1)||(pLmCmp(los[startf].p,los[startf+1].p)==-1));
  int i=startf;
  
  int j;
  while(i>=0){
    assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)<=0));
    j=kFindDivisibleByInS_easy(strat,los[i]);
    if(j>=0){
     
      erg.to_reduce_u=i;
      erg.reduce_by=j;
      erg.fromS=TRUE;
      int i2;
      for(i2=i-1;i2>=0;i2--){
	if(!pLmEqual(los[i].p,los[i2].p))
	  break;
      }
      erg.to_reduce_l=i2+1;
      assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)==-1));
      return;
    }
    if (j<0){
      
      //not reduceable, try to use this for reducing higher terms
      int i2;
      i2=i;
      while((i2>0)&&(pLmEqual(los[i].p,los[i2-1].p)))
	i2--;
      if(i2!=i){
	
	erg.to_reduce_u=i-1;
	erg.to_reduce_l=i2;
	erg.reduce_by=i;
	erg.fromS=FALSE;
	assume((i==losl-1)||(pLmCmp(los[i].p,los[i+1].p)==-1));
	return;
      }
 
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
    multi_reduction_lls_trick(los,losl,c,erg);
    int sum=0;
    BOOLEAN join=FALSE;
    for(i=erg.to_reduce_l;i<=erg.to_reduce_u;i++){
      if(!los[i].sum) sum++;
      if (sum>=AC_NEW_MIN) {join=TRUE;break;}
    }
    
    int i;
    int len;

    reduction_step *rs=create_reduction_step(erg, los, c);
    rs->reduce(los,erg.to_reduce_l,erg.to_reduce_u);
    finalize_reduction_step(rs);
		 
    int deleted=multi_reduction_clear_zeroes(los, losl, erg.to_reduce_l, erg.to_reduce_u);
    curr_pos=erg.to_reduce_u;
    losl -= deleted;
    curr_pos -= deleted;

    //Print("deleted %i \n",deleted);
    sort_region_down(los, erg.to_reduce_l, erg.to_reduce_u-deleted, c);
//   sort_region_down(los, 0, losl-1, c);
    //  qsort(los,losl,sizeof(red_object),red_object_better_gen);
    if(erg.expand)
      add_to_reductors(c,erg.expand,erg.expand_length);
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
	kBucketInit(bucket,clear_into,len);
      }
      else
	add_this=clear_into;
      pMult_nn(add_this, sum->c_ac);
      nDelete(&sum->c_ac);
      nDelete(&sum->c_my);
      nDelete(&mult_my);
      delete sum;
      kBucket_Add_q(bucket,add_this, &len);
      sum->ac->decrease_counter();
      
    }
  }
}
void red_object::validate(){
  if(sum!=NULL)
  {
    poly lm=kBucketGetLm(bucket);
    poly lm_ac=kBucketGetLm(sum->ac->bucket);
    if ((lm_ac==NULL)||((lm!=NULL) && (pLmCmp(lm,lm_ac)!=-1))){
      flatten();
      p=kBucketGetLm(bucket);
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
    sev=pGetShortExpVector(p);
  }
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
  nDelete(&n2);
  

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
  assume(ro.sum!=NULL);
  assume(ro.sum->ac!=NULL);
  if(ro.sum->ac->last_reduction_id!=reduction_id){
    number n1=kBucketPolyRed(ro.sum->ac->bucket,p, p_len, c->strat->kNoether);
    number n2=nMult(n1,ro.sum->ac->multiplied);
    nDelete(&ro.sum->ac->multiplied);
    nDelete(&n1);
    ro.sum->ac->multiplied=n2;
  }
}
void simple_reducer::reduce(red_object* r, int l, int u){
  int i;
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
  for(i=l;i<=u;i++)
    r[i].validate();
}
reduction_step::~reduction_step(){}
simple_reducer::~simple_reducer(){
  if(fill_back!=NULL)
  {
    kBucketInit(fill_back,p,p_len);
  }
    
}

reduction_step* create_reduction_step(find_erg & erg, red_object* r, calc_dat* c){
  static int id=0;
  id++;
  
  simple_reducer* pointer= new simple_reducer();
 
  if (erg.fromS){
    pointer->p=c->strat->S[erg.reduce_by];
    pointer->p_len=c->strat->lenS[erg.reduce_by];
    pointer->fill_back=NULL;
  }
  else
  {
    
    kBucket_pt bucket=r[erg.reduce_by].bucket;
    kBucketClear(bucket,&pointer->p,&pointer->p_len);

    pointer->fill_back=bucket;
    if(c->is_char0)
      pContent(pointer->p);
  }

  pointer->reduction_id=id;
  pointer->c=c;
 
  return pointer;
};
