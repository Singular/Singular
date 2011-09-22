LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeUsl2();
setring A; A;
def Aenv = envelope(A);
setring Aenv;
Aenv;
tst_status(1);$
