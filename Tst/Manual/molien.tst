LIB "tst.lib"; tst_init();
LIB "finvar.lib";
"         note the case of prime characteristic";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=group_reynolds(A);
matrix M=molien(L[2..size(L)]);
print(M);
ring S=3,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=group_reynolds(A);
molien(L[2..size(L)],"");
setring Finvar::newring;
print(M);
setring S;
tst_status(1);$
