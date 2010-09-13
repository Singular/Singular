//
// test script for jacob command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
LIB "lib0";
"-------------------------------";
int a=12;
int b=10;
int c=4;
int d=1;
poly s1=x^a+y^b+z^c+x^(c+3)*y^(d+3)+z^(a-3)*y^(d-1);
poly s2=6x5+3y6+8z6;
poly s3=12xyz3+2y3z6;
ideal i1=jacob(s1);
i1;
"-------------------------";
ideal i2=jacob(s2);
i2;
"--------------------------";
ideal i3=jacob(s3);
i3;
"-------------------------";
matrix m1[3][3]=jacob(i1);
pmat(m1);
"---------------------------";
ideal i10=std(i1);
i10;
degree(i10);
"--------------------------";
ideal i20=std(i2);
i20;
degree(i20);
"---------------------------";
ideal i30=std(i3);
i30;
degree(i30);
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
// jacob von ideal - matrix mit pmat
