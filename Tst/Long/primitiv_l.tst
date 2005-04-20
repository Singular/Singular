// File: primitiv_l.tst
// Intensive tests for primitiv lib
LIB "tst.lib";
tst_init();
LIB "primitiv.lib";
// ------------ test of primitive and primitive_extra: -------------------
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
// else minpoly(a)=0, g_1(a) is squareroot of 2, g_2(a) is squareroot of 3
// as it should be
setring r;
primitive_extra(i);
i=x2-3,y2-x;
primitive(i);
primitive_extra(i);
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
zeit=timer;
o=primitive_extra(ideal(f1,f2));
tst_ignore(timer-zeit,"time");
tst_ignore(kmemory(),"memory");
"size of the result :",size(string(o)),"characters";
kill r;
ring r=3,(x,y,z),dp;
ideal i=x2+1,y3-y-1,z2+yz-1;
primitive(i);
primitive_extra(i);
primitive_extra(ideal(i[1],i[2]));
kill r;
// ------------ test of splitring: -------------------
ring r=2,a,Dp;
def rr=splitring(a3+a+1);
setring rr; basering;
kill r,rr;
ring r=7,(a,b,c,d,e,n,o,p),Ds;
def rr=splitring(a2+2);
kill r,rr;
ring r=0,(x,y),ds;
def R1=splitring(x2-5);
setring R1; basering;
def R2=splitring(x2-7,a);
setring R2;
erg[1]^2;
basering;
def R3=splitring(y2+1,a);
setring R3; basering;
erg;
kill r,R1,R2,R3;
ring r=(2003,j),(a,b,c,d),dp;
minpoly=j2-2;
def R1=splitring(b3+b2+b-2,list(a,b,c,d,j));
setring R1; erg;
number(erg[5])^2;
factorize(b3+b2+b-2);
def R2=splitring(c2-2j,list(a2+ja,j,erg[5]));
setring R2;
erg[3]^2;
kill R1,R2,r;
tst_status(1); $
