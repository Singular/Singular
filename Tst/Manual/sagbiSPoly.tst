LIB "tst.lib"; tst_init();
LIB "sagbi.lib";
ring r= 0,(x,y),dp;
ideal A=x*y+x,x*y^2,y^2+y,x^2+x;
//------------------ Compute the SAGBI S-polynomials only
sagbiSPoly(A);
//------------------ Extended ring is to be returned, which contains
// the ideal of algebraic relations and the ideal of the S-polynomials
def rNew=sagbiSPoly(A,1);  setring rNew;
spolynomials;
algebraicRelations;
//----------------- Now we verify that the substitution of A[i] into @y(i)
// results in the spolynomials listed above
ideal A=fetch(r,A);
map phi=rNew,x,y,A;
ideal spolynomials2=simplify(phi(algebraicRelations),1);
spolynomials2;
tst_status(1);$
