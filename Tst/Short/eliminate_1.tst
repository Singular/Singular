LIB "tst.lib"; tst_init(); option(prot);
ring s2=32003,(t,x,y,z),dp;
ideal i= 
9x8+y7t+5x4y2t2+2xy2z3t2,
9y8+7xy6t+2x5yt2+2x2yz3t2,
9z8+3x2y2z2t2;
eliminate(i,tx);

tst_status(1);$
