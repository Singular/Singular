LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(x,y,z),lp;
poly f = x2y+3xz-5y+3;
newtonDiag(f);
tst_status(1);$
