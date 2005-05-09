#ifndef TGBGAUSS_HEADER
#define TGBGAUSS_HEADER
#include "mod2.h"
#include "numbers.h"
#include "tgb_internal.h"
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
class tgb_sparse_matrix{
 private:
  ring r;
  mac_poly* mp;
  int columns;
  int rows;
  BOOLEAN free_numbers;
 public:
  void sort_rows();
  friend poly free_row_to_poly(tgb_sparse_matrix* mat, int row, poly* monoms, int monom_index);
  friend void init_with_mac_poly(tgb_sparse_matrix* mat, int row, mac_poly m);
  tgb_sparse_matrix(int i, int j, ring rarg);
  ~tgb_sparse_matrix();
  int get_rows();
  int get_columns();
  void print();
  void row_normalize(int row);
  void row_content(int row);
  //  void perm_rows(int i, int j);
  void perm_rows(int i, int j){
  mac_poly h;
  h=mp[i];
  mp[i]=mp[j];
  mp[j]=h;
  }
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
void simple_gauss(tgb_sparse_matrix* mat, calc_dat* c);
void simple_gauss2(tgb_matrix* mat);
#endif
