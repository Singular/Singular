// File: primitiv_s.tst
// Short tests for primitiv lib
LIB "tst.lib";
tst_init();
LIB "primitiv.lib";
// ------------ test of primitive: -------------------
printlevel=3;
ring r=0,x,dp;
primitive(ideal(x2+1));
kill r;
ring r=0,(x,y),dp;
ideal i=x2-2,y2-3;
ideal j=primitive(i);
factorize(j[1]);
ring r1=(0,a),x,lp;
map phi=r,x,a;
string s=string(phi(j)[1]);
execute("minpoly="+s+";");
ideal v=phi(j)[1],phi(j)[2]^2-2,phi(j)[3]^2-3;
v;
if (size(v)>0)
  { "ERROR!!! Result of primitive(i) is not a primitive element!"; }
// else minpoly(a)==0, g_1(a) is squareroot of 2, g_2(a) is squareroot of 3
// as it should be
setring r;
i=x2-3,y2-x;
primitive(i);
kill r;
ring r=3,(x,y,z),dp;
ideal i=x2+1,y3-y-1,z2+yz-1;
primitive(i);
// ------------ test of primitive_extra: -------------
primitive_extra(i);
primitive_extra(ideal(i[1],i[2]));
kill r;
// ------------ test of splitring: -------------------
ring r=0,(x,y),ds;
splitring(x2-5,"R1");
R1;
list l=splitring(x2-7,"R2",a);
l[1]^2;
R2;
splitring(y2+1,"",a);
R2;
kill r,R1,R2;
ring r=(2003,j),(a,b,c,d),dp;
minpoly=j2-2;
list l=splitring(b3+b2+b-2,"R1",list(a,b,c,d,j));
l;
number(l[5])^2;
factorize(b3+b2+b-2);
list L=splitring(c2-2j,"",list(a2+ja,j,l[5]));
L;
L[3]^2;
kill R1,r;
tst_status(1);$

