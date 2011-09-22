LIB "tst.lib"; tst_init();
LIB "atkins.lib";
ring r = 0,x,dp;
poly f=x4+2x3-5x2+x;
rootsModp(7,f);
poly g=x5+112x4+655x3+551x2+1129x+831;
rootsModp(1223,g);
tst_status(1);$
