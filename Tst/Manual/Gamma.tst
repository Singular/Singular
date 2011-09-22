LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..5)),dp;
ideal J=x(1)^2*x(2)*x(3)^5*x(4)^2*x(5)^3;
list L=data(J,1,5);
list G=Gamma(L[2][1][1],9,5);   // critical value c=9
G[1]; // maximum exponents in the ideal
G[2]; // maximal value of Gamma function
G[3]; // center given by Gamma
tst_status(1);$
