LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=3,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=group_reynolds(A);
string newring="alskdfj";
molien(L[2..size(L)],newring);
matrix P=primary_charp_random(L[1],newring,1);
kill `newring`;
print(P);
tst_status(1);$
