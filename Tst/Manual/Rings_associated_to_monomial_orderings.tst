LIB "tst.lib"; tst_init();
ring R = 0,(x,y,z),dp;     // polynomial ring (global ordering)
poly f = y4z3+2x2y2z2+4z4+5y2+1;
f;                         // display f in a degrevlex-ordered way
short=0;                   // avoid short notation
f;
short=1;
leadmonom(f);              // leading monomial

ring r = 0,(x,y,z),ds;     // local ring (local ordering)
poly f = fetch(R,f);
f;                         // terms of f sorted by degree
leadmonom(f);              // leading monomial

// Now we implement more "advanced" examples of rings:
//
// 1)   (K[y]_<y>)[x]
//
int n,m=2,3;
ring A1 = 0,(x(1..n),y(1..m)),(dp(n),ds(m));
poly f  = x(1)*x(2)^2+1+y(1)^10+x(1)*y(2)^5+y(3);
leadmonom(f);
leadmonom(1+y(1));         // unit
leadmonom(1+x(1));         // no unit

//
// 2)  some ring in between (K[x]_<x>)[y] and K[x,y]_<x>
//
ring A2 = 0,(x(1..n),y(1..m)),(ds(n),dp(m));
leadmonom(1+x(1));       // unit
leadmonom(1+x(1)*y(1));  // unit
leadmonom(1+y(1));       // no unit

//
// 3)  K[x,y]_<x>
//
ring A4 = (0,y(1..m)),(x(1..n)),ds;
leadmonom(1+y(1));       // in ground field
leadmonom(1+x(1)*y(1));  // unit
leadmonom(1+x(1));       // unit

tst_status(1);$
