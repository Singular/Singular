//
// test script for subst command
//
pagelength = 10000;
ring r1 = 0,(x,y,z),dp;
r1;
"-------------------------------";
poly s1=x2+z3+x+2y+2z;
s1;
subst(s1,x,2);
"-----------------------------";
vector v=[x2,x,0,y2];
v;
subst(v,y,8);
"----------------------------";
ideal i=s1,z3;
i;
subst(i,z,-2);
"----------------------------";
module m=v,[0,x2,y2];
int a=4;
m;
subst(m,x,a);
"-----------------------------";
listvar(all);
kill r1;
$;
