LIB "tst.lib";
tst_init();

// crash in 3-1-1-3:
ring sage6=(complex,15,0,I),(x, y),dp;
poly f = y*x^2 + x + 1;
division(f,x);

tst_status(1);$
