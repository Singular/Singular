LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat L1[4][1]=
0,
0,
0,
2;
intmat L2[3][2]=
0, 0,
2, 0,
0, 3;
intmat A[3][4] =
1, 2, 3, 0,
7, 0, 0, 0,
1, 2, 0, 3;
print( A );
isGroupHomomorphism(L1, L2, A);
intmat B[3][4] =
1, 2, 3, 0,
7, 0, 0, 0,
1, 2, 0, 2;
print( B );
isGroupHomomorphism(L1, L2, B); // Not a homomorphism!
tst_status(1);$
