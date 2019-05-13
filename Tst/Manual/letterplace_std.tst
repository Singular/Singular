LIB "tst.lib"; tst_init();
LIB "freegb.lib";

// 1
ring r = 0,(x,y,z),Dp;
int degree_bound = 5;
def R = freeAlgebra(r,5);
setring R;
ideal I = -x*y-7*y*y+3*x*x, x*y*x-y*x*y;
std(I);

kill r;
kill R;

// 2 (c_4_1_7_W)
ring r = 0,(x4,x3,x2,x1),Dp;
def R = freeAlgebra(r, 7);
setring(R);
ideal I = x4*x4-25*x4*x2-x1*x4-6*x1*x3-9*x1*x2+x1*x1,
x4*x3+13*x4*x2+12*x4*x1-9*x3*x4+4*x3*x2+41*x3*x1-7*x1*x4-x1*x2,
x3*x3-9*x3*x2+2*x1*x4+x1*x1,
17*x4*x2-5*x2*x2-41*x1*x4,
x2*x2-13*x2*x1-4*x1*x3+2*x1*x2-x1*x1,
x2*x1+4*x1*x2-3*x1*x1;
std(I);

tst_status(1);$
