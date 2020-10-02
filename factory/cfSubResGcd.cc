#include "config.h"

#include "cf_defs.h"
#include "cf_algorithm.h"
#include "cfEzgcd.h"
#include "cfGcdUtil.h"
#include "templates/ftmpl_functions.h"
#include "cf_factory.h"
#include "cfUnivarGcd.h"

CanonicalForm
subResGCD_p( const CanonicalForm & f, const CanonicalForm & g )
{
    if (f.inCoeffDomain() || g.inCoeffDomain()) //zero case should be caught by gcd
      return 1;
    CanonicalForm pi, pi1;
    CanonicalForm C, Ci, Ci1, Hi, bi, pi2;
    bool bpure, ezgcdon= isOn (SW_USE_EZGCD_P);
    int delta = degree( f ) - degree( g );

    if ( delta >= 0 )
    {
        pi = f; pi1 = g;
    }
    else
    {
        pi = g; pi1 = f; delta = -delta;
    }
    if (pi.isUnivariate())
      Ci= 1;
    else
    {
      if (!ezgcdon)
        On (SW_USE_EZGCD_P);
      Ci = content( pi );
      if (!ezgcdon)
        Off (SW_USE_EZGCD_P);
      pi = pi / Ci;
    }
    if (pi1.isUnivariate())
      Ci1= 1;
    else
    {
      if (!ezgcdon)
        On (SW_USE_EZGCD_P);
      Ci1 = content( pi1 );
      if (!ezgcdon)
        Off (SW_USE_EZGCD_P);
      pi1 = pi1 / Ci1;
    }
    C = gcd( Ci, Ci1 );
    int d= 0;
    #ifdef HAVE_NTL // gcd_test_one, primitiveElement
    if ( !( pi.isUnivariate() && pi1.isUnivariate() ) )
    {
        if ( gcd_test_one( pi1, pi, true, d ) )
        {
          C=abs(C);
          //out_cf("GCD:",C,"\n");
          return C;
        }
        bpure = false;
    }
    else
    #endif
    {
        bpure = isPurePoly(pi) && isPurePoly(pi1);
#ifdef HAVE_FLINT
        if (bpure && (CFFactory::gettype() != GaloisFieldDomain))
          return gcd_univar_flintp(pi,pi1)*C;
#else
#ifdef HAVE_NTL
        if (bpure && (CFFactory::gettype() != GaloisFieldDomain))
            return gcd_univar_ntlp(pi, pi1 ) * C;
#endif
#endif
    }
    Variable v = f.mvar();
    Hi = power( LC( pi1, v ), delta );
    int maxNumVars= tmax (getNumVars (pi), getNumVars (pi1));

    if (!(pi.isUnivariate() && pi1.isUnivariate()))
    {
      if (size (Hi)*size (pi)/(maxNumVars*3) > 500) //maybe this needs more tuning
      {
        On (SW_USE_FF_MOD_GCD);
        C *= gcd (pi, pi1);
        Off (SW_USE_FF_MOD_GCD);
        return C;
      }
    }

    if ( (delta+1) % 2 )
        bi = 1;
    else
        bi = -1;
    CanonicalForm oldPi= pi, oldPi1= pi1, powHi;
    while ( degree( pi1, v ) > 0 )
    {
        if (!(pi.isUnivariate() && pi1.isUnivariate()))
        {
          if (size (pi)/maxNumVars > 500 || size (pi1)/maxNumVars > 500)
          {
            On (SW_USE_FF_MOD_GCD);
            C *= gcd (oldPi, oldPi1);
            Off (SW_USE_FF_MOD_GCD);
            return C;
          }
        }
        pi2 = psr( pi, pi1, v );
        pi2 = pi2 / bi;
        pi = pi1; pi1 = pi2;
        maxNumVars= tmax (getNumVars (pi), getNumVars (pi1));
        if (!pi1.isUnivariate() && (size (pi1)/maxNumVars > 500))
        {
            On (SW_USE_FF_MOD_GCD);
            C *= gcd (oldPi, oldPi1);
            Off (SW_USE_FF_MOD_GCD);
            return C;
        }
        if ( degree( pi1, v ) > 0 )
        {
            delta = degree( pi, v ) - degree( pi1, v );
            powHi= power (Hi, delta-1);
            if ( (delta+1) % 2 )
                bi = LC( pi, v ) * powHi*Hi;
            else
                bi = -LC( pi, v ) * powHi*Hi;
            Hi = power( LC( pi1, v ), delta ) / powHi;
            if (!(pi.isUnivariate() && pi1.isUnivariate()))
            {
              if (size (Hi)*size (pi)/(maxNumVars*3) > 1500) //maybe this needs more tuning
              {
                On (SW_USE_FF_MOD_GCD);
                C *= gcd (oldPi, oldPi1);
                Off (SW_USE_FF_MOD_GCD);
                return C;
              }
            }
        }
    }
    if ( degree( pi1, v ) == 0 )
    {
      C=abs(C);
      //out_cf("GCD:",C,"\n");
      return C;
    }
    if (!pi.isUnivariate())
    {
      if (!ezgcdon)
        On (SW_USE_EZGCD_P);
      Ci= gcd (LC (oldPi,v), LC (oldPi1,v));
      pi /= LC (pi,v)/Ci;
      Ci= content (pi);
      pi /= Ci;
      if (!ezgcdon)
        Off (SW_USE_EZGCD_P);
    }
    if ( bpure )
        pi /= pi.lc();
    C=abs(C*pi);
    //out_cf("GCD:",C,"\n");
    return C;
}

CanonicalForm
subResGCD_0( const CanonicalForm & f, const CanonicalForm & g )
{
    CanonicalForm pi, pi1;
    CanonicalForm C, Ci, Ci1, Hi, bi, pi2;
    int delta = degree( f ) - degree( g );

    if ( delta >= 0 )
    {
        pi = f; pi1 = g;
    }
    else
    {
        pi = g; pi1 = f; delta = -delta;
    }
    Ci = content( pi ); Ci1 = content( pi1 );
    pi1 = pi1 / Ci1; pi = pi / Ci;
    C = gcd( Ci, Ci1 );
    if ( pi.isUnivariate() && pi1.isUnivariate() )
    {
#ifdef HAVE_FLINT
        if (isPurePoly(pi) && isPurePoly(pi1) )
            return gcd_univar_flint0(pi, pi1 ) * C;
#else
#ifdef HAVE_NTL
        if (isPurePoly(pi) && isPurePoly(pi1) )
            return gcd_univar_ntl0(pi, pi1 ) * C;
#endif
#endif
#ifndef HAVE_NTL
        return gcd_poly_univar0( pi, pi1, true ) * C;
#endif
    }
    #ifdef HAVE__NTL // gcd_test_one, primitievElement
    else if ( gcd_test_one( pi1, pi, true, d ) )
      return C;
    #else
    else if (gcd(pi1,pi)==1)
      return C;
    #endif
    Variable v = f.mvar();
    Hi = power( LC( pi1, v ), delta );
    if ( (delta+1) % 2 )
        bi = 1;
    else
        bi = -1;
    while ( degree( pi1, v ) > 0 )
    {
        pi2 = psr( pi, pi1, v );
        pi2 = pi2 / bi;
        pi = pi1; pi1 = pi2;
        if ( degree( pi1, v ) > 0 )
        {
            delta = degree( pi, v ) - degree( pi1, v );
            if ( (delta+1) % 2 )
                bi = LC( pi, v ) * power( Hi, delta );
            else
                bi = -LC( pi, v ) * power( Hi, delta );
            Hi = power( LC( pi1, v ), delta ) / power( Hi, delta-1 );
        }
    }
    if ( degree( pi1, v ) == 0 )
        return C;
    else
        return C * pp( pi );
}
