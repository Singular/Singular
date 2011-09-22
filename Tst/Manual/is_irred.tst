LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=0,(x,y),ls;
is_irred(x2+y3);
is_irred(x2+y2);
is_irred(x2+y3+1);
tst_status(1);$
