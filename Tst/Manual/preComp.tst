LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,z,lp;
module N=z*gen(1), (z+1)*gen(2);
N=std(N);
list l; module check;
(l, check)=preComp(N,freemodule(2));
l;
check;
tst_status(1);$
