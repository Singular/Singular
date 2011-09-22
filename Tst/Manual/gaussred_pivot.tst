LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x),dp;
matrix A[5][4] = 1, 3,-1,4,
2, 5,-1,3,
1, 3,-1,4,
0, 4,-3,1,
-3,1,-5,-2;
list Z=gaussred_pivot(A);  //construct P,U,S s.t. P*A=U*S
print(Z[1]);               //P
print(Z[2]);               //U
print(Z[3]);               //S
print(Z[4]);               //rank
print(Z[1]*A);             //P*A
print(Z[2]*Z[3]);          //U*S
tst_status(1);$
