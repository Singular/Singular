/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_map.h"
#include "cf_algorithm.h"

static int
compareFactors( const CFFactor & f, const CFFactor & g )
{
    return f.exp() > g.exp();
}

CFFList
sortCFFList( CFFList & F )
{
    F.sort( compareFactors );

    int exp;
    CanonicalForm f;
    CFFListIterator I = F;
    CFFList result;

    // join elements with the same degree
    while ( I.hasItem() ) {
        f = I.getItem().factor();
        exp = I.getItem().exp();
        I++;
        while ( I.hasItem() && I.getItem().exp() == exp ) {
            f *= I.getItem().factor();
            I++;
        }
        result.append( CFFactor( f, exp ) );
    }

    return result;
}

CFFList sqrFreeZ ( const CanonicalForm & a )
{
    if ( a.inCoeffDomain() )
        return CFFactor( a, 1 );
    CanonicalForm aa, LcA;
    if (isOn (SW_RATIONAL))
    {
      LcA= bCommonDen (a);
      aa= a*LcA;
    }
    else
    {
      LcA= icontent (a);
      if (lc (a).sign() < 0)
        LcA= -LcA;
      aa= a/LcA;
    }
    CanonicalForm cont = content( aa );
    aa /= cont;
    CanonicalForm b = aa.deriv(), c = gcd( aa, b );
    CanonicalForm y, z, w = aa / c;
    int i = 1;
    CFFList F;
    Variable v = aa.mvar();
    CanonicalForm lcinv;
    while ( ! c.degree(v) == 0 )
    {
        y = gcd( w, c ); z = w / y;
        if ( degree( z, v ) > 0 )
        {
          if (isOn (SW_RATIONAL))
          {
            lcinv= 1/Lc (z);
            z *= lcinv;
            z *= bCommonDen (z);
          }
          if (lc (z).sign() < 0)
            z= -z;
          F.append( CFFactor( z, i ) );
        }
        i++;
        w = y; c = c / y;
    }
    if ( degree( w,v ) > 0 )
    {
      if (isOn (SW_RATIONAL))
      {
        lcinv= 1/Lc (w);
        w *= lcinv;
        w *= bCommonDen (w);
      }
      if (lc (w).sign() < 0)
        w= -w;
      F.append( CFFactor( w, i ) );
    }
    if ( ! cont.isOne() )
    {
        CFFList buf= sqrFreeZ (cont);
        buf.removeFirst();
        F = Union( F, buf );
    }
    F.insert (CFFactor (LcA, 1));
    return F;
}

CanonicalForm 
sqrfPart (const CanonicalForm& F)
{
  if (F.inCoeffDomain())
    return F;
  CFMap M;
  CanonicalForm A= compress (F, M);
  CanonicalForm w, v, b;
  CanonicalForm result;
  int i= 1;
  for (; i <= A.level(); i++)
  {
    if (!deriv (A, Variable (i)).isZero())
      break;
  }

  w= gcd (A, deriv (A, Variable (i)));
  b= A/w;
  result= b;
  if (degree (w) < 1)
    return M (result);
  i++;
  for (; i <= A.level(); i++)
  {
    if (!deriv (w, Variable (i)).isZero())
    {
      b= w;
      w= gcd (w, deriv (w, Variable (i)));
      b /= w;
      if (degree (b) < 1)
        break;
      CanonicalForm g= gcd (b, result);
      if (degree (g) > 0)
        result *= b/g;
      if (degree (g) <= 0)
        result *= b;
    }
  }
  result= M (result);
  return result;
}

