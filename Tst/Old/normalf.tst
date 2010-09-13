//
// test script for reduce command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
"-------------------------------";
poly s1=x2+xyz;
poly s2=x5z+y6z+xz3;
poly s3=xy+xz+yz;
ideal i1=x2+xyz,y2-z3y,z3+y5xz;
ideal i2=maxideal(3)+i1;
reduce(s1,i1);
reduce(s2,i1);
reduce(s3,i2);
vector v1=[s1,s2-s1,s3-s1]+s1*gen(5);
vector v2=[s1,s2,s3]+s2*gen(5);
vector v3=[s1,s2-s1,s3]+s1*gen(5);
module m=v1,v2,v3;
reduce(v1,m);
reduce(v2,m);
reduce(v3,m);
m=std(m);
reduce(v1,m);
reduce(v2,m);
reduce(v3,m);
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
