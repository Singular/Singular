LIB "tst.lib"; tst_init();
LIB "mregular.lib";
// The 1st example is the twisted cubic:
regMonCurve(0,1,2,3);
// The 2nd. example is the non arithm. Cohen-Macaulay monomial curve in P4
// parametrized by: x(0)-s6,x(1)-s5t,x(2)-s3t3,x(3)-st5,x(4)-t6:
regMonCurve(0,1,3,5,6);
// Additional information is displayed if you change printlevel (=1);
tst_status(1);$
