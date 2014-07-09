#include <gfanlib/gfanlib_vector.h>
#include <kernel/mod2.h>

static bool checkForNonPositiveEntries(const gfan::ZVector w)
{
  for (unsigned i=1; i<w.size(); i++)
  {
    if (w[i].sign()<=0)
    {
      std::cout << "ERROR: non-positive weight in weight vector" << std::endl
                << "weight: " << w << std::endl;
      return false;
    }
  }
  return true;
}

/***
 * Returns a strictly positive weight vector v with respect to whom
 * any x-homogeneous element is homogeneous to
 * if and only if it is homogeneous with respect to w.
 **/
gfan::ZVector nonvalued_adjustWeightForHomogeneity(const gfan::ZVector w)
{
  /* find the smallest entry min of w */
  gfan::Integer min=w[0];
  for (unsigned i=1; i<w.size(); i++)
    if (w[i]<min) min=w[i];
  /* compute w+(1-min)*(1,...,1) and return it */
  gfan::ZVector v=gfan::ZVector(w.size());
  if (min<1)
  {
    for (unsigned i=0; i<w.size(); i++)
      v[i]=w[i]-min+1;
  }
  assume(checkForNonPositiveEntries(v));
  return v;
}

gfan::ZVector valued_adjustWeightForHomogeneity(const gfan::ZVector w)
{
  /* find the biggest entry max of w
   * amongst the later entries w[1],...,w[n] */
  gfan::Integer max=w[1];
  for (unsigned i=2; i<w.size(); i++)
    if (max<w[i]) max=w[i];
  /* compute -w+(1+max)*(0,1,...,1) and return it */
  gfan::ZVector v=gfan::ZVector(w.size());
  v[0]=-w[0];
  for (unsigned i=1; i<w.size(); i++)
    v[i]=-w[i]+max+1;
  assume(checkForNonPositiveEntries(v));
  return v;
}

/***
 * Returns a weight vector v which coincides with a weight vector e
 * on any set of x-homogeneous elements that are also homogeneous with respect to w,
 * w containing only positive weights
 **/
gfan::ZVector nonvalued_adjustWeightUnderHomogeneity(const gfan::ZVector e, const gfan::ZVector w)
{
  assume(checkForNonPositiveEntries(w));
  /* find k such that e+k*w is strictly positive,
   * i.e. k such that e[i]+k*w[i] is strictly positive
   * for all i=0,...,n */
  gfan::Integer k((long)0);
  if (e[0].sign()<=0)
    k = gfan::Integer((long)1)-(gfan::Integer((long)e[0].sign())*e[0])/w[0];
  for (unsigned i=1; i<e.size(); i++)
  {
    gfan::Integer kk = gfan::Integer((long)1)-(gfan::Integer((long)e[i].sign())*e[i])/w[i];
    if (k<kk)
      k = kk;
  }
  /* compute e+k*w, check it for correctness and return it */
  gfan::ZVector v = e+k*w;
  assume(checkForNonPositiveEntries(v));
  return v;
}

gfan::ZVector valued_adjustWeightUnderHomogeneity(const gfan::ZVector e, const gfan::ZVector w)
{
  assume(checkForNonPositiveEntries(w));
  /* find k such that e+k*w is strictly positive,
   * i.e. k such that e[i]+k*w[i] is strictly positive
   * for all i=0,...,n */
  gfan::Integer k((long)0);
  if (e[0].sign()<=0)
    k = gfan::Integer((long)1)-(e[0]/w[0]);
  for (unsigned i=1; i<e.size(); i++)
  {
    gfan::Integer kk = gfan::Integer((long)1)-(gfan::Integer((long)e[i].sign())*e[i])/w[i];
    if (k<kk)
      k = kk;
  }
  /* compute e+k*w, check it for correctness and return it */
  gfan::ZVector v = e+k*w;
  assume(checkForNonPositiveEntries(v));
  return v;
}
