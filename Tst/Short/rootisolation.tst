LIB "tst.lib";
tst_init();

LIB "rootisolation.lib";

// start with examples
example bounds;
example ivmatInit;
example ivmatSet;
example unitMatrix;
example ivmatGaussian;
example evalJacobianAtBox;
example rootIsolationNoPreprocessing;
example rootIsolation;

// trivial example
ring R = 0,x,dp;
ideal I = x;
box B = list(bounds(-1,1));
list roots = rootIsolation(I,B,0); roots;
kill roots, B, R;

// maximal ideal with single root
ring R = 0,(x,y,z),dp;
ideal I = x-3,y-2,z+2/3;
box B = list(bounds(-5,5), bounds(-5,5), bounds(-5,5));
list roots = rootIsolation(I,B,0); roots;
kill roots, B, R;

// roots of I lie on initial boundary
ring R = 0,(x,y),dp;
ideal I = x2-4,y2-4;
box B = list(bounds(-2,2), bounds(-2,2));
list roots = rootIsolation(I,B,0); roots;
kill roots, B, R;

// roots of I lie on initial boundary for several
// iterations
ring R = 0,(x,y),dp;
ideal I = (x2-1)*(x2-4)*(x2-9),(y2-1)*(y2-4)*(y2-9);
box B = list(bounds(-1,1), bounds(-1,1));
list roots = rootIsolation(I,B,0); roots;
kill roots, B, R;

// starting box is a point
ring R = 0,(x,y),dp;
ideal I = x2-1,y2-9;
box B = list(bounds(-1,-1), bounds(3,3));
list roots = rootIsolation(I,B,0); roots;
kill roots, B, R;

// no real roots
ring R = 0,(x,y),dp;
ideal I = x2+1,y;
box B = list(bounds(-100,100),bounds(-100,100));
list roots = rootIsolation(I,B,0); roots;
kill roots, B, R;

// fixed bug: fglm lookup clashes with blackbox type
ring R = 0,(x,y),dp;
ideal I = x2-1,y-3;
interval fastGB = -10,10;
box B = list(fastGB, fastGB);
list roots = rootIsolation(I,B,0); roots;
kill roots, fastGB, B, R;

// eps > 0, some boxes land in result[1]
ring R = 0,(x,y),dp;
ideal I = 2x2-xy+2y2-2,2x2-3xy+3y2-2;
box B = list(bounds(-3/2,3/2), bounds(-3/2,3/2));
list roots = rootIsolation(I,B,1/10); roots;
kill roots, B, R;

tst_status(1);$;
// vim: ft=singular
