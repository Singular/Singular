LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUf4();
ncAlgebra;
setring ncAlgebra;
// ...  552  noncommutative relations
tst_status(1);$
