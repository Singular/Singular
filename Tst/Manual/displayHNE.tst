LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),dp;
poly f=x3+2xy2+y2;
list hn=develop(f);
displayHNE(hn);
tst_status(1);$
