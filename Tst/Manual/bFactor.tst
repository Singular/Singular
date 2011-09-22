LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y),dp;
bFactor((x^2-1)^2);
bFactor((x^2+1)^2);
bFactor((y^2+1/2)*(y+9)*(y-7));
bFactor(1);
bFactor(0);
tst_status(1);$
