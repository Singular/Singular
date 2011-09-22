LIB "tst.lib"; tst_init();
LIB "general.lib";
A_Z("c",5);
A_Z("Z",-5);
string sR = "ring R = (0,"+A_Z("A",6)+"),("+A_Z("a",10)+"),dp;";
sR;
execute(sR);
R;
tst_status(1);$
