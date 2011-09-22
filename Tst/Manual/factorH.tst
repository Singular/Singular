LIB "tst.lib"; tst_init();
LIB "general.lib";
system("random",992851144);
ring r=32003,(x,y,z,w,t),lp;
poly p=y2w9+yz7t-yz5w4-z2w4t4-w8t3;
factorize(p);  //fast
system("random",992851262);
//factorize(p);  //slow
system("random",992851262);
factorH(p);
tst_status(1);$
