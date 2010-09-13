//
// test script for dim command
//
ring r1 = 32003,(x,y,z),(dp,c);
r1;
"------------------------------";
ideal i1=x2,xz,y4;
i1;
ideal i11=std(i1);
dim(i11);
"------------------------------";
ideal i2=x6,y6x6,x6z6;
i2;
dim(i2);
"-------------------------------";
vector v1=[x6,y3,z6];
poly s1=xyz2;
module m1=v1,s1*v1;
m1;
module m11=std(m1);
dim(m11);
"------------------------------";
vector v2=[s1,x3y4z5,x,y6z12];
poly s2=12xyz;
module m2=v1,v2,s2*v2;
m2;
dim(m2);
"-------------------------------";
ideal i3=0,0,0;
i3;
dim(i3);
"---------------------------";
ideal i4=1,1,1;
i4;
dim(i4);
"--------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;

