LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def a = makeUg2();
a;
setring a;
// ...  56  noncommutative relations
tst_status(1);$
