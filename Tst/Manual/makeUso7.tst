LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso7();
ncAlgebra;
setring ncAlgebra;
// ...  107  noncommutative relations
tst_status(1);$
