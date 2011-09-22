LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso6();
ncAlgebra;
setring ncAlgebra;
// ...  60  noncommutative relations
tst_status(1);$
