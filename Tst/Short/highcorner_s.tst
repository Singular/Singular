LIB "tst.lib";
tst_init();
//
// test script for highcorner
//
ring r=32003,(x,y),ds;
ideal i=x2,xy,y3;
module m=i*gen(1),i*gen(2);
highcorner(i);
highcorner(m);
m=m,[0,y2];
highcorner(m);
//
tst_status(1);$;
