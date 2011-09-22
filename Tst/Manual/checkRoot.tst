LIB "tst.lib"; tst_init();
LIB "dmod.lib";
printlevel=0;
ring r = 0,(x,y),Dp;
poly F = x^4+y^5+x*y^4;
checkRoot(F,11/20);    // -11/20 is a root of bf
poly G = x*y;
checkRoot(G,1,"alg2"); // -1 is a root of bg with multiplicity 2
tst_status(1);$
