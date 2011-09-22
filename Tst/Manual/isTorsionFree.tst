LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring R = 0,(x,y),dp;
intmat M[2][2]=
1,0,
0,1;
intmat T[2][5]=
1, 2, 3, 4, 0,
0,10,20,30, 1;
setBaseMultigrading(M,T);
// Is the resulting group  free?
isTorsionFree();
kill R, M, T;
///////////////////////////////////////////
ring R=0,(x,y,z),dp;
intmat A[3][3] =
1,0,0,
0,1,0,
0,0,1;
intmat B[3][4]=
3,3,3,3,
2,1,3,0,
1,2,0,3;
setBaseMultigrading(A,B);
// Is the resulting group  free?
isTorsionFree();
kill R, A, B;
tst_status(1);$
