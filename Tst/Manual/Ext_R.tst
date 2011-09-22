LIB "tst.lib"; tst_init();
LIB "homolog.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 0,(x,y,z),dp;
ideal i    = x2y,y2z,z3x;
module E   = Ext_R(1,i);    //computes Ext^1(r/i,r)
is_zero(E);
qring R    = std(x2+yz);
intvec v   = 0,2;
printlevel = 2;             //shows what is going on
ideal i    = x,y,z;         //computes Ext^i(r/(x,y,z),r/(x2+yz)), i=0,2
list L     = Ext_R(v,i,1);  //over the qring R=r/(x2+yz), std and kbase
printlevel = p;
tst_status(1);$
