LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring R = 0,(x(1..3)),dp;
def ER = Exterior();
setring ER;
ER;
tst_status(1);$
