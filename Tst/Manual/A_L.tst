LIB "tst.lib"; tst_init();
LIB "classify.lib";
ring r=0,(a,b,c),ds;
poly f=A_L("E[13]");
f;
A_L(f);
tst_status(1);$
