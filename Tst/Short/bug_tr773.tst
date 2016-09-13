LIB "tst.lib";
tst_init();

// The exterior algebra over a single variable

LIB "nctools.lib";
ring a1 = (0),(e),dp;
def A1 = superCommutative(1);
setring A1;
e^2;
e^2==0;
ring a2 = (0),(e,f),dp;
def A2 = superCommutative(1);
setring A2;
e^2;
e^2==0;
setring A1;
e^2;
e^2==0;

tst_status(1);$
