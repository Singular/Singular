#include "mod2.h"
#include <omalloc.h>
#include "p_polys.h"
#include "prCopy.h"
#include "ideals.h"
#include "ring.h"
#include "febase.h"
#include "structs.h"
#include "polys.h"



#include "kutil.h"
#include "kInline.cc"
#include "kstd1.h"
#define FULLREDUCTIONS
enum calc_state {
  UNCALCULATED,
  HASTREP,
  UNIMPORTANT
};
struct calc_dat{
  int* rep;
  int** states;
  ideal S;
  ring r;
  int* lengths;
  kStrategy strat;
  int found_i;
  int found_j;
  int continue_i;
  int continue_j;
  int n;
  int skipped_i;
  int normal_forms;
};
bool find_next_pair(calc_dat* c);
void replace_pair(int & i, int & j, calc_dat* c);
void initial_data(calc_dat* c);
void add_to_basis(poly h, calc_dat* c);
void do_this_spoly_stuff(int i,int j,calc_dat* c);
ideal t_rep_gb(ring r,ideal arg_I);
bool has_t_rep(int arg_i, int arg_j, calc_dat* state);
int* make_connections(int from, poly bound, calc_dat* c);
void now_t_rep(int arg_i, int arg_j, calc_dat* c);

bool find_next_pair(calc_dat* c){
  int start_i,start_j,i,j;
  start_i=0;
  start_j=-1;

  start_i=c->continue_i;
  start_j=c->continue_j;

  /*  for (int i=start_i;i<c->n;i++){
    for(int j=(i==start_i)?start_j+1:0;j<i;j++){
      // printf("searching at %d,%d",i,j);
      if (c->states[i][j]==UNCALCULATED){
	c->continue_i=c->found_i=i;
	c->continue_j=c->found_j=j;
	return true;
      }
    }
    }*/
  int z=0;
  i=start_i;
  j=start_j;
  z=start_i+start_j;
  while(z<=(2*c->n-3)){
   
    while(i>j){
      if(i>=c->n) {
	if (c->skipped_i<0) c->skipped_i=i;
      } else{
	if(c->states[i][j]==UNCALCULATED){
	  c->continue_i=c->found_i=i;
	  c->continue_j=c->found_j=j;
	  return true;
	}
      }
      --i;
      ++j;
    }
    ++z;
    i=z;
    j=0;
  }
  return false;
}
void replace_pair(int & i, int & j, calc_dat* c){  
  poly lm=pOne();
  
  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  int* i_con =make_connections(i,lm,c);
  int* j_con =make_connections(j,lm,c);
  for (int n=0;((n<c->n) && (j_con[n]>=0));n++){
    for (int m=0;((m<c->n) && (i_con[m]>=0));m++){
      if (has_t_rep(j_con[n],i_con[m],c)){
	i= i_con[m];
	j=j_con[n];
	omfree(i_con);
	omfree(j_con);
	return;
      }
    }
  }
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
  int curr_deg=pFDeg(lm);
  for (int n=0;((n<c->n) && (j_con[n]>=0));n++){
    for (int m=0;((m<c->n) && (i_con[m]>=0));m++){
      pLcm(c->S->m[i_con[m]], c->S->m[j_con[n]], lm);
      pSetm(lm);
      int comp_deg=pFDeg(lm);
      //      if ((comp_deg<curr_deg)
      //  ||
      //  ((comp_deg==curr_deg) &&
      short_s=ksCreateShortSpoly(c->S->m[i_con[m]],c->S->m[j_con[n]],c->r);
      if (short_s==NULL) {
	p_Delete(&short_s,c->r);
	continue;
      }
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
      p_Delete(&short_s,c->r);
      if ((comp_deg<curr_deg)
	  ||
	  ((comp_deg==curr_deg) &&
      (c->lengths[i]+c->lengths[j]
	  <=
       c->lengths[i_con[m]]+c->lengths[j_con[n]])))
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
int* make_connections(int from, poly bound, calc_dat* c){
  ideal I=c->S;
  int* cans=(int*) omalloc(c->n*sizeof(int));
  int* connected=(int*) omalloc(c->n*sizeof(int));
  int cans_length=0;
  connected[0]=from;
  int connected_length=1;
  for (int i=0;i<c->n;i++){
    if (i!=from){
      if(p_LmDivisibleBy(I->m[i],bound,c->r)){
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
void initial_data(calc_dat* c){
  void* h;
  int i,j;
  c->normal_forms=0;
  int n=c->S->idelems();
  c->n=n;
  c->continue_i=0;
  c->continue_j=0;
  c->skipped_i=-1;
  h=omalloc(n*sizeof(int*));
  if (h!=NULL){
    c->states=(int**) h;
  } else {
    exit(1);
  }
  h=omalloc(n*sizeof(int));
  if (h!=NULL){
    c->lengths=(int*) h;
  } else{
    exit(1);
  }
  
  h=omalloc(n*sizeof(int));
  if (h!=NULL){
    c->rep=(int*) h;
  } else {
    exit(1);
  }
  for (i=0;i<n;i++){
    h=omalloc(i*sizeof(int));
    if (h!=NULL){
      c->states[i]=(int*) h;
    } else {
      exit(1);
    }
    for (j=0;j<i;j++){
      //check product criterion
      if (pHasNotCF(c->S->m[i],c->S->m[j])){
	c->states[i][j]=HASTREP;
      } else {
	c->states[i][j]=UNCALCULATED;
      }
    }
  
    c->rep[i]=i;
    c->lengths[i]=pLength(c->S->m[i]);
  }
  c->strat=new skStrategy;
  c->strat->syzComp = 0;
  initBuchMoraCrit(c->strat);
  initBuchMoraPos(c->strat);
  c->strat->initEcart = initEcartBBA;
  c->strat->enterS = enterSBba;
  c->strat->sl = -1;
  initS(c->S,NULL,c->strat);
  for (i=c->strat->sl;i>=0;i--)
     pNorm(c->strat->S[i]);
}
void add_to_basis(poly h, calc_dat* c){
  
  void* hp;
  PrintS("s");
  ++(c->n);
  ++(c->S->ncols);
  int i,j;
  i=c->n-1;
  hp=omrealloc(c->rep, c->n *sizeof(int));
  if (hp!=NULL){
    c->rep=(int*) hp;
  } else {
    exit(1);
  }
  hp=omrealloc(c->lengths, c->n *sizeof(int));
  if (hp!=NULL){
    c->lengths=(int*) hp;
  } else {
    exit(1);
  }
  hp=omrealloc(c->states, c->n * sizeof(int*));
  if (hp!=NULL){
    c->states=(int**) hp;
  } else {
    exit(1);
  }
  c->rep[i]=i;
  hp=omalloc(i*sizeof(int));
  if (hp!=NULL){
    c->states[i]=(int*) hp;
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
  for (j=0;j<i;j++){
    if (c->rep[j]==j){ 
      //check product criterion
      if (pHasNotCF(c->S->m[i],c->S->m[j])){
	c->states[i][j]=HASTREP;
      } else {
	c->states[i][j]=UNCALCULATED;
      }

      //lies I[i] under I[j] ?
      if(p_LmDivisibleBy(c->S->m[i],c->S->m[j],c->r)){
	c->rep[j]=i;
	PrintS("R");
	for(int z=0;z<j;z++){
	  if (c->states[j][z]==UNCALCULATED){
	    c->states[j][z]=UNIMPORTANT;
	  }
	}
	for(int z=j+1;z<i;z++){
	  if (c->states[z][j]==UNCALCULATED){
	    c->states[z][j]=UNIMPORTANT;
	  }
	}
      }
    }
    else {
      c->states[i][j]=UNIMPORTANT;
    }
  }
  if (c->skipped_i>0){
    c->continue_i=c->skipped_i;
    c->continue_j=0;
    c->skipped_i=-1;
  }

  i=posInS(c->strat,c->strat->sl,h,0 /*ecart*/);

  LObject P; memset(&P,0,sizeof(P));
  P.tailRing=c->r;
  P.p=pCopy(h);
  P.FDeg=pFDeg(P.p,c->r);
  pCleardenom(P.p);
  enterT(P,c->strat,-1);
  c->strat->enterS(P,i,c->strat);
  pNorm(c->strat->S[i]);
}
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

void do_this_spoly_stuff(int i,int j,calc_dat* c){
  poly f=c->S->m[i];
  poly g=c->S->m[j];
  poly h=ksOldCreateSpoly(f, g, NULL, c->r);
  poly hr=NULL;
#ifdef FULLREDUCTIONS
//  if (h!=NULL) hr=kNF2(c->S,NULL,h,c->strat, 0);
  if (h!=NULL) 
  {
    hr=redNF(pCopy(h),c->strat);
    if (hr!=NULL)
      hr = redtailBba(hr,c->strat->sl,c->strat);
  }
#else
  //if (h!=NULL) hr=kNF2(c->S,NULL,h,c->strat, 1);
  if (h!=NULL) 
    hr=redNF(pCopy(h),c->strat);
#endif
  c->normal_forms++;
  p_Delete(&h,c->r);
  if (hr==NULL)
  {
    PrintS("-");
  } else {
    
    add_to_basis(hr, c);
  }
}

ideal t_rep_gb(ring r,ideal arg_I){
  ideal I_temp=kInterRed(arg_I);
  ideal I=idCompactify(I_temp);
  idDelete(&I_temp);
  calc_dat* c=(calc_dat*) omalloc(sizeof(calc_dat));
  c->r=currRing;
  c->S=I;
  initial_data(c);
  while (find_next_pair(c)){
    int i,j;
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
  }
  omfree(c->rep);
  for(int z=0;z<c->n;z++){
    omfree(c->states[z]);
  }
  omfree(c->states);
  omfree(c->lengths);
  printf("calculated %d NFs\n",c->normal_forms);
  omfree(c);
  return(I);
}
void now_t_rep(int arg_i, int arg_j, calc_dat* c){
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
bool has_t_rep(int arg_i, int arg_j, calc_dat* state){
  int i,j;
  if (arg_i==arg_j){
    return (true);
  }
  if (arg_i>arg_j){
    i=arg_j;
    j=arg_i;
  } else {
    i=arg_i;
    j=arg_j;
  }
  if (state->states[j][i]==HASTREP)
    return(true);
  if (j==state->rep[i]) return(false);

  if (state->rep[i]!=i) 
    return(has_t_rep(i,state->rep[i],state) && has_t_rep(state->rep[i],j,state));
  
  if (state->rep[j]!=j)
    return(has_t_rep(j,state->rep[j],state) && has_t_rep(state->rep[j],i,state));
  return(false);
}
