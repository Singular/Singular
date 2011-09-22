LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring R=0,x(1..3),dp;
ideal x=maxideal(1);
module M=0;
KoszulHomology(x,M,0);  // H_0(x,R), x=(x_1,x_2,x_3)
KoszulHomology(x,M,1);  // H_1(x,R), x=(x_1,x_2,x_3)
qring S=std(x(1)*x(2));
module M=0;
ideal x=maxideal(1);
KoszulHomology(x,M,1);
KoszulHomology(x,M,2);
tst_status(1);$
