#include "tgb_internal.h"
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
