LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),ls;
poly f=x5+2x3y-x2y2+3xy5+y6-y7;
newtonpoly(f);
tst_status(1);$
