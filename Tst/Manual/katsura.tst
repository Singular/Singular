LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r; basering;
katsura();
katsura(4); basering;
tst_status(1);$
