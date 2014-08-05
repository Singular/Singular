LIB "tst.lib";
tst_init();

option(warn);

LIB("algebra.lib");
int p = printlevel;
ring r = 0,(a,b,c),ds;
ring s = 0,(x,y,z,u,v,w),dp;
ideal I = x-w,u2w+1,yz-v;
map phi = r,I;
is_injective(phi,r);


tst_status(1); $

