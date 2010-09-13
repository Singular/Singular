//
// test script for subst command
//
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
// gmg, 17.9.99
ring r = 0,(x,y),ds;
poly f = y2-2x10y-x9y3+1/4x8y5-1/8x7y7+5/64x6y9-7/128x5y11+21/512x4y13 
        -33/1024x3y15+429/16384x2y17+x20-715/32768xy19+x19y2+2431/131072y21;
	matrix Hess = jacob(jacob(f));
	subst(Hess,x,0,y,0);

LIB "tst.lib";tst_status(1);$;
