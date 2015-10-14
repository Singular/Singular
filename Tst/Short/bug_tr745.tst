LIB "tst.lib";
tst_init();

// there should be no automatic conversion bigintmat->intmat

bigint n=2;
n = n^90;
bigintmat A[1][1]=n;
intmat B[1][1]=1;
typeof(A*B);
A*B;


tst_status(1);$
