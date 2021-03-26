LIB "tst.lib"; tst_init();
LIB "freegb.lib";

// 1
ring r = 0,(x,y),Dp;
def R = freeAlgebra(r, 5);
setring R;
ideal G = x*y*y; // already a GB
dim(G);

kill r;
kill R;

// 2
ring r = 0,(x4,x3,x2,x1),Dp;
ring R = freeAlgebra(r, 7);
setring(R);
ideal I = x4*x4-25*x4*x2-x1*x4-6*x1*x3-9*x1*x2+x1*x1,
x4*x3+13*x4*x2+12*x4*x1-9*x3*x4+4*x3*x2+41*x3*x1-7*x1*x4-x1*x2,
x3*x3-9*x3*x2+2*x1*x4+x1*x1,
17*x4*x2-5*x2*x2-41*x1*x4,
x2*x2-13*x2*x1-4*x1*x3+2*x1*x2-x1*x1,
x2*x1+4*x1*x2-3*x1*x1;
ideal G = std(I);
dim(G);

kill r;
kill R;

// 3 (mon1)
ring r = 0,(x,y),Dp;
def R = freeAlgebra(r, 4);
setring(R);
ideal I = x*y*x, y*x*y;
ideal G = std(I);
dim(G);

kill r;
kill R;

// 4 (liP162E1)
ring r = 0,(x1,x2),Dp;
def R = makeLetterplaceRing(4);
setring(R);
ideal I = x1*x1*x2, x1*x2*x2;
ideal G = std(I);
dim(G);

tst_status(1);$