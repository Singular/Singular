LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r = 0,(x),dp;
matrix A[5][5] = 10, 4,  0, -9,  8,
-3, 6, -6, -4,  9,
0, 3, -1, -9, -8,
-4,-2, -6, -10,10,
-9, 5, -1, -6,  5;
list Z = U_D_O(A);              //construct P,U,D,O s.t. P*A=U*D*O
print(Z[1]);                    //P
print(Z[2]);                    //U
print(Z[3]);                    //D
print(Z[4]);                    //O
print(Z[1]*A);                  //P*A
print(Z[2]*Z[3]*Z[4]);          //U*D*O
tst_status(1);$
