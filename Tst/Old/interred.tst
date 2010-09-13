//
// test script for interred command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
poly s1=x2y3+45x6y3+68x4z5+80y6x8;
poly s2=6x5+3y6+8z6;
poly s3=12xyz3+2y3z6;
ideal i1=s1,s2,s3;
interred(i1);
"--------------------------------";
vector v1=[2x3y2,4xyz2];
vector v2=[11xy4,6xz3];
module m=v1,v2;
interred(m);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
