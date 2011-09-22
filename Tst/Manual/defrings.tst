LIB "tst.lib"; tst_init();
LIB "ring.lib";
def S5=defrings(5,0); S5; "";
def S30=defrings(30); S30;
kill S5,S30;
tst_status(1);$
