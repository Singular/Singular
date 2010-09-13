//
// test script for intersect command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
"-------------------------------";
poly s1=x2y3+45x6y3+68x4z5+80y6x8;
poly s2=6x5+3y6+8z6;
poly s3=12xyz3+2y3z6;
ideal i1=s1,s2,s3;
ideal i2=s1+s2,s2,s1;
vector v1=[s1,s2,s2*s3]+s1*gen(5);
vector v2=[s1-s2,s3,s2]+s1*gen(5);
module m1=v1,v2;
module m2=v2,2*v1;
intersect(i1,i2);
"-------------------------";
intersect(m1,m2);
"-------------------------";
listvar(all);
kill r1;
"--------------------------";
ring r1=0,(x,y,z),(c,ds);
poly s1=x2y3+45x6y3+68x4z5+80y6x8;
poly s2=6x5+3y6+8z6;
poly s3=12xyz3+2y3z6;
ideal i1=s1,s2,s3;
ideal i2=s1+s2,s2,s1;
vector v1=[s1,s2,s2*s3]+s1*gen(5);
vector v2=[s1-s2,s3,s2]+s1*gen(5);
module m1=v1,v2;
module m2=v2,2*v1;
intersect(i1,i2);
"-------------------------";
intersect(m1,m2);
"-------------------------";
listvar(all);
intersect(x,y,z);
intersect(ideal(x),ideal(y),ideal(z2),ideal(z));
intersect(x,y,z2,z);
kill r1;
LIB "tst.lib";tst_status(1);$;
//bsp mit char 0
