LIB "tst.lib"; tst_init();
LIB "finvar.lib";
"         note the case of prime characteristic";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
matrix REY,M=reynolds_molien(A);
print(REY);
print(M);
ring S=3,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
matrix REY=reynolds_molien(A,"");
print(REY);
setring Finvar::newring;
print(M);
setring S;
tst_status(1);$
