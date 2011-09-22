LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),dp;
list Hne=develop(x2+y3+y4);
Hne;
stripHNE(Hne);
tst_status(1);$
