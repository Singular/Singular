LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^2-x(2)^3;
list L=Eresol(J);
L[3]; // incomplete resolution function
resfunction(L[3],L[7],7,2); // complete resolution function
tst_status(1);$
