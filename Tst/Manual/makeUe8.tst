LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUe8();
ncAlgebra;
setring ncAlgebra;
// ...  7752  noncommutative relations
tst_status(1);$
