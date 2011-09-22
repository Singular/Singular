LIB "tst.lib"; tst_init();
LIB "homolog.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 0,(x,y),dp;
ideal i    = x2,y;
ideal j    = x;
list E     = Tor(0..2,i,j);    // Tor_k(r/i,r/j) for k=0,1,2 over r
qring R    = std(i);
ideal j    = fetch(r,j);
module M   = [x,0],[0,x];
printlevel = 2;
module E1  = Tor(1,M,j);       // Tor_1(R^2/M,R/j) over R=r/i
list l     = Tor(3,M,M,1);     // Tor_3(R^2/M,R^2/M) over R=r/i
printlevel = p;
tst_status(1);$
