LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;  D[1,3]=2*x;  D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
ideal F = x, y;
// find subalgebra generators of degree <= 4 of the subalgebra of
// all elements commuting with x and y:
ideal C = centralizerRed(F, 4);
C;
inCentralizer(C, F); // check the result
tst_status(1);$
