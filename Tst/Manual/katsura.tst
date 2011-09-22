LIB "tst.lib"; tst_init();
LIB "poly.lib";
ring r; basering;
katsura();
katsura(4); basering;
tst_status(1);$
