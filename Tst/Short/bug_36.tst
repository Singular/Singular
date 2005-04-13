LIB "tst.lib";
tst_init();


ring r=(0,a),x,dp;
minpoly=a4+16;
poly f=(1+1/2*x2);
factorize(f);
f;
poly g=(2+x2);
factorize(g);
g;

//
tst_status(1);$
