  ring r=32003,(x,y,z),dp;
  ideal i=x2,xy,y5;
  eliminate(i,x);
  ring R=0,(x,y,t,s,z),dp;
  ideal i=x-t,y-t2,z-t3,s-x+y3;
  eliminate(i,ts);
  bigintvec v=hilb(std(i),1);
  eliminate(i,ts,v);
LIB "tst.lib";tst_status(1);$
