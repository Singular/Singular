LIB "tst.lib"; tst_init();
LIB "alexpoly.lib";
ring r=0,(x,y),dp;
intvec v=8,12,14,17;
vector a=[1,1,1];
poly f=charexp2poly(v,a);
f;
invariants(f)[1][1];  // The characteristic exponents of f.
tst_status(1);$
