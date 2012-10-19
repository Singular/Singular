LIB "tst.lib";
tst_init();
LIB "polybori.lib";

///////////////////////////////////////////////////////////////////////////////

//////////////// test for boolean_poly(poly,ring) and recursive_boolean_poly(poly,ring)

"!!! Test: boolean_poly, recursive_boolean_poly !!!";

ring r=2,x(1..4),lp;
def br0=boolean_poly_ring(r);
poly f=x(1)^2+2*x(2)*(x(3))-x(4)^3;
"Singular Poly in char=2, x(1..4):";
f;

"Boolean Poly:";
boolean_poly(f,br0);

"Recursive Boolean Poly:";
recursive_boolean_poly(f,br0);
kill r;
// --------------------------------------------------

ring r=0,x(1..7),Dp;
def br1=boolean_poly_ring(r);
poly f=x(4)^3+2*x(7)+23*x(2)^2*x(6)^3-1;
"Singular Poly in char=0, x(1..7):";
f;

"Boolean Poly:";
boolean_poly(f,br1);

"Recursive Boolean Poly:";
recursive_boolean_poly(f,br1);
kill r;

// -------------------------------------------------
ring r=0,(w,x,y,z),Dp;
def br2=boolean_poly_ring(r);
poly f=xyz+20*x^2*y-3*xz+15*w-58699;
"Singular Poly in char=0,(w,x,y,z) :";
f;

"Boolean Poly:";
boolean_poly(f,br2);

"Recursive Boolean Poly:";
recursive_boolean_poly(f,br2);
kill r;
// --------------------------------------------------


//////////////// test for from_boolean_poly(pyobject) and recursive_from_boolean_poly(pyobject)
"!!! Test: from_boolean_poly, recursive_from_boolean_poly !!!";

python_run("ring = Ring(3)");
pyobject pp01 = python_eval("ring.variable(0)+ring.variable(1)");
"Boolean poly:";
pp01;

ring r=0,(x,y,z),Dp;
"Singular poly in char=0, (x,y,z):";
from_boolean_poly(pp01);

"Recursive Singular Poly in char=0,(x,y,z) :";
recursive_from_boolean_poly(pp01);
kill r;

// --------------------------------------------------
python_run("ring1 = Ring(5)");
pyobject pp = python_eval("ring1.variable(0)*ring1.variable(3)+(ring1.variable(1)+ring1.variable(2)*ring1.variable(4))*ring1.variable(0)");
"Boolean poly:";
pp;

ring r=5,(v,w,x,y,z),Dp;
"Singular poly in char=5, (v,w,x,y,z):";
from_boolean_poly(pp);

"Recursive Singular Poly in char=5, (v,w,x,y,z):";
recursive_from_boolean_poly(pp);
kill r;

// --------------------------------------------------

//////////////// test for poly2zdd(poly) and zdd2poly(zdd)
"!!! Test: poly2zdd, zdd2poly !!!";

ring r=0,x,Dp;
poly f=0;
"Singular Poly:";
f;

"poly2zdd:";
poly2zdd(f);

"zdd2poly:";
zdd2poly(poly2zdd(f));
kill r;

// --------------------------------------------------
ring r=0,x,Dp;
poly f=1;
"Singular Poly:";
f;

"poly2zdd:";
poly2zdd(f);

"zdd2poly:";
zdd2poly(poly2zdd(f));
kill r;

// --------------------------------------------------
ring r=0,(v,w,x,y,z),Dp;
poly f=(v+1)*(w+1)*(x+1)*y*z;
"Singular Poly in char=0,(v,w,x,y,z):";
f;
" ";
"poly2zdd:";
poly2zdd(f);
" ";
"zdd2poly:";
zdd2poly(poly2zdd(f));
kill r;

// --------------------------------------------------
ring r=0, x(1..5),Dp;
poly g=x(3);
"Singular Poly in char=0, x(1..5):";
g;

"poly2zdd:";
poly2zdd(g);

"zdd2poly:";
zdd2poly(poly2zdd(g));

kill r;
// --------------------------------------------------
// --------------------------------------------------


//////////////// test for boolean_ideal(ideal,pyobject)
"!!! Test: boolean_ideal !!!";

ring r=0,(x,y,z),Dp;
def br10=boolean_poly_ring(r);
poly f1=x3*y+2*x*z+2z-1;
poly f2=x10*y^2*z^15-x5*z^2+2x*y;
poly f3=19;
ideal I2=f1,f2,f3;
"Singular ideal in char=0,(x,y,z):";
I2;

"Boolean ideal:";
boolean_ideal(I2,br10);
kill r;

// --------------------------------------------------

ring r=2,(x,y,z),Dp;
def br200=boolean_poly_ring(r);
poly f1=xyz+20*x^2*y-3*xz+15;
poly f2=32001*xy+z2;
poly f3=19;
ideal I3=f1,f2,f3;
"Singular ideal in char=2,(x,y,z):";
I3;

"Boolean ideal:";
boolean_ideal(I3,br200);
kill r;

// --------------------------------------------------


//////////////// test for from_boolean_ideal(pyobject)
"!!! Test: from_boolean_ideal !!!";

ring rs=0,(x,y,z),Dp;
python_run("ring2 = Ring(3)");
pyobject pp5 = python_eval("ring2.variable(0)+ring2.variable(2)");
pyobject p5 = python_eval("ring2.variable(0)+ring2.variable(1)");
list I4=(p5,pp5);
"Boolean ideal:";
I4;

"Singular ideal in char=0,(x,y,z):";
from_boolean_ideal(I4);
kill rs;
// --------------------------------------------------

ring rs=5,(v,w,x,y,z),Dp;
python_run("ring7 = Ring(5)");
pyobject p1 = python_eval("ring7.variable(0)*ring7.variable(3)+ring7.variable(1)+ring7.variable(2)*ring7.variable(4)");
pyobject pp1 = python_eval("ring7.variable(4)+ring7.variable(1)*ring7.variable(0)+ring7.variable(0)");
pyobject ppp1 = python_eval("ring7.variable(2)+ring7.variable(3)");
list I5=(p1,pp1,ppp1);
"Boolean ideal:";
I5;

"Singular ideal in char=5,(v,w,x,y,z):";
from_boolean_ideal(I5);
kill rs;

// --------------------------------------------------
// --------------------------------------------------


//////////////// test for boolean_std(ideal)
"!!! Test: boolean_std !!!";
" ";
ring r=2,x(1..4),lp;
poly f1=x(1)^2+2*x(2)*(x(3))-x(4)^3;
poly f2=x(1)^2-x(3)*x(1);
poly f3=x(2)+5-2*x(1);
poly f4=x(1)*x(2)-x(3);
ideal I6=f1,f2,f3,f4;
"Singular ideal:";
I6;

"Boolean_std:";
boolean_std(I6);
kill r;
// --------------------------------------------------

ring r=0,x,Dp;
poly f1=x3+2*x+1;
poly f2=x10-x5+2x;
poly f3=19;
ideal I7=f1,f2,f3;
"Singular ideal:";
I7;

"Boolean_std:";
boolean_std(I7);
kill r;

// --------------------------------------------------

ring r=32003,(x,y,z),Dp;
poly f1=xz+y+20*x^2*y;
poly f2=32002*xy+xz2+y;
ideal I8=f1,f2;
"Singular ideal:";
I8;

"Boolean_std:";
boolean_std(I8);
kill r;

// --------------------------------------------------
// --------------------------------------------------
//////////////// test for boolean_set(zdd,pyobject) and from_boolean_set(pyobject, def)
"!!! Test: boolean_set, from_boolean_set !!!";

ring r=17,(x,y,z),Dp;
poly f=25015x7+13689x3y4+21482x6z+18377x5z2+21142x4yz2+29755xy4z2+6632y4z3+644x3z4+15302x2yz4;
def rb=boolean_poly_ring(r);
def fb=boolean_poly(f,rb);
"Boolean Polynomial:";
fb;

def fs=fb.set();
zdd fz=from_boolean_set(fs,rb);
"from_boolean_set visualized:";
disp_zdd(fz);


"boolean_set:";
boolean_set(fz,rb);
kill r;
// --------------------------------------------------

ring r=2,x(1..6),Dp;
def rb6=boolean_poly_ring(r);
poly h=x(3)^3+x(2)^2*x(4)+x(3)^2*x(5)+x(1)*x(2)*x(6)+x(3)*x(4)*x(6)+x(4)^2*x(6)+x(5)^2*x(6)+x(2)*x(6)^2;
def hb=boolean_poly(h,rb6);
"Boolean Polynomial:";
hb;

def hs=hb.set();
zdd hz=from_boolean_set(hs,rb6);
"from_boolean_set visualized:";
disp_zdd(hz);

"boolean_set:";
boolean_set(hz,rb6);
kill r;
// --------------------------------------------------

ring r=0,(a,b,c,d,e),Dp;
def rb9=boolean_poly_ring(r);
poly j=(a+1)*(b+1)*(c+1)*(d+1)*(e+1);
def jb=boolean_poly(j,rb9);
"Boolean Polynomial:";
jb;

def js=jb.set();
zdd jz=from_boolean_set(js,rb9);
"from_boolean_set visualized:";
disp_zdd(jz);

"boolean_set:";
boolean_set(jz,rb9);
kill r;
// --------------------------------------------------
// --------------------------------------------------


//////////////// test for Typecasts
"!!! Test: Typecasts !!!";

ring r=0,x(1..5),Dp;
poly f=(x(1)-1)*(x(2)*x(3)-x(4))+x(5);
"Poly f in Singular:";
f;

bpoly lb=f;
"bpoly lb=f;";
lb;

poly ftest=poly(lb);
"poly ftest=poly(lb);";
ftest;

pyobject g=lb;
"pyobject g=lb;";
g;

bpoly lbtest=g;
"bpoly lbtest=g;";
lbtest;

zdd s=f;
"zdd s=f;";
s;

poly sbtest=poly(s);
"poly sbtest=poly(s);";
sbtest;

pyobject sb=s;
"pyobject sb=s;";
sb;

zdd stest=sb;
"zdd stest=sb;";
stest;

poly f2=f^2;
ideal I=f,f2;
"Ideal I in Singular:";
I;

bideal Ib=I;
"bideal Ib=I;";
Ib;

ideal Itest=ideal(Ib);
Itest;

pyobject J=Ib;
J;

bideal Ibtest=J;
Ibtest;

// --------------------------------------------------
// --------------------------------------------------

// testing cached rings...
ring r4=0,x(1..10),Dp;
bring rb4 = r4;
bpoly bp4_1 = x(1)*x(2)*x(6)*x(7)+1;
bpoly bp4_2 = x(1)*x(2)*x(3)+1;
lead(bp4_1) < lead(bp4_2);

ring r5=0,x(1..10),(Dp(5),Dp(5));
bring rb5 = r5;
bpoly bp5_1 = x(1)*x(2)*x(6)*x(7)+1;
bpoly bp5_2 = x(1)*x(2)*x(3)+1;

lead(bp5_1) < lead(bp5_2);

ring r7=0,x(1..10),(Dp(3),Dp(3),Dp(6));
bring rb7 = r7;
bpoly bp7_1 = x(1)*x(2)*x(6)*x(7)+1;
bpoly bp7_2 = x(1)*x(2)*x(4)+1;

lead(bp7_1) < lead(bp7_2);

// Checking some more overloaded stuff
ring r11=2,x,lp;
bideal bI11 = ideal(x, x+1);
std(bI11);

bideal bI11 = list(x, x+1);

bI11[1..2];
bring br11 = r11;
nvars(br11);

bpoly bp11 = bI11[1];

bideal(bp11);

bp11 + 1;

bI11 + bideal(bp11);
bI11 + bp11;

ring r12=2,(x,y),lp;
bideal bI12_1 = ideal(x, x+1);
bideal bI12_2 = ideal(y, y+1);

bI12_1 + bI12_2;

bI12_2 = ideal(x, y, y+1);
bI12_1 + bI12_2;

// --------------------------
// execute examples

example bvar;
example boolean_set;
example from_boolean_set;
example from_boolean_poly;
example boolean_constant;
example boolean_poly;
example from_boolean_poly;
example boolean_poly;
example recursive_boolean_poly;
example boolean_ideal;
example from_boolean_ideal;
example boolean_std;
example from_boolean_constant;
example recursive_from_boolean_poly;
example poly2zdd;
example poly2zdd;
example disp_zdd;


// Finally statistics...
tst_status(1);

