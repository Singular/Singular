LIB "tst.lib";
tst_init();
// all real factors have exponents 2,
// S 3-0-0 had some factors twice
ring r=0,(x,y),dp;
poly f=4915089/64-104199/8*y2-2*y4+47*y6+y8;
factorize(f);

ring R=(0,a),(x,y),dp;
minpoly = 64a8+15040a6+1414656a4+62399080a2+5343171409;
poly f=imap(r,f);
factorize(f);

//
tst_status(1);$
