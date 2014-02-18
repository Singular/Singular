#include <gfanlib/gfanlib_vector.h>

/***
 * Returns a strictly positive weight vector v with respect to whom
 * any x-homogeneous element is homogeneous to
 * if and only if it is homogeneous with respect to w.
 **/
gfan::ZVector nonvalued_adjustWeightForHomogeneity(const gfan::ZVector w)
{
  gfan::ZVector v=gfan::ZVector(w.size());
  gfan::Integer min=-1;
  for (unsigned i=1; i<w.size(); i++)
    if (w[i]<min) min=w[i];
  if (min<1)
  {
    for (unsigned i=1; i<w.size(); i++)
      v[i]=w[i]-min+1;
  }
  return v;
}

gfan::ZVector valued_adjustWeightForHomogeneity(const gfan::ZVector w)
{
  gfan::ZVector v=gfan::ZVector(w.size());
  v[0]=-w[0];
  gfan::Integer max=-1;
  for (unsigned i=1; i<w.size(); i++)
    if (max<w[i]) max=w[i];
  for (unsigned i=1; i<w.size(); i++)
    v[i]=-w[i]+max+1;
  return v;
}

/***
 * Returns a weight vector v which coincides with a weight vector e
 * on any set of x-homogeneous elements that are also homogeneous with respect to w,
 * w containing only positive weights
 **/
gfan::ZVector nonvalued_adjustWeightUnderHomogeneity(const gfan::ZVector e, const gfan::ZVector w)
{
  gfan::ZVector v=gfan::ZVector(e.size());
  gfan::Integer min=-1;
  for (unsigned i=1; i<e.size(); i++)
    if (e[i]<min) min=w[i];
  if (min<1)
  {
    for (unsigned i=1; i<e.size(); i++)
      v[i]=e[i]-min+1;
  }
  return v;
}

gfan::ZVector valued_adjustWeightUnderHomogeneity(const gfan::ZVector e, const gfan::ZVector w)
{
  gfan::ZVector v=e-(e[1]/w[1]+1)*w;
  gfan::Integer min=1;
  for (unsigned i=1; i<v.size(); i++)
    if (v[i]<min) min=v[i];
  for (unsigned i=1; i<v.size(); i++)
    v[i]=v[i]-min+1;
  return v;
}
