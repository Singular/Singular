//
// test script for cols command
//
ring r = 32003,(x,y,z),dp;
r;
matrix m1[3][3];
int c1;
c1=ncols(m1);
c1;
"------------------------------------";
matrix m2[2][4];
int c2;
c2=ncols(m2);
c2;
"------------------------------------";
matrix m3[1][1];
int c3;
c3=ncols(m3);
c3;
"-------------------------------------";
int n=6;
matrix m4[1][n];
int c4;
c4=ncols(m4);
c4;
"-------------------------------------";
matrix m5[100][1];
int c5;
c5=ncols(m5);
c5;
listvar(all);
kill r;
LIB "tst.lib";tst_status(1);$;
