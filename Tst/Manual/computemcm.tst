LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^3-x(1)*x(2)^3;
list L=Eresol(J);   // 8 charts, rational exponents
L[8][2][2];         // maximal E-order at the first chart
computemcm(L[8][2][2]);
tst_status(1);$
