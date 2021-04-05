LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = 0,(x,y,z),dp;
ring R = freeAlgebra(r, 5);
ideal I = y*x*y - z*y*z, x*y*x - z*x*y, z*x*z - y*z*x, x*x*x + y*y*y + z*z*z + x*y*z;
ideal J = twostd(I);
poly p = y*x*y*z*y - y*z*z*y + z*y*z;
poly q = z*x*z*y*z - y*z*x*y*z;
reduce(p,J);
reduce(q,J);

kill r; kill R;

ring r = (0,q),(A,B,C),Dp;
ring R = freeAlgebra(r,7);
number nq = q^2 - 1/(q^2);
poly a = nq*A + q*B*C - 1/q*C*B;
poly b = nq*B + q*C*A - 1/q*A*C;
poly c = nq*C + q*A*B - 1/q*B*A;
ideal I = bracket(a,A), bracket(a,B), bracket(a,C),bracket(b,A), bracket(b,B), bracket(b,C),bracket(c,A), bracket(c,B), bracket(c,C);
option(redSB); option(redTail);
ideal J = twostd(I);
reduce(a*(A*B) - (A*B)*a, J); //0
reduce(a*(B*A) - (B*A)*a, J); // 0
reduce(bracket(a, A*B + B*A), J); // 0

tst_status(1);$
