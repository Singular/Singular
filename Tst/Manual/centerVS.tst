LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;  D[1,3]=2*x;  D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
// find a basis of the vector space of all
// central elements of degree <= 4:
ideal Z = centerVS(4);
Z;
// note that the second element is the square of the first
// plus a multiple of the first:
Z[2] - Z[1]^2 + 8*Z[1];
inCenter(Z); // check the result
tst_status(1);$
