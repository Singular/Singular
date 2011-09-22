LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 32003,(x,y,z),ds;
ideal i    = x3+y4,z4+yx;
nf_icis(i);
ideal j    = x3+y4,xy,yz;
nf_icis(j);
printlevel = p;
tst_status(1);$
