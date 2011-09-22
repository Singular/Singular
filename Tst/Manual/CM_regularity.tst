LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
ring R=0,(x,y,z,u),dp;
resolution T1=mres(maxideal(1),0);
module M=T1[3];
intvec v=2,2,2,2,2,2;
attrib(M,"isHomog",v);
CM_regularity(M);
tst_status(1);$
