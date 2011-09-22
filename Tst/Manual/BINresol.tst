LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^2-x(2)^3;
list B=BINresol(J);
B[1]; // list of final charts
B[2]; // list of all charts
ring r = 2,(x(1..3)),dp;
ideal J=x(1)^2-x(2)^2*x(3)^2;
list B=BINresol(J);
B[2]; // list of all charts
tst_status(1);$
