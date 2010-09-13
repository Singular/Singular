//
// test script for ring command
//

ring r = 32003,(x,y,z),lp;
r;
poly f=xy;
f;

poly p = x;
listvar(all);

kill r;
ring r1= 2,(x,y,z),lp;
r1;
poly f=x3y;
f;
ring r2= 3,(a,y,z),lp;
r2;
poly f=x3y;
f;
listvar(all);

kill r2;
listvar(all);

ring r3 ;
listvar(all);
kill r3;
listvar(all);
"---------------------------";
ring r3 =,3;
r3;
listvar(all);
kill r3;

ring r3 = ,3,x y z;
listvar(all);

ring r3 = ,3,x y z,l;
listvar(all);

ring r3= 32003,(x,y,z),l,a;
listvar(all);

ring r3= 32003,(x,y,z),qql,a;
listvar(all);

ring r3= 32003,(x,y,z),qql,a;
listvar(all);

"dot dot notation for ring vars";
ring r= 5,(v..z),lp;
r;
kill r;

"dot dot notation for indexed ring vars";
ring r= 7,(x(1..5)),lp;
r;
poly f=x(1)*x(2)^3;
f;
kill r;
kill r1;
ring r = 32003,(x,y,z,a,b,c,d),(lp(2),dp,wp(8));
r;
poly f=xy;
f;
ring r2 = 32003,(x,y,z,a,b,c,d),(lp(2),dp,wp(4,3,2,1));
r2;
ring r3 = 32003,(x,y,z,a,b,c,d),(lp(2),dp,3wp(4,3,2,1));
ring r3 = 32003,(x,y,z,a,b,c,d),(lp(2),dp,lp(4,3,2,1));
ring r3 = 32003,(x,y,z,a,b,c,d),(lp(2),dp,lp(4,3,2,1));
ring r3 = 32003,(x,y,z,a,b,c,d),(lp(2),dp,x(4,3,2,1));
ring r3 = 32003,(x,y,z,a,b,c,d),(lp(2),dp,x);
listvar(all);
ring r=0,(x,y,z),(dp(3),dp);
ring r=0,(x,y,z),(dp(7),dp);
ring r=(0,a),x,dp;
ordstr(r);
varstr(r);
charstr(r);
ring r=(0,a,b,c),(x(1..20)),(dp(2),ls(3),wp(1,2,3),M(6,7,8,9),ds);
r;
ordstr(r);
varstr(r);
charstr(r);
ring r;
ordstr(r);
varstr(r);
charstr(r);
LIB "tst.lib";tst_status(1);$
