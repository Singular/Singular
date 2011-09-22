LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r = 0,(t,x),lp;
matrix A[2][2] = 1,x2,x,x2+3x;
print(A);
list L = busadj(A);
poly X = L[1][1]+L[1][2]*t+L[1][3]*t2; X;
matrix bA[2][2] = L[2][1]+L[2][2]*t;
print(bA);               //the busadjoint of A;
print(bA*(t*unitmat(2)-A));
tst_status(1);$
