LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..4),y(5)),dp;
list M=identifyvar();
ideal J=x(1)^2*y(5)^2-x(2)^2*x(3)^2,6*x(4)^2;
list L=data(J,2,5);
L[1]; // Coefficients
L[2]; // Exponents
ideal J2=convertdata(L[1],L[2],5,M);
J2;
tst_status(1);$
