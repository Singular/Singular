//
// test script for minor command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
LIB "lib0";
"-------------------------------";
matrix m1[3][3]=maxideal(2);
pmat(m1);
minor(m1,3);
"---------------------------------";
ring r2=0,(x(1..9)),(dp);
matrix m2[3][3]=maxideal(1);
pmat(m2);
minor(m2,2);
"---------------------------------";
ring r3=0,(x(1..64)),ds;
matrix m3[8][8]= maxideal(1);
pmat(m3);
minor(m3,1);
"----------------------------------";
matrix m4[8][8]=maxideal(2);
pmat(m4);
minor(m4,1);
"----------------------------------";
listvar(all);
kill r1,r2,r3;
LIB "tst.lib";tst_status(1);$;
