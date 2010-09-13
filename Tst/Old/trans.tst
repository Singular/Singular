//
// test script for transpose command
//
ring r1 = 0,(x,y,z),dp;
r1;
"-------------------------------";
matrix m1[2][3]=1,2,3,4,-1,0;
LIB "lib0";
pmat(m1);
transpose(m1);
"---------------------------------";
matrix m2[3][2]=x2,y2,2z2,0,1,xz;
pmat(m2);
pmat(transpose(m2));
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
