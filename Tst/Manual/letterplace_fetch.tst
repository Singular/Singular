LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = (0,a),(x,y,z),dp;
ring R = freeAlgebra(r,4,2); // free bimodule of rank 2
poly p = z^2/a - a*y; // notice a in the denominator
ideal I = x,y,z,a*z*y*x - x*y + 7;
module M = (x*y*a +3)*ncgen(1)*gen(1), ncgen(2)*gen(2)*z, ncgen(2)*gen(2)*(x*y*a - 7);
M; // note that a stands on the left
ring r2 = 0,(a,z,y,x),dp; // note: a is a variable in r2 and R2
ring R2 = freeAlgebra(r2,6,2);
fetch(R,p); // correctly processes incorrect input
fetch(R,I);
fetch(R,M);
setring R; // now we show the factor ring behavior
ideal J = y*x-x*y,z; J = twostd(J); J;
qring Q = J;
fetch(R,p);
/* NF(_, twostd(0)); // the canonical representative in Q */
fetch(R,I);
/* NF(_, twostd(0)); // the canonical representative in Q */
fetch(R,M);
/* NF(_, twostd(0)); // the canonical representative in Q */

tst_status(1);$
