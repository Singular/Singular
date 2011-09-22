LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y),Dp;
ideal I  = y*(x3-y2),x*(x3-y2);
engine(I,0); // uses slimgb
engine(I,1); // uses std
tst_status(1);$
