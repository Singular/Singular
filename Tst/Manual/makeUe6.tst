LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUe6();
ncAlgebra;
setring ncAlgebra;
// ...  1008  noncommutative relations
tst_status(1);$
