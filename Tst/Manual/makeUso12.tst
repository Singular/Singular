LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso12();
ncAlgebra;
setring ncAlgebra;
// ...  714  noncommutative relations
tst_status(1);$
