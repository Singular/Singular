LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),(c,dp);
module M=[xy,yz],[xz,z2];

module K=syz(M);
K;

tst_status(1);$
