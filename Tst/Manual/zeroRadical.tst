LIB "tst.lib"; tst_init();
LIB "assprimeszerodim.lib";
ring R = 0, (x,y), dp;
ideal I = xy4-2xy2+x, x2-x, y4-2y2+1;
zeroRadical(I);
tst_status(1);$
