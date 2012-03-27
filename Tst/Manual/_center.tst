LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z,t),dp;
matrix D[4][4]=0;
D[1,2]=-z;  D[1,3]=2*x;  D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2) tensored with K[t]
// find generators of the center of degree <= 3:
ideal Z = center(3);
Z;
inCenter(Z); // check the result
// find at least one generator of the center:
ideal ZZ = center(-1, 1);
ZZ;
inCenter(ZZ); // check the result
tst_status(1);$
