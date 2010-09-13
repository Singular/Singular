//
// test script for string command
//
ring r1 = 0,(x,y,z),ds;
r1;
"------------------------------";
ring r2=12,(x(1..7)),(lp(2),dp(6));
r2;
"------------------------------";
ring r3=32003,(x,y,z,a,b,c,d),(ds(3),dp(3));
r3;
"-----------------------------";
ring r4=0,(x,y,z),(c,dp);
r4;
"----------------------------";
listvar(all);
kill r1,r2,r3,r4;
LIB "tst.lib";tst_status(1);$;
