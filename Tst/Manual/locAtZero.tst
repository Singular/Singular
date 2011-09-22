LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring r = 0,(x,y,z),dp;
poly f = z5+y4+x3+xyz;
ideal i = jacob(f),f;
i=std(i);
locAtZero(i);
i= std(i*ideal(x-1,y,z));
locAtZero(i);
tst_status(1);$
