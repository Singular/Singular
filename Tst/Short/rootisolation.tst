LIB "tst.lib";
tst_init();

LIB "rootisolation.lib";

// start with examples
example bounds;
example length;
example boxSet;
example ivmatInit;
example ivmatSet;
example unitMatrix;
example ivmatGaussian;
example evalPolyAtBox;
example evalJacobianAtBox;
example rootIsolationNoPreprocessing;
example rootIsolation;

// some interval/box only stuff
ring R = 0,(x,y,z),dp;
interval J = -2,1/5; J;
// not enough intervals for box
box B = list(J,J); B;
// right number
B = list(J,J,J); B;
// too many intervals for box
B = list(J,J,J,J); B;
kill J,B,R;

// trivial example
ring R = 0,x,dp;
ideal I = x;
box B = list(bounds(-1,1));
list roots = rootIsolation(I,B); roots;
kill roots, B, R;

// maximal ideal with single root
ring R = 0,(x,y,z),dp;
ideal I = x-3,y-2,z+2/3;
box B = list(bounds(-5,5), bounds(-5,5), bounds(-5,5));
list roots = rootIsolation(I,B); roots;
kill roots, B, R;

// roots of I lie on initial boundary
ring R = 0,(x,y),dp;
ideal I = x2-4,y2-4;
box B = list(bounds(-2,2), bounds(-2,2));
list roots = rootIsolation(I,B); roots;
kill roots, B, R;

// roots of I lie on initial boundary for several
// iterations
ring R = 0,(x,y),dp;
ideal I = (x2-1)*(x2-4)*(x2-9),(y2-1)*(y2-4)*(y2-9);
box B = list(bounds(-1,1), bounds(-1,1));
list roots = rootIsolation(I,B); roots;
kill roots, B, R;

// starting box is a point
ring R = 0,(x,y),dp;
ideal I = x2-1,y2-9;
box B = list(bounds(-1,-1), bounds(3,3));
list roots = rootIsolation(I,B); roots;
kill roots, B, R;

// no real roots
ring R = 0,(x,y),dp;
ideal I = x2+1,y;
box B = list(bounds(-100,100),bounds(-100,100));
list roots = rootIsolation(I,B); roots;
kill roots, B, R;

// fixed bug: fglm lookup clashes with blackbox type
ring R = 0,(x,y),dp;
ideal I = x2-1,y-3;
interval fastGB = -10,10;
box B = list(fastGB, fastGB);
list roots = rootIsolation(I,B); roots;
kill roots, fastGB, B, R;

// eps > 0, some boxes land in result[1]
ring R = 0,(x,y),dp;
ideal I = 2x2-xy+2y2-2,2x2-3xy+3y2-2;
box B = list(bounds(-3/2,3/2), bounds(-3/2,3/2));
list roots = rootIsolation(I,B,1/10); roots;
kill roots, B, R;

// too many generators but reduced Groebner basis with 2 generators exists
ring R = 0,(x,y),dp;
ideal I =
  x3-4x2y+5y2+4x,
  20y4-60x2y-5xy2-25y3+11x2-84xy+100y2+60x+100y+59,
  4xy3+16x2y+xy2+5y3+x2+4xy-20y2-12x-16y+1,
  4x2y2-xy2-5y3-x2-4xy-1;
box B = list(bounds(-4,4), bounds(-4,4));
list roots = rootIsolation(I, B); roots;
kill roots, B, R;

// 6 generators (this is a Groebner basis), no reduced GB has 3 generators
ring R = 0,(x,y,z),dp;
ideal I = yz-x,xz-y,y2-z2,xy-z,x2-z2,z3-z;
box B = list(bounds(-5,5),bounds(-5,5),bounds(-5,5));
list roots = rootIsolation(I, B); roots;
kill roots, B, R;

// no radical but zero-dimensional, |V(I)| = 2
ring R = 0,(x,y,z),dp;
ideal I = y2-xy-2zx,y3+z2+1,x2yz-yz;
box B = list(bounds(-5,5), bounds(-5,5), bounds(-5,5));
list roots = rootIsolation(I, B); roots;
kill roots, B, R;

// not zero-dimensional
ring R = 0,(x,y,z),dp;
ideal I = x2-x,y2-1,x;
box B = list(bounds(-1,1),bounds(-1,1),bounds(-1,1));
list roots = rootIsolation(I, B); roots;
kill roots, B, R;

// automatically determine starting box
ring R = 0,(x,y),dp;
ideal I = 2x2-xy+2y2-2,2x2-3xy+3y2-2;
list roots = rootIsolation(I); roots;
kill roots, R;

// apply to primary decomposition
ring R = 0,(x,y,z),dp;
ideal I = yz-x,xz-y,y2-z2,xy-z,x2-z2,z3-z;
list roots = rootIsolationPrimdec(I); roots;
kill roots, R;

// slightly longer example
ring R = 0,(a,b,c,d,t),dp;
ideal I =
  a4+a2c2+3c4-1,
  4a3b+2abc2+2a2cd+12c3d,
  6a2b2+b2c2+4abcd+a2d2+18c2d2-1,
  4ab3+2b2cd+2abd2+12cd3,
  b4+b2d2+3d4-t;
interval i = -100,100;
interval j = 0,2;
box B = list(i,i,i,i,j);
list roots1 = rootIsolation(I,B); roots1;
// compare to case where we use triangular decomposition
list roots2 = rootIsolation(std(I),B); roots2;
kill roots1, roots2, i, j, B, R;

tst_status(1);$;
// vim: ft=singular
