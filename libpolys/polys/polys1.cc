/*2
*returns TRUE if p1 is a skalar multiple of p2
*assume p1 != NULL and p2 != NULL
*/
BOOLEAN pComparePolys(poly p1,poly p2)
{
  number n,nn;
  pAssume(p1 != NULL && p2 != NULL);

  if (!pLmEqual(p1,p2)) //compare leading mons
      return FALSE;
  if ((pNext(p1)==NULL) && (pNext(p2)!=NULL))
     return FALSE;
  if ((pNext(p2)==NULL) && (pNext(p1)!=NULL))
     return FALSE;
  if (pLength(p1) != pLength(p2))
    return FALSE;
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    if (!nDivBy(pGetCoeff(p1), pGetCoeff(p2))) return FALSE;
  }
#endif
  n=nDiv(pGetCoeff(p1),pGetCoeff(p2));
  while ((p1 != NULL) /*&& (p2 != NULL)*/)
  {
    if ( ! pLmEqual(p1, p2))
    {
        nDelete(&n);
        return FALSE;
    }
    if (!nEqual(pGetCoeff(p1),nn=nMult(pGetCoeff(p2),n)))
    {
      nDelete(&n);
      nDelete(&nn);
      return FALSE;
    }
    nDelete(&nn);
    pIter(p1);
    pIter(p2);
  }
  nDelete(&n);
  return TRUE;
}
