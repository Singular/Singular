LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(b,c,t),dp;
ideal I = -bc+4b2c2t,bc2t-5b2c;
substitute(I,c,b+c,t,0,b,b-1);
ideal v = c,t,b;
ideal f = b+c,0,b-1;
substitute(I,v,f);
tst_status(1);$
