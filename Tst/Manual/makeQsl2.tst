LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeQsl2(3);
setring A;
Qideal;
qring Usl2q = Qideal;
Usl2q;
tst_status(1);$
