#include <kernel/tgb_internal.h>
#if 0
static void notice_miss(int i, int j, slimgb_alg* c){
  if (TEST_OPT_PROT)
    PrintS("-");
 
}
#endif
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
        worst=si_max(q,worst);
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
  if(c->T_deg_full){
    c->T_deg_full=(int*) omrealloc(c->T_deg_full,c->n*sizeof(int));
    c->T_deg_full[i]=pTotaldegree_full(h);
  }
  
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
  if (i>0)
    hp=omalloc(i*sizeof(char));
  else
    hp=NULL;
  if (hp!=NULL){
    c->states[i]=(char*) hp;
  } else {
    //exit(1);
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
      s->i=si_max(i,j);
      s->j=si_min(i,j);
      s->expected_length=c->lengths[i]+c->lengths[j]-2;
      
      poly lm=pOne();

      pLcm(c->S->m[i], c->S->m[j], lm);
      pSetm(lm);
      s->deg=pTotaldegree(lm);
      if(c->T_deg_full)
      {
        int t_i=c->T_deg_full[s->i]-c->T_deg[s->i];
        int t_j=c->T_deg_full[s->j]-c->T_deg[s->j];
        s->deg+=si_max(t_i,t_j);
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

#if 0
static void replace_pair(int & i, int & j, slimgb_alg* c)
{
  c->soon_free=NULL;
  int curr_deg;
  poly lm=pOne();

  pLcm(c->S->m[i], c->S->m[j], lm);
  pSetm(lm);
  int deciding_deg= pTotaldegree(lm);
  int* i_con =make_connections(i,j,lm,c);
  
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
#endif

#if 0
//not needed any more, but should work
static int* make_connections(int from, poly bound, slimgb_alg* c)
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
#endif
#if 0
static void soon_t_rep(const int& arg_i, const int& arg_j, slimgb_alg* c)
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
#endif
