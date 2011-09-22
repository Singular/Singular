LIB "tst.lib"; tst_init();
LIB "nctools.lib";
def a = makeHeisenberg(2);
setring a;   a;
def H3 = makeHeisenberg(3, 7, 2);
setring H3;  H3;
tst_status(1);$
