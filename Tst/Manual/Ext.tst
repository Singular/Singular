LIB "tst.lib"; tst_init();
LIB "homolog.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 0,(x,y),dp;
ideal i    = x2-y3;
ideal j    = x2-y5;
list E     = Ext(0..2,i,j);    // Ext^k(r/i,r/j) for k=0,1,2 over r
qring R    = std(i);
ideal j    = fetch(r,j);
module M   = [-x,y],[-y2,x];
printlevel = 2;
module E1  = Ext(1,M,j);       // Ext^1(R^2/M,R/j) over R=r/i
list l     = Ext(4,M,M,1);     // Ext^4(R^2/M,R^2/M) over R=r/i
printlevel = p;
tst_status(1);$
