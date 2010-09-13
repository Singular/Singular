//
// test script for gen command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
vector v1=gen(6);
v1;
poly f=xyz;
v1=v1+f*gen(4);
v1;
gen(2);
"--------------------------------";
vector v2=gen(36);
v2;
poly f1=x2;
poly f2=y2;
poly f3=z2;
v2=v2+f1*gen(1);
v2;
v2=v2+f2*gen(5);
v2;
v2=v2+f3*gen(36);
v2;
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
