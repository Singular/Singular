LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y,z),dp;
def R = freeAlgebra(r,5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
intmat u[3][1] = 1,1,2; intmat v[1][3] = 2,1,3; intmat w[2][3] = 3,1,1,2,3,1;
// defines intmats of different size containing intvec representations of
// monomials as rows
iv2lpMat(u); // returns the monomials contained in u
iv2lpMat(v); // returns the monomials contained in v
iv2lpMat(w); // returns the monomials contained in w
tst_status(1);$
