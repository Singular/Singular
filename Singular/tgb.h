#ifndef TGB_H
#define TGB_H

#include "mod2.h"
#include <omalloc.h>
#include "p_polys.h"
//#include "prCopy.h"
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

#define FULLREDUCTIONS
//#define HALFREDUCTIONS
#define HEAD_BIN
//#define HOMOGENEOUS_EXAMPLE
#define REDTAIL_S
#define PAR_N 1
//#define REDTAIL_PROT
//#define QUICK_SPOLY_TEST
//#define DIAGONAL_GOING
//#define RANDOM_WALK
struct int_pair_node{
  int_pair_node* next;
  int a;
  int b;
};
struct sorted_pair_node{
  //criterium, which is stable 0. small lcm 1. small i 2. small j
  sorted_pair_node* next;
  int i;
  int j;
  int deg;
  int expected_length;
  poly lcm_of_lm;
};

struct redNF_inf{
  poly h;
  LObject* P;
  int_pair_node* pending;
  int_pair_node* soon_free;
  int len_upper_bound;
  int i;
  int j;
  BOOLEAN need_std_rep;
  BOOLEAN is_free;
  BOOLEAN started;
  
};
enum calc_state
  {
    UNCALCULATED,
    HASTREP,
    UNIMPORTANT,
    SOONTREP
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
  int** deg;
  int* T_deg;
  int* misses;
  int_pair_node* soon_free;
  sorted_pair_node* pairs;
  sorted_pair_node** apairs;
  redNF_inf* work_on;
#ifdef HEAD_BIN
  struct omBin_s*   HeadBin;
#endif
  unsigned int reduction_steps;
  int max_misses;
  int found_i;
  int found_j;
  int continue_i;
  int continue_j;
  int n;
  int skipped_i;
  int normal_forms;
  int skipped_pairs;
  int current_degree;
  int misses_counter;
  int misses_series;
  int Rcounter;
  int last_index;
  int max_pairs;
  int pair_top;
};
static poly redNFTail (poly h,const int sl,kStrategy strat, int len);
static poly redNF2 (poly h,calc_dat* c , int &len);
static void free_sorted_pair_node(sorted_pair_node* s, ring r);
static void shorten_tails(calc_dat* c, poly monom);
static void replace_pair(int & i, int & j, calc_dat* c);
static void initial_data(calc_dat* c);
static void add_to_basis(poly h, int i, int j,calc_dat* c);
static void do_this_spoly_stuff(int i,int j,calc_dat* c);
ideal t_rep_gb(ring r,ideal arg_I);
static BOOLEAN has_t_rep(const int & arg_i, const int & arg_j, calc_dat* state);
static int* make_connections(int from, poly bound, calc_dat* c);
static int* make_connections(int from, int to, poly bound, calc_dat* c);
static void now_t_rep(const int & arg_i, const int & arg_j, calc_dat* c);
static void soon_t_rep(const int & arg_i, const int & arg_j, calc_dat* c);
static int pLcmDeg(poly a, poly b);
static int simple_posInS (kStrategy strat, poly p,int len);
static BOOLEAN find_next_pair(calc_dat* c, BOOLEAN go_higher=TRUE);
static void soon_free_them(redNF_inf* inf, calc_dat* c);
static sorted_pair_node* pop_pair(calc_dat* c);
static BOOLEAN no_pairs(calc_dat* c);
static void clean_top_of_pair_list(calc_dat* c);
static BOOLEAN state_is(calc_state state, const int & i, const int & j, calc_dat* c);
static BOOLEAN pair_better(sorted_pair_node* a,sorted_pair_node* b, calc_dat* c);
static void sort_pair_in(int i, int j,calc_dat* c);
static int pair_better_gen(const void* ap,const void* bp);
static poly redTailShort(poly h, kStrategy strat);
#endif
