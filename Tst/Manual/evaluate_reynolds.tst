LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=group_reynolds(A);
ideal I=x2,y2,z2;
print(evaluate_reynolds(L[1],I));
tst_status(1);$
