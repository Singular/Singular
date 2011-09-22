LIB "tst.lib"; tst_init();
LIB "central.lib";
ring R=0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;
D[1,3]=2*x;
D[2,3]=-2*y;
def r = nc_algebra(1,D); setring r; // this is U(sl_2) with cartan - z
isCartan(z); // yes!
poly p=4*x*y+z^2-2*z;
isCartan(p); // central elements are Cartan elements!
poly f=4*x*y;
isCartan(f); // no way!
isCartan( 10 + p + z ); // scalar + central + cartan
tst_status(1);$
