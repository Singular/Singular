LIB "tst.lib"; tst_init();
LIB "fpaprops.lib";

// 1 (toeplitz) 2
ring r = 0,(y,x),Dp;
ring R = freeAlgebra(r, 4);
ideal I = y*x - 1;
ideal J = twostd(I);
lpGlDimBound(J);

kill r;
kill R;

// 2 (quadrowmm) +infinity
ring r = 0,(y,x),Dp;
ring R = freeAlgebra(r, 4);
ideal I = y*x-x*y,
x*x,
y*y;
ideal J = twostd(I);
lpGlDimBound(J);

kill r;
kill R;

// 3 (mon1) +infinity
ring r = 0,(x,y),Dp;
ring R = freeAlgebra(r, 4);
ideal I = x*y*x,
y*x*y;
ideal J = twostd(I);
lpGlDimBound(J);

tst_status(1);$