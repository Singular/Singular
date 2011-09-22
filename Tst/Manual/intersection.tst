LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),dp;
list Hne=hnexpansion((x2-y3)*(x2+y3));
intersection(Hne[1],Hne[2]);
tst_status(1);$
