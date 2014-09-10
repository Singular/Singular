#include <witness.h>

/***
 * Suppose r and s are the same ring but with two adjacent orderings,
 * w is a weight vector in the relative interior of their common facet.
 * Given a standard basis of an ideal I with respect to the ordering of r,
 * a standard basis of its w-initial ideal inI with respect to
 * the ordering of s, computes a standard basis of I with respect to
 * the ordering of s.
 **/
// ideal lift(const ideal I, const ring r, const ideal inI, const ring s)
// {
//   nMapFunc identity = n_SetMap(r->cf,s->cf);
//   int k = idSize(I); ideal Is = idInit(k);
//   for (int i=0; i<k; i++)
//     Is->m[i] = p_PermPoly(I->m[i],NULL,r,s,identity,NULL,0);
//   ideal J = idInit(k);
//   for (int i=0; i<k; i++)
//     J->m[i] = witness(inI->m[i],Is,inI,s);
//   id_Delete(&Is,s);
//   return J;
// }

ideal lift(const ideal J, const ring r, const ideal inI, const ring s)
{
  nMapFunc identity = n_SetMap(s->cf,r->cf);
  int k = idSize(inI); ideal inIr = idInit(k);
  for (int i=0; i<k; i++)
    inIr->m[i] = p_PermPoly(inI->m[i],NULL,s,r,identity,NULL,0);
  ideal Ir = witness(inIr,J,r);

  identity = n_SetMap(r->cf,s->cf);
  ideal Is = idInit(k);
  for (int i=0; i<k; i++)
    Is->m[i] = p_PermPoly(Ir->m[i],NULL,r,s,identity,NULL,0);

  id_Delete(&inIr,r);
  id_Delete(&Ir,r);
  return Is;
}
