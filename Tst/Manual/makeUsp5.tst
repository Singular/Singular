LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUsp5();
ncAlgebra;
setring ncAlgebra;
// ...  523  noncommutative relations
tst_status(1);$
