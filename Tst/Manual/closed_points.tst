LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
ring s=2,(x,y),lp;
// this is just the affine plane over F_4 :
ideal I=x4+x,y4+y;
list L=closed_points(I);
// and here you have all the points :
L;
tst_status(1);$
