/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prCopyTemplate.cc,v 1.1.1.1 2003-10-06 12:16:02 Singular Exp $ */
/*
* ABSTRACT - templates for pr routines
*/


static poly PR_NAME
(poly &src, ring r_src, ring r_dest)
{
  spolyrec dest_s;
  poly dest = &dest_s;
  poly tmp;
  PR_INIT_EVECTOR_COPY(r_src, r_dest);

  while (src != NULL)
  {
    pNext(dest) = (poly) PR_ALLOC_MONOM(r_dest);
    pIter(dest);
    pSetCoeff0(dest, PR_NCOPY(pGetCoeff(src), r_src));
    PR_CPY_EVECTOR(dest, r_dest, src, r_src);
    tmp = pNext(src);
    PR_DELETE_MONOM(src, r_src);
    src = tmp;
  }
  pNext(dest) = NULL;
  dest = pNext(&dest_s);
  PR_SORT_POLY(dest, r_dest, r_src);
  p_Test(dest, r_dest);
  return dest;
}
