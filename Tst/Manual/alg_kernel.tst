LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring r = 0,(a,b,c),ds;
ring s = 0,(x,y,z,u,v,w),dp;
ideal I = x-w,u2w+1,yz-v;
map phi = r,I;                // a map from r to s:
alg_kernel(phi,r);            // a,b,c ---> x-w,u2w+1,yz-v
ring S = 0,(a,b,c),ds;
ring R = 0,(x,y,z),dp;
qring Q = std(x-y);
ideal i = x, y, x2-y3;
map phi = S,i;                 // a map to a quotient ring
alg_kernel(phi,S,"ker",3);     // uses algorithm 3
setring S;                     // you have access to kernel in preimage
ker;
tst_status(1);$
