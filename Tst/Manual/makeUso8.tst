LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso8();
ncAlgebra;
setring ncAlgebra;
// ...  180  noncommutative relations
tst_status(1);$
