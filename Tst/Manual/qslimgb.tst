LIB "tst.lib"; tst_init();
   ring R  = (0,v),(x,y,z,u),dp;
qring Q = std(x2-y3);
ideal i = x+y2,xy+yz+zu+u*v,xyzu*v-1;
ideal j = qslimgb(i); j;
module m = [x+y2,1,0], [1,1,x2+y2+xyz];
print(qslimgb(m));
tst_status(1);$
