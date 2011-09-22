LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUe7();
ncAlgebra;
setring ncAlgebra;
// ...  2541  noncommutative relations
tst_status(1);$
