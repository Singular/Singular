LIB "tst.lib"; tst_init();
LIB "dmod.lib";
def r = reiffen(4,5);
setring r;
RC;
tst_status(1);$
