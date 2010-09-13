//
// test script for char command
//
ring r1 = 32003,(x,y,z),dp;
r1;
int k1;
k1=char(r1);
k1;
"-----------------------------------";
ring r2 = 0,(x,y,z,a,b,c),(ds(3),dp);
int k2;
k2=char(r2);
k2;
"------------------------------------";
ring r3 = 10,(x(1..6)),(lp(2),dp(4));
int k3;
k3=char(r3);
k3;
"------------------------------------";
ring r4 =  0,(x,y,z),(c,dp);
int k4;
k4=char(r4);
k4;
listvar(all);
kill r1,r2,r3,r4,k1,k2,k3,k4;
LIB "tst.lib";tst_status(1);$
