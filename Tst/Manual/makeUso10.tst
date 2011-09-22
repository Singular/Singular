LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso10();
ncAlgebra;
setring ncAlgebra;
// ...  390  noncommutative relations
tst_status(1);$
