LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
ring R=0,(x,y,z,u),dp;
resolution T1=mres(maxideal(1),0);
module M=T1[3];
intvec v=2,2,2,2,2,2;
attrib(M,"isHomog",v);
dimH(0,M,2);
dimH(1,M,0);
dimH(2,M,1);
dimH(3,M,-5);
tst_status(1);$
