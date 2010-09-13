//
// test script for defined command
//
ring r = 32003,(x,y,z),dp;
r;
matrix m1[3][3];
int c1;
poly s1=3xz+zy4;
poly s2=5zx2;
intvec iv1=12,2,4,0,1,0;
ideal i1=s1,s2-s1,s2*s1;
"defined(r)";
defined(r);
"-----------------------------";
"defined(s6)";
defined(s6);
"-----------------------------";
"defined(s1) and defined(i1)";
defined(s1) and defined(i1);
"-------------------------------";
"defined(s2) and defined(s3)";
defined(s2) and defined(s3);
"-------------------------------";
"defined(m1) or defined(m3)";
defined(m1) or defined(m3);
listvar(all);
kill r;
LIB "tst.lib";tst_status(1);$;
// fur proc?
