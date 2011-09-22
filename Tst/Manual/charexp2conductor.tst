LIB "tst.lib"; tst_init();
LIB "alexpoly.lib";
charexp2conductor(intvec(2,3));  // A1-Singularity
charexp2conductor(intvec(28,64,66,77));
tst_status(1);$
