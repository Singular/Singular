LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
intvec flags=0,1,0;
matrix REY,M=reynolds_molien(A,flags);
flags=8,6;
print(invariant_basis_reynolds(REY,6,flags));
tst_status(1);$
