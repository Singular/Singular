LIB "tst.lib";
tst_init();

newstruct("tt", "number n");
tt n;
n.n = 2;

// and with a basering:
ring r;
n.n = 2;
n;
n.n;

tst_status(1);$
