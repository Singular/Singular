LIB "tst.lib"; tst_init();
LIB "elim.lib";
int p      = printlevel;
ring r     = 2,(x,y,z),dp;
poly F     = x5+y5+(x-y)^2*xyz;
ideal j    = jacob(F);
sat_with_exp(j,maxideal(1));
printlevel = 2;
sat_with_exp(j,maxideal(2));
printlevel = p;
tst_status(1);$
