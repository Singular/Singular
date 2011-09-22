LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
// take (3,2),(7,2),(15,2),(31,2),(63,2),(127,2) as Puiseux pairs:
puiseux2generators(intvec(3,7,15,31,63,127),intvec(2,2,2,2,2,2));
tst_status(1);$
