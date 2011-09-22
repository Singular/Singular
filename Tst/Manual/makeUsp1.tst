LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def ncAlgebra = makeUsp1();
setring ncAlgebra;
ncAlgebra;
tst_status(1);$
