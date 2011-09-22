LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring r = 0,(a,b,c),dp;
poly f = a3c+b3+c2+a;
intvec v = leadexp(f);
nonZeroEntry(v);
intvec w;
list L = 37,0,f,v,w;
nonZeroEntry(L);
tst_status(1);$
