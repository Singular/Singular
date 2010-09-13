//
// test script for map command
//
ring r1 = 32003,(x,y,z),dp;
r1;
ideal i=x,y,z;
ring r2=32003,(a,b),dp;
r2;
map m=r1,a,b,a+b;
m(i);
"------------------------------";
ring r3=0,(x,y,z),dp;
r3;
ideal i1=x2,y2,z2;
ring r4=0,(a,b),dp;
r4;
map m1=r3,a,b,a-b;
m1(i1);
"------------------------------";
listvar(all);
kill r1,r2,r3,r4;
LIB "tst.lib";tst_status(1);$;
