LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = ZZ,(x,y),dp;
ring R = freeAlgebra(r, 15);
ideal I = 4*x*y+3, 6*y*x+2;
std(I);

kill r;
kill R;

ring r = ZZ,(x,y),lp;
ring R = freeAlgebra(r,7);
ideal I = 2*x*y, 3*y*y*y+3, x*x-3*x;
std(I);
ideal I2 = I, x*y - y*x;
std(I2);

tst_status(1);$
