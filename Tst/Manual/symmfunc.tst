LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,x,dp;
poly p = (x-1)*(x-2)*(x-3);
list psums = list(1+2+3,1+4+9,1+8+27);
list l = symmfunc(psums);
l;
p; // Compare p with the elements of l
tst_status(1);$
