//
// test script for wedge command
//
ring r1 = 0,(x,y,z),ds;
r1;
"--------------------------------";
matrix m1[4][4]=1,2,3,4,-1,2,0,5,2,1,1,1,0,2,2,2;
LIB "lib0";
pmat(m1);
pmat(wedge(m1,2));
"--------------------------------";
matrix m2[3][3]=x2,y4,z4,x4,x,y,z,0,1;
pmat(m2);
int a=3;
pmat(wedge(m2,a));
"-------------------------------";
matrix m3[3][3]=1,1,2,2,1,1,2,2,1;
pmat(m3);
pmat(wedge(m3,2));
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
