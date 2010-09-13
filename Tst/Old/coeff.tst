//
// test script for coef command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
poly s1=5x2y7+12z6-3y6z5;
s1;
coef(s1,y);
"-----------------------------";
coef(s1,xy);
"------------------------------";
coef(s1,xyz);
"------------------------------";
poly s2=3;
coef(s2,x);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
