/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - templates for pr routines
*/


static poly PR_NAME
(poly &src, const ring r_src, const ring r_dest)
{
  if (src==NULL) return NULL;

  /* PrintS("src: "); p_Write(src, r_src); PrintLn(); */

  //  p_Test(src, r_src); // may fail due to wrong monomial order
  spolyrec dest_s;

  poly dest = &dest_s;
  PR_INIT_EVECTOR_COPY(r_src, r_dest);

  poly p = src; src = NULL;
  while (p != NULL)
  {
    pNext(dest) = (poly) PR_ALLOC_MONOM(r_dest); pIter(dest);

    p_SetCoeff0(dest, PR_NCOPY(p_GetCoeff(p, r_src), r_src), r_dest);
    PR_CPY_EVECTOR(dest, r_dest, p, r_src);

    { poly tmp = pNext(p); PR_DELETE_MONOM(p, r_src); p = tmp; }
  }
  pNext(dest) = NULL;
  dest = pNext(&dest_s);
  PR_SORT_POLY(dest, r_dest, r_src);
  p_Test(dest, r_dest);
  return dest;
}
