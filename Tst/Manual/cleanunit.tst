LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1),y(2),x(3),y(4)),dp;
list flag=identifyvar();
ideal J=x(1)^3*y(2)*x(3)^5*y(4)^8;
list L=data(J,1,4);
L[2][1][1];  // list of exponents of the monomial J
list M=cleanunit(L[2][1][1],4,flag);
M;           // new list without units
tst_status(1);$
