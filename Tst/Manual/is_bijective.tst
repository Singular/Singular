LIB "tst.lib"; tst_init();
LIB "algebra.lib";
int p = printlevel;  printlevel = 1;
ring R = 0,(x,y,z),dp;
ideal i = x, y, x2-y3;
map phi = R,i;                      // a map from R to itself, z->x2-y3
is_bijective(phi,R);
qring Q = std(z-x2+y3);
is_bijective(ideal(x,y,x2-y3),Q);
ring S = 0,(a,b,c,d),dp;
map psi = R,ideal(a,a+b,c-a2+b3,0); // a map from R to S,
is_bijective(psi,R);                // x->a, y->a+b, z->c-a2+b3
qring T = std(d,c-a2+b3);
map chi = Q,a,b,a2-b3;              // amap between two quotient rings
is_bijective(chi,Q);
printlevel = p;
tst_status(1);$
