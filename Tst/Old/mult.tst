//
// test script for multiplicity command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
"-------------------------------";
ideal i=3x2+yz,7y6+2x2y+5xz;
i;
multiplicity(i);
"-----------------------------";
module m=[x2,y2],[xy,xz],[x4,z8];
m;
multiplicity(m);
"-----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
