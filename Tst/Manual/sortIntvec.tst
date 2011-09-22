LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,x,dp;
intvec v = -1,0,1,-2,0,2;
list L = sortIntvec(v); L;
v[L[2]];
v = -3,0;
sortIntvec(v);
v = 0,-3;
sortIntvec(v);
tst_status(1);$
