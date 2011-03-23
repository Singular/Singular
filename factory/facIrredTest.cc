/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file facIrredTest.cc
 * 
 * This file implements a probabilistic irreducibility test for polynomials over
 * Z/p.
 * 
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>
#include <math.h>

#include "facIrredTest.h"
#include "cf_map.h"
#include "cf_random.h"

//returns 0 if number of zeros/k >= l
double numZeros (const CanonicalForm& F, int k)
{
  int p= getCharacteristic();
  int result= 0;

  FFRandom FFgen;
  CanonicalForm buf;
  for (int i= 0; i < k; i++)
  {
    buf= F;
    for (int j= F.level(); j > 0; j++) 
      buf= buf (FFgen.generate(), j);
    if (buf.isZero())
      result++;
  }

  return (double) result/k;
}

double inverseERF (double d)
{
  double pi= 3.141592653589793;
  double a= 0.140012288;

  double buf1;
  double buf= 2.0/(pi*a)+log (1.0-d*d)/2.0;
  buf1= buf;
  buf *= buf;
  buf -= log (1.0-d*d)/a;
  buf= sqrt (buf);
  buf -= buf1;
  buf= sqrt (buf);

  if (d < 0)
    buf= -buf;

  return buf;
}

//doesn't make much sense to use this if getCharacteristic() > 17 ?
int probIrredTest (const CanonicalForm& F, double error)
{
  CFMap N;
  CanonicalForm G= compress (F, N);
  int n= G.level();
  int p= getCharacteristic();

  double sqrtTrials= inverseERF (1-2.0*error)*sqrt (2.0);

  double s= sqrtTrials;

  double pn= pow ((double) p, (double) n);
  double p1= (double) 1/p;
  p1= p1*(1.0-p1);
  p1= p1/(double) pn;
  p1= sqrt (p1);
  p1 *= s;
  p1 += (double) 1/p;

  double p2= (double) (2*p-1)/(p*p);
  p2= p2*(1-p2);
  p2= p2/(double) pn;
  p2= sqrt (p2);
  p2 *= s;
  p2= (double) (2*p - 1)/(p*p)-p2;

  //no testing possible
  if (p2 < p1)
    return 0;

  double den= sqrt (p1*(1-p1))+sqrt (p2*(1-p2));
  double num= p2-p1;

  sqrtTrials *= den/num;

  int trials= (int) floor (pow (sqrtTrials, 2.0));

  double l= 2.0;
  double experimentalNumZeros= numZeros (G, trials);

  double pmiddle= sqrt (p1*p2);

  num= den;
  den= sqrt (p1*(1.0-p2))+sqrt (p2*(1.0-p1));
  pmiddle *= (den/num);

  if (experimentalNumZeros < pmiddle)
    return 1;
  else
    return -1;
}

