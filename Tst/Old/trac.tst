//
// test script for substring command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
matrix m1[2][2]=1,2,3,4;
LIB "lib0";
pmat(m1);
trace(m1);
"------------------------------";
matrix m2[2][2]=x2,4y2,5z2,12xy;
pmat(m2);
trace(m2);
"-----------------------------";
matrix m3[2][2]=0,x2,4,y2;
pmat(m3);
trace(m3);
"-----------------------------";
matrix m4[2][3]=1,2,3,4,5,6;
pmat(m4);
trace(m4);
"-------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
