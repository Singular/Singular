LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring r = 0,(x(1..3)),dp(3);
ideal J=x(3)^5+x(2)^4+x(1)^3+x(1)*x(2)*x(3);
list re=resolve(J);
list di=intersectionDiv(re);
di;
tst_status(1);$
