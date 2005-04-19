// File: hnoether_l.tst
// Intensive tests for hnoether lib
LIB "tst.lib";
tst_init();
LIB "hnoether.lib";
// ------------ test of develop: -------------------
ring zuklein=0,x,dp;
develop(x2+2x3);
ring r=0,(x,y),ds;
develop(x2+2x+3+y);
develop(0);
develop(x2+y2);
develop(x2+y+y2);
develop(2x2+3xy+4xy3-x2y);
poly p_1 = y2+x3;
poly p_2 = p_1^2 + x5y;
poly p_3 = p_2^2 + x^10 *p_1;
poly p_4 = p_3^2 + x^20 *p_2;
poly p_5 = p_4^2 + x^40 *p_3;
param(develop(p_2));
int z=timer;
param(develop(p_4));
tst_ignore(timer-z,"time");
z=timer;
list hne=develop(p_4,-1);
tst_ignore(timer-z,"time");
print(extdevelop(hne,4)[1]);
tst_ignore(timer-z,"time");
z=timer;
print(develop(p_4,4)[1]);
tst_ignore(timer-z,"time");
poly heme=xy8+y8+x4y6+4x3y6+2x5y5+6x6y4+4x8y3+x10y2+4x9y2+2x11y+x12;
hne=develop(heme);
print(hne[1]);
ring R=32003,(x,y),dp;
poly p_5=imap(r,p_5);
z=timer;
print(develop(p_5,-1)[1]);
tst_ignore(timer-z,"time");
kill r;
ring r=32003,(x,y),ls;
poly p_5=fetch(R,p_5);
z=timer;
print(develop(p_5,-1)[1]);
tst_ignore(timer-z,"time");
kill r,R;
ring r=2,(x,y,t),ds;
poly f=y2+x7y+x9+x8;
param(develop(f));
kill r;
ring r=7,(x,y),dp;
poly f=2*(y-3x7)^14;
develop(f+x12y14);
develop(f+x7y14);
kill r;
ring r=(0,i),(a,b),dp;
develop(a2+ib3);
develop(a5+i2*a4+2i*a2b+b2);
kill r;
ring r=(0,i),(a,b),dp;
minpoly=i2+1;
develop(a2+ib3);
develop(a5+i2*a4+2i*a2b+b2);
develop((a+2b)^2+ib5);
kill r;
ring r=(7,i),(x,y),dp;
develop(x+y);
kill r;
ring r=(7,i),(x,y),dp;
minpoly=i2+1;
develop(x);
kill r;
ring r=(49,k),(g,h),Dp;
develop(g3+h4);
develop((g2+2h)^7+h9);
kill r;
ring r=real,(x,y),dp;
develop(x-y);
kill r;
ring r=(11,a,b,c),(d,e,f),ls;
develop(ad2+a2b3e3+2bc4e4);
kill r;

// ------------ test of hnexpansion: -------------------
setring zuklein;
hnexpansion(x);
ring r=7,(a,b),dp;
hnexpansion(a6+b4);
kill r;
ring r=0,(x,y),dp;
hnexpansion(x6-y4);
hnexpansion((x6-y4)*(y6-x4));
hnexpansion((y-x2+x3)*(y-x2-x3));
hnexpansion((x7-2x4y2+xy4-1y5)*(x7-4x4y2+4xy4-1y5));
hnexpansion((y2-x3)*(y2-x3-x4));
hnexpansion((y2-x3-x4)*(y2+x3+x4));
hnexpansion(((x2-y)^2+x5)*((2x2-y)^2+x5));
hnexpansion(x2+1+y3);
hnexpansion(0);
kill r;
ring r=(49,i),(x,y),dp;
def L=hnexpansion(x);
def HNring = L[1]; setring HNring;  displayHNE(hne);
kill L,HNring,r;
ring r=(7,i),(x,y),dp;
list hne=hnexpansion(x3+y6);
displayHNE(hne);
kill hne;
def L=hnexpansion((x2+y4)*(x+y4));
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring r;
kill L,HNring;
hnexpansion(x2+y4);
kill r;
ring r=(0,e),(x,y),lp;
hnexpansion(x2-y2);
hnexpansion(x2-ey2);
kill r;
ring r=(32003,i),(x,y),dp;
minpoly=i2+1;
param(hnexpansion(x2+y4));
kill r;
ring r=(32003,i),(x,y),dp;
minpoly=i2+i+1;
param(hnexpansion(x2+y4));
kill r;
ring r=real,(x,y),dp;
hnexpansion(x);
kill r;
ring r=(11,a,b,c),(d,e,f,g),dp;
list L=hnexpansion(a2bd2+bce3+a2c3e4);
displayHNE(L);
kill r;
ring r=(11,a,b,c),(d,e),Ds;
hnexpansion(a2bd2+bce3+a2c3e4);
kill r;
ring r=(11,a),(x,y,z),wp(1,2,3);
minpoly=a2+4;
list L=hnexpansion(x2+ay3);
param(L,1);
kill r;
ring F2=2,(x,y,t),ds;
poly f=y2+x7y+x9+x8;
def L=hnexpansion(f);
def P=param(L[1],1);
map phi=F2,P[1][1],P[1][2];
phi(f);
kill L,P,phi,F2;

//--------------- examples with change to ring extension
ring R=32003,(x,y),dp;
poly f=(x6+(y-x)^4)*(y+x);
def L=hnexpansion(f);
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring R;
kill HNring;
f=x19-4x18+3x17+8x16y-1x16-10x15y-4x14y2+6x14y+10x13y2-16x12y2+26x10y3-5x9y4
     -30x8y4+2x7y5+26x6y5-16x4y6+6x2y7-1y8;
L=hnexpansion(f);
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring R;
kill HNring;
f=x29-6x28+14x27+12x26y-18x26-56x25y-6x24y2+13x25+110x24y+84x23y2-5x24
     -108x23y-282x22y2-56x21y3+54x22y+396x21y2+390x20y3+14x19y4-266x20y2
     -840x19y3-310x18y4+790x18y3+1134x17y4+138x16y5-1575x16y4-1008x15y5
     -30x14y6+2220x14y5+588x13y6+2x12y7-2268x12y6-216x11y7+1692x10y7+45x9y8
     -915x8y8-4x7y9+350x6y9-90x4y10+14x2y11-1y12;
L=hnexpansion(f);
def HNring = L[1]; setring HNring;  displayHNE(hne);
displayInvariants(hne);
setring R;
kill HNring;
L=hnexpansion(xy6+x3y4-4x5y3+6x7y2-4x9y+x11,"ess");
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring R; kill HNring;
L=hnexpansion(y10-4x3y8+6x6y6-4x9y4+x12y2+x16,"ess");
L[2];
def HNring = L[1]; setring HNring;
def P=param(hne[1]);
map phi=HNring,P[1][1],P[1][2];
phi(f);
kill P,phi;
setring R; kill HNring;
L=hnexpansion((y10-4x3y8+6x6y6-4x9y4+x12y2+x16)*(x6-y4),"ess");
L[2];
def HNring = L[1]; setring HNring;  displayHNE(hne);
ideal P; map phi;
for (int i=1; i<=size(hne); i++)
{ 
  P=param(hne[i]);
  phi=HNring,P[1],P[2];
  phi(f);
}
kill i,P,phi,L,HNring,R;

ring r=0,(x,y),ds;
poly g=x12+y15+x13+x16;
def L=hnexpansion(g,1);
L[2];
def HNring = L[1]; setring HNring;  displayHNE(hne);
ideal P; map phi;
for (int i=1; i<=size(hne); i++)
{ 
  P=param(hne[i]);
  phi=HNring,P[1],P[2];
  ord(phi(f));
}
kill i,P,phi,L,HNring;
setring r;
poly f=(x2-y2)*(x2+y2)*(x2+y4);
def L=hnexpansion(f,"ess"); 
L[2];
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring r; kill HNring;
f=(x2-y2)*(x2+y2)*(2y2+x4)*(y2-x7)*(2x2+y4);
L=hnexpansion(f,"ess");
L[2];
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring r; kill HNring;
f=(y2+x2)*(x2-y3)*(x6-13y12);
L=hnexpansion(f,1);
L[2];
def HNring = L[1]; setring HNring;
ideal P; map phi;
for (int i=1; i<=size(hne); i++)
{ 
  P=param(hne[i]);
  phi=HNring,P[1],P[2];
  phi(f);
}
kill i,P,phi,HNring,L,r;
ring r=(0,a),(x,y),ds;
minpoly=a2-2;
poly f=(x2-y2)*(x2+y2)*(2y2+x4)*(y2-x7)*(2x2+y4);
def L=hnexpansion(f,1); 
L[2];
def HNring = L[1]; setring HNring; 
ideal P; map phi;
for (int i=1; i<=size(hne); i++)
{ 
  P=param(hne[i]);
  phi=HNring,P[1],P[2];
  phi(f);
}
kill i,P,phi,HNring,L;

//--------------- examples with more than one ring change
ring F3=3,(x,y),dp;
list L=hnexpansion((x3-xy2+y3)*(x2+y2)*(x4-x3y+xy3+y4));
def HNring = L[1]; setring HNring;  
map T; int i;
for (i=1; i<=size(hne); i++) {
 T=basering,param(hne[i]);
 T(f);
}
displayHNE(hne);
def displayRing=displayHNE(hne[3],1);
setring displayRing; HNE;
kill displayRing,HNring,i;
setring F3;
poly f=(x3-xy4+y6)*(x2+y2)*(x4-x3y+xy3+y4);
L=hnexpansion(f,1);
L[2];
def HNring = L[1]; setring HNring;  
map T; int i;
for (i=1; i<=size(hne); i++) {
 T=basering,param(hne[i]);
 T(f);
}
kill HNring,i;
setring F3;
f=(x3-xy4+y6)*(x2+y6)*(x4-x3y4+xy12+y16);
L=hnexpansion((x3-xy4+y6)*(x2+y6)*(x4-x3y4+xy12+y16));
def HNring = L[1]; setring HNring;  
map T; int i;
for (i=1; i<=size(hne); i++) {
 T=basering,param(hne[i]);
 T(f);
}
kill L,HNring,i;

ring r=199,(x,y),ds;
poly g=y36+6x8y30-24x9y30+15x16y24-x17y24+20x24y18+41x18y24+42x25y18
 +15x32y12+31x26y18-42x33y12-86x27y18+6x40y6+23x34y12+x41y6-75x35y12
 +x48+31x42y6+59x36y12+24x49+75x43y6+41x50-64x44y6+86x51+25x45y6+59x52
 -25x53-83x54;
def L=hnexpansion(g);
invariants(L);
delta(L);
delta(g);
L=hnexpansion(g,1);
L[2];
invariants(L);
def HNring = L[1]; setring HNring;  displayHNE(hne);
kill L,r,HNring;

// -------------- bigger examples with timings
ring r=0,(x,y),dp;
poly p_1 = y2+x3;
poly p_2 = p_1^2 + x5y;
poly p_3 = p_2^2 + x^10 *p_1;
poly p_4 = p_3^2 + x^20 *p_2;
poly p_5 = p_4^2 + x^40 *p_3;
z=timer;
list L=hnexpansion(p_1*p_2*p_3*p_4);
tst_ignore(timer-z,"time");
tst_ignore(kmemory(),"memory");
print(L[3][1]);
print(L[4][1]);
displayInvariants(L);
z=timer;
L=hnexpansion(p_5);
tst_ignore(timer-z,"time");
tst_ignore(kmemory(),"memory");
print(L[1][1]);
displayInvariants(L);
displayInvariants(L[1]);
kill r;
ring r=0,(x,y),ds;
poly f=(x4-2y6)*(x10+y14+x16+y17)+x30+4x24y8+y32;
z=timer;
def L=hnexpansion(f);
tst_ignore(timer-z,"time");
tst_ignore(kmemory(),"memory");
def HNring = L[1]; setring HNring;  displayHNE(hne);
kill L,HNring,r;

ring r=32003,(x,y),ds;
poly p_1 = y4+x6;
poly p_2 = y7-2x14;
poly p_3 = p_1^14 + p_2^10+x100+y83;
z=timer;
def L=hnexpansion(p_3,"ess"); 
tst_ignore(timer-z,"time");
tst_ignore(kmemory(),"memory");
L[2];
def HNring = L[1]; setring HNring;  displayHNE(hne);
kill r,L,HNring;

// irreducible example with 3 Puiseux pairs over algebraic number field 
// (no field extension needed) 
ring r=(199,a),(x,y,t),ds;
minpoly = 5+12a+3a2-4a3+3a4+a6;
number I = 23-94a+94a2-38a3-81a4-91a5;   // I^2 = -1
number sq = -23+95a-94a2+38a3+81a4+91a5; // sq^3 = 2
ideal J = x-t^12,
          y-I*t^16-sq*t^18-t^19;
def f=eliminate(J,t)[1];
def L=hnexpansion(f);
displayInvariants(L[1]);

// Example with 1 field extension, 6 conjugate branches, 3 Puiseux pairs 
ring r=199,(x,y),ds;
poly g=y48+24*x3y46+77*x6y44+34*x9y42+79*x12y40+56*x9y43-82*x15y38-93*x12y41
 +72*x18y36+12*x15y39+43*x21y34+42*x18y37-6*x15y40-33*x24y32-99*x21y35
 -96*x18y38+74*x27y30+56*x24y33-74*x21y36-88*x30y28-41*x27y31-70*x24y34
 +87*x33y26+37*x30y29+36*x27y32+81*x24y35-55*x36y24-78*x33y27-27*x30y30
 +91*x27y33+87*x39y22+88*x36y25-56*x33y28-87*x30y31-88*x42y20+32*x39y23
 -87*x36y26+14*x33y29+15*x30y32+74*x45y18+42*x42y21-2*x39y24-92*x36y27
 +44*x33y30-33*x48y16-86*x45y19-39*x42y22-17*x39y25+42*x36y28+43*x51y14
 +58*x48y17+24*x45y20-27*x42y23-2*x39y26+72*x54y12-93*x51y15+40*x48y18
 -28*x45y21+84*x42y24+45*x39y27-82*x57y10+80*x54y13+16*x51y16+18*x48y19
 -21*x45y22-19*x42y25+79*x60y8+70*x57y11+20*x54y14+31*x51y17+85*x48y20
 +99*x45y23+34*x63y6-9*x60y9-95*x57y12-97*x54y15+17*x51y18-33*x48y21
 -20*x45y24+77*x66y4-88*x63y7-57*x60y10+86*x57y13+82*x54y16+64*x51y19
 -58*x48y22+24*x69y2+83*x66y5-94*x63y8+25*x60y11-34*x57y14-59*x54y17
 -50*x51y20+x72-16*x69y3-77*x66y6+77*x63y9+83*x60y12+62*x54y18+81*x72y
 +73*x69y4-25*x66y7+96*x63y10+27*x60y13+56*x57y16+13*x54y19+93*x72y2
 +71*x69y5-46*x66y8+58*x63y11+38*x60y14-78*x57y17+77*x75+91*x72y3
 -5*x69y6-65*x66y9-7*x63y12+93*x60y15-40*x75y-38*x72y4-11*x69y7+31*x66y10
 +21*x63y13+15*x60y16+92*x75y2+29*x72y5+76*x69y8-84*x66y11+76*x63y14
 -34*x78+65*x75y3-42*x72y6+4*x69y9+10*x66y12-83*x78y+62*x75y4-2*x72y7
 +39*x69y10-48*x78y2+28*x75y5-23*x72y8+31*x69y11-41*x81+72*x78y3+14*x75y6
 -21*x72y9+8*x81y+94*x78y4-87*x75y7+50*x81y2+74*x78y5-6*x75y8+61*x84
 -79*x81y3-95*x78y6+58*x84y-35*x81y4+19*x84y2-81*x87-27*x84y3+77*x87y+x90;
z=timer;
list L=hnexpansion(g);
tst_ignore(timer-z,"time");
tst_ignore(kmemory(),"memory");
def HNring = L[1]; setring HNring;  displayHNE(hne);
setring r; kill HNring;
z=timer;
L=hnexpansion(g,"ess");
tst_ignore(timer-z,"time");
tst_ignore(kmemory(),"memory");
L[2];

// ------------ test of extdevelop: -------------------
list hne=develop(x2+y3+y4-y5,-1);
show(extdevelop(hne,2));
show(extdevelop(hne,4));
show(extdevelop(hne,10));
kill hne,r;
ring r=(49,i),(x,y),ls;
list hne=develop((x2+iy3)^7+x15);
show(extdevelop(hne,8));
kill hne;
// ------------ test of essdevelop: -------------------
setring F3;
list L=essdevelop((x3-xy2+y3)*(x2+y2)*(x4-x3y+xy3+y4));
L[2];
def HNring = L[1]; setring HNring; displayHNE(hne);
setring F3;
kill HNring;
L=essdevelop(x2+y3+xy4);
L[2];
displayHNE(L[1]);
displayHNE(develop(x2+y3+xy4));
setring r;
kill F3;

// ------------ more tests of hnexpansion: --------------------
list L=hnexpansion((x2+iy3)^7+x15);  
def HNring = L[1]; setring HNring;  displayHNE(hne);
show(extdevelop(hne[1],8));          // Fehler in factorize endlos!
map T=basering,param(hne[1]);
T(f);
setring r; kill HNring;
L=hnexpansion((x2+iy3)^6+x15);     // Als Ersatz fuer die Zwischenzeit
def HNring = L[1]; setring HNring;
map T=basering,param(hne[1]);
ord(T(f));
kill L,HNring,r;

ring r=(0,i),(a,b),ds;
minpoly=i2+1;
list L=hnexpansion(a2+ib3-ia3b);
show(L);
kill r;
ring r=0,(x,y),ds;
displayHNE(hnexpansion((x3+3x2y-xy4+y10)*(x3-x2y+y8)));
// ------------ more tests of param: ------------------------
param(develop(x+1+y2));
param(develop(x2+y3));
param(develop(y2+x3));
param(develop(x+y2-y3));
param(develop(x2+y3+y4));
param(develop((x-2y3)^2+x3));
param(develop(y+x2-y3));
param(develop((y-2x3)^2+x7));
param(develop((y-2x3)^2+x7),0);
param(develop(x+y2-y3),0);
param(develop(x2+y3+y4),"Fritz");
param(develop(y2+x3+x4),x);
z=timer;
param(develop((y-2x3)^2+y5));
tst_ignore(timer-z,"time");
kill r;
ring r=(27,j),(k,l,t),lp;
list hne=develop((k3-jl)^6+l7,9);
param(hne);
// ------------ test of displayHNE: -------------------
displayHNE(hne);
kill r;
ring r=3,(k,l,t),lp;
list hne=develop((k3+l)^6+l7,9);
displayHNE(hne);
def RR=displayHNE(hne,1);
setring RR; HNE;
kill RR,r;
ring r=0,(x,y),dp;
displayHNE(develop(x));
displayHNE(develop(y));
displayHNE(develop(1));
// ------------ test of displayInvariants: ------------
list L=hnexpansion((x2+y3+y4)*(x+y2+y3-y4)*(x5+y7));
displayInvariants(L[1]);
displayInvariants(L[2]);
displayInvariants(L);
// ------------ test of is_irred: ---------------------
is_irred(x);
is_irred(x2);
is_irred((x+y)*(1+x+y));  // one HNE
is_irred((x+y)*(x+y+xy)); // one of two HNEs found
is_irred(x2+y2);          // no HNE found
is_irred(1+x+y);          // unit
// ----------------------------------------------------------------------------
// ------- test of invariants, delta, intersection, puiseux2generators,     ---
// -------         multiplicities, newtonpoly                               ---
example delta;
example invariants;
poly f=(x2-y3+y5)*(x2+y3-y5);
L=hnexpansion(f);
delta(f);
delta(L);
intersection(L[1],L[2]);
puiseux2generators(intvec(3,7,15,31,63,127),intvec(2,2,2,2,2,2));
example newtonpoly;
// ------- test of getnm, T_Transform, T1_Transform, T2_Transform, koeff, -----
// ------------  redleit, squarefree, allsquarefree, referencepoly   ----------
example getnm;
T_Transform(y2+x3,1,2);
T1_Transform(y-x2+x3,1,2);
T2_Transform(y2+x3-x2y,-1,3,2,referencepoly(newtonpoly(y2+x3-x2y)));
koeff(x2+2xy+3xy2-x2y-2y3,1,2);
example redleit;
example squarefree;
ring R=(27,k),(x,y),dp;
squarefree(x2);
squarefree(y3);
squarefree((x2+y)^3*(x-y)^2*(x+y));

example allsquarefree;
// ------ Test multSequence (depends on numbering of branches):
ring rr=0,(x,y),ls;
poly f=x6y-3x4y4-x4y5+3x2y7-x4y6+2x2y8-y10+2x2y9-y11+x2y10-y12-y13;
multsequence(f);

tst_status(1);$
