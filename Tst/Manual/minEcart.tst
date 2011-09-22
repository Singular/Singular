LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring R=0,(x,y,z),dp;
ideal T = x2y+x2,y3+xyz,xyz2+z4;
poly h = x2y2z2+x5+yx3+z6;
minEcart(T,h);"";
ring S=0,(x,y,z),(c,ds);
module T = [x2+x2y,y2],[y3+xyz,x3-z3],[x3y+z4,0,x2];
vector h = [x3y+x5+x2y2z2+z6,x3];
minEcart(T,h);
tst_status(1);$
