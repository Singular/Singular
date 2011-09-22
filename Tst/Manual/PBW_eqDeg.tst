LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(e,f,h),dp;
matrix D[3][3]=0;
D[1,2]=-h;  D[1,3]=2*e;  D[2,3]=-2*f;
def A = nc_algebra(1,D); setring A; // this algebra is U(sl_2)
// PBW Basis of A_2 \ A_1 - monomials of degree == 2:
PBW_eqDeg( 2 );
tst_status(1);$
