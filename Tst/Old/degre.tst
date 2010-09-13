//
// test script for degree command
//
ring r1=32003,(x,y,z),ds;
"------------------------------";
poly s1=x11+y12+z13+x8y6+x4y6z8;
s1;
ideal i1=jacob(s1);
ideal i2=std(i1);
i2;
degree(i2);
"------------------------------";
poly s2=x9+y6+z8+xy4+xz7+yz9+xyz3+xy6z+x5yz;
s2;
ideal i3=jacob(s2);
ideal i4=std(i3);
i4;
degree(i4);
"--------------------------------";
ideal i5=0,0,0;
i5;
degree(i5);
"-------------------------------";
ideal i6=1,1,1;
i6;
degree(i6);
"-----------------------------";
listvar(all);
kill r1;
"-----s92a/c-------------------";
ring r=32003,(t,x,y,z),dp;
ideal ii=
t2xyz,tx2yz,t3yz,x3yz;
ii=std(ii);
degree(ii);
LIB "tst.lib";tst_status(1);$

LIB "tst.lib";tst_status(1);$;
