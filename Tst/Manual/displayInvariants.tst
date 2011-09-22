LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=0,(x,y),dp;
list Hne=develop(y4+2x3y2+x6+x5y);
displayInvariants(Hne);
tst_status(1);$
