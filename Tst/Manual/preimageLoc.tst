LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring S =0,(x,y,z),dp;
ring R0=0,(x,y,z),ds;
qring R=std(x+x2);
map psi=S,x,y,z;
ideal null;
setring S;
ideal nu=preimageLoc("R","psi","null");
nu;
tst_status(1);$
