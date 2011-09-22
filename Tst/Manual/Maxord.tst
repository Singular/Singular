LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
ideal J=x(1)^2*x(2)*x(3)^5;
list L=data(J,1,3);
L[2]; // list of exponents
Maxord(L[2][1][1],3);
tst_status(1);$
