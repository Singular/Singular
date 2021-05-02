LIB "tst.lib"; tst_init();
LIB"freegb.lib";

// Test 1

ring r = 0,(x,y,z,w),dp;
ring r2 = 0,(a,b,c),dp;

ring R = freeAlgebra(r, 5);
ring R2 = freeAlgebra(r2, 10);

setring R;
poly j1 = y*x*z + x*z;
poly j2 = z * j1;
poly j3 = j1 * z;
ideal J = j1, j2, j3;

setring R2;
map f = R,a,b,c;
map g = R,c*a*b*a+b+c,b,c;

f(J);
f(j1);
f(j2);
f(j3);

g(J);

kill r;
kill r2;
kill R;
kill R2;

// Test 2: make sure ncgen is supported

ring r = 0,(x,y,z),dp;
ring r2 = 0,(a,b,c,d),dp;

ring R = freeAlgebra(r, 5, 3);
ring R2 = freeAlgebra(r2, 7, 2);

setring R;
vector v = x*ncgen(1)*gen(1) + y*ncgen(2)*gen(2) + z*ncgen(3)*gen(3);

setring R2;
// ncgen(3) -> 0
map f1 = R,a,b,c;
// d should be ignored, instead ncgen(1) -> ncgen(1)
map f2 = R,a,b,c,d;

f1(v);
f2(v);

// should be the same map
f1[1] == f2[1];
f1[2] == f2[2];
f1[3] == f2[3];
f1[4] == f2[4];
f1[5] == f2[5];
f1[6] == f2[6];

tst_status(1);$
