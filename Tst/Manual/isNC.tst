LIB "tst.lib"; tst_init();
LIB "nctools.lib";
def a = makeWeyl(2);
setring a;
isNC();
kill a;
ring r = 17,(x(1..7)),dp;
isNC();
kill r;
tst_status(1);$
