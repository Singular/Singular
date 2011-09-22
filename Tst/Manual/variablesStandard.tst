LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;  D[1,3]=2*x;  D[2,3]=-2*y;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
// Variables in their natural order:
variablesStandard();
tst_status(1);$
