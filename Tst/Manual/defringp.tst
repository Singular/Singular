LIB "tst.lib"; tst_init();
LIB "ring.lib";
def P5=defringp(5,0); P5; "";
def P30=defringp(30); P30;
kill P5,P30;
tst_status(1);$
