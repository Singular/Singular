LIB "tst.lib";
tst_init();

ring r;
poly f=2*xy-yz+x^2+z^3;
matrix M = jacob(jacob(jet(f,2)));
list lba = bareiss(M);

LIB "classify.lib";
poly h = =2*xy-yz+x^2+z^3;
classify(h);

tst_status(1);$
