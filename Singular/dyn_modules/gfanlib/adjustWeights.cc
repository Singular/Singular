#include <gfanlib/gfanlib_vector.h>
#include <kernel/mod2.h>


#ifndef NDEBUG
static bool checkForNonPositiveEntries(const gfan::ZVector &w)
{
  for (unsigned i=0; i<w.size(); i++)
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

static bool checkForNonPositiveLaterEntries(const gfan::ZVector &w)
{
  for (unsigned i=1; i<w.size(); i++)
  {
    if (w[i].sign()<=0)
    {
      std::cout << "ERROR: non-positive weight in weight vector later entries" << std::endl
                << "weight: " << w << std::endl;
      return false;
    }
  }
  return true;
}
#endif //NDEBUG

/***
 * Returns a strictly positive weight vector v with respect to whom
 * any x-homogeneous element is homogeneous to
 * if and only if it is homogeneous with respect to w.
 **/
gfan::ZVector nonvalued_adjustWeightForHomogeneity(const gfan::ZVector &w)
{
  /* find the smallest entry min of w */
  gfan::Integer min=w[0];
  for (unsigned i=1; i<w.size(); i++)
    if (w[i]<min) min=w[i];

  if (min.sign()>0)
    return w;

  /* compute w+(1-min)*(1,...,1) and return it */
  gfan::ZVector v=gfan::ZVector(w.size());
  for (unsigned i=0; i<w.size(); i++)
    v[i]=w[i]-min+1;
  assume(checkForNonPositiveEntries(v));
  return v;
}

gfan::ZVector valued_adjustWeightForHomogeneity(const gfan::ZVector &w)
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
  assume(checkForNonPositiveLaterEntries(v));
  return v;
}

/***
 * Returns a weight vector v which coincides with a weight vector e
 * on any set of x-homogeneous elements that are also homogeneous with respect to w,
 * w containing only positive weights
 **/
gfan::ZVector nonvalued_adjustWeightUnderHomogeneity(const gfan::ZVector &e, const gfan::ZVector &/*w*/)
{
  /* since our ideal is homogeneous, we can basically do the same as before */
  /* find the smallest entry min of w */
  gfan::Integer min=e[0];
  for (unsigned i=1; i<e.size(); i++)
    if (e[i]<min) min=e[i];

  if (min.sign()>0)
    return e;

  /* compute w+(1-min)*(1,...,1) and return it */
  gfan::ZVector v=gfan::ZVector(e.size());
  for (unsigned i=0; i<e.size(); i++)
    v[i]=e[i]-min+1;
  assume(checkForNonPositiveEntries(v));
  return v;
}

gfan::ZVector valued_adjustWeightUnderHomogeneity(const gfan::ZVector &e, const gfan::ZVector &w)
{
  assume(checkForNonPositiveLaterEntries(w));

  /* find k such that e+k*w is strictly positive,
   * i.e. k such that e[i]+k*w[i] is strictly positive
   * for all i=0,...,n */
  gfan::Integer k((long)0);
  if (e[0].sign()<=0 && w[0].sign()>0)
    k = gfan::Integer((long)1)-(e[0]/w[0]);
  for (unsigned i=1; i<e.size(); i++)
  {
    if (e[i].sign()<=0)
    {
      gfan::Integer kk = gfan::Integer((long)1)-(e[i]/w[i]);
      if (k<kk)
        k = kk;
    }
  }

  /* compute e+k*w, check it for correctness and return it */
  gfan::ZVector v = e+k*w;
  assume(checkForNonPositiveLaterEntries(v));
  return v;
}
