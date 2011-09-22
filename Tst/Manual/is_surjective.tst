LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring R = 0,(x,y,z),dp;
ideal i = x, y, x2-y3;
map phi = R,i;                    // a map from R to itself, z->x2-y3
is_surjective(phi);
qring Q = std(ideal(z-x37));
map psi = R, x,y,x2-y3;           // the same map to the quotient ring
is_surjective(psi);
ring S = 0,(a,b,c),dp;
map psi = R,ideal(a,a+b,c-a2+b3); // a map from R to S,
is_surjective(psi);               // x->a, y->a+b, z->c-a2+b3
tst_status(1);$
