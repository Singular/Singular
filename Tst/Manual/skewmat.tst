LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring R=0,x(1..5),lp;
print(skewmat(4));    // the generic skew-symmetric matrix
ring R1 = 0,(a,b,c),dp;
matrix A=skewmat(4,maxideal(1)^2);
print(A);
int n=3;
ideal i = ideal(randommat(1,n*(n-1) div 2,maxideal(1),9));
print(skewmat(n,i));  // skew matrix of generic linear forms
kill R1;
tst_status(1);$
