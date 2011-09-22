LIB "tst.lib"; tst_init();
LIB "solve.lib";
// Find all roots of an univariate polynomial using Laguerre's method:
ring rs1= 0,(x,y),lp;
poly f = 15x5 + x3 + x2 - 10;
// 10 digits precision
laguerre_solve(f,10);
// Now with complex coefficients,
// internal precision is 30 digits (default)
printlevel=2;
ring rsc= (real,10,i),x,lp;
poly f = (15.4+i*5)*x^5 + (25.0e-2+i*2)*x^3 + x2 - 10*i;
list l = laguerre_solve(f);
l;
// check result, value of substituted polynomial should be near to zero
// remember that l contains a list of strings
// in the case of a different ring
subst(f,x,l[1]);
subst(f,x,l[2]);
tst_status(1);$
