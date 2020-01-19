LIB "tst.lib"; tst_init();
LIB "fpaprops.lib";

// 1 (toeplitz) no
ring r = 0,(y,x),Dp;
ring R = freeAlgebra(r, 4);
ideal I = y*x - 1;
ideal J = twostd(I);
lpIsPrime(J);

kill r;
kill R;

// 2 (quadrowmm) no
ring r = 0,(y,x),Dp;
ring R = freeAlgebra(r, 4);
ideal I = y*x-x*y,
x*x,
y*y;
ideal J = twostd(I);
lpIsPrime(J);

kill r;
kill R;

// 3 (mon1) yes
ring r = 0,(x,y),Dp;
ring R = freeAlgebra(r, 4);
ideal I = x*y*x,
y*x*y;
ideal J = twostd(I);
lpIsPrime(J);

kill r;
kill R;

// 4 (liP176E2_2) no
ring r = 0,(x1,x2),Dp;
ring R = freeAlgebra(r, 4);
ideal I = x1*x2,
x2*x1;
ideal J = twostd(I);
lpIsPrime(J);

tst_status(1);$