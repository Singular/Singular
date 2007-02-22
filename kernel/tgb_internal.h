#ifndef TGB_INTERNAL_H
#define TGB_INTERNAL_H
//!\file tgb_internal.h
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tgb_internal.h,v 1.57 2007-02-22 14:46:50 bricken Exp $ */
/*
 * ABSTRACT: tgb internal .h file
*/
#include <omalloc.h>
#include "p_polys.h"

#include "ideals.h"
#include "ring.h"
#include "febase.h"
#include "structs.h"
#include "polys.h"
#include "stdlib.h"
#include <modulop.h>
//#define USE_NORO 1
#ifdef USE_NORO
#define NORO_CACHE 1
#define NORO_SPARSE_ROWS_PRE 1
#define NORO_NON_POLY 1
#endif
#ifdef NORO_CACHE
//#include <map>
#include <vector>
#endif
#ifdef HAVE_BOOST_DYNAMIC_BITSET_HPP
#define  HAVE_BOOST 1
#endif
//#define HAVE_BOOST 1
//#define USE_STDVECBOOL 1
#ifdef HAVE_BOOST
#include "boost/dynamic_bitset.hpp"
#include <vector>
using boost::dynamic_bitset;
using std::vector;
#endif
#ifdef USE_STDVECBOOL
#include <vector>
using std::vector;
#endif
#include "kutil.h"
#include "kInline.cc"
#include "kstd1.h"
#include "kbuckets.h"




//#define TGB_DEBUG
#define FULLREDUCTIONS
#define HANS_IDEA
//#define HALFREDUCTIONS
//#define HEAD_BIN
//#define HOMOGENEOUS_EXAMPLE
#define REDTAIL_S
#define PAR_N 100
#define PAR_N_F4 5000
#define AC_NEW_MIN 2
#define AC_FLATTEN 1

//#define FIND_DETERMINISTIC
//#define REDTAIL_PROT
//#define QUICK_SPOLY_TEST
class PolySimple{
public:
  PolySimple(poly p){
    impl=p;
  }
  PolySimple(){
    impl=NULL;
  }
  PolySimple(const PolySimple& a){
    //impl=p_Copy(a.impl,currRing);
    impl=a.impl;
  }
  PolySimple& operator=(const PolySimple& p2){
    //p_Delete(&impl,currRing);
    //impl=p_Copy(p2.impl,currRing);
    impl=p2.impl;
    return *this;
  }
  ~PolySimple(){
    //p_Delete(&impl,currRing);
  }
  bool operator< (const PolySimple& other) const{
    return pLmCmp(impl,other.impl)<0;
  }
  bool operator==(const PolySimple& other){
    return pLmEqual(impl,other.impl);
  }
  poly impl;
  
};
class DataNoroCacheNode;
class MonRedRes{
public:
  poly p;
  number coef;
  BOOLEAN changed;
  int len;
  BOOLEAN onlyBorrowed;
  bool operator<(const MonRedRes& other) const{
    int cmp=p_LmCmp(p,other.p,currRing);
    if ((cmp<0)||((cmp==0)&&((onlyBorrowed)&&(!(other.onlyBorrowed))))){
      return true;
    } else return false;
  }
  DataNoroCacheNode* ref;
  MonRedRes(){
    ref=NULL;
    p=NULL;
  }
};
class MonRedResNP{
public:
  number coef;


  DataNoroCacheNode* ref;
  MonRedResNP(){
    ref=NULL;
  }
};
struct sorted_pair_node{
  //criterium, which is stable 0. small lcm 1. small i 2. small j
  wlen_type expected_length;
  poly lcm_of_lm;
  int i;
  int j;
  int deg;
 
  
};


class NoroPlaceHolder{
public:
  DataNoroCacheNode* ref;
  number coef;
};

//static ideal debug_Ideal;


struct poly_list_node{
  poly p;
  poly_list_node* next;
};

struct int_pair_node{
  int_pair_node* next;
  int a;
  int b;
};
struct monom_poly{
  poly m;
  poly f;
};
struct mp_array_list{
  monom_poly* mp;
  int size;
  mp_array_list* next;
};


struct poly_array_list{
  poly* p;
  int size;
  poly_array_list* next;
};
class slimgb_alg
{
  public:
    slimgb_alg(ideal I, int syz_comp,BOOLEAN F4);
		void introduceDelayedPairs(poly* pa,int s);
    virtual ~slimgb_alg();
    void cleanDegs(int lower, int upper);
#ifndef HAVE_BOOST
#ifdef USE_STDVECBOOL
  vector<vector<bool> > states;
#else
  char** states;
#endif
#else
  vector<dynamic_bitset<> > states;
#endif
  ideal add_later;
  ideal S;
  ring r;
  int* lengths;
  wlen_type* weighted_lengths;
  long* short_Exps;
  kStrategy strat;
  int* T_deg;
  int* T_deg_full;
  poly tmp_lm;
  poly* tmp_pair_lm;
  sorted_pair_node** tmp_spn;
  poly* expandS;
  poly* gcd_of_terms;
  int_pair_node* soon_free;
  sorted_pair_node** apairs;
  #if 0
  BOOLEAN* modifiedS;
  #endif
  #ifdef TGB_RESORT_PAIRS
  bool* replaced;
  #endif
  poly_list_node* to_destroy;
  //for F4
  mp_array_list* F;
  poly_array_list* F_minus;

  //end for F4
#ifdef HEAD_BIN
  struct omBin_s*   HeadBin;
#endif
  unsigned int reduction_steps;
  int n;
  //! array_lengths should be greater equal n;
  int syz_comp;
  int array_lengths; 
  int normal_forms;
  int current_degree;
  int Rcounter;
  int last_index;
  int max_pairs;
  int pair_top;
  int easy_product_crit;
  int extended_product_crit;
  int average_length;
  int lastDpBlockStart;
  int lastCleanedDeg;
  BOOLEAN isDifficultField;
  BOOLEAN completed;
  BOOLEAN is_homog;
  BOOLEAN tailReductions;
  BOOLEAN eliminationProblem;
  BOOLEAN F4_mode;
  BOOLEAN nc;
  #ifdef TGB_RESORT_PAIRS
  BOOLEAN used_b;
  #endif
};
class red_object{
 public:
  kBucket_pt bucket;
  poly p;
  unsigned long sev;
  int sugar;
  void flatten();
  void validate();
  wlen_type initial_quality;
  void adjust_coefs(number c_r, number c_ac_r);
  wlen_type guess_quality(slimgb_alg* c);
  int clear_to_poly();
  void canonicalize();
};


enum calc_state
  {
    UNCALCULATED,
    HASTREP//,
    //UNIMPORTANT,
    //SOONTREP
  };
static BOOLEAN pair_cmp(sorted_pair_node* a,sorted_pair_node* b);
template <class len_type, class set_type>  int pos_helper(kStrategy strat, poly p, len_type len, set_type setL, polyset set);
static int add_to_reductors(slimgb_alg* c, poly h, int len, int ecart, BOOLEAN simplified=FALSE);
static int bucket_guess(kBucket* bucket);
static poly redNFTail (poly h,const int sl,kStrategy strat, int len);
static poly redNF2 (poly h,slimgb_alg* c , int &len, number&  m,int n=0);
void free_sorted_pair_node(sorted_pair_node* s, ring r);
static void shorten_tails(slimgb_alg* c, poly monom);
static void replace_pair(int & i, int & j, slimgb_alg* c);
//static sorted_pair_node** add_to_basis(poly h, int i, int j,slimgb_alg* c, int* ip=NULL);
static void do_this_spoly_stuff(int i,int j,slimgb_alg* c);
//ideal t_rep_gb(ring r,ideal arg_I);
static BOOLEAN has_t_rep(const int & arg_i, const int & arg_j, slimgb_alg* state);
static int* make_connections(int from, poly bound, slimgb_alg* c);
static int* make_connections(int from, int to, poly bound, slimgb_alg* c);
void now_t_rep(const int & arg_i, const int & arg_j, slimgb_alg* c);
static void soon_t_rep(const int & arg_i, const int & arg_j, slimgb_alg* c);
static int pLcmDeg(poly a, poly b);
static int simple_posInS (kStrategy strat, poly p,int len, wlen_type wlen);
static BOOLEAN find_next_pair(slimgb_alg* c, BOOLEAN go_higher=TRUE);

static sorted_pair_node* pop_pair(slimgb_alg* c);
static BOOLEAN no_pairs(slimgb_alg* c);
void clean_top_of_pair_list(slimgb_alg* c);
static void super_clean_top_of_pair_list(slimgb_alg* c);
static BOOLEAN state_is(calc_state state, const int & i, const int & j, slimgb_alg* c);
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, slimgb_alg* c=NULL);
static int tgb_pair_better_gen(const void* ap,const void* bp);
static poly redTailShort(poly h, kStrategy strat);
static poly gcd_of_terms(poly p, ring r);
static BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, slimgb_alg* c);
static poly kBucketGcd(kBucket* b, ring r);
static void multi_reduction(red_object* los, int & losl, slimgb_alg* c);
int slim_nsize(number n, ring r);
sorted_pair_node* quick_pop_pair(slimgb_alg* c);
sorted_pair_node* top_pair(slimgb_alg* c);
sorted_pair_node** add_to_basis_ideal_quotient(poly h, slimgb_alg* c, int* ip);//, BOOLEAN new_pairs=TRUE);
sorted_pair_node**  spn_merge(sorted_pair_node** p, int pn,sorted_pair_node **q, int qn,slimgb_alg* c);
int kFindDivisibleByInS_easy(kStrategy strat,const red_object & obj);
int tgb_pair_better_gen2(const void* ap,const void* bp);
int kFindDivisibleByInS_easy(kStrategy strat,poly p, long sev);
//static int quality(poly p, int len, slimgb_alg* c);
/**
   makes on each red_object in a region a single_step
 **/
class reduction_step{
 public:
  /// we assume hat all occuring red_objects have same lm, and all
  /// occ. lm's in r[l...u] are the same, only reductor does not occur
  virtual void reduce(red_object* r, int l, int u);
  //int reduction_id;
  virtual ~reduction_step();
  slimgb_alg* c;
  int reduction_id;
};
class simple_reducer:public reduction_step{
 public:
  poly p;
  kBucket_pt fill_back;
  int p_len;
  int reducer_deg;
  simple_reducer(poly p, int p_len,int reducer_deg, slimgb_alg* c =NULL){
    this->p=p;
    this->reducer_deg=reducer_deg;
    assume(p_len==pLength(p));
    this->p_len=p_len;
    this->c=c;
  }
  virtual void pre_reduce(red_object* r, int l, int u);
  virtual void reduce(red_object* r, int l, int u);
  ~simple_reducer();


  virtual void do_reduce(red_object & ro);
};

//class sum_canceling_reducer:public reduction_step {
//  void reduce(red_object* r, int l, int u);
//};
struct find_erg{
  poly expand;
  int expand_length;
  int to_reduce_u;
  int to_reduce_l;
  int reduce_by;//index of reductor
  BOOLEAN fromS;//else from los

};

static void multi_reduce_step(find_erg & erg, red_object* r, slimgb_alg* c);
static void finalize_reduction_step(reduction_step* r);

template <class len_type, class set_type>  int pos_helper(kStrategy strat, poly p, len_type len, set_type setL, polyset set){
  //Print("POSHELER:%d",sizeof(wlen_type));
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
#ifdef NORO_CACHE
typedef unsigned short tgb_uint16;
typedef unsigned char tgb_uint8;
typedef unsigned int tgb_uint32;
class NoroCacheNode{
public:
  NoroCacheNode** branches;
  int branches_len;

  
  NoroCacheNode(){
    branches=NULL;
    branches_len=0;
    
  }
  NoroCacheNode* setNode(int branch, NoroCacheNode* node){
    if (branch>=branches_len){
      if (branches==NULL){
        branches_len=branch+1;
        branches_len=si_max(branches_len,3);
        branches=(NoroCacheNode**) omalloc(branches_len*sizeof(NoroCacheNode*));
        int i;
        for(i=0;i<branches_len;i++){
          branches[i]=NULL;
        }
      }else{
        int branches_len_old=branches_len;
        branches_len=branch+1;
        branches=(NoroCacheNode**) omrealloc(branches,branches_len*sizeof(NoroCacheNode*));
        int i;
        for(i=branches_len_old;i<branches_len;i++){
          branches[i]=NULL;
        }
      }
    }
    assume(branches[branch]==NULL);
    branches[branch]=node;
    return node;
  }
  NoroCacheNode* getBranch(int branch){
    if (branch<branches_len) return branches[branch];
    return NULL;
  }
  virtual ~NoroCacheNode(){
    int i;
    for(i=0;i<branches_len;i++){
      delete branches[i];
    }
    omfree(branches);
  }
  NoroCacheNode* getOrInsertBranch(int branch){
    if ((branch<branches_len)&&(branches[branch]))
      return branches[branch];
    else{
      return setNode(branch,new NoroCacheNode());
    }
  }
};
class DenseRow{
public:
  number* array;
  int begin;
  int end;
  DenseRow(){
    array=NULL;
  }
  ~DenseRow(){
    omfree(array);
  }
};
class SparseRow{
public:
  int* idx_array;
  number* coef_array;
  int len;
  SparseRow(){
    len=0;
    idx_array=NULL;
    coef_array=NULL;
  }
  SparseRow(int n){
    len=n;
    idx_array=(int*) omalloc(n*sizeof(int));
    coef_array=(number*) omalloc(n*sizeof(number));
  }
  ~SparseRow(){
    omfree(idx_array);
    omfree(coef_array);
  }
};

class DataNoroCacheNode:public NoroCacheNode{
public:
  
  int value_len;
  poly value_poly;
  #ifdef NORO_SPARSE_ROWS_PRE
  SparseRow* row;
  #else
  DenseRow* row;
  #endif
  int term_index;
  DataNoroCacheNode(poly p, int len){
    value_len=len;
    value_poly=p;
    row=NULL;
    term_index=-1;
  }
  #ifdef NORO_SPARSE_ROWS_PRE
  DataNoroCacheNode(SparseRow* row){
    if (row!=NULL)
      value_len=row->len;
    else
      value_len=0;
    value_poly=NULL;
    this->row=row;
    term_index=-1;
  }
  #endif
  ~DataNoroCacheNode(
  ){
    //p_Delete(&value_poly,currRing);
    if (row) delete row;
  }
};
class NoroCache{
public:
  poly temp_term;
  void evaluatePlaceHolder(number* row,std::vector<NoroPlaceHolder>& place_holders);
  void collectIrreducibleMonomials( std::vector<DataNoroCacheNode*>& res);
  void collectIrreducibleMonomials(int level,  NoroCacheNode* node, std::vector<DataNoroCacheNode*>& res);
  void evaluateRows();
  void evaluateRows(int level, NoroCacheNode* node);
#ifdef NORO_RED_ARRAY_RESERVER
  int reserved;
  poly* recursionPolyBuffer;
#endif
  static const int backLinkCode=-222;
  DataNoroCacheNode* insert(poly term, poly nf, int len){
    //assume(impl.find(p_Copy(term,currRing))==impl.end());
    //assume(len==pLength(nf));
    assume(npIsOne(p_GetCoeff(term,currRing)));
    if (term==nf){
      term=p_Copy(term,currRing);

      ressources.push_back(term);
      nIrreducibleMonomials++;
      return treeInsertBackLink(term);
      
    } else{
      
      if (nf){
        //nf=p_Copy(nf,currRing);
        assume(p_LmCmp(nf,term,currRing)==-1);
        ressources.push_back(nf);
      }
      return treeInsert(term,nf,len);
      
    }
    
    //impl[term]=std::pair<PolySimple,int> (nf,len);
  }
  #ifdef NORO_SPARSE_ROWS_PRE
  DataNoroCacheNode* insert(poly term, SparseRow* srow){
    //assume(impl.find(p_Copy(term,currRing))==impl.end());
    //assume(len==pLength(nf));

      return treeInsert(term,srow);
      
 
    //impl[term]=std::pair<PolySimple,int> (nf,len);
  }
  #endif
  DataNoroCacheNode* insertAndTransferOwnerShip(poly t, ring r){
    
    ressources.push_back(t);
    DataNoroCacheNode* res=treeInsertBackLink(t);
    res->term_index=nIrreducibleMonomials;
    nIrreducibleMonomials++;
    return res;
  }
  poly lookup(poly term, BOOLEAN& succ, int & len);
  DataNoroCacheNode* getCacheReference(poly term);
  NoroCache(){
    buffer=NULL;
#ifdef NORO_RED_ARRAY_RESERVER
    reserved=0;
    recursionPolyBuffer=(poly*)omalloc(1000000*sizeof(poly));
#endif
    nIrreducibleMonomials=0;
    nReducibleMonomials=0;
    temp_term=pOne();
  }
#ifdef NORO_RED_ARRAY_RESERVER
  poly* reserve(int n){
    poly* res=recursionPolyBuffer+reserved;
    reserved+=n;
    return res;
  }
  void free(int n){
    reserved-=n;
  }
#endif
  ~NoroCache(){
    int s=ressources.size();
    int i;
    for(i=0;i<s;i++){
      p_Delete(&ressources[i].impl,currRing);
    }
    p_Delete(&temp_term,currRing);
#ifdef NORO_RED_ARRAY_RESERVER
    omfree(recursionPolyBuffer);
#endif
  }
  
  int nIrreducibleMonomials;
  int nReducibleMonomials;
protected:
  DataNoroCacheNode* treeInsert(poly term,poly nf,int len){
    int i;
    nReducibleMonomials++;
    int nvars=pVariables;
    NoroCacheNode* parent=&root;
    for(i=1;i<nvars;i++){
      parent=parent->getOrInsertBranch(p_GetExp(term,i,currRing));
    }
    return (DataNoroCacheNode*) parent->setNode(p_GetExp(term,nvars,currRing),new DataNoroCacheNode(nf,len));
  }
  #ifdef NORO_SPARSE_ROWS_PRE
  DataNoroCacheNode* treeInsert(poly term,SparseRow* srow){
    int i;
    nReducibleMonomials++;
    int nvars=pVariables;
    NoroCacheNode* parent=&root;
    for(i=1;i<nvars;i++){
      parent=parent->getOrInsertBranch(p_GetExp(term,i,currRing));
    }
    return (DataNoroCacheNode*) parent->setNode(p_GetExp(term,nvars,currRing),new DataNoroCacheNode(srow));
  }
  #endif
  DataNoroCacheNode* treeInsertBackLink(poly term){
    int i;
    int nvars=pVariables;
    NoroCacheNode* parent=&root;
    for(i=1;i<nvars;i++){
      parent=parent->getOrInsertBranch(p_GetExp(term,i,currRing));
    }
    return (DataNoroCacheNode*) parent->setNode(p_GetExp(term,nvars,currRing),new DataNoroCacheNode(term,backLinkCode));
  }
  
  //@TODO descruct nodes;
  typedef std::vector<PolySimple> poly_vec;
  poly_vec ressources;
  //typedef std::map<PolySimple,std::pair<PolySimple,int> > cache_map;
  //cache_map impl;
  NoroCacheNode root;
  number* buffer;
};
MonRedResNP noro_red_mon_to_non_poly(poly t,  NoroCache* cache,slimgb_alg* c);
template<class storage_type> SparseRow* noro_red_to_non_poly_t(poly p, int &len, NoroCache* cache,slimgb_alg* c){
  assume(len==pLength(p));
  poly orig_p=p;
  if (p==NULL) {
    len=0;
    return NULL;
  }
  
  number zero=npInit(0);
  MonRedResNP* mon=(MonRedResNP*) omalloc(len*sizeof(MonRedResNP));
  int i=0;

  while(p){

    poly t=p;
    pIter(p);
    pNext(t)=NULL;
    
#ifndef NDEBUG
    number coef_debug=p_GetCoeff(t,currRing);
#endif
    MonRedResNP red=noro_red_mon_to_non_poly(t,cache,c);
    mon[i]=red;
    i++;
  }
  
  assume(i==len);
  len=i;
  //in the loop before nIrreducibleMonomials increases, so position here is important
  storage_type* temp_array=(storage_type*) omalloc(cache->nIrreducibleMonomials*sizeof(storage_type));
  int temp_size=cache->nIrreducibleMonomials;
  memset(temp_array,0,sizeof(storage_type)*cache->nIrreducibleMonomials);
  for(i=0;i<len;i++){
    MonRedResNP red=mon[i];
    if ((red.ref)){
      if (red.ref->row){
        SparseRow* row=red.ref->row;
        number coef=red.coef;
        int j;
        if (!(npIsOne(coef))){
        for(j=0;j<row->len;j++){
          int idx=row->idx_array[j];
          assume(!(npIsZero(coef)));
          assume(!(npIsZero(row->coef_array[j])));
          temp_array[idx]=(storage_type) (unsigned int) npAddM((number) temp_array[idx],npMultM(row->coef_array[j],coef));
          assume(idx<temp_size);
        }}else{
          for(j=0;j<row->len;j++){
            int idx=row->idx_array[j];
            temp_array[idx]=(storage_type) (unsigned int) npAddM((number) temp_array[idx],row->coef_array[j]);
            assume(idx<temp_size);
          }
        }
      }
      else{
        if (red.ref->value_len==NoroCache::backLinkCode){
          temp_array[red.ref->term_index]=(storage_type) (unsigned int) npAddM((number) temp_array[red.ref->term_index],red.coef);
        } else {
          //PrintS("third case\n");
        }
      }
    }
  }
  int non_zeros=0;
  for(i=0;i<cache->nIrreducibleMonomials;i++){
    if (!(temp_array[i]==0)){
      non_zeros++;
    }
  }
  SparseRow* res=new SparseRow(non_zeros);
  int pos=0;
  for(i=0;i<cache->nIrreducibleMonomials;i++){
    if (!(0==temp_array[i])){
    
      res->idx_array[pos]=i;
      res->coef_array[pos]=(number) temp_array[i];

      pos++;  
    }
    
  }
  omfree(temp_array);

  omfree(mon);
  return res;
}
#endif
static wlen_type pair_weighted_length(int i, int j, slimgb_alg* c);
wlen_type pELength(poly p, ring r);
int terms_sort_crit(const void* a, const void* b);
//void simplest_gauss_modp(number* a, int nrows,int ncols);
// a: a[0,0],a[0,1]....a[nrows-1,ncols-1]
// assume: field is Zp
#ifdef USE_NORO
#define slim_prec_cast(a) (unsigned int) (a)
#define F4mat_to_number_type(a) (number_type) slim_prec_cast(a)

template <class number_type > void write_poly_to_row(number_type* row, poly h, poly*terms, int tn, ring r){
  //poly* base=row;
  while(h!=NULL){
    //Print("h:%i\n",h);
    number coef=p_GetCoeff(h,r);
    poly* ptr_to_h=(poly*) bsearch(&h,terms,tn,sizeof(poly),terms_sort_crit);
    assume(ptr_to_h!=NULL);
    int pos=ptr_to_h-terms;
    row[pos]=F4mat_to_number_type(coef);
    //number_type_array[base+pos]=coef;
    pIter(h);
  }
}
template <class number_type > poly row_to_poly(number_type* row, poly* terms, int tn, ring r){
  poly h=NULL;
  int j;
  number_type zero=0;//;npInit(0);
  for(j=tn-1;j>=0;j--){
    if (!(zero==(row[j]))){
      poly t=terms[j];
      t=p_LmInit(t,r);
      p_SetCoeff(t,(number) row[j],r);
      pNext(t)=h;
      h=t;
    }
    
  }
  return h;
}
template <class number_type > int modP_lastIndexRow(number_type* row,int ncols){
  int lastIndex;
  const number_type zero=0;//npInit(0);
  for(lastIndex=ncols-1;lastIndex>=0;lastIndex--){
    if (!(row[lastIndex]==zero)){
      return lastIndex;
    }
  }
  return -1;
}
template <class number_type>class ModPMatrixBackSubstProxyOnArray;
template <class number_type > class ModPMatrixProxyOnArray{
public:
  friend class ModPMatrixBackSubstProxyOnArray<number_type>;
               
  int ncols,nrows;
  ModPMatrixProxyOnArray(number_type* array, int nrows, int ncols){
    this->ncols=ncols;
    this->nrows=nrows;
    rows=(number_type**) omalloc(nrows*sizeof(number_type*));
    startIndices=(int*)omalloc(nrows*sizeof(int));
    int i;
    for(i=0;i<nrows;i++){
      rows[i]=array+(i*ncols);
      updateStartIndex(i,-1);
    }
  }
  ~ModPMatrixProxyOnArray(){
    omfree(rows);
    omfree(startIndices);
  }
  
  void permRows(int i, int j){
    number_type* h=rows[i];
    rows[i]=rows[j];
    rows[j]=h;
    int hs=startIndices[i];
    startIndices[i]=startIndices[j];
    startIndices[j]=hs;
  }
  void multiplyRow(int row, number_type coef){
    int i;
    number_type* row_array=rows[row];
    for(i=startIndices[row];i<ncols;i++){
      row_array[i]=F4mat_to_number_type(npMult((number) row_array[i],(number) coef));
    }
  }
  void reduceOtherRowsForward(int r){

    //assume rows "under r" have bigger or equal start index
    number_type* row_array=rows[r];
    number_type zero=F4mat_to_number_type(npInit(0));
    int start=startIndices[r];
    number_type coef=row_array[start];
    assume(start<ncols);
    int other_row;
    assume(!(npIsZero((number) row_array[start])));
    if (!(npIsOne((number) coef)))
      multiplyRow(r,F4mat_to_number_type(npInvers((number) coef)));
    assume(npIsOne((number) row_array[start]));
    int lastIndex=modP_lastIndexRow(row_array, ncols);
    number minus_one=npInit(-1);
    for (other_row=r+1;other_row<nrows;other_row++){
      assume(startIndices[other_row]>=start);
      if (startIndices[other_row]==start){
        int i;
        number_type* other_row_array=rows[other_row];
        number coef2=npNeg((number) other_row_array[start]);
        if (coef2==minus_one){
          for(i=start;i<=lastIndex;i++){
            if (row_array[i]!=zero)
              other_row_array[i]=F4mat_to_number_type(npSubM((number) other_row_array[i], (number) row_array[i]));
          }
      }else {
          //assume(FALSE);
          for(i=start;i<=lastIndex;i++){
            if (row_array[i]!=zero)
            other_row_array[i]=F4mat_to_number_type(npAddM(npMult(coef2,(number) row_array[i]),(number) other_row_array[i]));
          }
        }
        updateStartIndex(other_row,start);
        assume(npIsZero((number) other_row_array[start]));
      }
    }
  }
  void updateStartIndex(int row,int lower_bound){
    number_type* row_array=rows[row];
    assume((lower_bound<0)||(npIsZero((number) row_array[lower_bound])));
    int i;
    //number_type zero=npInit(0);
    for(i=lower_bound+1;i<ncols;i++){
      if (!(row_array[i]==0))
        break;
    }
    startIndices[row]=i;
  }
  int getStartIndex(int row){
    return startIndices[row];
  }
  BOOLEAN findPivot(int &r, int &c){
    //row>=r, col>=c
    
    while(c<ncols){
      int i;
      for(i=r;i<nrows;i++){
        assume(startIndices[i]>=c);
        if (startIndices[i]==c){
          //r=i;
          if (r!=i)
            permRows(r,i);
          return TRUE;
        }
      }
      c++;
    }
    return FALSE;
  }
protected:
  number_type** rows;
  int* startIndices;
};
template <class number_type > class ModPMatrixBackSubstProxyOnArray{
  int *startIndices;
  number_type** rows;
  int *lastReducibleIndices;
  int ncols;
  int nrows;
  int nonZeroUntil;
public:
  void multiplyRow(int row, number_type coef){
    int i;
    number_type* row_array=rows[row];
    for(i=startIndices[row];i<ncols;i++){
      row_array[i]=F4mat_to_number_type(npMult((number) row_array[i],(number) coef));
    }
  }
  ModPMatrixBackSubstProxyOnArray<number_type> (ModPMatrixProxyOnArray<number_type> & p){
//  (number_type* array, int nrows, int ncols, int* startIndices, number_type** rows){
    //we borrow some parameters ;-)
    //we assume, that nobody changes the order of the rows
    this->startIndices=p.startIndices;
    this->rows=p.rows;
    this->ncols=p.ncols;
    this->nrows=p.nrows;
    lastReducibleIndices=(int*) omalloc(nrows*sizeof(int));
    nonZeroUntil=0;
    while(nonZeroUntil<nrows){
      if (startIndices[nonZeroUntil]<ncols){
       
        nonZeroUntil++;
      } else break;
      
    }
    if (TEST_OPT_PROT)
      Print("rank:%i\n",nonZeroUntil);
    nonZeroUntil--;
    int i;
    for(i=0;i<=nonZeroUntil;i++){
      assume(startIndices[i]<ncols);
      assume(!(npIsZero((number) rows[i][startIndices[i]])));
      assume(startIndices[i]>=i);
      updateLastReducibleIndex(i,nonZeroUntil+1);
    }
  }
  void updateLastReducibleIndex(int r, int upper_bound){
    number_type* row_array=rows[r];
    if (upper_bound>nonZeroUntil) upper_bound=nonZeroUntil+1;
    int i;
    const number_type zero=0;//npInit(0);
    for(i=upper_bound-1;i>r;i--){
      int start=startIndices[i];
      assume(start<ncols);
      if (!(row_array[start]==zero)){
        lastReducibleIndices[r]=start;
        return;
      }
    }
    lastReducibleIndices[r]=-1;
  }
  void backwardSubstitute(int r){
    int start=startIndices[r];
    assume(start<ncols);
    number_type zero=0;//npInit(0);
    number_type* row_array=rows[r];
    assume((!(npIsZero((number) row_array[start]))));
    assume(start<ncols);
    int other_row;
    if (!(npIsOne((number) row_array[r]))){
      //it should be one, but this safety is not expensive
      multiplyRow(r, F4mat_to_number_type(npInvers((number) row_array[start])));
    }
    int lastIndex=modP_lastIndexRow(row_array, ncols);
    assume(lastIndex<ncols);
    assume(lastIndex>=0);
    for(other_row=r-1;other_row>=0;other_row--){
      assume(lastReducibleIndices[other_row]<=start);
      if (lastReducibleIndices[other_row]==start){
        number_type* other_row_array=rows[other_row];
        number coef=npNeg((number) other_row_array[start]);
        assume(!(npIsZero(coef)));
        int i;
        assume(start>startIndices[other_row]);
        for(i=start;i<=lastIndex;i++){
          if (row_array[i]!=zero)
            other_row_array[i]=F4mat_to_number_type(npAddM(npMult(coef,(number)row_array[i]),(number)other_row_array[i]));
        }
        updateLastReducibleIndex(other_row,r);
      }
    }
  }
  ~ModPMatrixBackSubstProxyOnArray<number_type>(){
    omfree(lastReducibleIndices);
  }
  void backwardSubstitute(){
    int i;
    for(i=nonZeroUntil;i>0;i--){
      backwardSubstitute(i);
    }
  }
};
template <class number_type > void simplest_gauss_modp(number_type* a, int nrows,int ncols){
  //use memmoves for changing rows
  if (TEST_OPT_PROT)
    PrintS("StartGauss\n");
  ModPMatrixProxyOnArray<number_type> mat(a,nrows,ncols);
  
  int c=0;
  int r=0;
  while(mat.findPivot(r,c)){
    //int pivot=find_pivot()
      mat.reduceOtherRowsForward(r);
    r++;
    c++;
  }
  ModPMatrixBackSubstProxyOnArray<number_type> backmat(mat);
  backmat.backwardSubstitute();
  //backward substitutions
  if (TEST_OPT_PROT)
    PrintS("StopGauss\n");
}
#endif

#endif
