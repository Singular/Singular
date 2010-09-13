//
// test script for homog command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
"-------------------------------";
poly s1=x2y3+45x6y3+68x4z5+80y6x8;
poly s2=6x5+3y6+8z6;
poly s3=12xyz3+2y3z6;
ideal i1=s1,s2,s3;
homog(s2,z);
"--------------------------------";
homog(i1,z);
"--------------------------------";
homog(s1,y);
"--------------------------------";
homog(i1,x);
"----------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
