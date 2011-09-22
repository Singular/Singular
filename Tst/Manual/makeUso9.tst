LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUso9();
ncAlgebra;
setring ncAlgebra;
// ...  264  noncommutative relations
tst_status(1);$
