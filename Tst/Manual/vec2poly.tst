LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
vector v = gen(1) + 3*gen(3) + 22/9*gen(4);
intvec iv = 3,2,1;
vec2poly(v,2);
vec2poly(iv);
tst_status(1);$
