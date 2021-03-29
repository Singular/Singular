LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = freeAlgebra(r,5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal G = x*x, y*y,x*y*x; // Groebner basis
ideal I = x*x, y*x*y, x*y*x; // Groebner basis
teach_lpKDimCheck(G); // invokes procedure, factor algebra is of finite K-dimension
teach_lpKDimCheck(I); // invokes procedure, factor algebra is of infinite Kdimension
tst_status(1);$
