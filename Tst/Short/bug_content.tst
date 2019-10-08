LIB "tst.lib";
tst_init();

// bug in removing the content
ring r=(0,a),b,lp;
poly f=(-4a2+8a-4)*b+2a2-2a;
factorize(f);

tst_status(1);$
