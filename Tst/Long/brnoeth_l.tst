LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: brnoeth_l.tst,v 1.2 2004-02-23 19:05:57 Singular Exp $");

// tst_status();


// killall();

LIB "brnoeth.lib";

// special CURVE over F_64 (p=2) 

ring r=2,(x,y),lp;
poly f=y9+y8+xy6+x2y3+y2+x3;
ideal I=f,jacob(f);
closed_points(I);
I=f,x2+x,y2+y;
closed_points(I);
I=f,x4+x,y4+y;
closed_points(I);
I=f,x8+x,y8+y;
closed_points(I);
I=f,x16+x,y16+y;
closed_points(I);
I=f,x32+x,y32+y;
closed_points(I);
I=f,x64+x,y64+y;
closed_points(I);
list CURVE=Adj_div(f);
CURVE=NSplaces(1..6,CURVE);
CURVE=extcurve(6,CURVE);
CURVE;
def affR=CURVE[1][1];
setring affR;
listvar();
Aff_SLocus;
Inf_Points;
def projR=CURVE[1][2];
setring projR;
CHI;
Weierstrass(4,10,CURVE);
tst_status();
Weierstrass(3,10,CURVE);
tst_status();
Weierstrass(2,10,CURVE);
tst_status();
intvec DD=1,1,2,1,0,1;
BrillNoether(DD,CURVE);
DD=2,0,3;
BrillNoether(DD,CURVE);
def RP=CURVE[1][5];
setring RP;
size(POINTS);
setring r;
killall();

// SUZUKI curve over F_8 (p=2) 

ring r=2,(x,y),lp;
poly f=x10+x3+y8+y;
ideal I=f,jacob(f);
closed_points(I);
I=f,x2+x,y2+y;
closed_points(I);
I=f,x4+x,y4+y;
closed_points(I);
I=f,x8+x,y8+y;
closed_points(I);
I=f,x16+x,y16+y;
closed_points(I);
I=f,x32+x,y32+y;
closed_points(I);
I=f,x64+x,y64+y;
closed_points(I);
list SUZUKI=Adj_div(f);
SUZUKI=NSplaces(1..3,SUZUKI);
SUZUKI=extcurve(3,SUZUKI);
SUZUKI;
def affR=SUZUKI[1][1];
setring affR;
listvar();
Aff_SLocus;
Inf_Points;
def projR=SUZUKI[1][2];
setring projR;
CHI;
Weierstrass(1,20,SUZUKI);
tst_status();
intvec DD=2,1,0,0,3;
DD[25]=3;
BrillNoether(DD,SUZUKI);
tst_status();
def RP=SUZUKI[1][5];
setring RP;
size(POINTS);
setring r;
killall();

// special HYPERELLIPTIC curve over F_16 (p=2) 

ring r=2,(x,y),lp;
intvec G,D,F;
poly f=x5+y2+y;
list EC=Adj_div(f);
EC=NSplaces(1..4,EC);
EC=extcurve(4,EC);
def R4=EC[1][4];
setring R4;
G[5]=4;
D=1..33;
matrix C=AGcode_Omega(G,D,EC);
print(C);
// the code C=AGcode_Omega(G,D) is of type [33,22,>=10] over F_16;
// the basic decoding algorithm can correct up to 3 errors;
// for it, we need an auxiliary divisor F of degree 5 :
EC=NSplaces(1..5,EC);
F[14]=1;
list SV=prepSV(G,D,F,EC);
matrix y[1][33];
y[1,1]=1;
y[1,2]=a;
y[1,3]=1;
print(decodeSV(y,SV));
y[1,1]=0;
y[1,2]=a;
y[1,10]=a+1;
y[1,3]=0;
y[1,21]=1;
print(decodeSV(y,SV));
y[1,4]=a;
y[1,2]=0;
y[1,12]=a;
y[1,10]=0;
y[1,21]=0;
y[1,29]=a+1;
print(decodeSV(y,SV));
matrix H=dual_code(C);
print(H);
list SC=sys_code(C);
print(SC[1]);
print(SC[2]);
SC[3];
list SH=sys_code(H);
print(SH[1]);
print(SH[2]);
SH[3];
setring r;
killall();


tst_status(1);
$
