LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y,z),dp;
def R = freeAlgebra(r,5);// constructs a Letterplace ring
setring R; //sets basering to Letterplace ring
intvec u = 1,1,2; intvec v = 2,1,3; intvec w = 3,1,1;
// u = x^2y, v = yxz, w = zx^2 in intvec representation
list L = u,v,w;
ivL2lpI(L);// invokes the procedure, returns the ideal containing u,v,w
tst_status(1);$
