/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include "kernel/mod2.h"

#include "factory/factory.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "coeffs/longrat.h" // snumber ...

#include "polys/monomials/ring.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"

#include "interpolation.h"

// parameters to debug
//#define shmat
//#define checksize
//#define writemsg

// possible strategies
#define unsortedmatrix
//#define integerstrategy

#define modp_number int
#define exponent int

STATIC_VAR modp_number myp;  // all modp computation done mod myp
STATIC_VAR int myp_index; // index of small prime in Singular table of primes

static inline modp_number modp_mul (modp_number x,modp_number y)
{
//    return (x*y)%myp;
   return (modp_number)(((unsigned long)x)*((unsigned long)y)%((unsigned long)myp));
}
static inline modp_number modp_sub (modp_number x,modp_number y)
{
//   if (x>=y) return x-y; else return x+myp-y;
     return (x+myp-y)%myp;
}

typedef exponent *mono_type;
typedef struct {mono_type mon; unsigned int point_ref;} condition_type;
typedef modp_number *coordinate_products;
typedef coordinate_products *coordinates;

struct mon_list_entry_struct {mono_type mon;
                              struct mon_list_entry_struct *next;};
typedef struct mon_list_entry_struct mon_list_entry;

struct row_list_entry_struct {modp_number *row_matrix;
                              modp_number *row_solve;
                              int first_col;
                              struct row_list_entry_struct *next;};

typedef struct row_list_entry_struct row_list_entry;

struct generator_struct {modp_number *coef;
                         mono_type lt;
                         modp_number ltcoef;
                         struct generator_struct *next;};

typedef struct generator_struct generator_entry;

struct modp_result_struct {modp_number p;
                           generator_entry *generator;
                           int n_generators;
                           struct modp_result_struct *next;
                           struct modp_result_struct *prev;};

typedef struct modp_result_struct modp_result_entry;

typedef modp_number *modp_coordinates;
typedef mpq_t *q_coordinates;
typedef mpz_t *int_coordinates;
typedef bool *coord_exist_table;

STATIC_VAR int final_base_dim;    // dimension of the quotient space, known from the beginning
STATIC_VAR int last_solve_column;  // last non-zero column in "solve" part of matrix, used for speed up
STATIC_VAR int n_points;  // modp_number of ideals (points)
STATIC_VAR int *multiplicity;  // multiplicities of points
STATIC_VAR int variables;  // modp_number of variables
STATIC_VAR int max_coord;  // maximal possible coordinate product used during Evaluation
STATIC_VAR bool only_modp;  // perform only one modp computations

STATIC_VAR modp_coordinates *modp_points; // coordinates of points for modp problem - used by Evaluate (this is also initial data for only modp)
STATIC_VAR q_coordinates *q_points; // coordinates of points for rational data (not used for modp)
STATIC_VAR int_coordinates *int_points; // coordinates of points for integer data - used to check generators (not used for modp)
STATIC_VAR coord_exist_table *coord_exist; // checks whether all coordinates has been initialized
STATIC_VAR mon_list_entry *check_list; // monomials to be checked in next stages
STATIC_VAR coordinates *points; // power products of coordinates of points used in modp cycles
STATIC_VAR condition_type *condition_list; // conditions stored in an array
STATIC_VAR mon_list_entry *lt_list; // leading terms found so far
STATIC_VAR mon_list_entry *base_list; // standard monomials found so far
STATIC_VAR row_list_entry *row_list; // rows of the matrix (both parts)
STATIC_VAR modp_number *my_row; // one special row to perform operations
STATIC_VAR modp_number *my_solve_row; // one special row to find the linear dependence ("solve" part)
STATIC_VAR mono_type *column_name; // monomials assigned to columns in solve_row

STATIC_VAR int n_results;  // modp_number of performed modp computations (not discarded)
STATIC_VAR modp_number modp_denom; // denominator of mod p computations
STATIC_VAR modp_result_entry *modp_result; // list of results for various mod p calculations (used for modp - first result is the desired one)
STATIC_VAR modp_result_entry *cur_result; // pointer to current result (as before)
STATIC_VAR modp_number *congr; // primes used in computations (chinese remainder theorem) (not used for modp)
STATIC_VAR modp_number *in_gamma; // inverts used in chinese remainder theorem (not used for modp)
STATIC_VAR mpz_t bigcongr; // result, in fact, is given in mod bigcongr (not used for modp)

STATIC_VAR mpz_t *polycoef; // polynomial integercoefficients (not used for modp)
STATIC_VAR mono_type *polyexp; // polynomial exponents

struct gen_list_struct {mpz_t *polycoef;
                        mono_type *polyexp;
                        struct gen_list_struct *next;};
typedef struct gen_list_struct gen_list_entry;

STATIC_VAR gen_list_entry *gen_list=NULL; // list of resulting generators - output data (integer version)

STATIC_VAR int generic_n_generators; // modp_number of generators - should be the same for all modp comp (not used for modp)
STATIC_VAR mono_type *generic_column_name; // monomials assigned to columns in solve_row - should be the same for all modp comp (!!! used for modp)
STATIC_VAR mon_list_entry *generic_lt=NULL; // leading terms for ordered generators - should be the same for all modp comp (not used for modp)
STATIC_VAR int good_primes; // modp_number of good primes so far;
STATIC_VAR int bad_primes; // modp_number of bad primes so far;
STATIC_VAR mpz_t common_denom; // common denominator used to force points coordinates to Z (not used for modp)
STATIC_VAR bool denom_divisible; // common denominator is divisible by p (not used for modp)

STATIC_VAR poly comparizon_p1;  //polynomials used to do comparizons by Singular
STATIC_VAR poly comparizon_p2;

STATIC_VAR modp_number *modp_Reverse; // reverses in mod p

STATIC_VAR bool protocol; // true to show the protocol

#ifdef checksize
STATIC_VAR int maximal_size=0;
#endif

#if 0  /* only for debuggig*/
void WriteMono (mono_type m) // Writes a monomial on the screen - only for debug
{
     int i;
     for (i=0;i<variables;i++) Print("_%d", m[i]);
     PrintS(" ");
}

void WriteMonoList (mon_list_entry *list)
{
     mon_list_entry *ptr;
     ptr=list;
     while (ptr!=NULL)
     {
           WriteMono(ptr->mon);
           ptr=ptr->next;
     }
}

void Info ()
{
     int i;
     row_list_entry *curptr;
     modp_number *row;
     modp_number *solve;
     char ch;
     cout << endl << "quotient basis: ";
     WriteMonoList (base_list);
     cout << endl << "leading terms: ";
     WriteMonoList (lt_list);
     cout << endl << "to be checked: ";
     WriteMonoList (check_list);
#ifdef shmat
     cout << endl << "Matrix:" << endl;
     cout << "                                      ";
     for (i=0;i<final_base_dim;i++)
     {
         WriteMono (column_name[i]);
     }
     cout << endl;
     curptr=row_list;
     while (curptr!=NULL)
     {
           row=curptr->row_matrix;
           solve=curptr->row_solve;
           for (i=0;i<final_base_dim;i++)
           {
               cout << *row << " , ";
               row++;
           }
           cout << "        ";
           for (i=0;i<final_base_dim;i++)
           {
               cout << *solve << " , ";
               solve++;
           }
           curptr=curptr->next;
           cout << endl;}
     cout << "Special row:                           Solve row:" << endl;
     row=my_row;
     for (i=0;i<final_base_dim;i++)
     {
         cout << *row << " , ";
         row++;
     }
     cout << "          ";
     row=my_solve_row;
     for (i=0;i<final_base_dim;i++)
     {
         cout << *row << " , ";
         row++;
     }
#endif
     cout << endl;
     cin >> ch;
     cout << endl << endl;
}
#endif

static modp_number OneInverse(modp_number a,modp_number p) // computes inverse of d mod p without using tables
{
   long  u, v, u0, u1, u2, q, r;
   u1=1; u2=0;
   u = a; v = p;
   while (v != 0)
   {
      q = u / v;
      r = u % v;
      u = v;
      v = r;
      u0 = u2;
      u2 = u1 - q*u2;
      u1 = u0;
   }
   if (u1 < 0) u1=u1+p;
// now it should be ok, but for any case...
   if ((u1<0)||((u1*a)%p!=1))
   {
     PrintS("?");
     modp_number i;
     for (i=1;i<p;i++)
     {
       if ((a*i)%p==1) return i;
     }
   }
   return (modp_number)u1;
}

static int CalcBaseDim () // returns the maximal (expected) dimension of quotient space
{
    int c;
    int i,j;
    int s=0;
    max_coord=1;
    for (i=0;i<n_points;i++) max_coord=max_coord+multiplicity[i];
    for (j=0;j<n_points;j++)
    {
        c=1;
        for (i=1;i<=variables;i++)
        {
            c=(c*(multiplicity[j]+i-1))/i;
        }
        s=s+c;
    }
    return s;
}

static bool EqualMon (mono_type m1, mono_type m2)  // compares two monomials, true if equal, false otherwise
{
     int i;
     for (i=0;i<variables;i++)
         if (m1[i]!=m2[i]) return false;;
     return true;
}

static exponent MonDegree (mono_type mon)  // computes the degree of a monomial
{
     exponent v=0;
     int i;
     for (i=0;i<variables;i++) v=v+mon[i];
     return v;
}

static bool Greater (mono_type m1, mono_type m2) // true if m1 > m2, false otherwise. uses Singular comparing function
{
     for (int j = variables; j; j--)
     {
       pSetExp(comparizon_p1, j, m1[j-1]);
       pSetExp(comparizon_p2, j, m2[j-1]);
     }
     pSetm(comparizon_p1);
     pSetm(comparizon_p2);
     bool res=(pLmCmp(comparizon_p1,comparizon_p2)>0);
     return res;
}

static mon_list_entry* MonListAdd (mon_list_entry *list, mono_type mon)  // adds one entry to the list of monomials structure
{
     mon_list_entry *curptr=list;
     mon_list_entry *prevptr=NULL;
     mon_list_entry *temp;

     while (curptr!=NULL)
     {
           if (EqualMon (mon,(*curptr).mon)) return list;
           if (Greater ((*curptr).mon,mon)) break;
           prevptr=curptr;
           curptr=curptr->next;
     }
     temp=(mon_list_entry*)omAlloc0(sizeof(mon_list_entry));
     (*temp).next=curptr;
     (*temp).mon=(exponent*)omAlloc(sizeof(exponent)*variables);
     memcpy(temp->mon,mon,sizeof(exponent)*variables);
     if (prevptr==NULL) return temp;
     else
     {
          prevptr->next=temp;
          return list;
     }
}

static mono_type MonListElement (mon_list_entry *list, int n)  // returns ith element from list of monomials - no error checking!
{
     mon_list_entry *cur=list;
     int i;
     for (i=0;i<n;i++) cur=cur->next;
     return cur->mon;
}

static mono_type ZeroMonomial ()  // allocates memory for new monomial with all enries equal 0
{
     mono_type m;
     m=(exponent*)omAlloc0(sizeof(exponent)*variables);
     return m;
}

static void GeneralInit ()  // general initialization
{
     int i,j;
     points=(coordinates*)omAlloc(sizeof(coordinates)*n_points);
     for (i=0;i<n_points;i++)
     {
         points[i]=(coordinate_products*)omAlloc(sizeof(coordinate_products)*variables);
         for (j=0;j<variables;j++) points[i][j]=(modp_number*)omAlloc0(sizeof(modp_number)*(max_coord));
     }
     condition_list=(condition_type*)omAlloc0(sizeof(condition_type)*final_base_dim);
     for (i=0;i<final_base_dim;i++) condition_list[i].mon=(exponent*)omAlloc0(sizeof(exponent)*variables);
     modp_points=(modp_coordinates*)omAlloc(sizeof(modp_coordinates)*n_points);
     for (i=0;i<n_points;i++) modp_points[i]=(modp_number*)omAlloc0(sizeof(modp_number)*variables);
     if (!only_modp)
     {
        q_points=(q_coordinates*)omAlloc0(sizeof(q_coordinates)*n_points);
        for (i=0;i<n_points;i++)
        {
            q_points[i]=(mpq_t*)omAlloc(sizeof(mpq_t)*variables);
            for (j=0;j<variables;j++) mpq_init(q_points[i][j]);
        }
        int_points=(int_coordinates*)omAlloc0(sizeof(int_coordinates)*n_points);
        for (i=0;i<n_points;i++)
        {
            int_points[i]=(mpz_t*)omAlloc(sizeof(mpz_t)*variables);
            for (j=0;j<variables;j++) mpz_init(int_points[i][j]);
        }
     }
     coord_exist=(coord_exist_table*)omAlloc(sizeof(coord_exist_table)*n_points);
     for (i=0;i<n_points;i++)
     {
         coord_exist[i]=(bool*)omAlloc0(sizeof(bool)*variables);
     }
     generic_column_name=(mono_type*)omAlloc(sizeof(mono_type)*final_base_dim);
     for (i=0;i<final_base_dim;i++) generic_column_name[i]=ZeroMonomial ();
     good_primes=0;
     bad_primes=1;
     generic_n_generators=0;
     if (!only_modp)
     {
        polycoef=(mpz_t*)omAlloc(sizeof(mpz_t)*(final_base_dim+1));
        polyexp=(mono_type*)omAlloc(sizeof(mono_type)*(final_base_dim+1));
        for (i=0;i<=final_base_dim;i++)
        {
             mpz_init(polycoef[i]);
             polyexp[i]=ZeroMonomial ();
        }
        mpz_init(common_denom);
     }

// set all globally used lists to be initially empty
     modp_result=NULL;
     cur_result=NULL;
     gen_list=NULL;
     n_results=0;

// creates polynomials to compare by Singular
     comparizon_p1=pOne();
     comparizon_p2=pOne();
}

static void InitProcData ()  // initialization for procedures which do computations mod p
{
     int i;
     check_list=MonListAdd (check_list,ZeroMonomial ());
     my_row=(modp_number*)omAlloc0(sizeof(modp_number)*final_base_dim);
     my_solve_row=(modp_number*)omAlloc0(sizeof(modp_number)*final_base_dim);
     column_name=(mono_type*)omAlloc(sizeof(mono_type)*final_base_dim);
     for (i=0;i<final_base_dim;i++) column_name[i]=ZeroMonomial ();
     last_solve_column=0;
     modp_number *gen_table;
     modp_number pos_gen;
     bool gen_ok;
     modp_Reverse=(modp_number*)omAlloc0(sizeof(modp_number)*myp);

// produces table of modp inverts by finding a generator of (Z_myp*,*)
     gen_table=(modp_number*)omAlloc(sizeof(modp_number)*myp);
     gen_table[1]=1;
     for (pos_gen=2;pos_gen<myp;pos_gen++)
     {
         gen_ok=true;
         for (i=2;i<myp;i++)
         {
             gen_table[i]=modp_mul(gen_table[i-1],pos_gen);
             if (gen_table[i]==1)
             {
                gen_ok=false;
                break;
             }
         }
         if (gen_ok) break;
     }
     for (i=2;i<myp;i++) modp_Reverse[gen_table[i]]=gen_table[myp-i+1];
     modp_Reverse[1]=1;
     omFree(gen_table);
}

static mon_list_entry* FreeMonList (mon_list_entry *list)  // destroys a list of monomials, returns NULL pointer
{
     mon_list_entry *ptr;
     mon_list_entry *pptr;
     ptr=list;
     while (ptr!=NULL)
     {
           pptr=ptr->next;
           omFree(ptr->mon);
           omFree(ptr);
           ptr=pptr;}
     return NULL;
}

static void GeneralDone ()  // to be called before exit to free memory
{
     int i,j;
     for (i=0;i<n_points;i++)
     {
         for (j=0;j<variables;j++)
         {
             omFree(points[i][j]);
         }
         omFree(points[i]);
     }
     omFree(points);
     for (i=0;i<final_base_dim;i++) omFree(condition_list[i].mon);
     omFree(condition_list);
     for (i=0;i<n_points;i++) omFree(modp_points[i]);
     omFree(modp_points);
     if (!only_modp)
     {
        for (i=0;i<n_points;i++)
        {
            for (j=0;j<variables;j++) mpq_clear(q_points[i][j]);
            omFree(q_points[i]);
        }
        omFree(q_points);
        for (i=0;i<n_points;i++)
        {
            for (j=0;j<variables;j++) mpz_clear(int_points[i][j]);
            omFree(int_points[i]);
        }
        omFree(int_points);
        generic_lt=FreeMonList (generic_lt);
        for (i=0;i<=final_base_dim;i++)
        {
            mpz_clear(polycoef[i]);
            omFree(polyexp[i]);
        }
        omFree(polycoef);
        omFree(polyexp);
        if (!only_modp) mpz_clear(common_denom);
     }
     for (i=0;i<final_base_dim;i++)
     {
         omFree(generic_column_name[i]);
     }
     omFree(generic_column_name);
     for (i=0;i<n_points;i++) omFree(coord_exist[i]);
     omFree(coord_exist);
     pDelete(&comparizon_p1);
     pDelete(&comparizon_p2);
}

static void FreeProcData ()  // to be called after one modp computation to free memory
{
     int i;
     row_list_entry *ptr;
     row_list_entry *pptr;
     check_list=FreeMonList (check_list);
     lt_list=FreeMonList (lt_list);
     base_list=FreeMonList (base_list);
     omFree(my_row);
     my_row=NULL;
     omFree(my_solve_row);
     my_solve_row=NULL;
     ptr=row_list;
     while (ptr!=NULL)
     {
           pptr=ptr->next;
           omFree(ptr->row_matrix);
           omFree(ptr->row_solve);
           omFree(ptr);
           ptr=pptr;
     }
     row_list=NULL;
     for (i=0;i<final_base_dim;i++) omFree(column_name[i]);
     omFree(column_name);
     omFree(modp_Reverse);
}

static void modp_Evaluate(modp_number *ev, mono_type mon, condition_type con)  // evaluates monomial on condition (modp)
{
    int i;
    *ev=0;
    for (i=0;i<variables;i++)
        if (con.mon[i] > mon[i]) return ;;
    *ev=1;
    int j,k;
    mono_type mn;
    mn=(exponent*)omAlloc(sizeof(exponent)*variables);
    memcpy(mn,mon,sizeof(exponent)*variables);
    for (k=0;k<variables;k++)
    {
        for (j=1;j<=con.mon[k];j++) // this loop computes the coefficient from derivation
        {
            *ev=modp_mul(*ev,mn[k]);
            mn[k]--;
        }
        *ev=modp_mul(*ev,points[con.point_ref][k][mn[k]]);
    }
    omFree(mn);
}

static void int_Evaluate(mpz_t ev, mono_type mon, condition_type con) // (***) evaluates monomial on condition for integer numbers
{
    int i;
    mpz_set_ui(ev,0);
    for (i=0;i<variables;i++)
        if (con.mon[i] > mon[i]) return ;;
    mpz_set_ui(ev,1);
    mpz_t mon_conv;
    mpz_init(mon_conv);
    int j,k;
    mono_type mn;
    mn=(exponent*)omAlloc(sizeof(exponent)*variables);
    memcpy(mn,mon,sizeof(exponent)*variables);
    for (k=0;k<variables;k++)
    {
        for (j=1;j<=con.mon[k];j++) // this loop computes the coefficient from derivation
        {
            mpz_set_si(mon_conv,mn[k]); // (***)
            mpz_mul(ev,ev,mon_conv);
            mn[k]--;
        }
        for (j=1;j<=mn[k];j++) mpz_mul(ev,ev,int_points[con.point_ref][k]);  // this loop computes the product of coordinate
    }
    omFree(mn);
    mpz_clear(mon_conv);
}

static void ProduceRow(mono_type mon)  // produces a row for monomial - first part is an evaluated part, second 0 to obtain the coefs of dependence
{
     modp_number *row;
     condition_type *con;
     int i;
     row=my_row;
     con=condition_list;
     for (i=0;i<final_base_dim;i++)
     {
         modp_Evaluate (row, mon, *con);
         row++;
         con++;
     }
     row=my_solve_row;
     for (i=0;i<final_base_dim;i++)
     {
         *row=0;
         row++;
     }
}

static void IntegerPoints ()  // produces integer points from rationals by scaling the coordinate system
{
     int i,j;
     mpz_set_ui(common_denom,1); // this is common scaling factor
     for (i=0;i<n_points;i++)
     {
         for (j=0;j<variables;j++)
         {
             mpz_lcm(common_denom,common_denom,mpq_denref(q_points[i][j]));
         }
     }
     mpq_t temp;
     mpq_init(temp);
     mpq_t denom_q;
     mpq_init(denom_q);
     mpq_set_z(denom_q,common_denom);
     for (i=0;i<n_points;i++)
     {
         for (j=0;j<variables;j++)
         {
             mpq_mul(temp,q_points[i][j],denom_q);  // multiplies by common denominator
             mpz_set(int_points[i][j],mpq_numref(temp));  // and changes into integer
         }
     }
     mpq_clear(temp);
     mpq_clear(denom_q);
}

static void int_PrepareProducts ()  // prepares coordinates of points and products for modp case (from integer coefs)
{
     int i,j,k;
     mpz_t big_myp;
     mpz_init(big_myp);
     mpz_set_si(big_myp,myp);
     mpz_t temp;
     mpz_init(temp);
     for (i=0;i<n_points;i++)
     {
         for (j=0;j<variables;j++)
         {
             mpz_mod(temp,int_points[i][j],big_myp);  // coordinate is now modulo myp
             points[i][j][1]=mpz_get_ui(temp);
             points[i][j][0]=1;
             for (k=2;k<max_coord;k++) points[i][j][k]=modp_mul(points[i][j][k-1],points[i][j][1]);
         }
     }
     mpz_mod(temp,common_denom,big_myp);  // computes the common denominator (from rational data) modulo myp
     denom_divisible=(mpz_sgn(temp)==0);  // checks whether it is divisible by modp
     mpz_clear(temp);
     mpz_clear(big_myp);
}

static void modp_PrepareProducts () // prepares products for modp computation from modp data
{
     int i,j,k;
     for (i=0;i<n_points;i++)
     {
         for (j=0;j<variables;j++)
         {
             points[i][j][1]=modp_points[i][j];
             points[i][j][0]=1;
             for (k=2;k<max_coord;k++) points[i][j][k]=modp_mul(points[i][j][k-1],points[i][j][1]);
         }
     }
}

static void MakeConditions ()  // prepares a list of conditions from list of multiplicities
{
     condition_type *con;
     int n,i,k;
     mono_type mon;
     mon=ZeroMonomial ();
     con=condition_list;
     for (n=0;n<n_points;n++)
     {
         for (i=0;i<variables;i++) mon[i]=0;
         while (mon[0]<multiplicity[n])
         {
             if (MonDegree (mon) < multiplicity[n])
             {
                memcpy(con->mon,mon,sizeof(exponent)*variables);
                con->point_ref=n;
                con++;
             }
             k=variables-1;
             mon[k]++;
             while ((k>0)&&(mon[k]>=multiplicity[n]))
             {
                 mon[k]=0;
                 k--;
                 mon[k]++;
             }
         }
     }
     omFree(mon);
}

static void ReduceRow ()  // reduces my_row by previous rows, does the same for solve row
{
     if (row_list==NULL) return ;
     row_list_entry *row_ptr;
     modp_number *cur_row_ptr;
     modp_number *solve_row_ptr;
     modp_number *my_row_ptr;
     modp_number *my_solve_row_ptr;
     int first_col;
     int i;
     modp_number red_val;
     modp_number mul_val;
#ifdef integerstrategy
     modp_number *m_row_ptr;
     modp_number prep_val;
#endif
     row_ptr=row_list;
     while (row_ptr!=NULL)
     {
           cur_row_ptr=row_ptr->row_matrix;
           solve_row_ptr=row_ptr->row_solve;
           my_row_ptr=my_row;
           my_solve_row_ptr=my_solve_row;
           first_col=row_ptr->first_col;
           cur_row_ptr=cur_row_ptr+first_col;  // reduction begins at first_col position
           my_row_ptr=my_row_ptr+first_col;
           red_val=*my_row_ptr;
           if (red_val!=0)
           {
#ifdef integerstrategy
              prep_val=*cur_row_ptr;
              if (prep_val!=1)
              {
                 m_row_ptr=my_row;
                 for (i=0;i<final_base_dim;i++)
                 {
                     if (*m_row_ptr!=0) *m_row_ptr=modp_mul(*m_row_ptr,prep_val);
                     m_row_ptr++;
                 }
                 m_row_ptr=my_solve_row;
                 for (i=0;i<last_solve_column;i++)
                 {
                     if (*m_row_ptr!=0) *m_row_ptr=modp_mul(*m_row_ptr,prep_val);
                     m_row_ptr++;
                 }
              }
#endif
              for (i=first_col;i<final_base_dim;i++)
              {
                  if (*cur_row_ptr!=0)
                  {
                    mul_val=modp_mul(*cur_row_ptr,red_val);
                    *my_row_ptr=modp_sub(*my_row_ptr,mul_val);
                  }
                  my_row_ptr++;
                  cur_row_ptr++;
              }
              for (i=0;i<=last_solve_column;i++)  // last_solve_column stores the last non-enmpty entry in solve matrix
              {
                  if (*solve_row_ptr!=0)
                  {
                     mul_val=modp_mul(*solve_row_ptr,red_val);
                     *my_solve_row_ptr=modp_sub(*my_solve_row_ptr,mul_val);
                  }
                  solve_row_ptr++;
                  my_solve_row_ptr++;
              }
           }
           row_ptr=row_ptr->next;
#if 0 /* only debugging */
           PrintS("reduction by row ");
           Info ();
#endif
     }
}

static bool RowIsZero () // check whether a row is zero
{
     bool zero=1;
     int i;
     modp_number *row;
     row=my_row;
     for (i=0;i<final_base_dim;i++)
     {
         if (*row!=0) {zero=0; break;}
         row++;
     }
     return zero;
}

static bool DivisibleMon (mono_type m1, mono_type m2) // checks whether m1 is divisible by m2
{
     int i;
     for (i=0;i<variables;i++)
         if (m1[i]>m2[i]) return false;;
     return true;
}

static void ReduceCheckListByMon (mono_type m)  // from check_list monomials divisible by m are thrown out
{
     mon_list_entry *c_ptr;
     mon_list_entry *p_ptr;
     mon_list_entry *n_ptr;
     c_ptr=check_list;
     p_ptr=NULL;
     while (c_ptr!=NULL)
     {
          if (DivisibleMon (m,c_ptr->mon))
          {
             if (p_ptr==NULL)
                check_list=c_ptr->next;
             else
                p_ptr->next=c_ptr->next;
             n_ptr=c_ptr->next;
             omFree(c_ptr->mon);
             omFree(c_ptr);
             c_ptr=n_ptr;
          }
          else
          {
              p_ptr=c_ptr;
              c_ptr=c_ptr->next;
          }
     }
}

static void TakeNextMonomial (mono_type mon)  // reads first monomial from check_list, then it is deleted
{
     mon_list_entry *n_check_list;
     if (check_list!=NULL)
     {
        memcpy(mon,check_list->mon,sizeof(exponent)*variables);
        n_check_list=check_list->next;
        omFree(check_list->mon);
        omFree(check_list);
        check_list=n_check_list;
     }
}

static void UpdateCheckList (mono_type m) // adds all monomials which are "next" to m (divisible by m and degree +1)
{
     int i;
     for (i=0;i<variables;i++)
     {
         m[i]++;
         check_list=MonListAdd (check_list,m);
         m[i]--;
     }
}

static void ReduceCheckListByLTs ()  // deletes all monomials from check_list which are divisible by one of the leading terms
{
     mon_list_entry *ptr;
     ptr=lt_list;
     while (ptr!=NULL)
     {
           ReduceCheckListByMon (ptr->mon);
           ptr=ptr->next;
     }
}

static void RowListAdd (int first_col, mono_type mon) // puts a row into matrix
{
     row_list_entry *ptr;
     row_list_entry *pptr;
     row_list_entry *temp;
     ptr=row_list;
     pptr=NULL;
     while (ptr!=NULL)
     {
#ifndef unsortedmatrix
         if (  first_col <= ptr->first_col ) break;
#endif
         pptr=ptr;
         ptr=ptr->next;
     }
     temp=(row_list_entry*)omAlloc0(sizeof(row_list_entry));
     (*temp).row_matrix=(modp_number*)omAlloc0(sizeof(modp_number)*final_base_dim);
     memcpy((*temp).row_matrix,my_row,sizeof(modp_number)*(final_base_dim));
     (*temp).row_solve=(modp_number*)omAlloc0(sizeof(modp_number)*final_base_dim);
     memcpy((*temp).row_solve,my_solve_row,sizeof(modp_number)*(final_base_dim));
     (*temp).first_col=first_col;
     temp->next=ptr;
     if (pptr==NULL) row_list=temp; else pptr->next=temp;
     memcpy(column_name[first_col],mon,sizeof(exponent)*variables);
}


static void PrepareRow (mono_type mon) // prepares a row and puts it into matrix
{
     modp_number *row;
     int first_col=-1;
     int col;
     modp_number red_val=1;
     row=my_row;
#ifdef integerstrategy
     for (col=0;col<final_base_dim;col++)
     {
         if (*row!=0)
         {
            first_col=col;
            break;
         }
         row++;
     }
     my_solve_row[first_col]=1;
     if (first_col > last_solve_column) last_solve_column=first_col;
#else
     for (col=0;col<final_base_dim;col++)
     {
         if (*row!=0)
         {
            first_col=col;
            red_val=modp_Reverse[*row]; // first non-zero entry, should multiply all row by inverse to obtain 1
            modp_denom=modp_mul(modp_denom,*row);  // remembers the divisor
            *row=1;
            break;
         }
         row++;
     }
     my_solve_row[first_col]=1;
     if (first_col > last_solve_column) last_solve_column=first_col;
     if (red_val!=1)
     {
        row++;
        for (col=first_col+1;col<final_base_dim;col++)
        {
            if (*row!=0) *row=modp_mul(*row,red_val);
            row++;
        }
        row=my_solve_row;
        for (col=0;col<=last_solve_column;col++)
        {
            if (*row!=0) *row=modp_mul(*row,red_val);
            row++;
        }
     }
#endif
     RowListAdd (first_col, mon);
}

static void NewResultEntry ()  // creates an entry for new modp result
{
    modp_result_entry *temp;
    temp=(modp_result_entry*)omAlloc0(sizeof(modp_result_entry));
    if (cur_result==NULL)
    {
       modp_result=temp;
       temp->prev=NULL;
    }
    else
    {
       temp->prev=cur_result;
       cur_result->next=temp;
    }
    cur_result=temp;
    cur_result->next=NULL;
    cur_result->p=myp;
    cur_result->generator=NULL;
    cur_result->n_generators=0;
    n_results++;
}

static void FreeResultEntry (modp_result_entry *e) // destroys the result entry, without worrying about where it is
{
     generator_entry *cur_gen;
     generator_entry *next_gen;
     cur_gen=e->generator;
     while (cur_gen!=NULL)
     {
         next_gen=cur_gen->next;
         omFree(cur_gen->coef);
         omFree(cur_gen->lt);
         omFree(cur_gen);
         cur_gen=next_gen;
     }
     omFree(e);
}


static void NewGenerator (mono_type mon)  // new generator in modp comp found, shoul be stored on the list
{
     generator_entry *cur_ptr;
     generator_entry *prev_ptr;
     generator_entry *temp;
     cur_ptr=cur_result->generator;
     prev_ptr=NULL;
     while (cur_ptr!=NULL)
     {
         prev_ptr=cur_ptr;
         cur_ptr=cur_ptr->next;
     }
     temp=(generator_entry*)omAlloc0(sizeof(generator_entry));
     if (prev_ptr==NULL) cur_result->generator=temp;
     else prev_ptr->next=temp;
     temp->next=NULL;
     temp->coef=(modp_number*)omAlloc0(sizeof(modp_number)*final_base_dim);
     memcpy(temp->coef,my_solve_row,sizeof(modp_number)*final_base_dim);
     temp->lt=ZeroMonomial ();
     memcpy(temp->lt,mon,sizeof(exponent)*variables);
     temp->ltcoef=1;
     cur_result->n_generators++;
}

static void MultGenerators () // before reconstructing, all denominators must be cancelled
{
#ifndef integerstrategy
     int i;
     generator_entry *cur_ptr;
     cur_ptr=cur_result->generator;
     while (cur_ptr!=NULL)
     {
         for (i=0;i<final_base_dim;i++)
             cur_ptr->coef[i]=modp_mul(cur_ptr->coef[i],modp_denom);
         cur_ptr->ltcoef=modp_denom;
         cur_ptr=cur_ptr->next;
     }
#endif
}
#if 0 /* only debbuging */
void PresentGenerator (int i)  // only for debuging, writes a generator in its form in program
{
     int j;
     modp_result_entry *cur_ptr;
     generator_entry *cur_gen;
     cur_ptr=modp_result;
     while (cur_ptr!=NULL)
     {
        cur_gen=cur_ptr->generator;
        for (j=0;j<i;j++) cur_gen=cur_gen->next;
        for (j=0;j<final_base_dim;j++)
        {
            Print("%d;", cur_gen->coef[j]);
        }
        PrintS(" and LT = ");
        WriteMono (cur_gen->lt);
        Print(" ( %d )  prime = %d\n", cur_gen->ltcoef, cur_ptr->p);
        cur_ptr=cur_ptr->next;
     }
}
#endif

static modp_number TakePrime (modp_number /*p*/)  // takes "previous" (smaller) prime
{
    myp_index--;
    return cf_getSmallPrime(myp_index);
}

static void PrepareChinese (int n) // initialization for CRA
{
     int i,k;
     modp_result_entry *cur_ptr;
     cur_ptr=modp_result;
     modp_number *congr_ptr;
     modp_number prod;
     in_gamma=(modp_number*)omAlloc0(sizeof(modp_number)*n);
     congr=(modp_number*)omAlloc0(sizeof(modp_number)*n);
     congr_ptr=congr;
     while (cur_ptr!=NULL)
     {
        *congr_ptr=cur_ptr->p;
        cur_ptr=cur_ptr->next;
        congr_ptr++;
     }
     for (k=1;k<n;k++)
     {
         prod=congr[0]%congr[k];
         for (i=1;i<=k-1;i++) prod=(prod*congr[i])%congr[k];
         in_gamma[i]=OneInverse(prod,congr[k]);
     }
     mpz_init(bigcongr);
     mpz_set_si(bigcongr,congr[0]);
     for (k=1;k<n;k++) mpz_mul_ui(bigcongr,bigcongr,congr[k]);
}

static void CloseChinese () // after CRA
{
     omFree(in_gamma);
     omFree(congr);
     mpz_clear(bigcongr);
}

static void ClearGCD () // divides polynomials in basis by gcd of coefficients
{
     bool first_gcd=true;
     int i;
     mpz_t g;
     mpz_init(g);
     for (i=0;i<=final_base_dim;i++)
     {
         if (mpz_sgn(polycoef[i])!=0)
         {
            if (first_gcd)
            {
               first_gcd=false;
               mpz_set(g,polycoef[i]);
            }
            else
               mpz_gcd(g,g,polycoef[i]);
         }
     }
     for (i=0;i<=final_base_dim;i++) mpz_divexact(polycoef[i],polycoef[i],g);
     mpz_clear(g);
}

static void ReconstructGenerator (int ngen,int n) // recostruction of generator from various modp comp
{
     int i,j,k;
     int coef;
     char *str=NULL;
     str=(char*)omAlloc0(sizeof(char)*1000);
     modp_result_entry *cur_ptr;
     generator_entry *cur_gen;
     modp_number *u;
     modp_number *v;
     modp_number temp;
     mpz_t sol;
     mpz_t nsol;
     mpz_init(sol);
     mpz_init(nsol);
     u=(modp_number*)omAlloc0(sizeof(modp_number)*n);
     v=(modp_number*)omAlloc0(sizeof(modp_number)*n);
     for (coef=0;coef<=final_base_dim;coef++)
     {
         i=0;
         cur_ptr=modp_result;
         while (cur_ptr!=NULL)
         {
            cur_gen=cur_ptr->generator;
            for (j=0;j<ngen;j++) cur_gen=cur_gen->next; // we have to take jth generator
            if (coef<final_base_dim) u[i]=cur_gen->coef[coef]; else u[i]=cur_gen->ltcoef;
            cur_ptr=cur_ptr->next;
            i++;
         }
         v[0]=u[0]; // now CRA begins
         for (k=1;k<n;k++)
         {
             temp=v[k-1];
             for (j=k-2;j>=0;j--) temp=(temp*congr[j]+v[j])%congr[k];
             temp=u[k]-temp;
             if (temp<0) temp=temp+congr[k];
             v[k]=(temp*in_gamma[k])%congr[k];
         }
         mpz_set_si(sol,v[n-1]);
         for (k=n-2;k>=0;k--)
         {
             mpz_mul_ui(sol,sol,congr[k]);
             mpz_add_ui(sol,sol,v[k]);
         }          // now CRA ends
         mpz_sub(nsol,sol,bigcongr);
         int s=mpz_cmpabs(sol,nsol);
         if (s>0) mpz_set(sol,nsol); // chooses representation closer to 0
         mpz_set(polycoef[coef],sol);
         if (coef<final_base_dim)
            memcpy(polyexp[coef],generic_column_name[coef],sizeof(exponent)*variables);
         else
            memcpy(polyexp[coef],MonListElement (generic_lt,ngen),sizeof(exponent)*variables);
#ifdef checksize
         size=mpz_sizeinbase(sol,10);
         if (size>maximal_size) maximal_size=size;
#endif
     }
     omFree(u);
     omFree(v);
     omFree(str);
     ClearGCD ();
     mpz_clear(sol);
     mpz_clear(nsol);
}

static void Discard ()  // some unlucky prime occures
{
     modp_result_entry *temp;
#ifdef writemsg
     Print(", prime=%d", cur_result->p);
#endif
     bad_primes++;
     if (good_primes>bad_primes)
     {
#ifdef writemsg
        Print("-discarding this comp (%dth)\n", n_results);
#endif
        temp=cur_result;
        cur_result=cur_result->prev;
        cur_result->next=NULL;
        n_results--;
        FreeResultEntry (temp);
     }
     else
     {
#ifdef writemsg
        PrintS("-discarding ALL.\n");
#endif
        int i;
        modp_result_entry *ntfree;
        generator_entry *cur_gen;
        temp=cur_result->prev;
        while (temp!=NULL)
        {
            ntfree=temp->prev;
            FreeResultEntry (temp);
            temp=ntfree;
        }
        modp_result=cur_result;
        cur_result->prev=NULL;
        n_results=1;
        good_primes=1;
        bad_primes=0;
        generic_n_generators=cur_result->n_generators;
        cur_gen=cur_result->generator;
        generic_lt=FreeMonList (generic_lt);
        for (i=0;i<generic_n_generators;i++)
        {
            generic_lt=MonListAdd (generic_lt,cur_gen->lt);
            cur_gen=cur_gen->next;
        }
        for (i=0;i<final_base_dim;i++) memcpy(generic_column_name[i],column_name[i],sizeof(exponent)*variables);
     }
}

static void modp_SetColumnNames ()  // used by modp - sets column names, the old table will be destroyed
{
    int i;
    for (i=0;i<final_base_dim;i++) memcpy(generic_column_name[i],column_name[i],sizeof(exponent)*variables);
}

static void CheckColumnSequence () // checks if scheme of computations is as generic one
{
     int i;
     if (cur_result->n_generators!=generic_n_generators)
     {
#ifdef writemsg
        PrintS("wrong number of generators occurred");
#else
        if (protocol) PrintS("ng");
#endif
        Discard ();
        return;
     }
     if (denom_divisible)
     {
#ifdef writemsg
        PrintS("denom of coef divisible by p");
#else
        if (protocol) PrintS("dp");
#endif
        Discard ();
        return;
     }
     generator_entry *cur_gen;
     mon_list_entry *cur_mon;
     cur_gen=cur_result->generator;
     cur_mon=generic_lt;
     for (i=0;i<generic_n_generators;i++)
     {
         if (!EqualMon(cur_mon->mon,cur_gen->lt))
         {
#ifdef writemsg
            PrintS("wrong leading term occurred");
#else
            if (protocol) PrintS("lt");
#endif
            Discard ();
            return;
         }
         cur_gen=cur_gen->next;
         cur_mon=cur_mon->next;
     }
     for (i=0;i<final_base_dim;i++)
     {
         if (!EqualMon(generic_column_name[i],column_name[i]))
         {
#ifdef writemsg
            PrintS("wrong seq of cols occurred");
#else
            if (protocol) PrintS("sc");
#endif
            Discard ();
            return;
         }
     }
     good_primes++;
}
#if 0 /* only debuggig */
void WriteGenerator () // writes generator (only for debugging)
{
     char *str;
     str=(char*)omAlloc0(sizeof(char)*1000);
     int i;
     for (i=0;i<=final_base_dim;i++)
     {
         str=mpz_get_str(str,10,polycoef[i]);
         PrintS(str);
         PrintS("*");
         WriteMono(polyexp[i]);
         PrintS(" ");
     }
     omFree(str);
     PrintLn();
}
#endif

static bool CheckGenerator () // evaluates generator to check whether it is good
{
     mpz_t val,sum;
     int con,i;
     mpz_init(val);
     mpz_init(sum);
     for (con=0;con<final_base_dim;con++)
     {
       mpz_set_ui(sum,0);
       for (i=0;i<=final_base_dim;i++)
       {
         int_Evaluate(val, polyexp[i], condition_list[con]);
         mpz_mul(val,val,polycoef[i]);
         mpz_add(sum,sum,val);
       }
       if (mpz_sgn(sum)!=0)
       {
          mpz_clear(val);
          mpz_clear(sum);
          return false;
       }
    }
    mpz_clear(val);
    mpz_clear(sum);
    return true;
}

static void ClearGenList ()
{
     gen_list_entry *temp;
     int i;
     while (gen_list!=NULL)
     {
         temp=gen_list->next;
         for (i=0;i<=final_base_dim;i++)
         {
             mpz_clear(gen_list->polycoef[i]);
             omFree(gen_list->polyexp[i]);
         }
         omFree(gen_list->polycoef);
         omFree(gen_list->polyexp);
         omFree(gen_list);
         gen_list=temp;
      }
}

static void UpdateGenList ()
{
     gen_list_entry *temp,*prev;
     int i,j;
     prev=NULL;
     temp=gen_list;
     exponent deg;
     for (i=0;i<=final_base_dim;i++)
     {
         deg=MonDegree(polyexp[i]);
         for (j=0;j<deg;j++)
         {
             mpz_mul(polycoef[i],polycoef[i],common_denom);
         }
     }
     ClearGCD ();
     while (temp!=NULL)
     {
         prev=temp;
         temp=temp->next;
     }
     temp=(gen_list_entry*)omAlloc0(sizeof(gen_list_entry));
     if (prev==NULL) gen_list=temp; else prev->next=temp;
     temp->next=NULL;
     temp->polycoef=(mpz_t*)omAlloc(sizeof(mpz_t)*(final_base_dim+1));
     temp->polyexp=(mono_type*)omAlloc(sizeof(mono_type)*(final_base_dim+1));
     for (i=0;i<=final_base_dim;i++)
     {
         mpz_init(temp->polycoef[i]);
         mpz_set(temp->polycoef[i],polycoef[i]);
         temp->polyexp[i]=ZeroMonomial ();
         memcpy(temp->polyexp[i],polyexp[i],sizeof(exponent)*variables);
     }
}

#if 0 /* only debugging */
void ShowGenList ()
{
     gen_list_entry *temp;
     int i;
     char *str;
     str=(char*)omAlloc0(sizeof(char)*1000);
     temp=gen_list;
     while (temp!=NULL)
     {
         PrintS("generator: ");
         for (i=0;i<=final_base_dim;i++)
         {
             str=mpz_get_str(str,10,temp->polycoef[i]);
             PrintS(str);
             PrintS("*");
             WriteMono(temp->polyexp[i]);
         }
         PrintLn();
         temp=temp->next;
     }
     omFree(str);
}
#endif


static void modp_Main ()
{
     mono_type cur_mon;
     cur_mon= ZeroMonomial ();
     modp_denom=1;
     bool row_is_zero;

#if 0 /* only debugging */
     Info ();
#endif

     while (check_list!=NULL)
     {
           TakeNextMonomial (cur_mon);
           ProduceRow (cur_mon);
#if 0 /* only debugging */
           cout << "row produced for monomial ";
           WriteMono (cur_mon);
           cout << endl;
           Info ();
#endif
           ReduceRow ();
           row_is_zero = RowIsZero ();
           if (row_is_zero)
           {
              lt_list=MonListAdd (lt_list,cur_mon);
              ReduceCheckListByMon (cur_mon);
              NewGenerator (cur_mon);
#if 0 /* only debugging */
              cout << "row is zero - linear dependence found (should be seen in my_solve_row)" << endl;
              cout << "monomial added to leading terms list" << endl;
              cout << "check list updated" << endl;
              Info ();
#endif
           }
           else
           {
              base_list= MonListAdd (base_list,cur_mon);
              UpdateCheckList (cur_mon);
              ReduceCheckListByLTs ();
#if 0 /* only debugging */
              cout << "row is non-zero" << endl;
              cout << "monomial added to quotient basis list" << endl;
              cout << "new monomials added to check list" << endl;
              cout << "check list reduced by monomials from leading term list" << endl;
              Info ();
#endif
              PrepareRow (cur_mon);
#if 0 /* only debugging */
              cout << "row prepared and put into matrix" << endl;
              Info ();
#endif
           }
        }
        omFree(cur_mon);
}

static void ResolveCoeff (mpq_t c, number m)
{
  if ((long)m & SR_INT)
  {
    long m_val=SR_TO_INT(m);
    mpq_set_si(c,m_val,1);
  }
  else
  {
    if (m->s<2)
    {
      mpz_set(mpq_numref(c),m->z);
      mpz_set(mpq_denref(c),m->n);
      mpq_canonicalize(c);
    }
    else
    {
      mpq_set_z(c,m->z);
    }
  }
}

ideal interpolation(const std::vector<ideal>& L, intvec *v)
{
  protocol=TEST_OPT_PROT;  // should be set if option(prot) is enabled

  bool data_ok=true;

  // reading the ring data ***************************************************
  if ((currRing==NULL) || ((!rField_is_Zp (currRing))&&(!rField_is_Q (currRing))))
  {
     WerrorS("coefficient field should be Zp or Q!");
     return NULL;
  }
  if ((currRing->qideal)!=NULL)
  {
     WerrorS("quotient ring not supported!");
     return NULL;
  }
  if ((currRing->OrdSgn)!=1)
  {
     WerrorS("ordering must be global!");
     return NULL;
  }
  n_points=v->length ();
  if (n_points!=L.size())
  {
     WerrorS("list and intvec must have the same length!");
     return NULL;
  }
  assume( n_points > 0 );
  variables=currRing->N;
  only_modp=rField_is_Zp(currRing);
  if (only_modp) myp=rChar(currRing);
  // ring data read **********************************************************


  multiplicity=(int*)malloc(sizeof(int)*n_points); // TODO: use omalloc!
  int i;
  for (i=0;i<n_points;i++) multiplicity[i]=(*v)[i];

  final_base_dim = CalcBaseDim ();

#ifdef writemsg
  Print("number of variables: %d\n", variables);
  Print("number of points: %d\n", n_points);
  PrintS("multiplicities: ");
  for (i=0;i<n_points;i++) Print("%d ", multiplicity[i]);
  PrintLn();
  Print("general initialization for dimension %d ...\n", final_base_dim);
#endif

  GeneralInit ();

// reading coordinates of points from ideals **********************************
  mpq_t divisor;
  if (!only_modp) mpq_init(divisor);
  int j;
  for(i=0; i<L.size();i++)
  {
    ideal I = L[i];
    for(j=0;j<IDELEMS(I);j++)
    {
      poly p=I->m[j];
      if (p!=NULL)
      {
        poly ph=pHead(p);
        int pcvar=pVar(ph);
        if (pcvar!=0)
        {
          pcvar--;
          if (coord_exist[i][pcvar])
          {
             Print("coordinate %d for point %d initialized twice!\n",pcvar+1,i+1);
             data_ok=false;
          }
          number m;
          m=pGetCoeff(p); // possible coefficient standing by a leading monomial
          if (!only_modp) ResolveCoeff (divisor,m);
          number n;
          if (pNext(p)!=NULL) n=pGetCoeff(pNext(p));
          else n=nInit(0);
          if (only_modp)
          {
            n=nInpNeg(n);
            n=nDiv(n,m);
            modp_points[i][pcvar]=(int)((long)n);
          }
          else
          {
            ResolveCoeff (q_points[i][pcvar],n);
            mpq_neg(q_points[i][pcvar],q_points[i][pcvar]);
            mpq_div(q_points[i][pcvar],q_points[i][pcvar],divisor);
          }
          coord_exist[i][pcvar]=true;
        }
        else
        {
          PrintS("not a variable? ");
          wrp(p);
          PrintLn();
          data_ok=false;
        }
        pDelete(&ph);
      }
    }
  }
  if (!only_modp) mpq_clear(divisor);
  // data from ideal read *******************************************************

  // ckecking if all coordinates are initialized
  for (i=0;i<n_points;i++)
  {
      for (j=0;j<variables;j++)
      {
          if (!coord_exist[i][j])
          {
             Print("coordinate %d for point %d not known!\n",j+1,i+1);
             data_ok=false;
          }
      }
  }

  if (!data_ok)
  {
     GeneralDone();
     WerrorS("data structure is invalid");
     return NULL;
  }

  if (!only_modp) IntegerPoints ();
  MakeConditions ();
#ifdef writemsg
  PrintS("done.\n");
#else
  if (protocol) Print("[vdim %d]",final_base_dim);
#endif


// main procedure *********************************************************************
  int modp_cycles=10;
  bool correct_gen=false;
  if (only_modp) modp_cycles=1;
  myp_index=cf_getNumSmallPrimes ();

  while ((!correct_gen)&&(myp_index>1))
  {
#ifdef writemsg
        Print("trying %d cycles mod p...\n",modp_cycles);
#else
        if (protocol) Print("(%d)",modp_cycles);
#endif
        while ((n_results<modp_cycles)&&(myp_index>1))  // some computations mod p
        {
            if (!only_modp) myp=TakePrime (myp);
            NewResultEntry ();
            InitProcData ();
            if (only_modp) modp_PrepareProducts (); else int_PrepareProducts ();

            modp_Main ();

            if (!only_modp)
            {
               MultGenerators ();
               CheckColumnSequence ();
            }
            else
            {
               modp_SetColumnNames ();
            }
            FreeProcData ();
        }

        if (!only_modp)
        {
           PrepareChinese (modp_cycles);
           correct_gen=true;
           for (i=0;i<generic_n_generators;i++)
           {
               ReconstructGenerator (i,modp_cycles);
               correct_gen=CheckGenerator ();
               if (!correct_gen)
               {
#ifdef writemsg
                  PrintS("wrong generator!\n");
#else
//                  if (protocol) PrintS("!g");
#endif
                  ClearGenList ();
                  break;
               }
               else
               {
                  UpdateGenList ();
               }
           }
#ifdef checksize
           Print("maximal size of output: %d, precision bound: %d.\n",maximalsize,mpz_sizeinbase(bigcongr,10));
#endif
           CloseChinese ();
           modp_cycles=modp_cycles+10;
        }
        else
        {
           correct_gen=true;
        }
  }
// end of main procedure ************************************************************************************

#ifdef writemsg
  PrintS("computations finished.\n");
#else
  if (protocol) PrintLn();
#endif

  if (!correct_gen)
  {
     GeneralDone ();
     ClearGenList ();
     WerrorS("internal error - coefficient too big!");
     return NULL;
  }

// passing data to ideal *************************************************************************************
  ideal ret;

  if (only_modp)
  {
    mono_type mon;
    ret=idInit(modp_result->n_generators,1);
    generator_entry *cur_gen=modp_result->generator;
    for(i=0;i<IDELEMS(ret);i++)
    {
      poly p,sum;
      sum=NULL;
      int a;
      int cf;
      for (a=final_base_dim;a>=0;a--)
      {
        if (a==final_base_dim) cf=cur_gen->ltcoef; else cf=cur_gen->coef[a];
        if (cf!=0)
        {
            p=pISet(cf);
            if (a==final_base_dim) mon=cur_gen->lt; else mon=generic_column_name[a];
            for (j=0;j<variables;j++) pSetExp(p,j+1,mon[j]);
            pSetm(p);
            sum=pAdd(sum,p);
        }
      }
      ret->m[i]=sum;
      cur_gen=cur_gen->next;
    }
  }
  else
  {
    ret=idInit(generic_n_generators,1);
    gen_list_entry *temp=gen_list;
    for(i=0;i<IDELEMS(ret);i++)
    {
      poly p,sum;
      sum=NULL;
      int a;
      for (a=final_base_dim;a>=0;a--) // build one polynomial
      {
          if (mpz_sgn(temp->polycoef[a])!=0)
          {
             number n=ALLOC_RNUMBER();
#ifdef LDEBUG
             n->debug=123456;
#endif
             mpz_init_set(n->z,temp->polycoef[a]);
             n->s=3;
             n_Normalize(n, currRing->cf);
             p=pNSet(n); //a monomial
             for (j=0;j<variables;j++) pSetExp(p,j+1,temp->polyexp[a][j]);
             pSetm(p); // after all pSetExp
             sum=pAdd(sum,p);
          }
      }
      ret->m[i]=sum;
      temp=temp->next;
    }
  }
// data transferred ****************************************************************************


  GeneralDone ();
  ClearGenList ();
  return ret;
}


