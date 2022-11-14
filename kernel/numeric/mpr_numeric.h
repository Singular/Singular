#ifndef MPR_NUMERIC_H
#define MPR_NUMERIC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/


/*
* ABSTRACT - multipolynomial resultants - numeric stuff
*            ( root finder, vandermonde system solver, simplex )
*/

//-> include & define stuff
#include "coeffs/numbers.h"
#include "coeffs/mpr_global.h"
#include "coeffs/mpr_complex.h"

// define polish mode when finding roots
#define PM_NONE    0
#define PM_POLISH  1
#define PM_CORRUPT 2
//<-

//-> vandermonde system solver
/**
 * vandermonde system solver for interpolating polynomials from their values
 */
class vandermonde
{
public:
  vandermonde( const long _cn, const long _n,
               const long _maxdeg, number *_p, const bool _homog = true );
  ~vandermonde();

  /** Solves the Vandermode linear system
   *    \sum_{i=1}^{n} x_i^k-1 w_i = q_k, k=1,..,n.
     * Any computations are done using type number to get high pecision results.
   * @param  q n-tuple of results (right hand of equations)
   * @return w n-tuple of coefficients of resulting polynomial, lowest deg first
   */
  number * interpolateDense( const number * q );

  poly numvec2poly(const number * q );

private:
  void init();

private:
  long n;       // number of variables
  long cn;      // real number of coefficients of poly to interpolate
  long maxdeg;  // degree of the polynomial to interpolate
  long l;       // max number of coefficients in poly of deg maxdeg = (maxdeg+1)^n

  number *p;    // evaluation point
  number *x;    // coefficients, determinend by init() from *p

  bool homog;
};
//<-

//-> rootContainer
/**
 * complex root finder for univariate polynomials based on laguers algorithm
 */
class rootContainer
{
public:
  enum rootType { none, cspecial, cspecialmu, det, onepoly };

  rootContainer();
  ~rootContainer();

  void fillContainer( number *_coeffs, number *_ievpoint,
                      const int _var, const int _tdg,
                      const rootType _rt, const int _anz );

  bool solver( const int polishmode= PM_NONE );

  poly getPoly();

  //gmp_complex & operator[] ( const int i );
  inline gmp_complex & operator[] ( const int i )
  {
    return *theroots[i];
  }
  gmp_complex & evPointCoord( const int i );

  inline gmp_complex * getRoot( const int i )
  {
    return theroots[i];
  }

  bool swapRoots( const int from, const int to );

  int getAnzElems() { return anz; }
  int getLDim() { return anz; }
  int getAnzRoots() { return tdg; }

private:
  rootContainer( const rootContainer & v );

  /** Given the degree tdg and the tdg+1 complex coefficients ad[0..tdg]
   * (generated from the number coefficients coeffs[0..tdg]) of the polynomial
   * this routine successively calls "laguer" and finds all m complex roots in
   * roots[0..tdg]. The bool var "polish" should be input as "true" if polishing
   * (also by "laguer") is desired, "false" if the roots will be subsequently
   * polished by other means.
   */
  bool laguer_driver( gmp_complex ** a, gmp_complex ** roots, bool polish = true );
  bool isfloat(gmp_complex **a);
  void divlin(gmp_complex **a, gmp_complex x, int j);
  void divquad(gmp_complex **a, gmp_complex x, int j);
  void solvequad(gmp_complex **a, gmp_complex **r, int &k, int &j);
  void sortroots(gmp_complex **roots, int r, int c, bool isf);
  void sortre(gmp_complex **r, int l, int u, int inc);

  /** Given the degree m and the m+1 complex coefficients a[0..m] of the
   * polynomial, and given the complex value x, this routine improves x by
   * Laguerre's method until it converges, within the achievable roundoff limit,
   * to a root of the given polynomial. The number of iterations taken is
   * returned at its.
   */
  void laguer(gmp_complex ** a, int m, gmp_complex * x, int * its, bool type);
  void computefx(gmp_complex **a, gmp_complex x, int m,
                gmp_complex &f0, gmp_complex &f1, gmp_complex &f2,
                gmp_float &ex, gmp_float &ef);
  void computegx(gmp_complex **a, gmp_complex x, int m,
                gmp_complex &f0, gmp_complex &f1, gmp_complex &f2,
                gmp_float &ex, gmp_float &ef);
  void checkimag(gmp_complex *x, gmp_float &e);

  int var;
  int tdg;

  number * coeffs;
  number * ievpoint;
  rootType rt;

  gmp_complex ** theroots;

  int anz;
  bool found_roots;
};
//<-

class slists; typedef slists * lists;

//-> class rootArranger
class rootArranger
{
public:
  friend lists listOfRoots( rootArranger*, const unsigned int oprec );

  rootArranger( rootContainer ** _roots,
                rootContainer ** _mu,
                const int _howclean = PM_CORRUPT );
  ~rootArranger() {}

  void solve_all();
  void arrange();

  bool success() { return found_roots; }

private:
  rootArranger( const rootArranger & );

  rootContainer ** roots;
  rootContainer ** mu;

  int howclean;
  int rc,mc;
  bool found_roots;
};



//<-

//-> simplex computation
//   (used by sparse matrix construction)
#define SIMPLEX_EPS 1.0e-12

/** Linear Programming / Linear Optimization using Simplex - Algorithm
 *
 * On output, the tableau LiPM is indexed by two arrays of integers.
 * ipsov[j] contains, for j=1..m, the number i whose original variable
 * is now represented by row j+1 of LiPM. (left-handed vars in solution)
 * (first row is the one with the objective function)
 * izrov[j] contains, for j=1..n, the number i whose original variable
 * x_i is now a right-handed variable, rep. by column j+1 of LiPM.
 * These vars are all zero in the solution. The meaning of n<i<n+m1+m2
 * is the same as above.
 */
class simplex
{
public:

  int m;         // number of constraints, make sure m == m1 + m2 + m3 !!
  int n;         // # of independent variables
  int m1,m2,m3;  // constraints <=, >= and ==
  int icase;     // == 0: finite solution found;
                 // == +1 objective function unbound; == -1: no solution
  int *izrov,*iposv;

  mprfloat **LiPM; // the matrix (of size [m+2, n+1])

  /** #rows should be >= m+2, #cols >= n+1
   */
  simplex( int rows, int cols );
  ~simplex();

  BOOLEAN mapFromMatrix( matrix m );
  matrix mapToMatrix( matrix m );
  intvec * posvToIV();
  intvec * zrovToIV();

  void compute();

private:
  simplex( const simplex & );
  void simp1( mprfloat **a, int mm, int ll[], int nll, int iabf, int *kp, mprfloat *bmax );
  void simp2( mprfloat **a, int n, int l2[], int nl2, int *ip, int kp, mprfloat *q1 );
  void simp3( mprfloat **a, int i1, int k1, int ip, int kp );

  int LiPM_cols,LiPM_rows;
};

//<-

#endif /*MPR_NUMERIC_H*/

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
