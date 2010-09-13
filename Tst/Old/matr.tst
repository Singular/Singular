//
// test script for matrix command
//
ring r1 = 0,(x,y,z),ds;
r1;
LIB "lib0";
"------------------------------";
poly s1=12x2y4;
poly s2=3*x^2*y^3+x^3;
poly s4=7/9x3z7;
poly s5=12;
poly s6;
matrix m1[2][2]=1,2,3,4;
pmat(m1);
"-----------------------------";
matrix m2[2][3]=x2,y3,z2,zx,4xy,yz;
pmat(m2);
"-------------------------------";
matrix m3[3][2]=s1,s2,s4,s5,s6,x2;
pmat(m3);
"--------------------------------";
matrix m4;
pmat(m4);
"--------------------------------";
matrix m5=m3;
pmat(m5);
"----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
