#include <config.h>

#include "cf_defs.h"
#include "canonicalform.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "cf_reval.h"
#include "cf_random.h"
#include "cf_primes.h"
#include "fac_distrib.h"
#include "ftmpl_functions.h"
#include "ffreval.h"
#include "cf_binom.h"
#include "fac_iterfor.h"
#include "cf_iter.h"

void FFREvaluation::nextpoint()
{
  // enumerates the points stored in values
  int n = values.max();
  int p = getCharacteristic();
  for( int i=values.min(); i<=n; i++ )
  {
    if( values[i] != p-1 )
    {
      values[i] += 1;
      break;
    }
    values[i] += 1; // becomes 0
  }
}

bool FFREvaluation::hasNext()
{
  int n = values.max();

  for( int i=values.min(); i<=n; i++ )
  {
    if( values[i]!=start[i] )
      return true;
  }
  return false;
}

FFREvaluation& FFREvaluation::operator= ( const FFREvaluation & e )
{
  if( this != &e )
  {
    if( gen != NULL )
      delete gen;
    values = e.values;
    start = e.start;
    if( e.gen == NULL )
      gen = NULL;
    else
      gen = e.gen->clone();
  }
  return *this;
}

/* ------------------------------------------------------------------------ */
/* forward declarations: fin_ezgcd stuff*/
static bool findeval_P( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db, REvaluation & b, int delta, int degF, int degG, int bound, int & counter );
static void solveF_P ( const CFArray & P, const CFArray & Q, const CFArray & S, const CFArray & T, const CanonicalForm & C, int r, CFArray & a );
static CanonicalForm derivAndEval_P ( const CanonicalForm & f, int n, const Variable & x, const CanonicalForm & a );
static CanonicalForm checkCombination_P ( const CanonicalForm & W, const Evaluation & a, const IteratedFor & e, int k );
static CFArray findCorrCoeffs_P ( const CFArray & P, const CFArray & Q, const CFArray & P0, const CFArray & Q0, const CFArray & S, const CFArray & T, const CanonicalForm & C, const Evaluation & I, int r, int k, int h, int * n );
static bool liftStep_P ( CFArray & P, int k, int r, int t, const Evaluation & A, const CFArray & lcG, const CanonicalForm & Uk, int * n, int h );
static bool Hensel_P ( const CanonicalForm & U, CFArray & G, const CFArray & lcG, const Evaluation & A, const Variable & x );
static bool hasFirstAlgVar( const CanonicalForm & f, Variable & a );

CanonicalForm fin_ezgcd( const CanonicalForm & FF, const CanonicalForm & GG )
{
  CanonicalForm F, G, f, g, d, Fb, Gb, Db, Fbt, Gbt, Dbt, B0, B, D0, lcF, lcG, lcD;
  CFArray DD( 1, 2 ), lcDD( 1, 2 );
  int degF, degG, delta, count, maxeval;
  maxeval = getCharacteristic(); // bound on the number of eval. to use
  count = 0; // number of eval. used
  REvaluation b, bt;
  bool gcdfound = false;
  Variable x = Variable(1);
  f = content( FF, x ); g = content( GG, x ); d = gcd( f, g );
  F = FF / f; G = GG / g;
  if( F.isUnivariate() && G.isUnivariate() )
  {
    if( F.mvar() == G.mvar() )
      d *= gcd( F, G );
    return d;
  }
  if( gcd_test_one( F, G, false ) )
    return d;

  lcF = LC( F, x ); lcG = LC( G, x );
  lcD = gcd( lcF, lcG );
  delta = 0;
  degF = degree( F, x ); degG = degree( G, x );
  Variable a,bv;
  if(hasFirstAlgVar(F,a))
  {
    if(hasFirstAlgVar(G,bv))
    {
      if(bv.level() > a.level())
        a = bv;
    }
    b = REvaluation( 2, tmax(F.level(), G.level()), AlgExtRandomF( a ) );
  }
  else // F not in extension
  {
    if(hasFirstAlgVar(G,a))
      b = REvaluation( 2, tmax(F.level(), G.level()), AlgExtRandomF( a ) );
    else // both not in extension
      b = REvaluation( 2, tmax(F.level(), G.level()), FFRandom() );
  }
  while( !gcdfound )
  {
    if( !findeval_P( F, G, Fb, Gb, Db, b, delta, degF, degG, maxeval, count ))
    { // too many eval. used --> try another method
      Off( SW_USE_EZGCD_P );
      d *= gcd( F, G );
      On( SW_USE_EZGCD_P );
      return d;
    }
    delta = degree( Db );
    if( delta == 0 )
      return d;
    while( true )
    {
      bt = b;
      if( !findeval_P( F, G, Fbt, Gbt, Dbt, bt, delta+1, degF, degG, maxeval, count ))
      { // too many eval. used --> try another method
        Off( SW_USE_EZGCD_P );
        d *= gcd( F, G );
        On( SW_USE_EZGCD_P );
        return d;
      }
      int dd = degree( Dbt );
      if( dd == 0 )
        return d;
      if( dd == delta )
        break;
      if( dd < delta )
      {
        delta = dd;
        b = bt;
        Db = Dbt; Fb = Fbt; Gb = Gbt;
      }
    }
    if( degF <= degG && delta == degF && fdivides( F, G ) )
      return d*F;
    if( degG < degF && delta == degG && fdivides( G, F ) )
      return d*G;
    if( delta != degF && delta != degG )
    {
      bool B_is_F;
      CanonicalForm xxx;
      DD[1] = Fb / Db;
      xxx = gcd( DD[1], Db );
      if( xxx.inCoeffDomain() )
      {
        B = F;
        DD[2] = Db;
        lcDD[1] = lcF;
        lcDD[2] = lcF;
        B *= lcF;
        B_is_F = true;
      }
      else
      {
        DD[1] = Gb / Db;
        xxx = gcd( DD[1], Db );
        if( xxx.inCoeffDomain() )
        {
          B = G;
          DD[2] = Db;
          lcDD[1] = lcG;
          lcDD[2] = lcG;
          B *= lcG;
          B_is_F = false;
        }
        else // special case handling
        {
          Off( SW_USE_EZGCD_P );
          d *= gcd( F, G ); // try another method
          On( SW_USE_EZGCD_P );
          return d;
        }
      }
      DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
      DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );

      gcdfound = Hensel_P( B, DD, lcDD, b, x );

      if( gcdfound )
      {
        CanonicalForm cand = DD[2] / content( DD[2], Variable(1) );
        if( B_is_F )
          gcdfound = fdivides( cand, G );
        else
          gcdfound = fdivides( cand, F );
      }
    }
    delta++;
  }
  return d * DD[2] / content( DD[2],Variable(1) );
}


static bool hasFirstAlgVar( const CanonicalForm & f, Variable & a )
{
  if( f.inBaseDomain() ) // f has NO alg. variable
    return false;
  if( f.level()<0 ) // f has only alg. vars, so take the first one
  {
    a = f.mvar();
    return true;
  }
  for(CFIterator i=f; i.hasTerms(); i++)
    if( hasFirstAlgVar( i.coeff(), a ))
      return true; // 'a' is already set
  return false;
}


static bool findeval_P( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db, REvaluation & b, int delta, int degF, int degG, int maxeval, int & count )
{
  if( delta != 0 )
  {
    b.nextpoint();
    if( count++ > maxeval )
      return false; // too many eval. used
  }
  while( true )
  {
    Fb = b( F );
    if( degree( Fb ) == degF )
    {
      Gb = b( G );
      if( degree( Gb ) == degG )
      {
        Db = gcd( Fb, Gb );
        if( delta > 0 )
        {
          if( degree( Db ) < delta )
            return true;
        }
        else
          return true;
      }
    }
    b.nextpoint();
    if( count++ > maxeval ) // too many eval. used
      return false;
  }
}


static void solveF_P ( const CFArray & P, const CFArray & Q, const CFArray & S, const CFArray & T, const CanonicalForm & C, int r, CFArray & a )
{
  CanonicalForm bb, b = C;
  for ( int j = 1; j < r; j++ )
  {
    divrem( S[j] * b, Q[j], a[j], bb );
    a[j] = T[j] * b + a[j] * P[j];
    b = bb;
  }
  a[r] = b;
}


static CanonicalForm derivAndEval_P ( const CanonicalForm & f, int n, const Variable & x, const CanonicalForm & a )
{
  if ( n == 0 )
    return f( a, x );
  if ( f.degree( x ) < n )
    return 0;
  CFIterator i;
  CanonicalForm sum = 0, fact;
  int min, j;
  Variable v = Variable( f.level() + 1 );
  for ( i = swapvar( f, x, v); i.hasTerms() && i.exp() >= n; i++ )
  {
    fact = 1;
    min = i.exp() - n;
    for ( j = i.exp(); j > min; j-- )
      fact *= j;
    sum += fact * i.coeff() * power( v, min );
  }
  return sum( a, v );
}


static CFArray findCorrCoeffs_P ( const CFArray & P, const CFArray & Q, const CFArray & P0, const CFArray & Q0, const CFArray & S, const CFArray & T, const CanonicalForm & C, const Evaluation & I, int r, int k, int h, int * n )
{
  bool what;
  int i, j, m;
  CFArray A(1,r), a(1,r);
  CanonicalForm C0, Dm, g, prodcomb;
  C0 = I( C, 2, k-1 );
  solveF_P( P0, Q0, S, T, 1, r, a );
  for ( i = 1; i <= r; i++ )
    A[i] = (a[i] * C0) % P0[i];
  for ( m = 0; m <= h && ( m == 0 || Dm != 0 ); m++ )
  {
    Dm = -C;
    prodcomb = 1;
    for ( i = 1; i <= r; i++ )
    {
      Dm += prodcomb * A[i] * Q[i];
      prodcomb *= P[i];
    }
    if ( Dm != 0 )
    {
      if ( k == 2 )
      {
        solveF_P( P0, Q0, S, T, Dm, r, a );
        for ( i = 1; i <= r; i++ )
          A[i] -= a[i];
      }
      else
      {
        IteratedFor e( 2, k-1, m+1 );
        while ( e.iterations_left() )
        {
          j = 0;
          what = true;
          for ( i = 2; i < k; i++ )
          {
            j += e[i];
            if ( e[i] > n[i] )
            {
              what = false;
              break;
            }
          }
          if ( what && j == m+1 )
          {
            g = Dm;
            for ( i = k-1; i >= 2 && ! g.isZero(); i-- )
              g = derivAndEval_P( g, e[i], Variable( i ), I[i] );
            if ( ! g.isZero() )
            {
              prodcomb = 1;
              for ( i = 2; i < k; i++ )
                for ( j = 2; j <= e[i]; j++ )
                  prodcomb *= j;
              g /= prodcomb;
              if( ! (g.mvar() > Variable(1)) )
              {
                prodcomb = 1;
                for ( i = k-1; i > 1; i-- )
                  prodcomb *= binomialpower( Variable(i), -I[i], e[i] );
                solveF_P( P0, Q0, S, T, g, r, a );
                for ( i = 1; i <= r; i++ )
                  A[i] -= a[i] * prodcomb;
              }
            }
          }
          e++;
        }
      }
    }
  }
  return A;
}


static bool liftStep_P ( CFArray & P, int k, int r, int t, const Evaluation & A, const CFArray & lcG, const CanonicalForm & Uk, int * n, int h )
{
  CFArray K( 1, r ), Q( 1, r ), Q0( 1, r ), P0( 1, r ), S( 1, r ), T( 1, r ), alpha( 1, r );
  CanonicalForm Rm, C, D, xa = Variable(k) - A[k];
  CanonicalForm xa1 = xa, xa2 = xa*xa;
  int i, m;
  for ( i = 1; i <= r; i++ )
  {
    Variable vm = Variable( t + 1 );
    Variable v1 = Variable(1);
    K[i] = swapvar( replaceLc( swapvar( P[i], v1, vm ), swapvar( A( lcG[i], k+1, t ), v1, vm ) ), v1, vm );
    P[i] = A( K[i], k, t );
  }
  Q[r] = 1;
  for ( i = r; i > 1; i-- )
  {
    Q[i-1] = Q[i] * P[i];
    P0[i] = A( P[i], 2, k-1 );
    Q0[i] = A( Q[i], 2, k-1 );
    (void) extgcd( P0[i], Q0[i], S[i], T[i] );
  }
  P0[1] = A( P[1], 2, k-1 );
  Q0[1] = A( Q[1], 2, k-1 );
  (void) extgcd( P0[1], Q0[1], S[1], T[1] );
  for ( m = 1; m <= n[k]+1; m++ )
  {
    Rm = prod( K ) - Uk;
    for ( i = 2; i < k; i++ )
      Rm.mod( binomialpower( Variable(i), -A[i], n[i]+1 ) );
    if ( mod( Rm, xa2 ) != 0 )
    {
      C = derivAndEval_P( Rm, m, Variable( k ), A[k] );
      D = 1;
      for ( i = 2; i <= m; i++ )
        D *= i;
      C /= D;
      alpha = findCorrCoeffs_P( P, Q, P0, Q0, S, T, C, A, r, k, h, n );
      for ( i = 1; i <= r; i++ )
        K[i] -= alpha[i] * xa1;
    }
    xa1 = xa2;
    xa2 *= xa;
  }
  for ( i = 1; i <= r; i++ )
    P[i] = K[i];
  return prod( K ) - Uk == 0;
}


static bool Hensel_P ( const CanonicalForm & U, CFArray & G, const CFArray & lcG, const Evaluation & A, const Variable & x )
{
  int k, i, h, t = A.max();
  bool goodeval = true;
  CFArray Uk( A.min(), A.max() );
  int * n = new int[t+1];
  Uk[t] = U;
  for ( k = t-1; k > 1; k-- )
  {
    Uk[k] = Uk[k+1]( A[k+1], Variable( k+1 ) );
    n[k] = degree( Uk[k], Variable( k ) );
  }
  for ( k = A.min(); goodeval && (k <= t); k++ )
  {
    h = totaldegree( Uk[k], Variable(A.min()), Variable(k-1) );
    for ( i = A.min(); i <= k; i++ )
      n[i] = degree( Uk[k], Variable(i) );
    goodeval = liftStep_P( G, k, G.max(), t, A, lcG, Uk[k], n, h );
  }
  delete[] n;
  return goodeval;
}
