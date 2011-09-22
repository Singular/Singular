LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring R = 0,x(1..16),lp;
print(genericmat(3,3));      // the generic 3x3 matrix
ring R1 = 0,(a,b,c,d),dp;
matrix A = genericmat(3,4,maxideal(1)^3);
print(A);
int n,m = 3,2;
ideal i = ideal(randommat(1,n*m,maxideal(1),9));
print(genericmat(n,m,i));    // matrix of generic linear forms
tst_status(1);$
