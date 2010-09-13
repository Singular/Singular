//
// test script for kbase command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
poly s1=x2+xyz;
poly s2=x5z+y6z+xz3;
poly s3=xy+xz+yz;
ideal i1=x2,y2,z2;
ideal i2=maxideal(2)+i1;
i1;
kbase(i1);
"---------------------------------";
i2;
kbase(i2);
"-------------------------------";
module m=freemodule(3);
m;
kbase(m);
"-----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
