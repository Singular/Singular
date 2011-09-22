LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z; D[1,3]=2*x; D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
poly f = 4*x*y+z^2-2*z; // a central polynomial
ideal I = f, f*f, f*f*f - 10*f*f, f+3*z^3; I;
sa_reduce(I); // should be just f and z^3
tst_status(1);$
