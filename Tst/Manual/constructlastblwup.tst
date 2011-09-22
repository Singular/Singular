LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
list flag=identifyvar();
ideal chy=maxideal(1);
ideal J=x(1)^4*x(2)^2, x(1)^2+x(3)^3;
list L=Eresol(J);   // 7 charts
// history of the blow ups at the 7-th chart, center {x(1)=x(3)=0} every time
L[1][7][8]; // blow ups at x(3)-th, x(1)-th and x(1)-th charts
constructlastblwup(L[1][7][8],3,chy,flag);
tst_status(1);$
