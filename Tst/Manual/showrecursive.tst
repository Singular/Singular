LIB "tst.lib"; tst_init();
LIB "inout.lib";
ring r=2,(a,b,c,d,x,y),ds;
poly f=y+ax2+bx3+cx2y2+dxy3;
showrecursive(f,x);
showrecursive(f,xy,"lp");
tst_status(1);$
