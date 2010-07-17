#include <kernel/mod2.h>
#include <kernel/F4.h>
#include <kernel/tgb_internal.h>
#include <kernel/tgbgauss.h>
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: F4 implementation
*/
static int posInPolys (poly*  p, int pn, poly qe,slimgb_alg* c)
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

static tgb_sparse_matrix* build_sparse_matrix(poly* p,int p_index,poly* done, int done_index, slimgb_alg* c){
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
static tgb_matrix* build_matrix(poly* p,int p_index,poly* done, int done_index, slimgb_alg* c){
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

static int retranslate(poly* m,tgb_sparse_matrix* mat,poly* done, slimgb_alg* c){
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
//!returns m_index and destroys mat
 static int retranslate(poly* m,tgb_matrix* mat,poly* done, slimgb_alg* c){
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
static int pLmCmp_func(const void* ap1, const void* ap2){
    poly p1,p2;
    p1=*((poly*) ap1);
    p2=*((poly*)ap2);

    return pLmCmp(p1,p2);
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
static int posInMonomPolys (monom_poly*  p, int pn, monom_poly & qe,slimgb_alg* c)
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
static void simplify(monom_poly& h, slimgb_alg* c){
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
    for(i=si_min(posm,F->size-1);i>=0;i--)
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

void go_on_F4 (slimgb_alg* c){
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
        chosen_size+=si_max(max_par,2);
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
    sbuf[j]=add_to_basis_ideal_quotient(p,c,ibuf+j);
    
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

  qsort(big_sbuf,sum,sizeof(sorted_pair_node*),tgb_pair_better_gen2);
  c->apairs=spn_merge(c->apairs,c->pair_top+1,big_sbuf,sum,c);
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
