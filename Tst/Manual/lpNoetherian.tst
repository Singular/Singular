LIB "tst.lib"; tst_init();
LIB "fpaprops.lib";

// 1 (toeplitz) weak Noetherian
ring r = 0,(y,x),Dp;
ring R = freeAlgebra(r, 4);
ideal I = y*x - 1;
ideal J = twostd(I);
lpNoetherian(J);

kill r;
kill R;

// 2 (quadrowmm) Noetherian
ring r = 0,(y,x),Dp;
ring R = freeAlgebra(r, 4);
ideal I = y*x-x*y,
x*x,
y*y;
ideal J = twostd(I);
lpNoetherian(J);

kill r;
kill R;

// 3 (mon1) not Noetherian
ring r = 0,(x,y),Dp;
ring R = freeAlgebra(r, 4);
ideal I = x*y*x,
y*x*y;
ideal J = twostd(I);
lpNoetherian(J);

tst_status(1);$