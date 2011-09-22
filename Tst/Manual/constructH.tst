LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
list flag=identifyvar();
ideal J=x(1)^4*x(2)^2, x(1)^2+x(3)^3;
list L=Eresol(J);   // 7 charts
// history of the exceptional divisors at the 7-th chart
L[1][7][7]; // blow ups at x(3)-th, x(1)-th and x(1)-th charts
constructH(L[1][7][7],3,flag);
tst_status(1);$
