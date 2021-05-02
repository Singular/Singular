LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = (0,a),(x,y,z),dp;
ring R = freeAlgebra(r,4,2); // free bimodule of rank 2
poly p = z^2/a - a*y;
ideal I = x,y,z,a*z*y*x - x*y + 7;
module M = (x*y*a +3)*ncgen(1)*gen(1), ncgen(2)*gen(2)*z, ncgen(2)*gen(2)*(x*y*a - 7);
M; // note that a stands on the left
ring r2 = 0,(a,z,y,x),dp; // note: a is a variable in r2
ring R2 = freeAlgebra(r2,6,2);
imap(R,p); // correctly processes incorrect input
imap(R,I);
imap(R,M);

tst_status(1);$
