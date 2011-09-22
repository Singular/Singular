LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso5();
ncAlgebra;
setring ncAlgebra;
// ...  28  noncommutative relations
tst_status(1);$
