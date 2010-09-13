//
// test script for preimage command
//
ring r1 = 32003,(x,y,z),dp;
r1;
ideal i1=x,y,z;
ring r2=32003,(a,b),dp;
r2;
map f=r1,a,b,a+b;
f(i1);
ideal i2=a2,b2,a+b;
setring r1;
preimage(r2,f,i2);
"-------------------------------";
setring r2;
map f1=r1,a2,a2-b2,a+b;
f1(i1);
ideal i3=a2,(a+b)^2,b2;
setring r1;
preimage(r2,f1,i3);
"--------------------------------";
preimage(r2,f1,i2);
"---------------------------------";
setring r2;
map f2=r1,6a2,4b3,a+b;
f2(i1);
setring r1;
preimage(r2,f2,i3);
"-------------------------------";
listvar(all);
kill r1,r2;
LIB "tst.lib";tst_status(1);$;
