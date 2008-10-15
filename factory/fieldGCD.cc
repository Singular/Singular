#include <stdio.h>
#include <config.h>
#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "algext.h"
#include "fieldGCD.h"
#include "cf_map.h"
#include "cf_generator.h"

void out_cf(char *s1,const CanonicalForm &f,char *s2);


CanonicalForm fieldGCD( const CanonicalForm & F, const CanonicalForm & G );
void CRA(const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew);


CanonicalForm fieldGCD( const CanonicalForm & F, const CanonicalForm & G )
{// this is the modular method by Brown
 // assume F,G are multivariate polys over Z/p for big prime p
  if(F.isZero())
  {
    if(G.isZero())
      return G; // G is zero
    if(G.inCoeffDomain())
      return CanonicalForm(1);
    return G/lc(G); // return monic G
  }
  if(G.isZero()) // F is non-zero
  {
    if(F.inCoeffDomain())
      return CanonicalForm(1);
    return F/lc(F); // return monic F
  }
  if(F.inCoeffDomain() || G.inCoeffDomain())
    return CanonicalForm(1);
  //out_cf("F=",F,"\n");
  //out_cf("G=",G,"\n");
  CFMap MM,NN;
  CFArray ps(1,2);
  ps[1] = F;
  ps[2] = G;
  compress(ps,MM,NN); // maps MM, NN are created
  CanonicalForm f=MM(F);
  CanonicalForm g=MM(G);
  // here: f, g are compressed
  // compute largest variable in f or g (least one is Variable(1))
  int mv = f.level();
  if(g.level() > mv)
    mv = g.level();
  // here: mv is level of the largest variable in f, g
  if(mv == 1) // f,g univariate
    return NN(gcd( f, g )); // do not forget to map back
  // here: mv > 1
  CanonicalForm cf = vcontent(f, Variable(2)); // cf is univariate poly in var(1)
  CanonicalForm cg = vcontent(g, Variable(2));
  CanonicalForm c = gcd( cf, cg );
  f/=cf;
  g/=cg;
  if(f.inCoeffDomain() || g.inCoeffDomain())
  {
    //printf("=============== inCoeffDomain\n");
    return NN(c);
  }
  int *L = new int[mv+1]; // L is addressed by i from 2 to mv
  int *M = new int[mv+1];
  for(int i=2; i<=mv; i++)
    L[i] = M[i] = 0;
  L = leadDeg(f, L);
  M = leadDeg(g, M);
  CanonicalForm gamma = gcd( firstLC(f), firstLC(g) );
  for(int i=2; i<=mv; i++) // entry at i=1 is not visited
    if(M[i] < L[i])
      L[i] = M[i];
  // L is now upper bound for leading degrees of gcd
  int *dg_im = new int[mv+1]; // for the degree vector of the image we don't need any entry at i=1
  for(int i=2; i<=mv; i++)
    dg_im[i] = 0; // initialize
  CanonicalForm gamma_image, m=1;
  CanonicalForm gm=0;
  CanonicalForm g_image, alpha, gnew, mnew;
  FFGenerator gen = FFGenerator();
  for(FFGenerator gen = FFGenerator(); gen.hasItems(); gen.next())
  {
    alpha = gen.item();
    gamma_image = gamma(alpha, Variable(1)); // plug in alpha for var(1)
    if(gamma_image.isZero()) // skip lc-bad points var(1)-alpha
      continue;
    g_image = gcd( f(alpha, Variable(1)), g(alpha, Variable(1)) ); // recursive call with one var less
    if(g_image.inCoeffDomain()) // early termination
    {
      //printf("================== inCoeffDomain\n");
      return NN(c);
    }
    for(int i=2; i<=mv; i++)
      dg_im[i] = 0; // reset (this is necessary, because some entries may not be updated by call to leadDeg)
    dg_im = leadDeg(g_image, dg_im); // dg_im cannot be NIL-pointer
    if(isEqual(dg_im, L, 2, mv))
    {
      g_image /= lc(g_image); // make g_image monic
      g_image *= gamma_image; // multiply by multiple of image lc(gcd)
      CRA( g_image, Variable(1)-alpha, gm, m, gnew, mnew );
      // gnew = gm mod m
      // gnew = g_image mod var(1)-alpha
      // mnew = m * (var(1)-alpha)
      m = mnew;
      if(gnew == gm) // gnew did not change
      {
        g_image = gm / vcontent(gm, Variable(2));
        //out_cf("=========== try ",g_image,"\n");
        if(fdivides(g_image,f) && fdivides(g_image,g)) // trial division
        {
          //printf("=========== okay\n");
          return NN(c*g_image);
        }
      }
      gm = gnew;
      continue;
    }
    if(isLess(L, dg_im, 2, mv)) // dg_im > L --> current point unlucky
      continue;

    // here: dg_im < L --> all previous points were unlucky
    //printf("=========== reset\n");
    m = CanonicalForm(1); // reset
    gm = 0; // reset
    for(int i=2; i<=mv; i++) // tighten bound
      L[i] = dg_im[i];
  }
  // hopefully, we never reach this point
  Off( SW_USE_fieldGCD );
  g_image = gcd(f,g);
  On( SW_USE_fieldGCD );
  return g_image;
}


void CRA(const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew)
{ // this is polynomial Chinese Remaindering. q1, q2 are assumed to be coprime.
  // polynomials of level <= 1 are considered coefficients
  // xnew = x1 mod q1 (coefficientwise in the above sense)
  // xnew = x2 mod q2
  // qnew = q1*q2
  if(x1.level() <= 1 && x2.level() <= 1) // base case
  {
    (void) extgcd(q1,q2,xnew,qnew);
    xnew = x1 + (x2-x1) * xnew * q1;
    qnew = q1*q2;
    xnew = mod(xnew,qnew);
    return;
  }
  CanonicalForm tmp,tmp2;
  xnew = 0;
  qnew = q1 * q2;
  // here: x1.level() > 1 || x2.level() > 1
  if(x1.level() > x2.level())
  {
    for(CFIterator i=x1; i.hasTerms(); i++)
    {
      if(i.exp() == 0) // const. term
      {
        CRA(i.coeff(),q1,x2,q2,tmp,tmp2);
        xnew += tmp;
      }
      else
      {
        CRA(i.coeff(),q1,0,q2,tmp,tmp2);
        xnew += tmp * power(x1.mvar(),i.exp());
      }
    }
    return;
  }
  // here: x1.level() <= x2.level() && ( x1.level() > 1 || x2.level() > 1 )
  if(x2.level() > x1.level())
  {
    for(CFIterator j=x2; j.hasTerms(); j++)
    {
      if(j.exp() == 0) // const. term
      {
        CRA(x1,q1,j.coeff(),q2,tmp,tmp2);
        xnew += tmp;
      }
      else
      {
        CRA(0,q1,j.coeff(),q2,tmp,tmp2);
        xnew += tmp * power(x2.mvar(),j.exp());
      }
    }
    return;
  }
  // here: x1.level() == x2.level() && x1.level() > 1 && x2.level() > 1
  CFIterator i = x1;
  CFIterator j = x2;
  while(i.hasTerms() || j.hasTerms())
  {
    if(i.hasTerms())
    {
      if(j.hasTerms())
      {
        if(i.exp() == j.exp())
        {
          CRA(i.coeff(),q1,j.coeff(),q2,tmp,tmp2);
          xnew += tmp * power(x1.mvar(),i.exp());
          i++; j++;
        }
        else
        {
          if(i.exp() < j.exp())
          {
            CRA(i.coeff(),q1,0,q2,tmp,tmp2);
            xnew += tmp * power(x1.mvar(),i.exp());
            i++;
          }
          else // i.exp() > j.exp()
          {
            CRA(0,q1,j.coeff(),q2,tmp,tmp2);
            xnew += tmp * power(x1.mvar(),j.exp());
            j++;
          }
        }
      }
      else // j is out of terms
      {
        CRA(i.coeff(),q1,0,q2,tmp,tmp2);
        xnew += tmp * power(x1.mvar(),i.exp());
        i++;
      }
    }
    else // i is out of terms
    {
      CRA(0,q1,j.coeff(),q2,tmp,tmp2);
      xnew += tmp * power(x1.mvar(),j.exp());
      j++;
    }
  }
}
