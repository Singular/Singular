// File: primitiv_l.tst
// Intensive tests for primitiv lib
LIB "tst.lib";
tst_init();
LIB "primitiv.lib";
// ------------ test of primitive: -------------------
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
// else minpoly(a)=0, g_1(a) is squareroot of 2, g_2(a) is squareroot of 3
// as it should be
setring r;
i=x2-3,y2-x;
primitive(i);
kill r;
ring r=0,(x(0..3)),lp;
int zeit=timer;
ideal i=x(0)^2-2,x(1)^2-3,x(2)^3-x(0)^2+x(1),x(3)^2-x(2);
primitive(i);
tst_ignore(timer-zeit,"time");
kill r;
ring r=0,(x,y),dp;
poly f1=39x7+37x6+11x5+48x4+30x3+30x2+14x+49;
poly f2=43x5y2+8x2y5+40x6+48y6+22x4y+7x2y3+7x2y2+16x3+49x2+x+12y+40;
zeit=timer;
ideal o=primitive(ideal(f1,f2));
tst_ignore(timer-zeit,"time");
tst_ignore(kmemory(),"memory");
"size of the result :",size(string(o)),"characters";
kill r;
ring r=3,(x,y,z),dp;
ideal i=x2+1,y3-y-1,z2+yz-1;
primitive(i);
kill r;
// ------------ test of splitring: -------------------
ring r=2,a,Dp;
splitring(a3+a+1,"");
r;
kill r;
ring r=7,(a,b,c,d,e,n,o,p),Ds;
splitring(a2+2,"never");
kill r;
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
// ------------ test of randomLast: ------------------
example randomLast;
$
