LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(e,f,h),dp;
matrix D[3][3]=0;
D[1,2]=-h;  D[1,3]=2*e;  D[2,3]=-2*f;
def A = nc_algebra(1,D); // this algebra is U(sl_2)
setring A;
// At most 1st degree in e, h and at most 2nd degree in f, unit is omitted:
PBW_maxMonom( e*(f^2)* h );
tst_status(1);$
