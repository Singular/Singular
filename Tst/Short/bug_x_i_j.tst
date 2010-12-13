LIB "tst.lib";
tst_init();

// expansion of x(1..n)(1..n)
int n=3;
ring r=0,(x(1..n)(1..n),T),lp;
r;

tst_status(1);$
