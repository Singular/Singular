LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def B = makeQsl3(5);
setring B;
qring Usl3q = Qideal;
Usl3q;
tst_status(1);$
