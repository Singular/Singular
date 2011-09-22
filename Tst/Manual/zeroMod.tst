LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,z,dp;
module N=z*gen(1),(z-1)*gen(2),(z+1)*gen(3);
list l=zeroMod(N);
l;
tst_status(1);$
