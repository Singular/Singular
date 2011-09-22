LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring R=0,(x,y,z),ds;  // local ring R = Q[x,y,z]_<x,y,z>
module M=xz,yz,z2;
isCM(M);             // test if R/<xz,yz,z2> is Cohen-Macaulay
M=x2+y2,z7;          // test if R/<x2+y2,z7> is Cohen-Macaulay
isCM(M);
tst_status(1);$
