LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
print(cyclotomic(25));
tst_status(1);$
