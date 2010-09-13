//
// test script for jet command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
poly s1=x2+xyz;
poly s2=x5z+y6z+xz3;
poly s3=xy+xz+yz;
ideal i1=x2+xyz,y2-z3y,z3+y5xz;
ideal i2=maxideal(3)+i1;
s1;
jet(s1,1);
jet(s1,2);
jet(s1,3);
s1-jet(s1,3);
jet(s1,3)-jet(s1,2);
"---------------------------------";
i1;
jet(i1,1);
jet(i1,2);
//jet(i1,3)-jet(i1,1);
"-------------------------------";
vector v1=[s1,s2-s1,s3-s1]+s1*gen(5);
vector v2=[s1,s2,s3]+s2*gen(5);
vector v3=[s1,s2-s1,s3]+s1*gen(5);
module m=v1,v2,v3;
v1;
jet(v1,1);
jet(v1,2);
"-----------------------------";
m;
jet(m,1);
jet(m,2);
jet(m,8);
"-----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
