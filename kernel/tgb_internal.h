#ifndef TGB_INTERNAL_H
#define TGB_INTERNAL_H
//!\file tgb_internal.h
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tgb_internal.h,v 1.20 2005-11-24 11:28:29 bricken Exp $ */
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
struct sorted_pair_node{
  //criterium, which is stable 0. small lcm 1. small i 2. small j
  int i;
  int j;
  int deg;
  int expected_length;
  poly lcm_of_lm;
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
    slimgb_alg(ideal I, BOOLEAN F4);
    virtual ~slimgb_alg();
  char** states;
  ideal S;
  ring r;
  int* lengths;
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
  BOOLEAN* modifiedS;
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
  BOOLEAN is_char0;
  BOOLEAN is_homog;
  BOOLEAN doubleSugar;
  BOOLEAN F4_mode;
  BOOLEAN nc;
};
class red_object{
 public:
  kBucket_pt bucket;
  poly p;
  unsigned long sev;
  void flatten();
  void validate();
  void adjust_coefs(number c_r, number c_ac_r);
  wlen_type guess_quality(slimgb_alg* c);
  int clear_to_poly();
  void canonicalize();
};


enum calc_state
  {
    UNCALCULATED,
    HASTREP,
    UNIMPORTANT,
    SOONTREP
  };
template <class len_type, class set_type>  int pos_helper(kStrategy strat, poly p, len_type len, set_type setL, polyset set);
static int add_to_reductors(slimgb_alg* c, poly h, int len);
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
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, slimgb_alg* c);
static int tgb_pair_better_gen(const void* ap,const void* bp);
static poly redTailShort(poly h, kStrategy strat);
static poly gcd_of_terms(poly p, ring r);
static BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, slimgb_alg* c);
static poly kBucketGcd(kBucket* b, ring r);
static void multi_reduction(red_object* los, int & losl, slimgb_alg* c);
sorted_pair_node* quick_pop_pair(slimgb_alg* c);
sorted_pair_node* top_pair(slimgb_alg* c);
sorted_pair_node** add_to_basis_ideal_quotient(poly h, int i_pos, int j_pos,slimgb_alg* c, int* ip);
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
  simple_reducer(poly p, int p_len, slimgb_alg* c =NULL){
    this->p=p;
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





#endif
