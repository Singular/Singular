LIB "tst.lib"; tst_init();
LIB "central.lib";
ring R=0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;
D[1,3]=2*x;
D[2,3]=-2*y;
def r = nc_algebra(1,D); setring r; // this is U(sl_2)
poly p=4*x*y+z^2-2*z;
inCenter(p);
poly f=4*x*y;
inCenter(f);
list l= list( 1, p, p^2, p^3);
inCenter(l);
ideal I= p, f;
inCenter(I);
tst_status(1);$
