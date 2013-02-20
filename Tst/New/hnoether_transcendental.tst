LIB "tst.lib";
tst_init();
LIB "hnoether.lib";
ring r = (3,t),(x,y),ds;
    poly f = y8+4x3y6+6x6y4+2x5y5+4x9y2+4x8y3+x12+2x11y+2x10y2+x13;
hnexpansion(f);
tst_status(1);$
