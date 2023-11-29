LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  ideal i=x2,xy,y5;
  eliminate(i,x);
  ring R=0,(x,y,t,s,z),dp;
  ideal i=x-t,y-t2,z-t3,s-x+y3;
  eliminate(i,ts);
  ideal j=x2,xy,y2;
  bigintvec v=hilb(std(j),1);
  eliminate(j,y,v);
tst_status(1);$
