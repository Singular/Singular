LIB "tst.lib";
tst_init();

//input script that crashes Singular
//  GMG
ring r = 0,(a,b,c),ds;
ring r2 = 0,(x,y,z,u,v,w),dp;
ideal I = x-w,u2w+1,yz-v;
map phi = r,I;
ideal Z;
setring r;
preimage(r2,phi,Z);

tst_status(1);$
