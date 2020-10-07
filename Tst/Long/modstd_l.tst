LIB "tst.lib";
tst_init();

LIB "modstd.lib";

/* more than 16 primes needed: cyclic 7 */
ring r1 = 0, x(1..7), dp;
ideal I = cyclic(7);
ideal j=modStd(I,0);
system("verify",j);

/* example from the book by Decker and Lossen */
ring R = 0, (x,y,z), lp;
ideal I = 3x3y+x3+xy3+y2z2, 2x3z-xy-xz3-y4-z2, 2x2yz-2xy2+xz2-y4;
ideal j=modStd(I,0);
system("verify",j);

tst_status(1);$
