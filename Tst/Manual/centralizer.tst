LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z; D[1,3]=2*x; D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
poly f = 4*x*y+z^2-2*z; // a central polynomial
f;
// find generators of the centralizer of f of degree <= 2:
ideal c = centralizer(f, 2);
c;  // since f is central, the answer consists of generators of A
inCentralizer(c, f); // check the result
// find at least two generators of the centralizer of f:
ideal cc = centralizer(f,-1,2);
cc;
inCentralizer(cc, f); // check the result
poly g = z^2-2*z; // some non-central polynomial
// find generators of the centralizer of g of degree <= 2:
c = centralizer(g, 2);
c;
inCentralizer(c, g); // check the result
// find at least one generator of the centralizer of g:
centralizer(g,-1,1);
// find at least two generators of the centralizer of g:
cc = centralizer(g,-1,2);
cc;
inCentralizer(cc, g); // check the result
tst_status(1);$
