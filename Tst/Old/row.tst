//
// test script for nrows command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
"-------------------------------";
matrix m1[4][5];
nrows(m1);
"-------------------------------";
int k=7;
matrix m2[k][5];
nrows(m2);
"------------------------------";
matrix m3[k+3][11];
nrows(m3);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
