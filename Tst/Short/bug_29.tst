LIB "tst.lib";
tst_init();

ring r=0,(x,y),dp;
ideal i=xy;
qring rq=std(i);
ideal j=x;
mres(j,0);
list mr=_;
size(mr);

tst_status(1);$

