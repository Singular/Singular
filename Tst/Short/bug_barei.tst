LIB "tst.lib";
tst_init();

ring r;
poly f=2*xy-yz+x^2+z^3;
matrix M = jacob(jacob(jet(f,2)));
bareiss(M);

matrix m[3][3] = x,2,3,4,5,6;
det(m);

LIB "classify.lib";
poly h = 2*xy-yz+x^2+z^3;
classify(h);

tst_status(1);$
