LIB "tst.lib"; tst_init();
ring r=0,(z,u,v,w),dp;
def R=nc_algebra(-1,0); // an anticommutative algebra
setring R;
option(redSB);
option(redTail);
ideal i=z^2,u^2,v^2,w^2, zuv-w;
qring Q = i;  // incorrect call produces error
kill Q;
setring R;  // go back to the ring R
qring q=twostd(i); // now it is an exterior algebra modulo <zuv-w>
q;
poly k = (v-u)*(zv+u-w);
k; // the output is not yet totally reduced
poly ek=reduce(k,std(0));
ek; // the reduced form
tst_status(1);$
