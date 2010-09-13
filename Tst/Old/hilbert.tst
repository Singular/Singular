//
// test script for degree command
//
ring r1=32003,(x,y,z),ds;
"------------------------------";
ideal i1=x2,y2,z2;
i1;
ideal i2=std(i1);
i2;
hilb(i2);
"-----------------------------";
module m=[x2,0],[0,y2],[0,z2];
m;
module m1=std(m);
m1;
hilb(m1);
"-----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;

