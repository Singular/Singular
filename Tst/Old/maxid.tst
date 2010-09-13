//
// test script for maxideal command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
LIB "lib0";
"-------------------------------";
matrix m1[3][3]=maxideal(2);
pmat(m1);
"---------------------------------";
ring r2=0,(x(1..9)),(dp);
matrix m2[3][3]=maxideal(1);
pmat(m2);
"-------------------------------";
matrix m5[3][3]=maxideal(4);
pmat(m5);
"---------------------------------";
ring r3=0,(x(1..64)),ds;
matrix m3[8][8]= maxideal(1);
pmat(m3);
"----------------------------------";
matrix m4[8][8]=maxideal(3);
pmat(m4);
"----------------------------------";
listvar(all);
kill r1,r2,r3;
LIB "tst.lib";tst_status(1);$;
