#ifndef TGB_H
#define TGB_H
//#define OM_CHECK 3
//#define OM_TRACK 5
#include "mod2.h"
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
#include "tok.h"
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

ideal t_rep_gb(ring r,ideal arg_I, BOOLEAN F4_mode=FALSE);
//static ideal debug_Ideal;
/** 
    reduction_accumulators are objects which are shared by several sums
 **/

class reduction_accumulator{
  
 public:
  /// (1/multiplied)*bucket=reduced original data
  number multiplied;
  ///the polynomial data
  kBucket_pt bucket;
  /// the short exponent vector
  unsigned long sev;
  /// the reference counter
  int counter;
  /// decrease the reference counter, at 0 it deletes the object
  void decrease_counter(){ 
    if((--counter)==0)
      {
	delete this; //self destruction
      }
  }
  int last_reduction_id;
  reduction_accumulator(poly p, int p_len, poly high_to);
  ~reduction_accumulator(){
    nDelete(&multiplied);
    kBucketDeleteAndDestroy(&bucket);
  }  

  
};
struct poly_list_node{
  poly p;
  poly_list_node* next;
};
struct formal_sum_descriptor{
  number c_my;
  number c_ac;
  reduction_accumulator* ac;
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
class tgb_matrix{
 private:
  number** n;
  int columns;
  int rows;
  BOOLEAN free_numbers;
 public:
  tgb_matrix(int i, int j);
  ~tgb_matrix();
  int get_rows();
  int get_columns();
  void print();
  void perm_rows(int i, int j);
  void set(int i, int j, number n);
  number get(int i, int j);
  BOOLEAN is_zero_entry(int i, int j);
  void free_row(int row, BOOLEAN free_non_zeros=TRUE);
  int min_col_not_zero_in_row(int row);
  int next_col_not_zero(int row,int pre);
  BOOLEAN zero_row(int row);
  void mult_row(int row,number factor);
  void add_lambda_times_row(int add_to,int summand,number factor);
  int non_zero_entries(int row);
};
class mac_poly_r{
public:
  number coef;
  mac_poly_r* next;
  int exp;
  mac_poly_r():next(NULL){}
};
//mac_polys exp are smaller iff they are greater by monomial ordering
//corresponding to solving linear equations notation

typedef mac_poly_r* mac_poly;
class tgb_sparse_matrix{
 private:
  mac_poly* mp;
  int columns;
  int rows;
  BOOLEAN free_numbers;
 public:
  friend poly free_row_to_poly(tgb_sparse_matrix* mat, int row, poly* monoms, int monom_index);
  friend void init_with_mac_poly(tgb_sparse_matrix* mat, int row, mac_poly m);
  tgb_sparse_matrix(int i, int j);
  ~tgb_sparse_matrix();
  int get_rows();
  int get_columns();
  void print();
  void perm_rows(int i, int j);
  void set(int i, int j, number n);
  number get(int i, int j);
  BOOLEAN is_zero_entry(int i, int j);
  void free_row(int row, BOOLEAN free_non_zeros=TRUE);
  int min_col_not_zero_in_row(int row);
  int next_col_not_zero(int row,int pre);
  BOOLEAN zero_row(int row);
  void mult_row(int row,number factor);
  void add_lambda_times_row(int add_to,int summand,number factor);
  int non_zero_entries(int row);
};
struct poly_array_list{
  poly* p;
  int size;
  poly_array_list* next;
};
struct calc_dat
{
  int* rep;
  char** states;
  ideal S;
  ring r;
  int* lengths;
  long* short_Exps;
  kStrategy strat;
  int* T_deg;
  poly tmp_lm;
  poly* tmp_pair_lm;
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
  BOOLEAN F4_mode;
};
class red_object{
 public:
  kBucket_pt bucket;
  poly p;
  formal_sum_descriptor* sum;
  unsigned long sev;
  void flatten();
  void validate();
  void adjust_coefs(number c_r, number c_ac_r);
  int guess_quality(calc_dat* c);
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

static int add_to_reductors(calc_dat* c, poly h, int len);
static int bucket_guess(kBucket* bucket);
static poly redNFTail (poly h,const int sl,kStrategy strat, int len);
static poly redNF2 (poly h,calc_dat* c , int &len, number&  m,int n=0);
static void free_sorted_pair_node(sorted_pair_node* s, ring r);
static void shorten_tails(calc_dat* c, poly monom);
static void replace_pair(int & i, int & j, calc_dat* c);
static sorted_pair_node** add_to_basis(poly h, int i, int j,calc_dat* c, int* ip=NULL);
static void do_this_spoly_stuff(int i,int j,calc_dat* c);
//ideal t_rep_gb(ring r,ideal arg_I);
static BOOLEAN has_t_rep(const int & arg_i, const int & arg_j, calc_dat* state);
static int* make_connections(int from, poly bound, calc_dat* c);
static int* make_connections(int from, int to, poly bound, calc_dat* c);
static void now_t_rep(const int & arg_i, const int & arg_j, calc_dat* c);
static void soon_t_rep(const int & arg_i, const int & arg_j, calc_dat* c);
static int pLcmDeg(poly a, poly b);
static int simple_posInS (kStrategy strat, poly p,int len, BOOLEAN is_char0);
static BOOLEAN find_next_pair(calc_dat* c, BOOLEAN go_higher=TRUE);

static sorted_pair_node* pop_pair(calc_dat* c);
static BOOLEAN no_pairs(calc_dat* c);
static void clean_top_of_pair_list(calc_dat* c);
static void super_clean_top_of_pair_list(calc_dat* c);
static BOOLEAN state_is(calc_state state, const int & i, const int & j, calc_dat* c);
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, calc_dat* c);
static int pair_better_gen(const void* ap,const void* bp);
static poly redTailShort(poly h, kStrategy strat);
static poly gcd_of_terms(poly p, ring r);
static BOOLEAN extended_product_criterion(poly p1, poly gcd1, poly p2, poly gcd2, calc_dat* c);
static poly kBucketGcd(kBucket* b, ring r);
static void multi_reduction(red_object* los, int & losl, calc_dat* c);
static sorted_pair_node* quick_pop_pair(calc_dat* c);
static sorted_pair_node* top_pair(calc_dat* c);
static int quality(poly p, int len, calc_dat* c);
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
  calc_dat* c;
  int reduction_id;
};
class simple_reducer:public reduction_step{
 public:
  poly p;
  kBucket_pt fill_back;
  int p_len;
  simple_reducer(poly p, int p_len, calc_dat* c =NULL){
    this->p=p;
    this->p_len=p_len;
    this->c=c;
  }
  virtual void pre_reduce(red_object* r, int l, int u);
  virtual void reduce(red_object* r, int l, int u);
  ~simple_reducer();

  virtual void target_is_a_sum_reduce(red_object & ro);
  virtual void target_is_no_sum_reduce(red_object & ro);
};
class join_simple_reducer:public simple_reducer{
 public:
  join_simple_reducer(poly p, int p_len, poly high_to):simple_reducer(p,p_len){
    ac=new reduction_accumulator( p, p_len, high_to);
   
    
  }
    ~join_simple_reducer();
   void pre_reduce(red_object* r, int l, int u);
  void target_is_no_sum_reduce(red_object & ro);
  reduction_accumulator* ac;
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

static void multi_reduce_step(find_erg & erg, red_object* r, calc_dat* c);
static void finalize_reduction_step(reduction_step* r);

#endif
