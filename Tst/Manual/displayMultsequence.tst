LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),dp;
// Example 1: Input = output of develop
displayMultsequence(develop(x3-y5));
// Example 2: Input = bivariate polynomial
displayMultsequence((x6-y10)*(x+y2-y3)*(x+y2+y3));
tst_status(1);$
