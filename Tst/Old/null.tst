//
// test script for nullspace command
//
pagelength = 10000;
ring r1 = 32003,(x,y,z),(c,ds);
r1;
LIB "lib0";
"--------------------------------";
matrix m4[2][2]=1,2,2,4;
pmat(m4);
nullspace(m4);
"-------------------------------";
matrix m1[4][4]=1,0;
pmat(m1);
nullspace(m1);
//mistake
"-------------------------------";
int k=7;
matrix m2[k][5];
pmat(m2);
nullspace(m2);
//mistake
"------------------------------";
int k=7;
matrix m3[k+3][9]=1,2,3,4,5,6,0,0,4,5;
pmat(m3);
nullspace(m3);
//mistake
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
