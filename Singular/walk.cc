/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: walk.cc,v 1.3 2000-08-14 12:56:57 obachman Exp $ */
/*
* ABSTRACT: Implementation of the Groebner walk
*/

#include "mod2.h"
#include "walk.h"
#include "polys.h"
#include "ideals.h"
#include "intvec.h"
#include "ipid.h"


// scalar product of weights and exponent vector of p
// assumes that weights and exponent vector have length n
inline long walkWeightDegree(const poly p, const int* weights,
                             const long n)
{
  assume(p != NULL && weights != NULL);

  long i, res = 0;

  for (i=0; i<n; i++) res += pGetExp(p, i+1) * weights[i];

  return res;
}


// returns gcd of integers a and b
inline long gcd(const long a, const long b)
{
  long r, p0 = a, p1 = b;
  assume(p0 >= 0 && p1 >= 0);

  while(p1 != 0)
  {
    r = p0 % p1;
    p0 = p1;
    p1 = r;
  }
  return p0;
}

// cancel gcd of integers zaehler and nenner
inline void cancel(long &zaehler, long &nenner)
{
  assume(zaehler >= 0 && nenner > 0);
  long g = gcd(zaehler, nenner);
  if (g > 1)
  {
    zaehler = zaehler / g;
    nenner = nenner / g;
  }
}

// Returns the next Weight vector for the Groebner walk
// Assumes monoms of polys of G are ordered decreasingly w.r.t. curr_weight
int* walkNextWeight(const int* curr_weight,
                    const int* target_weight,
                    const ideal G)
{
  assume(currRing != NULL && target_weight != NULL && curr_weight != NULL &&
         G != NULL);

  int* diff_weight =
    (int*)omAlloc(currRing->N*sizeof(int));
  long j, t_zaehler = 0, t_nenner = 0;

  for (j=0; j<currRing->N; j++)
    diff_weight[j] = target_weight[j] - curr_weight[j];

  for (j=0; j<IDELEMS(G); j++)
  {
    poly g = G->m[j];
    if (g != 0)
    {
      long deg_w0_p1 = pGetOrder(g);
      long deg_d0_p1 = walkWeightDegree(g, diff_weight, currRing->N);

      pIter(g);

      while (g != NULL)
      {
        // compute s = s_zahler / s_nenner
        long s_zaehler = deg_w0_p1 - pGetOrder(g);

        if (s_zaehler != 0)
        {
          long s_nenner =
            walkWeightDegree(g, diff_weight, currRing->N) - deg_d0_p1;
          // check for 0 < s <= 1
          if ( (s_zaehler > 0 && s_nenner >= s_zaehler) ||
               (s_zaehler < 0 && s_nenner <= s_zaehler) )
          {
            // make both positive
            if (s_zaehler < 0)
            {
              s_zaehler = - s_zaehler;
              s_nenner = - s_nenner;
            }

            // look whether s < t
            if (t_nenner == 0 ||
                s_zaehler*t_nenner < t_zaehler * s_nenner)
            {
              cancel(s_zaehler, s_nenner);
              t_zaehler = s_zaehler;
              t_nenner = s_nenner;
            }
          }
        }
        pIter(g);
      }
    }
  }

  // return if no t or if t == 1
  if (t_nenner == 0 || t_nenner == 1)
  {
    omFreeSize(diff_weight, currRing->N*sizeof(int));
    return  (int*) t_nenner;
  }

  // construct new weight vector
  for (j=0; j<currRing->N; j++)
    diff_weight[j] = t_nenner*curr_weight[j] + t_zaehler*diff_weight[j];
  // and take out the content
  long temp = diff_weight[0];

  for (j=1; j<currRing->N && temp != 1; j++)
  {
    temp = gcd(temp, diff_weight[j]);
    if (temp == 1) goto Finish;
  }

  for (j=0; j<currRing->N; j++)
      diff_weight[j] = diff_weight[j] / temp;

  Finish:
  return diff_weight;
}


// next weight vector given weights as intvecs
intvec* walkNextWeight(intvec* curr_weight, intvec* target_weight, ideal G)
{
  assume(curr_weight->length() == currRing->N);
  assume(target_weight->length() == currRing->N);

  int* nw = walkNextWeight(curr_weight->ivGetVec(),
                           target_weight->ivGetVec(),
                           G);
  intvec* next_weight;

  if (nw != NULL && nw != (int*) 1)
  {
    next_weight = new intvec(currRing->N);
    int *nw_i = next_weight->ivGetVec();
    int i;

    for (i=0; i<currRing->N; i++)
      nw_i[i] = nw[i];
    omFreeSize(nw, (currRing->N)*sizeof(int));
  }
  else
  {
    next_weight = (intvec*) nw;
  }

  return next_weight;
}


// returns ideals of initials (w.r.t. curr_weight) of ideal G
// assumes that monoms are ordered by descending W-degree (w.r.t curr_weight)

poly walkInitials(poly p)
{
  assume(p != NULL);

  poly pi = pHead(p);
  poly pr = pi;
  long d_lm = pGetOrder(p);

  pIter(p);

  while (p != NULL && pGetOrder(p) == d_lm)
  {
    pNext(pi) = pHead(p);
    pIter(pi);
    pIter(p);
  }

  assume(p == NULL || pGetOrder(p) < d_lm);

  pNext(pi) = NULL;
  pTest(pr);
  return pr;
}

ideal walkInitials(ideal G)
{
  ideal GI = idInit(IDELEMS(G),1);
  int i;

  for (i=0; i<IDELEMS(G); i++)
    GI->m[i] = walkInitials(G->m[i]);

  return GI;
}
