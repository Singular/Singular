
// #define OM_CHECK 3
// #define OM_TRACK 5
// #define OM_KEEP  1

#include "tgb.h"
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
sorted_pair_node**  merge(sorted_pair_node** p, int pn,sorted_pair_node **q, int qn,calc_dat* c){
  int i;
  int* a= (int*) omalloc(qn*sizeof(int));
  
  int lastpos=0;
  for(i=0;i<qn;i++){
    lastpos=posInPairs(p,pn-1,q[i],c, max(lastpos-1,0));
    //   cout<<lastpos<<"\n";
    a[i]=lastpos;

  }
  p=(sorted_pair_node**) realloc(p,(pn+qn)*sizeof(sorted_pair_node*));
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


  poly f1=c->S->m[pos1];
  poly f2=c->S->m[pos2];
  ring r=c->r;
 
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;

  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = f1->exp[i]+f2->exp[i];
      lb = bound->exp[i];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
//        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = f1->exp[r->VarL_Offset[i]]+f2->exp[r->VarL_Offset[i]];
      lb = bound->exp[r->VarL_Offset[i]];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
	//     pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
//  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == TRUE);
  PrintS("trivial");
  return TRUE;

}
BOOLEAN good_has_t_rep(int i, int j,calc_dat* c){
  assume(i>=0);
    assume(j>=0);
  if (has_t_rep(i,j,c)) return TRUE;
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  int deciding_deg= pFDeg(lm);
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
  c->misses_counter++;
  if(i>=0)
    c->misses[i]++;
  if (j>=0)
    c->misses[j]++;
  c->misses_series++;
}
static void soon_trep_them(redNF_inf* inf, calc_dat* c){
  int_pair_node* hf=inf->soon_free;
  
  while(hf!=NULL)
  {
    int_pair_node* s=hf;
    soon_t_rep(hf->a,hf->b,c);
              
    hf=hf->next;
    omfree(s);
  }
  inf->soon_free=NULL;
}
static void trep_them(redNF_inf* inf, calc_dat* c){
  int_pair_node* hf=inf->soon_free;
  
  while(hf!=NULL)
  {
    int_pair_node* s=hf;
    now_t_rep(hf->a,hf->b,c);
              
    hf=hf->next;
    omfree(s);
  }
  inf->soon_free=NULL;
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
  for (i=0;i<MAX_BUCKET+1;i++){
    sum+=bucket->buckets_length[i];
  }
  return sum;
}


#ifdef RANDOM_WALK
int my_rand(int n){
  //RANDOM integer beetween 0,..,n-1
  int erg=(double(rand())/RAND_MAX) *n;
  return ((erg>=n)?erg-1:erg);

}
#endif

static BOOLEAN is_empty(calc_dat* c){
  int i;
  for(i=0;i<PAR_N;i++){
    if (!c->work_on[i].is_free) return FALSE;
  }
  return TRUE;
}


static void add_to_reductors(calc_dat* c, poly h, int len){
  int i=simple_posInS(c->strat,h,len);

  LObject P; memset(&P,0,sizeof(P));
  P.tailRing=c->r;
  P.p=h; /*p_Copy(h,c->r);*/
  P.FDeg=pFDeg(P.p,c->r);
  if (!rField_is_Zp(c->r)) pCleardenom(P.p);
 
  c->strat->enterS(P,i,c->strat);
  c->strat->lenS[i]=len;
  pNorm(c->strat->S[i]);

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
  
  if (s){
  c->found_i=s->i;
  c->found_j=s->j;
  } //this passage is deprecated
  //free_sorted_pair_node(s, c->r);
  return s;

}
static BOOLEAN find_next_pair(calc_dat* c, BOOLEAN go_higher)
{
  int start_i,start_j,i,j;
  start_i=0;
  start_j=-1;
#ifndef HOMOGENEOUS_EXAMPLE
  if (c->misses_series>80){
    c->current_degree++;
    c->misses_series=0;
  }
#endif
  start_i=c->continue_i;
  start_j=c->continue_j;

  for (int i=start_i;i<c->n;i++){
    if (c->T_deg[i]>c->current_degree)
    {
      c->skipped_pairs++;
      continue;
    }
    for(int j=(i==start_i)?start_j+1:0;j<i;j++){
      // printf("searching at %d,%d",i,j);
      if (c->misses_counter>=2) {
        c->skipped_pairs++;
        break;
      }
      if (c->states[i][j]==UNCALCULATED){
        if(c->deg[i][j]<=c->current_degree)
	{
	  c->continue_i=c->found_i=i;
	  c->continue_j=c->found_j=j;
	  return TRUE;
	}
        else
	{
	  ++(c->skipped_pairs);
	}
      }
    }
    c->misses_counter=0;
  }


  if (!((start_i==1) &&(start_j==0))){
    c->continue_i=1;
    c->continue_j=0;
    return find_next_pair(c);
  }


  if ((c->skipped_pairs>0) && go_higher){
    ++(c->current_degree);
    c->skipped_pairs=0;
    c->continue_i=0;
    c->continue_j=0;
    return find_next_pair(c);
  }
  return FALSE;
}
static void move_forward_in_S(int old_pos, int new_pos,kStrategy strat)
{
  assume(old_pos>=new_pos);
  poly p=strat->S[old_pos];
  int ecart=strat->ecartS[old_pos];
  long sev=strat->sevS[old_pos];
  int s_2_r=strat->S_2_R[old_pos];
  int length=strat->lenS[old_pos];
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

  strat->S[new_pos]=p;
  strat->ecartS[new_pos]=ecart;
  strat->sevS[new_pos]=sev;
  strat->S_2_R[new_pos]=s_2_r;
  strat->lenS[new_pos]=length;
  //assume(lenS_correct(strat));
}
static void replace_pair(int & i, int & j, calc_dat* c)
{
  c->soon_free=NULL;
  int curr_deg;
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  int deciding_deg= pFDeg(lm);
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
  curr_deg=pFDeg(lm);
  int_pair_node* last=NULL;

  for (int n=0;((n<c->n) && (j_con[n]>=0));n++){
    for (int m=0;((m<c->n) && (i_con[m]>=0));m++){
      pLcm(c->S->m[i_con[m]], c->S->m[j_con[n]], lm);
      pSetm(lm);
      if (pFDeg(lm)>=deciding_deg)
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
      int comp_deg(pFDeg(short_s));
      p_Delete(&short_s,c->r);
      if ((comp_deg<curr_deg)
          ||
          ((comp_deg==curr_deg) &&
           (c->misses[i]+c->misses[j]
            <=
            c->misses[i_con[m]]+c->misses[j_con[n]])))

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

static void replace_pair(redNF_inf* inf, calc_dat* c)
{
  
//  inf->soon_free=NULL;
  
  int & i=inf->i;
  int & j=inf->j;
  assume(i>=0);
  assume(j>=0);
  int_pair_node* last=NULL;
  if (last){
    while(last->next){
      last=last->next;
    }
  }
  {
    int_pair_node* h= (int_pair_node*)omalloc(sizeof(int_pair_node));
    if (last!=NULL)
      last->next=h;
    else
      inf->soon_free=h;
    h->next=NULL;
    h->a=i;
    h->b=j;
    last=h;
  }
  int curr_deg;
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  int deciding_deg= pFDeg(lm);
  int* i_con =make_connections(i,j,lm,c);
  int z=0;

  for (int n=0;((n<c->n) && (i_con[n]>=0));n++){
    if (i_con[n]==j){
      //       curr_deg=pFDeg(lm);
      //       for(int z1=0;((z1<c->n) && (i_con[z1]>=0));z1++)
      //         for (int z2=z1+1;((z2<c->n)&&(i_con[z2]>=0));z2++)
      //         {
      //           pLcm(c->S->m[i_con[z1]], c->S->m[i_con[z2]], lm);
      //           pSetm(lm);
      //           if (pFDeg(lm)==curr_deg)
      //             now_t_rep(i_con[z1],i_con[z2],c);
      //         }
      now_t_rep(i,j,c);
//hack
//      i=j;
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
  curr_deg=pFDeg(lm);


  for (int n=0;((n<c->n) && (j_con[n]>=0));n++){
    for (int m=0;((m<c->n) && (i_con[m]>=0));m++){
      pLcm(c->S->m[i_con[m]], c->S->m[j_con[n]], lm);
      pSetm(lm);
      if (pFDeg(lm)>=deciding_deg)
      {
	//soon_t_rep(i_con[m],j_con[n],c);
	int_pair_node* h= (int_pair_node*)omalloc(sizeof(int_pair_node));
	if (last!=NULL)
	  last->next=h;
	else
	  inf->soon_free=h;
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
      int comp_deg(pFDeg(short_s));
      p_Delete(&short_s,c->r);
      if ((comp_deg<curr_deg)
          ||
          ((comp_deg==curr_deg) &&
           (c->misses[i]+c->misses[j]
            <=
            c->misses[i_con[m]]+c->misses[j_con[n]])))
	//       if ((comp_deg<curr_deg)
	//           ||
	//           ((comp_deg==curr_deg) &&
	//            (c->lengths[i]+c->lengths[j]
	//             <=
	//             c->lengths[i_con[m]]+c->lengths[j_con[n]])))

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
  int s=pFDeg(bound);
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
      if (has_t_rep(pos,cans[i],c)){

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
  int s=pFDeg(bound);
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
        if (has_t_rep(pos,cans[i],c)||(trivial_syzygie(pos,cans[i],bound,c))
){

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
    } else {

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
static int init_red_input_element(calc_dat* c, poly el, int pos){
    c->work_on[pos].soon_free=NULL;
  c->work_on[pos].is_free=FALSE;
  c->work_on[pos].started=FALSE;
  c->work_on[pos].i=-1 ;
  c->work_on[pos].j=-1;
  c->work_on[pos].h=NULL;
  c->work_on[pos].need_std_rep=TRUE;
 

  
  
  c->work_on[pos].started=TRUE;
  
  if (!el){
    c->work_on[pos].is_free=TRUE;
    c->work_on[pos].h=NULL;
    return pos;
  }
  if (c->work_on[pos].P!=NULL)
    delete c->work_on[pos].P;


  
  int len=pLength(el);
  c->work_on[pos].len_upper_bound=len;
  c->work_on[pos].P=new LObject(el);
  c->work_on[pos].P->SetShortExpVector();
  c->work_on[pos].P->bucket = kBucketCreate(currRing);
  assume(c->work_on[pos].P->p==el);
  kBucketInit(c->work_on[pos].P->bucket,c->work_on[pos].P->p,len /*pLength(P.p)*/);
  

  return pos;
  
}
static int init_red_phase1(calc_dat* c, int i, int j, int pos)
{
 
  c->work_on[pos].soon_free=NULL;
  c->work_on[pos].is_free=FALSE;
  c->work_on[pos].started=FALSE;
  c->work_on[pos].i=i;
  c->work_on[pos].j=j;
  c->work_on[pos].h=NULL;
  c->work_on[pos].need_std_rep=FALSE;

  replace_pair(&c->work_on[pos],c);
//verhindern daß ein Paar mehrmals ausgerechnet wird
  soon_trep_them(&c->work_on[pos], c);

  return pos;
}
static void init_red_spoly_phase2(calc_dat* c,int pos){
  replace_pair(&c->work_on[pos],c);
  assume(c->work_on[pos].i>=0);
    assume(c->work_on[pos].j>=0);  
  if(c->work_on[pos].i==c->work_on[pos].j){
    c->work_on[pos].is_free=TRUE;
    c->work_on[pos].h=NULL;
    return;
  }
  c->work_on[pos].started=TRUE;
  poly h=ksOldCreateSpoly(c->S->m[c->work_on[pos].i], c->S->m[c->work_on[pos].j], NULL, c->r);
  if (h==NULL){
    c->work_on[pos].is_free=TRUE;
    c->work_on[pos].h=NULL;
    return;
  }
  if (c->work_on[pos].P!=NULL)
    delete c->work_on[pos].P;


  
  int len=pLength(h);
  c->work_on[pos].len_upper_bound=len;
  c->work_on[pos].P=new LObject(h);
  c->work_on[pos].P->SetShortExpVector();
  c->work_on[pos].P->bucket = kBucketCreate(currRing);
  kBucketInit(c->work_on[pos].P->bucket,c->work_on[pos].P->p,len /*pLength(P.p)*/);
  
};
static void initial_data(calc_dat* c){
  void* h;
  int i,j;
  c->last_index=-1;
  c->work_on=(redNF_inf*) omalloc(PAR_N*sizeof(redNF_inf));
  int counter;
  for(counter=0;counter<PAR_N;counter++){
    c->work_on[counter].is_free=TRUE;
    c->work_on[counter].P=NULL;
  }
  c->misses_series=0;
  c->soon_free=NULL;
  c->misses_counter=0;
  c->max_misses=0;
  c->normal_forms=0;
  c->current_degree=1;
  c->skipped_pairs=0;
  c->pairs=NULL;
  int n=c->S->idelems();
  c->n=n;
  c->T_deg=(int*) omalloc(n*sizeof(int));
  c->continue_i=0;
  c->continue_j=0;
  c->skipped_i=-1;
#ifdef HEAD_BIN
  c->HeadBin=omGetSpecBin(POLYSIZE + (currRing->ExpL_Size)*sizeof(long));
#endif
  /* omUnGetSpecBin(&(c->HeadBin)); */
  h=omalloc(n*sizeof(char*));
  if (h!=NULL)
    c->states=(char**) h;
  else
    exit(1);
  c->misses=(int*) omalloc(n*sizeof(int));
  c->deg=(int **) omalloc(n*sizeof(int*));
  h=omalloc(n*sizeof(int));
  if (h!=NULL)
    c->lengths=(int*) h;
  else
    exit(1);

  h=omalloc(n*sizeof(int));
  if (h!=NULL)
    c->rep=(int*) h;
  else
    exit(1);
  c->short_Exps=(long*) omalloc(n*sizeof(long));
  for (i=0;i<n;i++)
  {
#ifdef HEAD_BIN
    c->S->m[i]=p_MoveHead(c->S->m[i],c->HeadBin);
#endif
    c->T_deg[i]=pFDeg(c->S->m[i]);
    c->lengths[i]=pLength(c->S->m[i]);
    c->misses[i]=0;
    c->states[i]=(char *)omAlloc((i+1)*sizeof(char));
    c->deg[i]=(int*) omAlloc((i+1)*sizeof(int));
    for (j=0;j<i;j++){
      //check product criterion
      if (pHasNotCF(c->S->m[i],c->S->m[j])){
	c->states[i][j]=HASTREP;
      } else {
	c->states[i][j]=UNCALCULATED;
      }
      if ((c->lengths[i]==1) && (c->lengths[j]==1))
	c->states[i][j]=HASTREP;
      c->deg[i][j]=pLcmDeg(c->S->m[i],c->S->m[j]);
      if (c->states[i][j]==UNCALCULATED){
	sort_pair_in(i,j,c);
      }
    }
      
    c->rep[i]=i;
    c->short_Exps[i]=p_GetShortExpVector(c->S->m[i],c->r);

  }


  c->strat=new skStrategy;
  c->strat->syzComp = 0;
  initBuchMoraCrit(c->strat);
  initBuchMoraPos(c->strat);
  c->strat->initEcart = initEcartBBA;
  c->strat->enterS = enterSBba;
  c->strat->sl = -1;
  /* initS(c->S,NULL,c->strat); */
/* intS start: */
  i=((i+IDELEMS(c->S)+15)/16)*16;
  c->strat->ecartS=(intset)omAlloc(i*sizeof(int)); /*initec(i);*/
  c->strat->sevS=(unsigned long*)omAlloc0(i*sizeof(unsigned long));
  /*initsevS(i);*/
  c->strat->S_2_R=(int*)omAlloc0(i*sizeof(int));/*initS_2_R(i);*/
  c->strat->fromQ=NULL;
  c->strat->Shdl=idInit(i,1);
  c->strat->S=c->strat->Shdl->m;
  c->strat->lenS=(int*)omAlloc0(i*sizeof(int));
  for (i=0; i<IDELEMS(c->S); i++)
  {
    int pos;
    assume (c->S->m[i]!=NULL);
    LObject h;
    h.p = c->S->m[i];
    h.pNorm();
    c->strat->initEcart(&h);
    assume(c->lengths[i]==pLength(h.p));
    if (c->strat->sl==-1) pos=0;
    else pos = simple_posInS(c->strat,h.p,c->lengths[i]);
    h.sev = pGetShortExpVector(h.p);
    c->strat->enterS(h,pos,c->strat);
    c->strat->lenS[pos]=c->lengths[i];
  }
  //c->strat->lenS=(int*)omAlloc0(IDELEMS(c->strat->Shdl)*sizeof(int));
  //for (i=c->strat->sl;i>=0;i--)
  //{
  //  pNorm(c->strat->S[i]);
  //  c->strat->lenS[i]=pLength(c->strat->S[i]);
  //}
  /* initS end */
}
static void initial_data2(calc_dat* c, ideal I){
  void* h;
  poly hp;
  int i,j;
  c->reduction_steps=0;
  c->last_index=-1;
  c->work_on=(redNF_inf*) omalloc(PAR_N*sizeof(redNF_inf));
  int counter;
  for(counter=0;counter<PAR_N;counter++){
    c->work_on[counter].is_free=TRUE;
    c->work_on[counter].P=NULL;
  }
  c->Rcounter=0;
  c->misses_series=0;
  c->soon_free=NULL;
  c->misses_counter=0;
  c->max_misses=0;
  c->normal_forms=0;
  c->current_degree=1;
  c->skipped_pairs=0;
  c->pairs=NULL;
  int n=I->idelems();
  for (i=0;i<n;i++){
    wrp(I->m[i]);
    PrintS("\n");
  }
    i=0;

  c->n=0;
  c->T_deg=(int*) omalloc(n*sizeof(int));
  c->continue_i=0;
  c->continue_j=0;
  c->skipped_i=-1;
#ifdef HEAD_BIN
  c->HeadBin=omGetSpecBin(POLYSIZE + (currRing->ExpL_Size)*sizeof(long));
#endif
  /* omUnGetSpecBin(&(c->HeadBin)); */
  h=omalloc(n*sizeof(char*));
 
  c->states=(char**) h;
  
  c->misses=(int*) omalloc(n*sizeof(int));
  c->deg=(int **) omalloc(n*sizeof(int*));
  h=omalloc(n*sizeof(int));
  c->lengths=(int*) h;
  h=omalloc(n*sizeof(int));
  
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
  sorted_pair_node* si;
  assume(n>0);
  add_to_basis(I->m[0],-1,-1,c);
  sorted_pair_node** set_this= & (c->pairs);
  assume(c->strat->sl==c->strat->Shdl->idelems()-1);
 
  for (i=1;i<n;i++)//the 1 is wanted, because first element is added to basis
   {
//     add_to_basis(I->m[i],-1,-1,c);
     si=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
      si->i=-1;
      si->j=-1;
      si->expected_length=pLength(I->m[i]);
      si->deg=pFDeg(I->m[i]);
      si->lcm_of_lm=I->m[i];
      si->next=NULL;
      PrintS("ho");
     (*set_this)=si;
      set_this=&(si->next);
      

   }
  
}
static int simple_posInS (kStrategy strat, poly p,int len)
{
  if(strat->sl==-1) return 0;
  polyset set=strat->S;
  intset setL=strat->lenS;
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
	    || ((len==setL[length]) && (pLmCmp(set[an],p) == 1))) return an;
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
static void add_to_basis(poly h, int i_pos, int j_pos,calc_dat* c)
{
//  BOOLEAN corr=lenS_correct(c->strat);
  BOOLEAN R_found=FALSE;
  void* hp;
  PrintS("s");
  ++(c->n);
  ++(c->S->ncols);
  int i,j;
  i=c->n-1;
  sorted_pair_node** nodes=(sorted_pair_node**) omalloc(sizeof(sorted_pair_node*)*i);
  int spc=0;
  c->T_deg=(int*) omrealloc(c->T_deg,c->n*sizeof(int));
  c->T_deg[i]=pFDeg(h);
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
  c->misses=(int*) omrealloc(c->misses,c->n*sizeof(int));
  c->misses[i]=0;
  c->lengths[i]=pLength(h);
  hp=omrealloc(c->states, c->n * sizeof(char*));
  if (hp!=NULL){
    c->states=(char**) hp;
  } else {
    exit(1);
  }
  c->deg=(int**) omrealloc(c->deg, c->n * sizeof(int*));
  c->rep[i]=i;
  hp=omalloc(i*sizeof(char));
  if (hp!=NULL){
    c->states[i]=(char*) hp;
  } else {
    exit(1);
  }
  c->deg[i]=(int*) omalloc(i*sizeof(int));
  hp=omrealloc(c->S->m,c->n*sizeof(poly));
  if (hp!=NULL){
    c->S->m=(poly*) hp;
  } else {
    exit(1);
  }
  c->S->m[i]=h;
  c->short_Exps[i]=p_GetShortExpVector(h,c->r);
  for (j=0;j<i;j++){
    c->deg[i][j]=pLcmDeg(c->S->m[i],c->S->m[j]);
    if (c->rep[j]==j){
      //check product criterion

      c->states[i][j]=UNCALCULATED;


      //lies I[i] under I[j] ?
      if(p_LmShortDivisibleBy(c->S->m[i],c->short_Exps[i],c->S->m[j],~(c->short_Exps[j]),c->r)){
        c->rep[j]=i;
        PrintS("R"); R_found=TRUE;
        c->Rcounter++;
        if((i_pos>=0) && (j_pos>=0)){
	  c->misses[i_pos]--;
	  c->misses[j_pos]--;
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
    else if (pHasNotCF(c->S->m[i],c->S->m[j]))
      c->states[i][j]=HASTREP;
    if (c->states[i][j]==UNCALCULATED){
//      sort_pair_in(i,j,c);
      poly short_s=ksCreateShortSpoly(c->S->m[i],c->S->m[j],c->r);
      if (short_s)
      {
	sorted_pair_node* s=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
	s->i=max(i,j);
	s->j=min(i,j);
	s->expected_length=c->lengths[i]+c->lengths[j]-2;
	s->deg=pFDeg(short_s);
	//poly lm=pOne();
      
	// pLcm(c->S->m[i], c->S->m[j], lm);
	//pSetm(lm);
	s->lcm_of_lm=short_s;
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
  if (c->skipped_i>0){
    c->continue_i=c->skipped_i;
    c->continue_j=0;
    c->skipped_i=-1;
  }
  add_to_reductors(c, h, c->lengths[c->n-1]);
  //i=posInS(c->strat,c->strat->sl,h,0 /*ecart*/);
  
  if (c->lengths[c->n-1]==1)
    shorten_tails(c,c->S->m[c->n-1]);
  // if (corr){
  
//     corr=lenS_correct(c->strat);
//     if(!corr){
//       PrintS("korupted in shorten tails");
//     }
//     }
  //you should really update c->lengths, c->strat->lenS, and the oder of polys in strat if you sort after lengths

  //for(i=c->strat->sl; i>0;i--)
  //  if(c->strat->lenS[i]<c->strat->lenS[i-1]) printf("fehler bei %d\n",i);
  if (c->Rcounter>50) {
    c->Rcounter=0;
    cleanS(c->strat);
  }
  qsort(nodes,spc,sizeof(sorted_pair_node*),pair_better_gen);
  clean_top_of_pair_list(c);
  sorted_pair_node* last=c->pairs;
  
  if(spc>0){
    if (!c->pairs) 
    {
      --spc;
      c->pairs=nodes[spc];
      c->pairs->next=NULL;
      last=c->pairs;
    }
    while(spc>0){
      --spc;
      sorted_pair_node* h=last->next;
      
      while((h!=NULL)&&(pair_better(h,nodes[spc],c)))
      {
	if ((h->i>=0) && (!state_is(UNCALCULATED,h->j, h->i,c))){
	  last->next=h->next;
	  
	  free_sorted_pair_node(h,c->r);
	  h=last->next;
	  if(!h) break;
	}
	last=h;
       
	h=h->next;
	assume((h==NULL)||(h->lcm_of_lm!=NULL));
      }
      last->next=nodes[spc];
      nodes[spc]->next=h;
      last=nodes[spc];
    }
  }

  omfree(nodes);

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
  int j;

  kStrategy strat=c->strat;
  len=pLength(h);
  int len_upper_bound=len;
  if (0 > strat->sl)
  {
    return h;
  }
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
	      sec_copy=kBucketClear(P.bucket);
	      kBucketInit(P.bucket,pCopy(sec_copy),pLength(sec_copy));
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
	  int new_pos=simple_posInS(c->strat,strat->S[j],new_length);//hack
            
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
	      int i=0;
	      for(i=new_pos;i<old_pos;i++){
		if (strat->lenS[i]<=new_length)
		  new_pos++;
		else
		  break;
	      }
	      if (new_pos<old_pos)
		move_forward_in_S(old_pos,new_pos,c->strat);
                
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
  int len;
  for(i=0;i<=strat->sl;i++){
    if(strat->lenS[i]>2)
      break;
  }
  return(redNFTail(h,i-1,strat, len));
}



static BOOLEAN redNF2_n_steps (redNF_inf* obj,calc_dat* c, int n)
{
  
  obj->P->SetShortExpVector();
  poly h;
  if (obj->is_free){
    return TRUE;}
  int len; 
  int j;
  kStrategy strat=c->strat;
  assume(strat->sl<50000);
  if (0 > strat->sl)
  {
    
    kBucketClear(obj->P->bucket,&(obj->P->p),&len);
    kBucketDestroy(&obj->P->bucket);
    pNormalize(obj->P->p);
    obj->h=obj->P->p;
    obj->is_free=TRUE;
    return TRUE;

  }
  loop
    {
      
      if (n<=0){
        
        return FALSE;
      }
      kBucketCanonicalize(obj->P->bucket);
      int compare_bound;
      compare_bound=bucket_guess(obj->P->bucket);
      obj->len_upper_bound=min(compare_bound,obj->len_upper_bound);
      j=kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,obj->P);
      if (j>=0)
      {
	poly sec_copy=NULL;

	BOOLEAN must_expand=FALSE;
	BOOLEAN must_replace_in_basis=(obj->len_upper_bound<strat->lenS[j]);//first test
	if (must_replace_in_basis)
	{
	  //second test
	  if (pLmEqual(obj->P->p,strat->S[j]))
	  {
	    PrintS("b");
	    sec_copy=kBucketClear(obj->P->bucket);
	    kBucketInit(obj->P->bucket,pCopy(sec_copy),pLength(sec_copy));
	  }
	  else
	  {
	    must_replace_in_basis=FALSE;
	    if ((obj->len_upper_bound==1)
		||(obj->len_upper_bound==2)
		||(obj->len_upper_bound<strat->lenS[j]/2))
	    {
	      PrintS("e");
	      sec_copy=kBucketClear(obj->P->bucket);
	      kBucketInit(obj->P->bucket,pCopy(sec_copy),pLength(sec_copy));
	      must_expand=TRUE;
	    }
	  }
	}

	nNormalize(pGetCoeff(obj->P->p));
#ifdef KDEBUG
	if (TEST_OPT_DEBUG)
	{
	  PrintS("red:");
	  wrp(h);
	  PrintS(" with ");
	  wrp(strat->S[j]);
	}
#endif
	obj->len_upper_bound=obj->len_upper_bound+strat->lenS[j]-2;
	number coef=kBucketPolyRed(obj->P->bucket,strat->S[j],
				   strat->lenS[j]/*pLength(strat->S[j])*/,
				   strat->kNoether);
	nDelete(&coef);
	h = kBucketGetLm(obj->P->bucket);
        
	if (must_replace_in_basis){
	  obj->need_std_rep=TRUE;
	  int pos_in_c=-1;
	  poly p=strat->S[j];
	  int z;
            
	  int new_length=pLength(sec_copy);
	  Print("%i",strat->lenS[j]-new_length);
	  obj->len_upper_bound=new_length +strat->lenS[j]-2;//old entries length
	  int new_pos=simple_posInS(c->strat,strat->S[j],new_length);//hack
            
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
	      int i=0;
	      for(i=new_pos;i<old_pos;i++){
		if (strat->lenS[i]<=new_length)
		  new_pos++;
		else
		  break;
	      }
	      if (new_pos<old_pos)
		move_forward_in_S(old_pos,new_pos,c->strat);
                
	      c->S->m[pos_in_c]=sec_copy;
                                
	      c->lengths[pos_in_c]=new_length;
	      length_one_crit(c,pos_in_c, new_length);
        
	    }
	  }
	}
	if(must_expand){
	  obj->need_std_rep=TRUE;
	  add_to_reductors(c,sec_copy,pLength(sec_copy));
	}
	if (h==NULL) {
	  obj->h=NULL;
	  obj->is_free=TRUE;
	  return TRUE;
	}
	obj->P->p=h;
	obj->P->t_p=NULL;
	obj->P->SetShortExpVector();

      }
      else
      {
        kBucketClear(obj->P->bucket,&(obj->P->p),&len);
        kBucketDestroy(&obj->P->bucket);
        pNormalize(obj->P->p);
        obj->h=obj->P->p;
	obj->is_free=TRUE;
        return TRUE;
      }
      n--;
      c->reduction_steps++;
    }
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
  if (pNext(h)==NULL) return h;
  pTest(h);
  if (0 > sl)
    return h;

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
    c->misses_series=0;
#ifdef HEAD_BIN
    hr=p_MoveHead(hr,c->HeadBin);
#endif
    add_to_basis(hr, i,j,c);
  }
}
//try to fill, return FALSE iff queue is empty
static int find_next_empty(calc_dat* c){
  int n;
  for(n=0;n<PAR_N;n++)
    if(c->work_on[n].is_free)
      break;
  return n;
}
static BOOLEAN fillup(calc_dat* c){
  int n=find_next_empty(c);
  sorted_pair_node* p;
  while (n<PAR_N) {
    if 
      (p=find_next_pair2(c, TRUE)) //the = is wanted
    {
      if(p->i<0){
	init_red_input_element(c,p->lcm_of_lm,n);
	omfree(p);
      }
      
      else {
	init_red_phase1(c,p->i,p->j,n);
	free_sorted_pair_node(p,c->r);
      }
    } 
    else {
      if (n==0) return (!is_empty(c));
      return TRUE;
    }
    
    n=find_next_empty(c);
  }
  return TRUE;
}

static BOOLEAN compute(calc_dat* c){
  int i=(c->last_index+ 1)%PAR_N;
  int len;
  BOOLEAN suc=FALSE;
  while(1){
    if(!c->work_on[i].is_free){
      if ((!c->work_on[i].need_std_rep) && (good_has_t_rep(c->work_on[i].i,c->work_on[i].j,c))){
        suc=TRUE;
        //clear ressources ;; needs to be done
        c->work_on[i].is_free=TRUE;
        
        trep_them(&c->work_on[i],c);
	c->work_on[i].soon_free=NULL;
            
	c->work_on[i].is_free=TRUE;
	c->last_index=i;
	return suc;
      }
      if (!c->work_on[i].started)
	init_red_spoly_phase2(c,i);
      if (!c->work_on[i].is_free){
//	Print("Computing i=%i,j=%i",c->work_on[i].i,c->work_on[i].j);
	if(redNF2_n_steps(&(c->work_on[i]),c,((c->reduction_steps+50)/(c->normal_forms+1))/2+5)){
	  

	  trep_them(&c->work_on[i],c);
            
	  c->work_on[i].soon_free=NULL;
            
	  c->work_on[i].is_free=TRUE;
	  poly hr=c->work_on[i].h;
	  len=pLength(hr);
	  suc=TRUE;
	  int c2=0;
	  for(c2=0;c2<=c->strat->sl;c2++){
	    if(c->strat->lenS[c2]>2)
	      break;
	  }
	  int sec_sl=c->strat->sl;
	  c->strat->sl=c2-1;

#ifdef FULLREDUCTIONS
	  if (hr!=NULL)
#ifdef REDTAIL_S
	    hr = redNFTail(hr,c->strat->sl,c->strat,len);
#else
	  hr = redtailBba(hr,c->strat->sl,c->strat);
#endif
#endif
	  c->strat->sl=sec_sl;
	  c->normal_forms++;
          
	  if (hr==NULL)
	  {
	    notice_miss(c->work_on[i].i, c->work_on[i].j, c);

	  }
	  else
	  {
	    c->misses_series=0;
#ifdef HEAD_BIN
	    hr=p_MoveHead(hr,c->HeadBin);
#endif
	    add_to_basis(hr, c->work_on[i].i,c->work_on[i].j,c);
         
	  }
	}
      }
      else {
	suc=TRUE;
	int_pair_node* hf=c->work_on[i].soon_free;
	//now_t_rep(c->work_on[i].i,c->work_on[i].j,c);
	while(hf!=NULL)
	{
	  int_pair_node* s=hf;
	  now_t_rep(hf->a,hf->b,c);
            
	  hf=hf->next;
	  omfree(s);
	}
	c->work_on[i].soon_free=NULL;
	PrintS("-");
	c->misses_counter++;
	if(c->work_on[i].i>=0){
	c->misses[c->work_on[i].i]++;
	c->misses[c->work_on[i].j]++;
	}
	c->misses_series++;
      }
      c->last_index=i;
      return suc;
    }
    
    
    
    i++;
    i=i%PAR_N;
  }
}
static int poly_crit(const void* ap1, const void* ap2){
  poly p1,p2;
  p1=*((poly*) ap1);
  p2=*((poly*)ap2);
  
  if (pLmCmp(p1,p2)!=0) return pLmCmp(p1,p2);
  if (pLength(p1)<pLength(p2)) return -1;
  if (pLength(p1)>pLength(p2)) return 1;
  return 0;
}
ideal t_rep_gb(ring r,ideal arg_I){
   Print("Idelems %i \n----------\n",IDELEMS(arg_I));
  ideal I_temp=idCopy(arg_I); //kInterRed(arg_I);
  ideal I=idCompactify(I_temp);
  qsort(I->m,IDELEMS(I),sizeof(poly),poly_crit);
  idDelete(&I_temp);
  Print("Idelems %i \n----------\n",IDELEMS(I));
  calc_dat* c=(calc_dat*) omalloc(sizeof(calc_dat));
  c->r=currRing;
 
  initial_data2(c,I);
#if 0
  while (find_next_pair(c)){
    int i,j;
    int z;
    i=c->found_i;
    j=c->found_j;
    replace_pair(i,j,c);
    if (!(c->states[max(i,j)][min(i,j)]==HASTREP)){
      do_this_spoly_stuff(i,j,c);
    }
    //else
    //  PrintS("f");

    now_t_rep(i,j,c);
    now_t_rep(c->found_i,c->found_j,c);
#ifdef RANDOM_WALK
    c->continue_i=my_rand(c->n-1)+1;
    c->continue_j=my_rand(c->continue_i);
#endif
    int_pair_node* h=c->soon_free;
    while(h!=NULL)
    {
      int_pair_node* s=h;
      now_t_rep(h->a,h->b,c);

      h=h->next;
      omfree(s);
    }


  }
#endif
  BOOLEAN SUC=TRUE;
  while((!SUC)||fillup(c)){
    if (is_empty(c)) break;
    SUC=compute(c);
  }
  omfree(c->rep);
  for(int z=0;z<c->n;z++){
    omfree(c->states[z]);
  }
  omfree(c->states);
  omfree(c->lengths);
  printf("calculated %d NFs\n",c->normal_forms);
  I=c->S;
  IDELEMS(I)=c->n;
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
static int pMinDeg3(poly f){
  if (f==NULL){
    return(-1);

  }

  poly h=f->next;
  int n=pFDeg(h);
  int i=0;
  while((i<2)){
    if (h==NULL)
      return(n);
    h=h->next;
    if (h!=NULL){
      n=min(n,pFDeg(h));
    }
    i++;
  }

  return n;
}


static void shorten_tails(calc_dat* c, poly monom)
{
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
      int new_pos=simple_posInS(c->strat,c->S->m[i],c->lengths[i]);
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
      assume(pLength(c->strat->S[old_pos])==c->lengths[i]);
      c->strat->lenS[old_pos]=c->lengths[i];
      if (new_pos<old_pos)
	move_forward_in_S(old_pos,new_pos,c->strat);
          
      length_one_crit(c,i,c->lengths[i]);
    }
      
  }

}
static sorted_pair_node* pop_pair(calc_dat* c){
  clean_top_of_pair_list(c);
  if(c->pairs==NULL) return NULL;
  sorted_pair_node* h=c->pairs;
  c->pairs=c->pairs->next;
  return h;
  
}
static BOOLEAN no_pairs(calc_dat* c){
  clean_top_of_pair_list(c);
  return (c==NULL);
}

static void clean_top_of_pair_list(calc_dat* c){
  while((c->pairs) && (c->pairs->i>=0) && (!state_is(UNCALCULATED,c->pairs->j, c->pairs->i,c))){
    sorted_pair_node* s=c->pairs;
    c->pairs=c->pairs->next;
    free_sorted_pair_node(s,c->r);
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
  if (a->deg<b->deg) return TRUE;
  if (a->deg>b->deg) return FALSE;

 
  if (a->expected_length<b->expected_length) return TRUE;
   if (a->expected_length>b->expected_length) return FALSE;
 int comp=pLmCmp(a->lcm_of_lm, b->lcm_of_lm);
  if ((comp==0)&& (((a->i==b->i) && (a->j==b->j))||((a->j==b->i)&&(a->i==b->j))))
    return 0;
  if (comp==1) return -1;
  if (-1==comp) return 1;
  if (a->i<b->i) return 1;
  if (a->j<b->j) return 1;
  return 1;
}
static void sort_pair_in(int i, int j,calc_dat* c){
  assume(0<=i);
  assume(0<=j);
  assume(i<c->n);
  assume(j<c->n);
  assume(i!=j);
  sorted_pair_node* s=(sorted_pair_node*) omalloc(sizeof(sorted_pair_node));
  s->i=max(i,j);
  s->j=min(i,j);
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  s->lcm_of_lm=lm;
  
  sorted_pair_node* last=c->pairs;
  if (!c->pairs) {c->pairs=s;s->next=NULL;}
  else
  {
    sorted_pair_node* h=c->pairs->next;
    while((h!=NULL)&&(pair_better(h,s,c))){
      last=h;
      h=h->next;
    }
    last->next=s;
    s->next=h;
  }
}
