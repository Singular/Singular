LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y),dp;
poly f = xy+1;
isVar(f);
poly g = y^3;
isVar(g);
poly h = 7*x^3;
isVar(h);
poly i = 1;
isVar(i);
tst_status(1);$
