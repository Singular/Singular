LIB "tst.lib"; tst_init();
LIB "findifs.lib";
ring r = (0,d),(g,i,sin,cos),dp;
poly P = d*i*sin - g*cos +d^2*i;
NF( magnitude(P), std(i^2+1) );
tst_status(1);$
