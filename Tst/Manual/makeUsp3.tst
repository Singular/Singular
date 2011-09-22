LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUsp3();
ncAlgebra;
setring ncAlgebra;
// ...  107  noncommutative relations
tst_status(1);$
