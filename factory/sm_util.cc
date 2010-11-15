/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// sm_util.cc - utlities for sparse modular gcd.
//
// Dependencies: Routines used by and only by sm_sparsemod.cc.
//
// Contributed by Marion Bruder <bruder@math.uni-sb.de>.
//
//}}}

#include <config.h>

#include "assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "cf_algorithm.h"
#include "cf_iter.h"
#include "cf_reval.h"
#include "canonicalform.h"
#include "variable.h"
#include "ftmpl_array.h"

//{{{ static CanonicalForm fmonome( const CanonicalForm & f  )
//{{{ docu
//
// fmonome() - return the leading monomial of a poly.
//
// As in Leitkoeffizient(), the leading monomial is calculated
// with respect to inCoeffDomain().  The leading monomial is
// returned with coefficient 1.
//
//}}}
static CanonicalForm
fmonome( const CanonicalForm & f  )
{
  if ( f.inCoeffDomain() )
    {
      return 1;
    }
  else
    {
      CFIterator J = f;
      CanonicalForm  result;
      result = power( f.mvar() , J.exp() ) * fmonome( J.coeff() );
      return result;
    }
}
//}}}

//{{{ static CanonicalForm interpol( const CFArray & values, const CanonicalForm & point, const CFArray & points, const Variable & x, int d, int CHAR )
//{{{ docu
//
// interpol() - Newton interpolation.
//
// Calculate f in x such that f(point) = values[1],
// f(points[i]) = values[i], i=2, ..., d+1.
// Make sure that you are calculating in a field.
//
// alpha: the values at the interpolation points (= values)
// punkte: the point at which we interpolate (= (point, points))
//
//}}}
static CanonicalForm
interpol( const CFArray & values, const CanonicalForm & point, const CFArray & points, const Variable & x, int d, int CHAR )
{
  CFArray alpha( 1, d+1 );
  int i;
  for ( i = 1 ; i <= d+1 ; i++ )
    {
      alpha[i] = values[i];
    }

  int k, j;
  CFArray punkte( 1 , d+1 );
  for ( i = 1 ; i <= d+1 ; i++ )
    {
      if ( i == 1 )
        {
          punkte[i] = point;
        }
      else
        {
          punkte[i] = points[i-1];
        }
    }

  // calculate Newton coefficients alpha[i]
  for ( k = 2 ; k <= d+1 ; k++ )
    {
      for ( j = d+1 ; j >= k ; j-- )
        {
          alpha[j] = (alpha[j] - alpha[j-1]) / (punkte[j] - punkte[j-k+1]);
        }
    }

  // calculate f from Newton coefficients
  CanonicalForm f = alpha [1], produkt = 1;
  for ( i = 1 ; i <= d ; i++ )
    {
      produkt *= ( x - punkte[i] );
      f += ( alpha[i+1] * produkt ) ;
    }

  return f;
}
//}}}

//{{{ int countmonome( const CanonicalForm & f )
//{{{ docu
//
// countmonome() - count the number of monomials in a poly.
//
// As in Leitkoeffizient(), the number of monomials is calculated
// with respect to inCoeffDomain().
//
//}}}
int
countmonome( const CanonicalForm & f )
{
  if ( f.inCoeffDomain() )
    {
      return 1;
    }
  else
    {
      CFIterator I = f;
      int result = 0;

      while ( I.hasTerms() )
        {
          result += countmonome( I.coeff() );
          I++;
        }
      return result;
    }
}
//}}}

//{{{ CanonicalForm Leitkoeffizient( const CanonicalForm & f )
//{{{ docu
//
// Leitkoeffizient() - get the leading coefficient of a poly.
//
// In contrary to the method lc(), the leading coefficient is calculated
// with respect to to the method inCoeffDomain(), so that a poly over an
// algebraic extension will have a leading coefficient in this algebraic
// extension (and *not* in its groundfield).
//
//}}}
CanonicalForm
Leitkoeffizient( const CanonicalForm & f )
{
  if ( f.inCoeffDomain() )
    return f;
  else
    {
      CFIterator J = f;
      CanonicalForm result;
      result = Leitkoeffizient( J.coeff() );
      return result;
    }
}
//}}}

//{{{ void ChinesePoly( int arraylength, const CFArray & Polys, const CFArray & primes, CanonicalForm & result )
//{{{ docu
//
// ChinesePoly - chinese remaindering mod p.
//
// Given n=arraylength polynomials Polys[1] (mod primes[1]), ...,
// Polys[n] (mod primes[n]), we calculate result such that
// result = Polys[i] (mod primes[i]) for all i.
//
// Note: We assume that all monomials which occure in Polys[2],
// ..., Polys[n] also occure in Polys[1].
//
// bound: number of monomials of Polys[1]
// mono: array of monomials of Polys[1].  For each monomial, we
//   get the coefficients of this monomial in all Polys, store them
//   in koeffi and do chinese remaindering over these coeffcients.
//   The resulting polynomial is constructed monomial-wise from
//   the results.
// polis: used to trace through monomials of Polys
// h: result of remaindering of koeffi[1], ..., koeffi[n]
// Primes: do we need that?
//
//}}}
void
ChinesePoly( int arraylength, const CFArray & Polys, const CFArray & primes, CanonicalForm & result )
{
  DEBINCLEVEL( cerr, "ChinesePoly" );

  CFArray koeffi( 1, arraylength ), polis( 1, arraylength );
  CFArray Primes( 1, arraylength );
  int bound = countmonome( Polys[1] );
  CFArray mono( 1, bound );
  int i, j;
  CanonicalForm h, unnecessaryforme;

  DEBOUTLN( cerr, "Interpolating " << Polys );
  DEBOUTLN( cerr, "modulo" << primes );
  for ( i = 1 ; i <= arraylength ; i++ )
    {
      polis[i]  = Polys[i];
      Primes[i] = primes[i];
    }

  for ( i = 1 ; i <= bound ; i++ )
    {
      mono[i] = fmonome( polis[1] );
      koeffi[1] = lc( polis[1] );                // maybe better use Leitkoeffizient ??
      polis[1] -= mono[i] * koeffi[1];
      for ( j = 2 ; j <= arraylength ; j++ )
        {
          koeffi[j] = lc( polis[j] );                // see above
          polis[j] -= mono[i] * koeffi[j];
        }

      // calculate interpolating poly for each monomial
      chineseRemainder( koeffi, Primes, h, unnecessaryforme );
      result += h * mono[i];
    }
  DEBOUTLN( cerr, "result = " << result );

  DEBDECLEVEL( cerr, "ChinesePoly" );
}
//}}}

//{{{ CanonicalForm dinterpol( int d, const CanonicalForm & gi, const CFArray & zwischen, const REvaluation & alpha, int s, const CFArray & beta, int ni, int CHAR )
//{{{ docu
//
// dinterpol() - calculate interpolating poly (???).
//
// Calculate f such that f is congruent to gi mod (x_s - alpha_s) and
// congruent to zwischen[i] mod (x_s - beta[i]) for all i.
//
//}}}
CanonicalForm
dinterpol( int d, const CanonicalForm & gi, const CFArray & zwischen, const REvaluation & alpha, int s, const CFArray & beta, int ni, int CHAR )
{
  int i, j, lev = s;
  Variable x( lev );

  CFArray polis( 1, d+1 );
  polis[1] = gi;
  for ( i = 2 ; i <= d+1 ; i++ )
    {
      polis[i] = zwischen[i-1];
    }

  CFArray mono( 1, ni ), koeffi( 1, d+1 );
  CanonicalForm h , f = 0;

  for ( i = 1 ; i <= ni ; i++ )
    {
      mono[i] = fmonome( polis[1] );

      koeffi[1] = Leitkoeffizient( polis[1] );
      polis[1] -= mono[i] * koeffi[1];

      for ( j = 2 ; j <= d+1 ; j++ )
        {
          koeffi[j] = Leitkoeffizient( polis[j] );
          polis[j] -= mono[i] * koeffi[j];
        }

      // calculate interpolating poly for each monomial
      h = interpol( koeffi, alpha[s] , beta, x , d , CHAR );

      f += h * mono[i];
    }

  return f;
}
//}}}

//{{{ CanonicalForm sinterpol( const CanonicalForm & gi, const Array<REvaluation> & xi, CanonicalForm* zwischen, int n )
//{{{ docu
//
// sinterpol - sparse interpolation (???).
//
// Loese Gleichungssystem:
// x[1], .., x[q]( Tupel ) eingesetzt fuer die Variablen in gi ergibt
// zwischen[1], .., zwischen[q]
//
//}}}
CanonicalForm
sinterpol( const CanonicalForm & gi, const Array<REvaluation> & xi, CanonicalForm* zwischen, int n )
{
  CanonicalForm f = gi;
  int i, j;
  CFArray mono( 1 , n );

  //  mono[i] is the i'th monomial
  for ( i = 1 ; i <= n ; i++ )
    {
      mono[i] = fmonome( f );
      f -= mono[i]*Leitkoeffizient(f);
    }

  //  fill up matrix a
  CFMatrix a( n , n + 1 );
  for ( i = 1 ; i <= n ; i++ )
    for ( j = 1 ; j <= n + 1 ; j++ )
      {
        if ( j == n+1 )
          {
            a[i][j] = zwischen[i];
          }
        else
          {
            a[i][j] = xi[i]( mono[j] );
          }
      }

  // sove a*y=zwischen and get soultions y1, .., yn mod p
  linearSystemSolve( a );

  for ( i = 1 ; i <= n ; i++ )
    f += a[i][n+1] * mono[i];

  return f;
}
//}}}
